/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

// Demonstrates how to lag a client in the past using the interpolation history class,
// in order to get smooth visuals despite choppy input.
// Start two instances on the same computer, press 's' on one instance, 'c' on the other.
// Hold down space to see the actual networking.
// Change SERVER_IP_ADDRESS to connect over the internet

#ifdef WIN32
#include "WindowsIncludes.h"
#else
#define HWND void*
#endif

// Ogre includes
#include "OgreTextAreaOverlayElement.h"
#include "Ogre.h"
#include <OIS.h>

// Stuff to help run ogre
#include "OverlayHelper.h"
#include "App3D.h"

// RakNet includes
#include "GetTime.h"
#include "RakSleep.h"
#include "RakAssert.h"
#include "StringTable.h"
#include "RakPeerInterface.h"

#include "BitStream.h"
#include "MessageIdentifiers.h"
#include "ReplicaManager3.h"
#include "NetworkIDManager.h"
#include "RakSleep.h"
#include "FormatString.h"
#include "StringCompressor.h"
#include "Rand.h"
#include "TransformationHistory.h"

using namespace Ogre;
using namespace DataStructures;
using namespace OIS;
using namespace RakNet;

class Popcorn;

// Network variables
static const char *SERVER_IP_ADDRESS="127.0.0.1";
static const unsigned short SERVER_PORT=12345;
// How often the server sends position updates to the client
static const int DEFAULT_SERVER_MILLISECONDS_BETWEEN_UPDATES=250;

// Demo variables
static const int MIN_KERNELS=100;
static const int KERNELS_VARIANCE=60;
static const RakNet::TimeMS POP_COUNTDOWN_MIN_DELAY_MS=1000;
static const RakNet::TimeMS POP_COUNTDOWN_VARIANCE_MS=5000;
static const RakNet::TimeMS RESTART_TIMER_MS=14000;
static const float POSITION_VARIANCE=100.0f;
static const float PLANE_VELOCITY_VARIANCE=30.0f;
static const float UPWARD_VELOCITY_MINIMUM=35.0f;
static const float UPWARD_VELOCITY_VARIANCE=25.0f;
static const float DOWNWARD_ACCELERATION = -15.0f;

bool isServer;
Ogre::Entity *popcornKernel, *popcornPopped;
RakNet::RakPeerInterface *rakPeer;
DataStructures::List<Popcorn*> popcornList;
bool enableInterpolation;

App3D *app;

// This class represents a kernel of popcorn, which pops after a short delay
// When to pop, and the physics the popcorn takes is entirely controlled by the server
// Every DEFAULT_SERVER_MILLISECONDS_BETWEEN_UPDATES the server will send an update
// The client intentionally lags this far behind, so it always has a recent position to interpolate to
class Popcorn : public Replica3
{
public:
	Popcorn() {
		// Buffer up for 3 seconds if we were to get 30 updates per second
		transformationHistory.Init(30,3000);

		position=Ogre::Vector3::ZERO;
		orientation=Ogre::Quaternion::IDENTITY;
		// Visible position is where we are interpolating at, which is behind the real position
		visiblePosition=Ogre::Vector3::ZERO;
		visibleOrientation=Ogre::Quaternion::IDENTITY;
		isKernel=true;
	}
	virtual ~Popcorn()
	{
		if (isServer)
			BroadcastDestruction();

		app->GetSceneManager()->destroyEntity(sceneNode->getAttachedObject(0)->getName());
		app->GetSceneManager()->getRootSceneNode()->removeAndDestroyChild(sceneNode->getName());
		popcornList.RemoveAtIndex(popcornList.GetIndexOf(this));
	}

	bool isKernel;
	Ogre::Vector3 position;
	Ogre::Quaternion orientation;
	Ogre::Quaternion rotationalVelocity;
	Ogre::Vector3 velocity;
	Ogre::SceneNode *sceneNode;
	RakNet::TimeMS popCountdown;
	Ogre::Vector3 visiblePosition;
	Ogre::Quaternion visibleOrientation;
	TransformationHistory transformationHistory;

