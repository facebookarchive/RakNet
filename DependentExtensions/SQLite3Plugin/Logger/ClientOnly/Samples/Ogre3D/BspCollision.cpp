/*
-----------------------------------------------------------------------------
This source file is part of OGRE
    (Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2006 Torus Knot Software Ltd
Also see acknowledgements in Readme.html

You may use this sample code for anything you like, it is not covered by the
LGPL like the rest of the engine.
-----------------------------------------------------------------------------
*/

/*
-----------------------------------------------------------------------------
Filename:    BspCollision.cpp
Description: Somewhere to play in the sand...
-----------------------------------------------------------------------------
*/

#include "OgreReferenceAppLayer.h"

#include "ExampleRefAppApplication.h"
#include "OgreStringConverter.h"

// Hacky globals
ApplicationObject *ball;

SceneNode* targetNode;
RaySceneQuery* rsq = 0;
static const int num_rows = 3;

// RakNet: Logger includes.
#include "SQLiteClientLoggerPlugin.h"
#include "PacketizedTCP.h"
#include "Ogre3D_DX9_BackbufferGrabber.h"
#include "RakNetTime.h"
#include "GetTime.h"

// Event handler to add ability to alter curvature
class BspCollisionListener : public ExampleRefAppFrameListener
{
protected:
	// RakNet: For logging video
	PacketizedTCP packetizedTCP;
	RakNet::SQLiteClientLoggerPlugin loggerPlugin;
	Ogre3D_DX9_BackbufferGrabber backbufferGrabber;
	RakNet::TimeMS lastScreenshotTime;

	// Also save the world * so we can log it out
	World* mWorld;

public:
    BspCollisionListener(RenderWindow* win, CollideCamera* cam, World* world)
        : ExampleRefAppFrameListener(win, cam)
    {
		// RakNet:  Connect to server using TCP, for logging video
		packetizedTCP.AttachPlugin(&loggerPlugin);
		packetizedTCP.Start(0,0);
		loggerPlugin.SetServerParameters(packetizedTCP.Connect("127.0.0.1", 38123, true), "ogrevideo.sqlite");
		// For testing, I'm using 512x512 with a huge memory constraint at 30 FPS
		// For a real game, you probably want to limit this to 256x256, with a 8MB memory constraint, at 15-20 FPS
		loggerPlugin.SetMemoryConstraint(128000000);
		backbufferGrabber.InitBackbufferGrabber(mWindow, 512, 512);
		lastScreenshotTime=0;

		mWorld=world;
    }


    bool frameEnded(const FrameEvent& evt)
    {
        // local just to stop toggles flipping too fast
        static Real timeUntilNextToggle = 0;

        // Deal with time delays that are too large
        // If we exceed this limit, we ignore
        static const Real MAX_TIME_INCREMENT = 0.5f;
        if (evt.timeSinceLastEvent > MAX_TIME_INCREMENT)
        {
            return true;
        }
        
        if (timeUntilNextToggle >= 0) 
            timeUntilNextToggle -= evt.timeSinceLastFrame;

        // Call superclass
        bool ret = ExampleRefAppFrameListener::frameEnded(evt);        

		if (mKeyboard->isKeyDown(OIS::KC_SPACE) && timeUntilNextToggle <= 0)
        {
            timeUntilNextToggle = 2;
            ball->setPosition(mCamera->getPosition() + 
                mCamera->getDirection() * mCamera->getNearClipDistance() * 2);
            ball->setLinearVelocity(mCamera->getDirection() * 200);
            ball->setAngularVelocity(Vector3::ZERO);

			// RakNet: Log events, which in this case is only firing the ball. Give the event a color so we can plot it
			rakSqlLog("EventData", "x,y,z,name,color",
				(mCamera->getPosition().x, mCamera->getPosition().y, mCamera->getPosition().z, "Fired Ball", "green"));
        }

        // Move the targeter
        rsq->setRay(mCamera->getRealCamera()->getCameraToViewportRay(0.5, 0.5));
        RaySceneQueryResult& rsqResult = rsq->execute();
        RaySceneQueryResult::iterator ri = rsqResult.begin();
        if (ri != rsqResult.end())
        {
            RaySceneQueryResultEntry& res = *ri;
            targetNode->setPosition(rsq->getRay().getPoint(res.distance));
        }

		// RakNet: Send screenshot and FPS info to server if connected, at most once every 30 milliseconds
		// This is constrained so we don't overflow the server with screenshots
		// Also only do it if we connected to the server
		RakNet::TimeMS timeSinceLastLog=RakNet::GetTimeMS()-lastScreenshotTime;
		if (packetizedTCP.GetConnectionCount()>0 && timeSinceLastLog>30)
		{
			RakNet::RGBImageBlob blob;
			backbufferGrabber.LockBackbufferCopy(&blob);
			RakAssert(blob.data!=0);
			// RakNet: Log frame data, including screenshot and FPS
			RakNet::SQLLogResult logResult = rakSqlLog("FrameData", "screenshot,averageFPS,lastFPS,bestFPS,worstFPS,numTris,DebugText",
				( &blob,mWindow->getAverageFPS(),mWindow->getLastFPS(),mWindow->getBestFPS(),mWindow->getWorstFPS(),(int) mWindow->getTriangleCount(),mDebugText.c_str() ));
			// Release backbuffer as soon as possible, after sending frame data
			backbufferGrabber.ReleaseBackbufferCopy();
			if ( logResult==RakNet::SQLLR_WOULD_EXCEED_MEMORY_CONSTRAINT )
			{
				/// Sending too large of screenshots, or can't transfer data fast enough. See loggerPlugin.SetMemoryConstraint
			}

			// Also log out position of all world objects
			Entity *entity;
			SceneNode *sceneNode;
			entity = mWorld->getSceneManager()->getEntity("ball");
			sceneNode = entity->getParentSceneNode();
			// RakNet: Log object position data over time
			rakSqlLog("ObjectData", "x,y,z,name,color",
				(sceneNode->getPosition().x, sceneNode->getPosition().y, sceneNode->getPosition().z, entity->getName().c_str(), "blue"));
			for (int row = 0; row < num_rows; ++row)
			{
				for (int i = 0; i < (num_rows-row); ++i)
				{
					String name = "box";
					name += StringConverter::toString((row*num_rows) + i);
					entity = mWorld->getSceneManager()->getEntity(name);
					sceneNode = entity->getParentSceneNode();
					rakSqlLog("ObjectData", "x,y,z,name,color",
						(sceneNode->getPosition().x, sceneNode->getPosition().y, sceneNode->getPosition().z, entity->getName().c_str(), "red"));
				}
			}

			lastScreenshotTime=RakNet::GetTimeMS();
		}

        return ret;
    }
};

class BspCollisionApplication : public ExampleRefAppApplication
{
public:
    BspCollisionApplication() {
    }

    ~BspCollisionApplication() 
    {
		delete rsq;
    }

protected:
    
    void chooseSceneManager(void)
    {
        mSceneMgr = mRoot->createSceneManager("BspSceneManager");
    }
    void createWorld(void)
    {
        // Create BSP-specific world
        mWorld = new World(mSceneMgr, World::WT_REFAPP_BSP);
    }
    void createScene(void)
    {
        mSceneMgr->setShadowTechnique(SHADOWTYPE_STENCIL_MODULATIVE);
        // Set ambient light
        mSceneMgr->setAmbientLight(ColourValue(0.2, 0.2, 0.2));
        // Create a point light
        Light* l = mSceneMgr->createLight("MainLight");
        l->setPosition(-100,50,100);
        l->setAttenuation(8000,1,0,0);


        // Setup World
        mWorld->setGravity(Vector3(0, 0, -60));
        mWorld->getSceneManager()->setWorldGeometry("ogretestmap.bsp");

        // modify camera for close work
        mCamera->setNearClipDistance(10);
        mCamera->setFarClipDistance(20000);

        // Also change position, and set Quake-type orientation
        // Get random player start point
        ViewPoint vp = mSceneMgr->getSuggestedViewpoint(true);
        mCamera->setPosition(vp.position);
        mCamera->pitch(Degree(90)); // Quake uses X/Y horizon, Z up
        mCamera->rotate(vp.orientation);
        // Don't yaw along variable axis, causes leaning
        mCamera->setFixedYawAxis(true, Vector3::UNIT_Z);
        // Look at the boxes
		mCamera->lookAt(-150,40,30);

        ball = mWorld->createBall("ball", 7, vp.position + Vector3(0,0,80));
        ball->setDynamicsEnabled(true);
        ball->getEntity()->setMaterialName("Ogre/Eyes");

		OgreRefApp::Box* box = mWorld->createBox("shelf", 75, 125, 5, Vector3(-150, 40, 30));
        box->getEntity()->setMaterialName("Examples/Rocky");

        static const Real BOX_SIZE = 15.0f;

        for (int row = 0; row < num_rows; ++row)
        {
            for (int i = 0; i < (num_rows-row); ++i)
            {
                Real row_size = (num_rows - row) * BOX_SIZE * 1.25;
                String name = "box";
                name += StringConverter::toString((row*num_rows) + i);
                box = mWorld->createBox(name, BOX_SIZE,BOX_SIZE,BOX_SIZE , 
                    Vector3(-150, 
                        40 - (row_size * 0.5) + (i * BOX_SIZE * 1.25) , 
                        32.5 + (BOX_SIZE / 2) + (row * BOX_SIZE)));
                box->setDynamicsEnabled(false, true);
                box->getEntity()->setMaterialName("Examples/10PointBlock");
            }
        }
        mCamera->setCollisionEnabled(false);
        mCamera->getRealCamera()->setQueryFlags(0);

        // Create the targeting sphere
        Entity* targetEnt = mSceneMgr->createEntity("testray", "sphere.mesh");
        MaterialPtr mat = MaterialManager::getSingleton().create("targeter", 
            ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        Pass* pass = mat->getTechnique(0)->getPass(0);
        TextureUnitState* tex = pass->createTextureUnitState();
        tex->setColourOperationEx(LBX_SOURCE1, LBS_MANUAL, LBS_CURRENT, 
            ColourValue::Red);
        pass->setLightingEnabled(false);
        pass->setSceneBlending(SBT_ADD);
        pass->setDepthWriteEnabled(false);


        targetEnt->setMaterialName("targeter");
        targetEnt->setCastShadows(false);
        targetEnt->setQueryFlags(0);
        targetNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
        targetNode->scale(0.025, 0.025, 0.025);
        targetNode->attachObject(targetEnt);

        rsq = mSceneMgr->createRayQuery(Ray());
        rsq->setSortByDistance(true, 1);
        rsq->setWorldFragmentType(SceneQuery::WFT_SINGLE_INTERSECTION);
    }
    // Create new frame listener
    void createFrameListener(void)
    {
        mFrameListener= new BspCollisionListener(mWindow, mCamera, mWorld);
        mRoot->addFrameListener(mFrameListener);
    }

public:
};



#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"


INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
#else
int main(int argc, char **argv)
#endif
{
    // Create application object
    BspCollisionApplication app;

    try {
        app.go();
    } catch( Exception& e ) {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
        MessageBox( NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
        std::cerr << "An exception has occured: " << e.getFullDescription();
#endif
    }


    return 0;
}