	virtual void WriteAllocationID(RakNet::Connection_RM3 *destinationConnection, RakNet::BitStream *allocationIdBitstream) const
	{
		StringTable::Instance()->EncodeString("Popcorn", 128, allocationIdBitstream);
	}
	virtual RM3ConstructionState QueryConstruction(RakNet::Connection_RM3 *destinationConnection, ReplicaManager3 *replicaManager3)
	{
		if (isServer)
			return QueryConstruction_ServerConstruction(destinationConnection, isServer);
		else
			return QueryConstruction_ClientConstruction(destinationConnection, isServer);
	}
	virtual bool QueryRemoteConstruction(RakNet::Connection_RM3 *sourceConnection){
		if (isServer)
			return QueryRemoteConstruction_ServerConstruction(sourceConnection, isServer);
		else
			return QueryRemoteConstruction_ClientConstruction(sourceConnection, isServer);
	}
	virtual void SerializeConstruction(RakNet::BitStream *constructionBitstream, RakNet::Connection_RM3 *destinationConnection){}
	virtual bool DeserializeConstruction(RakNet::BitStream *constructionBitstream, RakNet::Connection_RM3 *sourceConnection){return true;}
	virtual void SerializeDestruction(RakNet::BitStream *destructionBitstream, RakNet::Connection_RM3 *destinationConnection){}
	virtual bool DeserializeDestruction(RakNet::BitStream *destructionBitstream, RakNet::Connection_RM3 *sourceConnection){return true;}
	virtual RM3ActionOnPopConnection QueryActionOnPopConnection(RakNet::Connection_RM3 *droppedConnection) const
	{
		if (isServer)
			return QueryActionOnPopConnection_Server(droppedConnection);
		else
			return QueryActionOnPopConnection_Client(droppedConnection);
	}
	virtual void DeallocReplica(RakNet::Connection_RM3 *sourceConnection) {delete this;}
	virtual RM3QuerySerializationResult QuerySerialization(RakNet::Connection_RM3 *destinationConnection)
	{
		if (isServer)
			return QuerySerialization_ServerSerializable(destinationConnection, isServer);
		else
			return QuerySerialization_ClientSerializable(destinationConnection, isServer);
	}
	virtual RM3SerializationResult Serialize(RakNet::SerializeParameters *serializeParameters)
	{
		// Autoserialize causes a network packet to go out when any of these member variables change.
		RakAssert(isServer==true);
		serializeParameters->outputBitstream[0].Write(isKernel);
		serializeParameters->outputBitstream[0].WriteAlignedBytes((const unsigned char*)&position,sizeof(position));
		serializeParameters->outputBitstream[0].WriteAlignedBytes((const unsigned char*)&velocity,sizeof(velocity));
		serializeParameters->outputBitstream[0].WriteAlignedBytes((const unsigned char*)&orientation,sizeof(orientation));
		return RM3SR_BROADCAST_IDENTICALLY;
	}	
	virtual void Deserialize(RakNet::DeserializeParameters *deserializeParameters)
	{
		bool lastIsKernel = isKernel;

		// Doing this because we are also lagging position and orientation behind by DEFAULT_SERVER_MILLISECONDS_BETWEEN_UPDATES
		// Without it, the kernel would pop immediately but would not start moving
		deserializeParameters->serializationBitstream[0].Read(isKernel);
		if (isKernel==false && lastIsKernel==true)
		popCountdown=DEFAULT_SERVER_MILLISECONDS_BETWEEN_UPDATES;

		deserializeParameters->serializationBitstream[0].ReadAlignedBytes((unsigned char*)&position,sizeof(position));
		deserializeParameters->serializationBitstream[0].ReadAlignedBytes((unsigned char*)&velocity,sizeof(velocity));
		deserializeParameters->serializationBitstream[0].ReadAlignedBytes((unsigned char*)&orientation,sizeof(orientation));

		// Scene node starts invisible until we deserialize the intial startup data
		// This data could also have been passed in SerializeConstruction()
		sceneNode->setVisible(true,true);

		// Every time we get a network packet, we write it to the transformation history class.
		// This class, given a time in the past, can then return to us an interpolated position of where we should be in at that time
		transformationHistory.Write(position,velocity,orientation,RakNet::GetTimeMS());
	}

	virtual void SetToPopped(void)
	{
		// Change the mesh, and add some velocity.
		isKernel=false;
		if (sceneNode->getAttachedObject(0))
			app->GetSceneManager()->destroyEntity(sceneNode->getAttachedObject(0)->getName());
		sceneNode->detachAllObjects();
		sceneNode->attachObject(popcornPopped->clone(FormatString("%p",this)));
		if (isServer)
		{
			velocity.x=-PLANE_VELOCITY_VARIANCE/2.0f+frandomMT()*PLANE_VELOCITY_VARIANCE;
			velocity.y=UPWARD_VELOCITY_MINIMUM+frandomMT()*UPWARD_VELOCITY_VARIANCE;
			velocity.z=-PLANE_VELOCITY_VARIANCE/2.0f+frandomMT()*PLANE_VELOCITY_VARIANCE;
		}		
	}
	
	virtual void Update(RakNet::TimeMS timeElapsedMs)
	{
		visiblePosition=position;
		visibleOrientation=orientation;

		if (isKernel==false)
		{
			if (isServer)
			{
				// Only the server is doing physics
				float timeElapsedSec = timeElapsedMs * .001f;
				position += velocity * timeElapsedSec + .5f * Ogre::Vector3(0.0f, DOWNWARD_ACCELERATION, 0.0f) * timeElapsedSec*timeElapsedSec;;
				velocity += Ogre::Vector3(0.0f, DOWNWARD_ACCELERATION, 0.0f) * timeElapsedSec;
				orientation = Quaternion::Slerp(timeElapsedSec, orientation, orientation * rotationalVelocity, true);
			}
			else
			{
				// See above - delay the pop until we start moving
				if (popCountdown <= timeElapsedMs)
				{
					SetToPopped();
					popCountdown=-1;
				}
				else
					popCountdown-=timeElapsedMs;

				// interpolate visible position, lagging behind by a small amount so where know where to update to
				if (enableInterpolation)
				{
					// Important: the first 3 parameters are in/out parameters, so set their values to the known current values before calling Read()
					// We are subtracting DEFAULT_SERVER_MILLISECONDS_BETWEEN_UPDATES from the current time to get an interpolated position in the past
					// Without this we wouldn't have a node to interpolate to, and wouldn't know where to go
					transformationHistory.Read(&visiblePosition, 0, &visibleOrientation, RakNet::GetTimeMS()-DEFAULT_SERVER_MILLISECONDS_BETWEEN_UPDATES,RakNet::GetTimeMS());
				}
			}
		}
		else
		{
			if (isServer)
			{
				if (popCountdown <= timeElapsedMs)
				{
					// Only the server controls when to pop
					SetToPopped();
				}
				else
					popCountdown-=timeElapsedMs;
				
			}
			
		}

		sceneNode->setPosition(visiblePosition);
		sceneNode->setOrientation(visibleOrientation);
	}


	static void ClearPopcorn()
	{
		// Destructor removes itself from this list
		while (popcornList.Size())
			delete popcornList[popcornList.Size()-1];
	}
	static Popcorn * CreateKernel(ReplicaManager3 *replicaManager3)
	{
		Popcorn *p = new Popcorn;
		// Tell the replication system about this new Replica instance.
		replicaManager3->Reference(p);
		static int count=0;
		count++;
		popcornList.Insert(p, _FILE_AND_LINE_ );
		p->sceneNode = app->GetSceneManager()->getRootSceneNode()->createChildSceneNode();
		p->sceneNode->attachObject(popcornKernel->clone(FormatString("%p",p)));

		// Only server sets up initial positions, etc.
		if (isServer)
		{
			p->position.x=-POSITION_VARIANCE/2.0f+frandomMT()*POSITION_VARIANCE;
			p->position.y=0.0f;
			p->position.z=-POSITION_VARIANCE/2.0f+frandomMT()*POSITION_VARIANCE;
			p->velocity=Ogre::Vector3::ZERO;
			p->popCountdown=POP_COUNTDOWN_MIN_DELAY_MS + randomMT() % POP_COUNTDOWN_VARIANCE_MS;
			p->orientation.FromAngleAxis(Ogre::Radian(frandomMT()*6.28f), Ogre::Vector3(-1.0f+frandomMT()*2.0f,-1.0f+frandomMT()*2.0f,-1.0f+frandomMT()*2.0f).normalisedCopy());
			p->rotationalVelocity.FromAngleAxis(Ogre::Radian(frandomMT()*6.28f), Ogre::Vector3(-1.0f+frandomMT()*2.0f,-1.0f+frandomMT()*2.0f,-1.0f+frandomMT()*2.0f).normalisedCopy());
			p->visiblePosition=p->position;
			p->visibleOrientation=p->orientation;
		}
		else
			p->sceneNode->setVisible(false,true);


		return p;
	}
};

// One instance of Connection_RM2 is implicitly created per connection that uses ReplicaManager2. The most important function to implement is Construct() as this creates your game objects.
// It is designed this way so you can override per-connection behavior in your own game classes
class PopcornSampleConnection : public Connection_RM3
{
public:
	PopcornSampleConnection(const SystemAddress &_systemAddress, RakNetGUID _guid) : Connection_RM3(_systemAddress,_guid) {}
	virtual ~PopcornSampleConnection() {}

	// Callback used to create objects
	// See Connection_RM2::Construct in ReplicaManager2.h for a full explanation of each parameter
	virtual Replica3 *AllocReplica(RakNet::BitStream *allocationIdBitstream, ReplicaManager3 *replicaManager3)
	{
		char objectName[128];
		StringTable::Instance()->DecodeString(objectName,128,allocationIdBitstream);
		if (strcmp(objectName,"Popcorn")==0)
		{
			return Popcorn::CreateKernel(replicaManager3);
		}
		return 0;
	}
};
class PopcornDemoRM3 : public ReplicaManager3
{
	virtual Connection_RM3* AllocConnection(const SystemAddress &systemAddress, RakNetGUID rakNetGUID) const {return new PopcornSampleConnection(systemAddress,rakNetGUID);}
	virtual void DeallocConnection(Connection_RM3 *connection) const {delete connection;}
};
PopcornDemoRM3 replicaManager3;
class ExampleApp : public App3D
{
public:
	ExampleApp()
	{
		quit=false;
	}

	~ExampleApp()
	{
	}

	void OnAppShutdown( void )
	{
		Popcorn::ClearPopcorn();

		App3D::OnAppShutdown();

		if( mInputManager )
		{
			mInputManager->destroyInputObject( mKeyboard );
			OIS::InputManager::destroyInputSystem(mInputManager);
			mInputManager = 0;
		}

		RakNet::RakPeerInterface::DestroyInstance(rakPeer);

	}

	void Render(AppTime curTimeMS)
	{
		App3D::Render(curTimeMS);
	}

	bool ShouldQuit(void) const {return quit || window->isClosed();}

	void Update(AppTime curTimeMS, AppTime elapsedTimeMS)
	{
#ifndef WIN32
		WindowEventUtilities::messagePump();
#endif

		// Update all subsystems
		App3D::Update(curTimeMS, elapsedTimeMS);
		overlayHelper.Update(elapsedTimeMS);

		mKeyboard->capture();

		Ogre::Vector3 mTranslateVector = Ogre::Vector3::ZERO;

		float mMoveSpeed=50.0f;
		float mRotateSpeed=1.0f;
		float mMoveScale = mMoveSpeed * elapsedTimeMS * .001f;
		// Take about 10 seconds for full rotation
		Ogre::Radian mRotScale(mRotateSpeed * elapsedTimeMS * .001f);

		if(mKeyboard->isKeyDown(KC_A))
			mTranslateVector.x = -mMoveScale;	// Move camera left

		if(mKeyboard->isKeyDown(KC_D))
			mTranslateVector.x = mMoveScale;	// Move camera RIGHT

		if(mKeyboard->isKeyDown(KC_UP) || mKeyboard->isKeyDown(KC_W) )
			mTranslateVector.z = -mMoveScale;	// Move camera forward

		if(mKeyboard->isKeyDown(KC_DOWN) || mKeyboard->isKeyDown(KC_S) )
			mTranslateVector.z = mMoveScale;	// Move camera backward

		if(mKeyboard->isKeyDown(KC_PGUP))
			mTranslateVector.y = mMoveScale;	// Move camera up

		if(mKeyboard->isKeyDown(KC_PGDOWN))
			mTranslateVector.y = -mMoveScale;	// Move camera down

		if(mKeyboard->isKeyDown(KC_RIGHT))
			camera->yaw(-mRotScale);

		if(mKeyboard->isKeyDown(KC_LEFT))
			camera->yaw(mRotScale);

		// Hold down space to see what it looks like without interpolation
		if(mKeyboard->isKeyDown(KC_SPACE))
			enableInterpolation=false;
		else
			enableInterpolation=true;

		static bool didUpdateDelayLastTick=false;
		bool didUpdateDelayThisTick=false;
		
		if (isStarted==false)
		{
			RakNet::SocketDescriptor sd;

			if(mKeyboard->isKeyDown(KC_S))
			{
				// Start server
				isStarted=true;
				isServer=true;
				ShowMessage("Server started");
				sd.port=SERVER_PORT;

			}

			if(mKeyboard->isKeyDown(KC_C))
			{
				isStarted=true;
				ShowMessage(FormatString("Client started, connecting to %s", SERVER_IP_ADDRESS));
				// Start server
				isStarted=true;
				isServer=false;
				sd.port=0;
			}

			if (isStarted)
			{
				// Start RakNet, up to 32 connections if the server
				rakPeer = RakNet::RakPeerInterface::GetInstance();
				StartupResult sr = rakPeer->Startup(isServer ? 32 : 1,&sd,1);
				RakAssert(sr==RAKNET_STARTED);
				rakPeer->AttachPlugin(&replicaManager3);
				replicaManager3.SetNetworkIDManager(&networkIdManager);
				//rakPeer->SetNetworkIDManager(&networkIdManager);
				// The server should allow systems to connect. Clients do not need to unless you want to use RakVoice or for some other reason want to transmit directly between systems.
				if (isServer)
				{
					rakPeer->SetMaximumIncomingConnections(32);
				}
				else
				{
					ConnectionAttemptResult car = rakPeer->Connect(SERVER_IP_ADDRESS,SERVER_PORT,0,0);
					RakAssert(car==CONNECTION_ATTEMPT_STARTED);

				}
				replicaManager3.SetAutoSerializeInterval(DEFAULT_SERVER_MILLISECONDS_BETWEEN_UPDATES);

				// StringTable has to be called after RakPeer started, or else first call StringTable::AddRef() yourself
				StringTable::Instance()->AddString("Popcorn",false);
			}
		}

		if (isStarted)
		{
			RakNet::Packet *packet;
			for (packet = rakPeer->Receive(); packet; rakPeer->DeallocatePacket(packet), packet = rakPeer->Receive())
			{
				switch (packet->data[0])
				{
				case ID_CONNECTION_ATTEMPT_FAILED:
					ShowMessage("ID_CONNECTION_ATTEMPT_FAILED\n");
					break;
				case ID_NO_FREE_INCOMING_CONNECTIONS:
					ShowMessage("ID_NO_FREE_INCOMING_CONNECTIONS\n");
					break;
				case ID_CONNECTION_REQUEST_ACCEPTED:
					ShowMessage("ID_CONNECTION_REQUEST_ACCEPTED\n");
					break;
				case ID_NEW_INCOMING_CONNECTION:
					ShowMessage(FormatString("ID_NEW_INCOMING_CONNECTION from %s\n", packet->systemAddress.ToString()));
					break;
				case ID_DISCONNECTION_NOTIFICATION:
					ShowMessage("ID_DISCONNECTION_NOTIFICATION\n");
					break;
				case ID_CONNECTION_LOST:
					ShowMessage("ID_CONNECTION_LOST\n");
					break;
				}
			}

			if (isServer)
			{
				// Restart the demo every RESTART_TIMER_MS milliseconds
				if (popcornLifetimeCountdown<=elapsedTimeMS)
				{
					Popcorn::ClearPopcorn();
					CreateKernels(&replicaManager3);
					popcornLifetimeCountdown=RESTART_TIMER_MS;
				}
				popcornLifetimeCountdown-=elapsedTimeMS;
			}


			unsigned i;
			for (i=0; i < popcornList.Size(); i++)
			{
				popcornList[i]->Update(elapsedTimeMS);
			}
		}
		
		camera->moveRelative(mTranslateVector);

		if( mKeyboard->isKeyDown(KC_ESCAPE) || mKeyboard->isKeyDown(KC_Q) )
			quit=true;
	}

	// Just Ogre startup stuff
	virtual void PostConfigure(const char *defaultResourceConfigurationPath, bool recursive)
	{
		App3D::PostConfigure(defaultResourceConfigurationPath, false);
		App3D::InitSceneManager(0);
		App3D::InitGUIManager();
		App3D::InitCamera(0);
		App3D::InitViewport(0);

		ParamList pl;
		size_t windowHnd = 0;
		std::ostringstream windowHndStr;

		window->getCustomAttribute("WINDOW", &windowHnd);
		windowHndStr << windowHnd;
		pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));

		mInputManager = InputManager::createInputSystem( pl );
		//Create all devices (We only catch joystick exceptions here, as, most people have Key/Mouse)
		mKeyboard = static_cast<Keyboard*>(mInputManager->createInputObject( OISKeyboard, false ));

		// Start the overlay helper class, which handles fading of overlays and other stuff
		overlayHelper.Startup();

		sceneManager->setAmbientLight( ColourValue( .5, .5, .5 ) );

		mainLight = sceneManager->createLight("MainLight");
		mainLightNode = sceneManager->getRootSceneNode()->createChildSceneNode( "MainLightNode" );
		mainLightNode->attachObject(mainLight);
		mainLight->setType(Light::LT_POINT);
		mainLight->setPosition(200.0f, 200.0f, 200.0f);
		
		camera->setPosition(150.0f, 150.0f, 70.0f);
		camera->lookAt(0.0f,50.0f,0.0f);
		camera->setNearClipDistance(1.0f);

		popcornKernel = sceneManager->createEntity("PopcornKernel", "PopcornKernel.mesh");
		popcornPopped = sceneManager->createEntity("PopcornPopped", "PopcornPopped.mesh");
		popcornLifetimeCountdown=0;

		sceneManager->setSkyBox(true, "Examples/SteveCubeSkyBox");
		isStarted=false;
		enableInterpolation=true;


		// Bug: Since updating to ogre OgreSDK_vc8_v1-7-1 from Ogre 3D 1.6.2, the first call to ShowMessage doesn't show up anymore
		ShowMessage("'S'erver. 'C'lient. Hold ' ' to disable interp.");
		ShowMessage("'S'erver. 'C'lient. Hold ' ' to disable interp.");
	}

	virtual void CreateKernels(ReplicaManager3 *replicaManager3)
	{
		RakAssert(isServer);
		unsigned int kernelCount;
		if (KERNELS_VARIANCE!=0)
			kernelCount = MIN_KERNELS + randomMT() % KERNELS_VARIANCE;
		else
			kernelCount = MIN_KERNELS;
		for (unsigned int i=0; i < kernelCount; i++)
			Popcorn::CreateKernel(replicaManager3);

	}

protected:
	virtual char * GetWindowTitle(void) const {return (char *)"Popcorn popper";}
	void ShowMessage(const char *msg, float timescale=1.0f)
	{
		// Create a panel
		static int count=0;
		OverlayContainer* panel =overlayHelper.CreatePanel(FormatString("%i",count++));
		panel->setMetricsMode(Ogre::GMM_PIXELS);
		panel->setPosition(10, 10);
		panel->setDimensions(100, 100);
		//panel->setMaterialName("MaterialName"); // Optional background material

		// Create a text area
		TextAreaOverlayElement *textArea = overlayHelper.CreateTextArea(FormatString("%i",count++), "BlueHighway", panel);
		textArea->setMetricsMode(Ogre::GMM_PIXELS);
		textArea->setPosition(10, 10);
		textArea->setDimensions(200, 200);
		textArea->setCaption(msg);
		textArea->setCharHeight(32);
		textArea->setColourBottom(ColourValue(0.3, 0.5, 0.3));
		textArea->setColourTop(ColourValue(0.5, 0.7, 0.5));

		// Destroy the children (the text area) before destroying the parents.
		overlayHelper.FadeOverlayElement(textArea, 3000*timescale, 1000*timescale, 0.0f, true);
		overlayHelper.FadeOverlayElement(panel, 3000*timescale, 1000*timescale, 0.0f, true);
	}

	// Our major systems.  Note the base class ExampleApplication has all the Ogre 3D systems
	OverlayHelper overlayHelper;
	bool quit;

	SceneNode *mainLightNode;
	Light *mainLight;

	OIS::InputManager* mInputManager;
	OIS::Keyboard* mKeyboard;

	NetworkIDManager networkIdManager;
	bool isStarted;
	RakNet::TimeMS popcornLifetimeCountdown;
};


#ifdef WIN32
int APIENTRY WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#else
int main (int argc, char** argv)
#endif
{
	
	HWND     hWnd;
	RakNet::TimeMS curTime, lastTime, elapsed;
	app = new ExampleApp;
	app->PreConfigure();
	if (app->Configure()==false)
	{
		delete app;
		return 0;
	}
#ifdef WIN32
	app->window->getCustomAttribute("HWND", &hWnd);
	MSG msg;
#else
	app->window->getCustomAttribute("GLXWINDOW", &hWnd);
#endif

	app->PostConfigure("resources.cfg",false);
	lastTime=RakNet::GetTimeMS();

	while (app->ShouldQuit()==false)
	{
		curTime=RakNet::GetTimeMS();
		elapsed = curTime-lastTime;
		if (elapsed > 100)
			elapsed=100; // Spike limiter
		app->Update(curTime, elapsed);
		lastTime=curTime;
		app->Render(curTime);
#ifdef WIN32
		if (PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE )>0)
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
#endif
		// Make sure the RakNet thread runs
		RakSleep(0);
	}

	app->OnAppShutdown();

	delete app;

	return 0;
}
