// This is a Demo of the Irrlicht Engine (c) 2005-2009 by N.Gebhardt.
// This file is not documented.

#include "CDemo.h"

// RakNet includes
#include "GetTime.h"
#include "MessageIdentifiers.h"
#include "RakNetTypes.h"
#include "Itoa.h"
#include "RakNetSmartPtr.h"
#include "miniupnpc.h"
#include "upnpcommands.h"
#include "upnperrors.h"

CDemo::CDemo(bool f, bool m, bool s, bool a, bool v, bool fsaa, video::E_DRIVER_TYPE d, core::stringw &_playerName)
: fullscreen(f), music(m), shadows(s), additive(a), vsync(v), aa(fsaa),
 driverType(d), device(0), playerName(_playerName),
#ifdef USE_IRRKLANG
	irrKlang(0), ballSound(0), impactSound(0),
#endif
#ifdef USE_SDL_MIXER
	stream(0), ballSound(0), impactSound(0),
#endif
 currentScene(-2), backColor(0), statusText(0), inOutFader(0),
 quakeLevelMesh(0), quakeLevelNode(0), skyboxNode(0), model1(0), model2(0),
 campFire(0), metaSelector(0), mapSelector(0), sceneStartTime(0),
 timeForThisScene(0), whenOutputMessageStarted(0), isConnectedToNATPunchthroughServer(false)
{
	for (u32 i=0; i<KEY_KEY_CODES_COUNT; ++i)
		KeyIsDown[i] = false;
}


CDemo::~CDemo()
{
	if (mapSelector)
		mapSelector->drop();

	if (metaSelector)
		metaSelector->drop();

#ifdef USE_IRRKLANG
	if (irrKlang)
		irrKlang->drop();
#endif
}


void CDemo::run()
{
	core::dimension2d<u32> resolution ( 800, 600 );

	if ( driverType == video::EDT_BURNINGSVIDEO || driverType == video::EDT_SOFTWARE )
	{
		resolution.Width = 640;
		resolution.Height = 480;
	}

	irr::SIrrlichtCreationParameters params;
	params.DriverType=driverType;
	params.WindowSize=resolution;
	params.Bits=32;
	params.Fullscreen=fullscreen;
	params.Stencilbuffer=shadows;
	params.Vsync=vsync;
	params.AntiAlias=aa;
	params.EventReceiver=this;

	device = createDeviceEx(params);
	if (!device)
		return;

	if (device->getFileSystem()->existFile("irrlicht.dat"))
		device->getFileSystem()->addFileArchive("irrlicht.dat", true, true, io::EFAT_ZIP);
	else
		device->getFileSystem()->addFileArchive(IRRLICHT_MEDIA_PATH "irrlicht.dat", true, true, io::EFAT_ZIP);
	if (device->getFileSystem()->existFile("map-20kdm2.pk3"))
		device->getFileSystem()->addFileArchive("map-20kdm2.pk3", true, true, io::EFAT_ZIP);
	else
		device->getFileSystem()->addFileArchive(IRRLICHT_MEDIA_PATH "map-20kdm2.pk3", true, true, io::EFAT_ZIP);

	video::IVideoDriver* driver = device->getVideoDriver();
	scene::ISceneManager* smgr = device->getSceneManager();
	gui::IGUIEnvironment* guienv = device->getGUIEnvironment();

	device->setWindowCaption(L"Irrlicht Engine Demo");

	// set ambient light
	smgr->setAmbientLight ( video::SColorf ( 0x00c0c0c0 ) );


//	wchar_t tmp[255];

	// RakNet startup
	char dest[1024];
	memset(dest,0,sizeof(dest));
	wcstombs(dest, playerName.c_str(), playerName.size());
	InstantiateRakNetClasses();

	// Hook RakNet stuff into this class
	playerReplica->playerName=RakNet::RakString(dest);
	playerReplica->demo=this;
	replicaManager3->demo=this;

	CalculateSyndeyBoundingBox();

	// draw everything

	s32 now = 0;
	sceneStartTime = device->getTimer()->getTime();
	while(device->run() && driver)
	{
		// RakNet: Render even if not active, multiplayer never stops
		//if (device->isWindowActive())
		{
#ifdef USE_IRRKLANG
			// update 3D position for sound engine
			scene::ICameraSceneNode* cam = smgr->getActiveCamera();
			if (cam && irrKlang)
				irrKlang->setListenerPosition(cam->getAbsolutePosition(), cam->getTarget());
#endif

			// load next scene if necessary
			now = device->getTimer()->getTime();
			if (now - sceneStartTime > timeForThisScene && timeForThisScene!=-1)
				switchToNextScene();

			createParticleImpacts();

			driver->beginScene(timeForThisScene != -1, true, backColor);

			smgr->drawAll();
			guienv->drawAll();

			driver->endScene();

			/*
			// write statistics
			static s32 lastfps = 0;
			s32 nowfps = driver->getFPS();

			swprintf(tmp, 255, L"%ls fps:%3d triangles:%0.3f mio",
				driver->getName(),
				driver->getFPS(),
				(f32) driver->getPrimitiveCountDrawn( 1 ) * ( 1.f / 1000000.f )
				);
			if ( nowfps != lastfps )
			{
				device->setWindowCaption ( tmp );
				lastfps = nowfps;
			}
*/
			RakNet::RakString curMsg = GetCurrentMessage();
			if (curMsg.IsEmpty()==false)
			{
				wchar_t dest[1024];
				memset(dest,0,sizeof(dest));
				mbstowcs(dest, curMsg.C_String(), curMsg.GetLength());
				statusText->setText(dest);
			}
			else
			{
		//		statusText->setText(tmp);
				statusText->setText(0);
			}
		}

		// RakNet per tick update
		UpdateRakNet();
	}

	// RakNet shutdown
	DeinitializeRakNetClasses();

	device->drop();
}


bool CDemo::OnEvent(const SEvent& event)
{
	if (!device)
		return false;

	// Remember whether each key is down or up
	if (event.EventType == irr::EET_KEY_INPUT_EVENT)
		KeyIsDown[event.KeyInput.Key] = event.KeyInput.PressedDown;

	if (event.EventType == EET_KEY_INPUT_EVENT &&
		event.KeyInput.Key == KEY_ESCAPE &&
		event.KeyInput.PressedDown == false)
	{
		// user wants to quit.
//		if (currentScene < 3)
//			timeForThisScene = 0;
//		else
			//device->closeDevice();

		// RakNet: Escape to get the mouse back
		if (GetSceneManager()->getActiveCamera()->isVisible())
		{
			device->getCursorControl()->setVisible(true);
			GetSceneManager()->getActiveCamera()->setVisible(false);
		}
		else
		{
			device->closeDevice();
		}
	}
	else
	if (
		// RakNet: Use space to jump, not shoot
//		(event.EventType == EET_KEY_INPUT_EVENT &&
//		event.KeyInput.Key == KEY_SPACE &&
//		event.KeyInput.PressedDown == false) ||
		(event.EventType == EET_MOUSE_INPUT_EVENT &&
		event.MouseInput.Event == EMIE_LMOUSE_LEFT_UP) &&
		//currentScene == 3
		currentScene == 1
		)
	{

		// RakNet: Click without focus to get focus back
		if (GetSceneManager()->getActiveCamera()->isVisible()==false)
		{
			device->getCursorControl()->setVisible(false);
			GetSceneManager()->getActiveCamera()->setVisible(true);
		}
		else
		{
			// shoot
			shoot();
		}
	}
	else
	if (event.EventType == EET_KEY_INPUT_EVENT &&
		event.KeyInput.Key == KEY_F9 &&
		event.KeyInput.PressedDown == false)
	{
		video::IImage* image = device->getVideoDriver()->createScreenShot();
		if (image)
		{
			device->getVideoDriver()->writeImageToFile(image, "screenshot.bmp");
			device->getVideoDriver()->writeImageToFile(image, "screenshot.png");
			device->getVideoDriver()->writeImageToFile(image, "screenshot.tga");
			device->getVideoDriver()->writeImageToFile(image, "screenshot.ppm");
			device->getVideoDriver()->writeImageToFile(image, "screenshot.jpg");
			device->getVideoDriver()->writeImageToFile(image, "screenshot.pcx");
			image->drop();
		}
	}
	else
	if (device->getSceneManager()->getActiveCamera())
	{
		device->getSceneManager()->getActiveCamera()->OnEvent(event);
		return true;
	}

	return false;
}


void CDemo::switchToNextScene()
{
	currentScene++;
	//if (currentScene > 3)
	if (currentScene > 1)
		currentScene = 1;

	scene::ISceneManager* sm = device->getSceneManager();
	scene::ISceneNodeAnimator* sa = 0;
	scene::ICameraSceneNode* camera = 0;

	camera = sm->getActiveCamera();
	if (camera)
	{
		sm->setActiveCamera(0);
		camera->remove();
		camera = 0;
	}

	switch(currentScene)
	{
	case -1: // loading screen
		timeForThisScene = 0;
		createLoadingScreen();
		break;

	case 0: // load scene
		timeForThisScene = 0;
		loadSceneData();
		break;
/*
	case 1: // panorama camera
		{
			currentScene += 1;
			//camera = sm->addCameraSceneNode(0, core::vector3df(0,0,0), core::vector3df(-586,708,52));
			//camera->setTarget(core::vector3df(0,400,0));

			core::array<core::vector3df> points;

			points.push_back(core::vector3df(-931.473755f, 138.300003f, 987.279114f)); // -49873
			points.push_back(core::vector3df(-847.902222f, 136.757553f, 915.792725f)); // -50559
			points.push_back(core::vector3df(-748.680420f, 152.254501f, 826.418945f)); // -51964
			points.push_back(core::vector3df(-708.428406f, 213.569580f, 784.466675f)); // -53251
			points.push_back(core::vector3df(-686.217651f, 288.141174f, 762.965576f)); // -54015
			points.push_back(core::vector3df(-679.685059f, 365.095612f, 756.551453f)); // -54733
			points.push_back(core::vector3df(-671.317871f, 447.360107f, 749.394592f)); // -55588
			points.push_back(core::vector3df(-669.468445f, 583.335632f, 747.711853f)); // -56178
			points.push_back(core::vector3df(-667.611267f, 727.313232f, 746.018250f)); // -56757
			points.push_back(core::vector3df(-665.853210f, 862.791931f, 744.436096f)); // -57859
			points.push_back(core::vector3df(-642.649597f, 1026.047607f, 724.259827f)); // -59705
			points.push_back(core::vector3df(-517.793884f, 838.396790f, 490.326050f)); // -60983
			points.push_back(core::vector3df(-474.387299f, 715.691467f, 344.639984f)); // -61629
			points.push_back(core::vector3df(-444.600250f, 601.155701f, 180.938095f)); // -62319
			points.push_back(core::vector3df(-414.808899f, 479.691406f, 4.866660f)); // -63048
			points.push_back(core::vector3df(-410.418945f, 429.642242f, -134.332687f)); // -63757
			points.push_back(core::vector3df(-399.837585f, 411.498383f, -349.350983f)); // -64418
			points.push_back(core::vector3df(-390.756653f, 403.970093f, -524.454407f)); // -65005
			points.push_back(core::vector3df(-334.864227f, 350.065491f, -732.397400f)); // -65701
			points.push_back(core::vector3df(-195.253387f, 349.577209f, -812.475891f)); // -66335
			points.push_back(core::vector3df(16.255573f, 363.743134f, -833.800415f)); // -67170
			points.push_back(core::vector3df(234.940964f, 352.957825f, -820.150696f)); // -67939
			points.push_back(core::vector3df(436.797668f, 349.236450f, -816.914185f)); // -68596
			points.push_back(core::vector3df(575.236206f, 356.244812f, -719.788513f)); // -69166
			points.push_back(core::vector3df(594.131042f, 387.173828f, -609.675598f)); // -69744
			points.push_back(core::vector3df(617.615234f, 412.002899f, -326.174072f)); // -70640
			points.push_back(core::vector3df(606.456848f, 403.221954f, -104.179291f)); // -71390
			points.push_back(core::vector3df(610.958252f, 407.037750f, 117.209778f)); // -72085
			points.push_back(core::vector3df(597.956909f, 395.167877f, 345.942200f)); // -72817
			points.push_back(core::vector3df(587.383118f, 391.444519f, 566.098633f)); // -73477
			points.push_back(core::vector3df(559.572449f, 371.991333f, 777.689453f)); // -74124
			points.push_back(core::vector3df(423.753204f, 329.990051f, 925.859741f)); // -74941
			points.push_back(core::vector3df(247.520050f, 252.818954f, 935.311829f)); // -75651
			points.push_back(core::vector3df(114.756012f, 199.799759f, 805.014160f));
			points.push_back(core::vector3df(96.783348f, 181.639481f, 648.188110f));
			points.push_back(core::vector3df(97.865623f, 138.905975f, 484.812561f));
			points.push_back(core::vector3df(99.612457f, 102.463669f, 347.603210f));
			points.push_back(core::vector3df(99.612457f, 102.463669f, 347.603210f));
			points.push_back(core::vector3df(99.612457f, 102.463669f, 347.603210f));

			timeForThisScene = (points.size()-3)* 1000;

			camera = sm->addCameraSceneNode(0, points[0], core::vector3df(0 ,400,0));
			//camera->setTarget(core::vector3df(0,400,0));

			sa = sm->createFollowSplineAnimator(device->getTimer()->getTime(),
				points);
			camera->addAnimator(sa);
			sa->drop();

			model1->setVisible(false);
			model2->setVisible(false);
			campFire->setVisible(false);
			inOutFader->fadeIn(7000);
		}
		break;

	case 2:	// down fly anim camera
		camera = sm->addCameraSceneNode(0, core::vector3df(100,40,-80), core::vector3df(844,670,-885));
		sa = sm->createFlyStraightAnimator(core::vector3df(94, 1002, 127),
			core::vector3df(108, 15, -60), 10000, true);
		camera->addAnimator(sa);
		timeForThisScene = 9900;
		model1->setVisible(true);
		model2->setVisible(false);
		campFire->setVisible(false);
		sa->drop();
		break;
*/
//	case 3: // interactive, go around
	case 1: // interactive, go around
		{
			if (model1)
				model1->setVisible(true);
			if (model2)
				model2->setVisible(true);
			campFire->setVisible(true);
			timeForThisScene = -1;

			SKeyMap keyMap[9];
			keyMap[0].Action = EKA_MOVE_FORWARD;
			keyMap[0].KeyCode = KEY_UP;
			keyMap[1].Action = EKA_MOVE_FORWARD;
			keyMap[1].KeyCode = KEY_KEY_W;

			keyMap[2].Action = EKA_MOVE_BACKWARD;
			keyMap[2].KeyCode = KEY_DOWN;
			keyMap[3].Action = EKA_MOVE_BACKWARD;
			keyMap[3].KeyCode = KEY_KEY_S;

			keyMap[4].Action = EKA_STRAFE_LEFT;
			keyMap[4].KeyCode = KEY_LEFT;
			keyMap[5].Action = EKA_STRAFE_LEFT;
			keyMap[5].KeyCode = KEY_KEY_A;

			keyMap[6].Action = EKA_STRAFE_RIGHT;
			keyMap[6].KeyCode = KEY_RIGHT;
			keyMap[7].Action = EKA_STRAFE_RIGHT;
			keyMap[7].KeyCode = KEY_KEY_D;

			keyMap[8].Action = EKA_JUMP_UP;
			//keyMap[8].KeyCode = KEY_KEY_J;
			keyMap[8].KeyCode = KEY_SPACE;

			// Last parameter is jump speed
			// Tweaked so you can get up ladders
			camera = sm->addCameraSceneNodeFPS(0, 100.0f, .4f, -1, keyMap, 9, false, 2.05f);
			camera->setPosition(core::vector3df(108,140,-140));

			scene::ISceneNodeAnimatorCollisionResponse* collider =
				sm->createCollisionResponseAnimator(
				metaSelector, camera, core::vector3df(25,CAMERA_HEIGHT,25),
				core::vector3df(0, quakeLevelMesh ? -10.f : 0.0f,0),
					core::vector3df(0,45,0), 0.005f);

			camera->addAnimator(collider);
			collider->drop();
		}
		break;
	}

	sceneStartTime = device->getTimer()->getTime();

}


void CDemo::loadSceneData()
{
	// load quake level

	video::IVideoDriver* driver = device->getVideoDriver();
	scene::ISceneManager* sm = device->getSceneManager();

	// Quake3 Shader controls Z-Writing
	sm->getParameters()->setAttribute(scene::ALLOW_ZWRITE_ON_TRANSPARENT, true);

	quakeLevelMesh = (scene::IQ3LevelMesh*) sm->getMesh("maps/20kdm2.bsp");

	if (quakeLevelMesh)
	{
		u32 i;

		//move all quake level meshes (non-realtime)
		core::matrix4 m;
		m.setTranslation ( core::vector3df(-1300,-70,-1249) );

		for ( i = 0; i!= scene::quake3::E_Q3_MESH_SIZE; ++i )
		{
			sm->getMeshManipulator()->transform ( quakeLevelMesh->getMesh(i), m );
		}

		quakeLevelNode = sm->addOctreeSceneNode(
			quakeLevelMesh->getMesh( scene::quake3::E_Q3_MESH_GEOMETRY)
			);
		if (quakeLevelNode)
		{
			//quakeLevelNode->setPosition(core::vector3df(-1300,-70,-1249));
			quakeLevelNode->setVisible(true);

			// create map triangle selector
			mapSelector = sm->createOctreeTriangleSelector(quakeLevelMesh->getMesh(0),
				quakeLevelNode, 128);

			// if not using shader and no gamma it's better to use more lighting, because
			// quake3 level are usually dark
			quakeLevelNode->setMaterialType ( video::EMT_LIGHTMAP_M4 );

			// set additive blending if wanted
			if (additive)
				quakeLevelNode->setMaterialType(video::EMT_LIGHTMAP_ADD);
		}

		// the additional mesh can be quite huge and is unoptimized
		scene::IMesh * additional_mesh = quakeLevelMesh->getMesh ( scene::quake3::E_Q3_MESH_ITEMS );

		for ( i = 0; i!= additional_mesh->getMeshBufferCount (); ++i )
		{
			scene::IMeshBuffer *meshBuffer = additional_mesh->getMeshBuffer ( i );
			const video::SMaterial &material = meshBuffer->getMaterial();

			//! The ShaderIndex is stored in the material parameter
			s32 shaderIndex = (s32) material.MaterialTypeParam2;

			// the meshbuffer can be rendered without additional support, or it has no shader
			const scene::quake3::IShader *shader = quakeLevelMesh->getShader ( shaderIndex );
			if ( 0 == shader )
			{
				continue;
			}
			// Now add the MeshBuffer(s) with the current Shader to the Manager
			sm->addQuake3SceneNode ( meshBuffer, shader );
		}


	}

	// load sydney model and create 2 instances
	/*
	scene::IAnimatedMesh* mesh = 0;
	mesh = sm->getMesh(IRRLICHT_MEDIA_PATH "sydney.md2");
	if (mesh)
	{
		model1 = sm->addAnimatedMeshSceneNode(mesh);
		if (model1)
		{
			model1->setMaterialTexture(0, driver->getTexture(IRRLICHT_MEDIA_PATH "spheremap.jpg"));
			model1->setPosition(core::vector3df(100,40,-80));
			model1->setScale(core::vector3df(2,2,2));
			model1->setMD2Animation(scene::EMAT_STAND);
			model1->setMaterialFlag(video::EMF_LIGHTING, false);
			model1->setMaterialType(video::EMT_SPHERE_MAP);
			model1->addShadowVolumeSceneNode();
			model1->setAutomaticCulling ( scene::EAC_BOX );
		}

		model2 = sm->addAnimatedMeshSceneNode(mesh);
		if (model2)
		{
			model2->setPosition(core::vector3df(180,15,-60));
			model2->setScale(core::vector3df(2,2,2));
			model2->setMD2Animation(scene::EMAT_RUN);
			model2->setMaterialTexture(0, device->getVideoDriver()->getTexture(IRRLICHT_MEDIA_PATH "sydney.bmp"));
			model2->setMaterialFlag(video::EMF_LIGHTING, true);
			model2->addShadowVolumeSceneNode();
			model2->setAutomaticCulling ( scene::EAC_BOX );
		}
	}
	*/

	scene::ISceneNodeAnimator* anim = 0;

	// create sky box
	driver->setTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS, false);
	skyboxNode = sm->addSkyBoxSceneNode(
		driver->getTexture(IRRLICHT_MEDIA_PATH "irrlicht2_up.jpg"),
		driver->getTexture(IRRLICHT_MEDIA_PATH "irrlicht2_dn.jpg"),
		driver->getTexture(IRRLICHT_MEDIA_PATH "irrlicht2_lf.jpg"),
		driver->getTexture(IRRLICHT_MEDIA_PATH "irrlicht2_rt.jpg"),
		driver->getTexture(IRRLICHT_MEDIA_PATH "irrlicht2_ft.jpg"),
		driver->getTexture(IRRLICHT_MEDIA_PATH "irrlicht2_bk.jpg"));
	driver->setTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS, true);

	//driver->setTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS, true);

	// create walk-between-portals animation

	core::vector3df waypoint[2];
	waypoint[0].set(-150,40,100);
	waypoint[1].set(350,40,100);

	if (model2)
	{
		anim = device->getSceneManager()->createFlyStraightAnimator(waypoint[0],
			waypoint[1], 2000, true);
		model2->addAnimator(anim);
		anim->drop();
	}

	// create animation for portals;

	core::array<video::ITexture*> textures;
	for (s32 g=1; g<8; ++g)
	{
		core::stringc tmp(IRRLICHT_MEDIA_PATH "portal");
		tmp += g;
		tmp += ".bmp";
		video::ITexture* t = driver->getTexture( tmp );
		textures.push_back(t);
	}

	anim = sm->createTextureAnimator(textures, 100);

	// create portals

	scene::IBillboardSceneNode* bill = 0;

	for (int r=0; r<2; ++r)
	{
		bill = sm->addBillboardSceneNode(0, core::dimension2d<f32>(100,100),
			waypoint[r]+ core::vector3df(0,20,0));
		bill->setMaterialFlag(video::EMF_LIGHTING, false);
		bill->setMaterialTexture(0, driver->getTexture(IRRLICHT_MEDIA_PATH "portal1.bmp"));
		bill->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
		bill->addAnimator(anim);
	}

	anim->drop();

	// create cirlce flying dynamic light with transparent billboard attached

	scene::ILightSceneNode* light = 0;

	light = sm->addLightSceneNode(0,
		core::vector3df(0,0,0),	video::SColorf(1.0f, 1.0f, 1.f, 1.0f), 500.f);

	anim = sm->createFlyCircleAnimator(
		core::vector3df(100,150,80), 80.0f, 0.0005f);

	light->addAnimator(anim);
	anim->drop();

	bill = device->getSceneManager()->addBillboardSceneNode(
		light, core::dimension2d<f32>(40,40));
	bill->setMaterialFlag(video::EMF_LIGHTING, false);
	bill->setMaterialTexture(0, driver->getTexture(IRRLICHT_MEDIA_PATH "particlewhite.bmp"));
	bill->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);

	// create meta triangle selector with all triangles selectors in it.
	metaSelector = sm->createMetaTriangleSelector();
	metaSelector->addTriangleSelector(mapSelector);

	// create camp fire

	campFire = sm->addParticleSystemSceneNode(false);
	campFire->setPosition(core::vector3df(100,120,600));
	campFire->setScale(core::vector3df(2,2,2));


	scene::IParticleEmitter* em = campFire->createBoxEmitter(
		core::aabbox3d<f32>(-7,0,-7,7,1,7),
		core::vector3df(0.0f,0.06f,0.0f),
		80,100, video::SColor(0,255,255,255),video::SColor(0,255,255,255), 800,2000);

	em->setMinStartSize(core::dimension2d<f32>(20.0f, 10.0f));
	em->setMaxStartSize(core::dimension2d<f32>(20.0f, 10.0f));
	campFire->setEmitter(em);
	em->drop();

	scene::IParticleAffector* paf = campFire->createFadeOutParticleAffector();
	campFire->addAffector(paf);
	paf->drop();

	campFire->setMaterialFlag(video::EMF_LIGHTING, false);
	campFire->setMaterialFlag(video::EMF_ZWRITE_ENABLE, false);
	campFire->setMaterialTexture(0, driver->getTexture(IRRLICHT_MEDIA_PATH "fireball.bmp"));
	campFire->setMaterialType(video::EMT_TRANSPARENT_VERTEX_ALPHA);

	// load music

#ifdef USE_IRRKLANG
	if (music)
		startIrrKlang();
#endif
#ifdef USE_SDL_MIXER
	if (music)
		startSound();
#endif

}



void CDemo::createLoadingScreen()
{
	core::dimension2d<u32> size = device->getVideoDriver()->getScreenSize();

	device->getCursorControl()->setVisible(false);

	// setup loading screen

	backColor.set(255,90,90,156);

	// create in fader

	inOutFader = device->getGUIEnvironment()->addInOutFader();
	inOutFader->setColor(backColor,	video::SColor ( 0, 230, 230, 230 ));

	// irrlicht logo
	device->getGUIEnvironment()->addImage(device->getVideoDriver()->getTexture("../../media/irrlichtlogo2.png"),
		core::position2d<s32>(5,5));

	// loading text

	const int lwidth = size.Width - 20;
	const int lheight = 16;

	core::rect<int> pos(10, size.Height-lheight-10, 10+lwidth, size.Height-10);

	device->getGUIEnvironment()->addImage(pos);
	statusText = device->getGUIEnvironment()->addStaticText(L"Loading...",	pos, true);
	statusText->setOverrideColor(video::SColor(255,205,200,200));

	// load bigger font

	device->getGUIEnvironment()->getSkin()->setFont(
		device->getGUIEnvironment()->getFont(IRRLICHT_MEDIA_PATH "fonthaettenschweiler.bmp"));

	// set new font color

	device->getGUIEnvironment()->getSkin()->setColor(gui::EGDC_BUTTON_TEXT,
		video::SColor(255,100,100,100));
}
void CDemo::CalculateSyndeyBoundingBox(void)
{
	// Find the extents of the player character's model (for networking collision checks)
	scene::IAnimatedMesh* mesh = 0;
	scene::ISceneManager *sm = device->getSceneManager();
	mesh = sm->getMesh(IRRLICHT_MEDIA_PATH "sydney.md2");
	irr::scene::IAnimatedMeshSceneNode* model;
	model = sm->addAnimatedMeshSceneNode(mesh, 0);
	model->setScale(core::vector3df(2,2,2));
	// Bounding box changed in Irrlicht 1.5.1
	core::aabbox3df modelBoundingBox = model->getMesh()->getBoundingBox();
	// core::aabbox3df modelBoundingBox = model->getBoundingBox();
	core::vector3df minEdgeExtended = modelBoundingBox.MinEdge;
	core::vector3df maxEdgeExtended = modelBoundingBox.MaxEdge;
	minEdgeExtended.X-=BALL_DIAMETER/2;
	minEdgeExtended.Y-=BALL_DIAMETER/2;
	minEdgeExtended.Z-=BALL_DIAMETER/2;
	maxEdgeExtended.X+=BALL_DIAMETER/2;
	maxEdgeExtended.Y+=BALL_DIAMETER/2;
	maxEdgeExtended.Z+=BALL_DIAMETER/2;
	syndeyBoundingBox.MinEdge=minEdgeExtended;
	syndeyBoundingBox.MaxEdge=maxEdgeExtended;
	model->remove();
};
// RakNet - Precalculate bounding box of Sydney.md2, since our own player's model is never loaded
// This only works on the assumption that all players have the same model
const core::aabbox3df& CDemo::GetSyndeyBoundingBox(void) const
{
	return syndeyBoundingBox;
}
void CDemo::PlayDeathSound(core::vector3df position)
{
	if (irrKlang)
	{
		irrklang::ISound* sound = 
			irrKlang->play3D(impactSound, position, false, false, true);

		if (sound)
		{
			// adjust max value a bit to make to sound of an impact louder
			sound->setMinDistance(400);
			sound->drop();
		}
	}	
}
void CDemo::EnableInput(bool enabled)
{
	scene::ICameraSceneNode* camera = GetSceneManager()->getActiveCamera();
	camera->setInputReceiverEnabled(enabled);
}
// RakNet - change shoot from assuming the camera, to taking any starting location
// This way the same function can be called from the network
RakNet::TimeMS CDemo::shootFromOrigin(core::vector3df camPosition, core::vector3df camAt)
{
	scene::ISceneManager* sm = device->getSceneManager();
	scene::ICameraSceneNode* camera = sm->getActiveCamera();

	if (!camera || !mapSelector)
		return 0;

	SParticleImpact imp;
	imp.when = 0;

	// get line of camera

	core::vector3df start = camPosition;
	core::vector3df end = (camAt);
	//end.normalize();
	start += end*8.0f;
	end = start + (end * camera->getFarValue());

	core::triangle3df triangle;

	core::line3d<f32> line(start, end);

	// get intersection point with map
	scene::ISceneNode* hitNode;
	if (sm->getSceneCollisionManager()->getCollisionPoint(
		line, mapSelector, end, triangle, hitNode))
	{
		// collides with wall
		core::vector3df out = triangle.getNormal();
		out.setLength(0.03f);

		imp.when = 1;
		imp.outVector = out;
		imp.pos = end;
	}
	else
	{
		// doesnt collide with wall
		core::vector3df start = camPosition;
		core::vector3df end = (camAt);
		//end.normalize();
		start += end*8.0f;
		end = start + (end * camera->getFarValue());
	}

	// create fire ball
	scene::ISceneNode* node = 0;
	node = sm->addBillboardSceneNode(0,
		core::dimension2d<f32>(BALL_DIAMETER,BALL_DIAMETER), start);

	node->setMaterialFlag(video::EMF_LIGHTING, false);
	node->setMaterialTexture(0, device->getVideoDriver()->getTexture(IRRLICHT_MEDIA_PATH "fireball.bmp"));
	node->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);

	f32 length = (f32)(end - start).getLength();
	const f32 speed = SHOT_SPEED;
	u32 time = (u32)(length / speed);

	scene::ISceneNodeAnimator* anim = 0;

	// set flight line

	anim = sm->createFlyStraightAnimator(start, end, time);
	node->addAnimator(anim);
	anim->drop();

	anim = sm->createDeleteAnimator(time);
	node->addAnimator(anim);
	anim->drop();

	if (imp.when)
	{
		// create impact note
		imp.when = device->getTimer()->getTime() + (time - 100);
		Impacts.push_back(imp);
	}

	// play sound
#ifdef USE_IRRKLANG
	if (ballSound)
	{
		//	irrKlang->play2D(ballSound);

		// RakNet: Make the sound 3d so others can hear it from the proper origin
		irrklang::ISound* sound = 
			irrKlang->play3D(ballSound, node->getPosition(), false, false, true);

		if (sound)
		{
			// adjust max value a bit to make to sound of an impact louder
			sound->setMinDistance(400);
			sound->drop();
		}

	}
#endif
#ifdef USE_SDL_MIXER
	if (ballSound)
		playSound(ballSound);
#endif

	return (RakNet::TimeMS) time;
}

void CDemo::shoot()
{
	if (playerReplica->IsDead())
		return;

	scene::ISceneManager* sm = device->getSceneManager();
	scene::ICameraSceneNode* camera = sm->getActiveCamera();
	core::vector3df camPosition = camera->getPosition();
	core::vector3df camAt = (camera->getTarget() - camPosition);
	camAt.normalize();

	BallReplica *br = new BallReplica;
	br->demo=this;
	br->position=camPosition;
	br->shotDirection=camAt;
	br->shotLifetime=RakNet::GetTimeMS() + shootFromOrigin(camPosition, camAt);
	replicaManager3->Reference(br);
}

void CDemo::createParticleImpacts()
{
	u32 now = device->getTimer()->getTime();
	scene::ISceneManager* sm = device->getSceneManager();

	for (s32 i=0; i<(s32)Impacts.size(); ++i)
		if (now > Impacts[i].when)
		{
			// create smoke particle system
			scene::IParticleSystemSceneNode* pas = 0;

			pas = sm->addParticleSystemSceneNode(false, 0, -1, Impacts[i].pos);

			pas->setParticleSize(core::dimension2d<f32>(10.0f, 10.0f));

			scene::IParticleEmitter* em = pas->createBoxEmitter(
				core::aabbox3d<f32>(-5,-5,-5,5,5,5),
				Impacts[i].outVector, 20,40, video::SColor(0,255,255,255),video::SColor(0,255,255,255),
				1200,1600, 20);

			pas->setEmitter(em);
			em->drop();

			scene::IParticleAffector* paf = campFire->createFadeOutParticleAffector();
			pas->addAffector(paf);
			paf->drop();

			pas->setMaterialFlag(video::EMF_LIGHTING, false);
			pas->setMaterialTexture(0, device->getVideoDriver()->getTexture(IRRLICHT_MEDIA_PATH "smoke.bmp"));
			pas->setMaterialType(video::EMT_TRANSPARENT_VERTEX_ALPHA);

			scene::ISceneNodeAnimator* anim = sm->createDeleteAnimator(2000);
			pas->addAnimator(anim);
			anim->drop();

			// play impact sound
			#ifdef USE_IRRKLANG
			if (irrKlang)
			{
				irrklang::ISound* sound = 
					irrKlang->play3D(impactSound, Impacts[i].pos, false, false, true);

				if (sound)
				{
					// adjust max value a bit to make to sound of an impact louder
					sound->setMinDistance(400);
					sound->drop();
				}
			}
			#endif

			#ifdef USE_SDL_MIXER
			if (impactSound)
				playSound(impactSound);
			#endif

			// delete entry
			Impacts.erase(i);
			i--;
		}
}

/// RakNet stuff
void CDemo::UpdateRakNet(void)
{
	RakNet::SystemAddress facilitatorSystemAddress(DEFAULT_NAT_PUNCHTHROUGH_FACILITATOR_IP, DEFAULT_NAT_PUNCHTHROUGH_FACILITATOR_PORT);
	RakNet::Packet *packet;
	RakNet::TimeMS curTime = RakNet::GetTimeMS();
	RakNet::RakString targetName;
	for (packet=rakPeer->Receive(); packet; rakPeer->DeallocatePacket(packet), packet=rakPeer->Receive())
	{
		if (strcmp(packet->systemAddress.ToString(false),DEFAULT_NAT_PUNCHTHROUGH_FACILITATOR_IP)==0)
		{
			targetName="NATPunchthroughServer";
		}
		else
		{
			targetName=packet->systemAddress.ToString(true);
		}

		switch (packet->data[0])
		{
		case ID_IP_RECENTLY_CONNECTED:
			{
				PushMessage(RakNet::RakString("This IP address recently connected from ") + targetName + RakNet::RakString("."));
				if (packet->systemAddress==facilitatorSystemAddress)
					PushMessage("Multiplayer will not work without the NAT punchthrough server!");
			}
			break;
		case ID_INCOMPATIBLE_PROTOCOL_VERSION:
			{
				PushMessage(RakNet::RakString("Incompatible protocol version from ") + targetName + RakNet::RakString("."));
				if (packet->systemAddress==facilitatorSystemAddress)
					PushMessage("Multiplayer will not work without the NAT punchthrough server!");
			}
			break;
		case ID_DISCONNECTION_NOTIFICATION:
			{
				PushMessage(RakNet::RakString("Disconnected from ") + targetName + RakNet::RakString("."));
				if (packet->systemAddress==facilitatorSystemAddress)
					isConnectedToNATPunchthroughServer=false;
			}
			break;
		case ID_CONNECTION_LOST:
			{
				PushMessage(RakNet::RakString("Connection to ") + targetName + RakNet::RakString(" lost."));
				if (packet->systemAddress==facilitatorSystemAddress)
					isConnectedToNATPunchthroughServer=false;
			}
			break;
		case ID_NO_FREE_INCOMING_CONNECTIONS:
			{
				PushMessage(RakNet::RakString("No free incoming connections to ") + targetName + RakNet::RakString("."));
				if (packet->systemAddress==facilitatorSystemAddress)
					PushMessage("Multiplayer will not work without the NAT punchthrough server!");
			}
			break;
		case ID_NEW_INCOMING_CONNECTION:
			{
				if (fullyConnectedMesh2->IsHostSystem())
				{
					PushMessage(RakNet::RakString("Sending player list to new connection"));
					fullyConnectedMesh2->StartVerifiedJoin(packet->guid);
				}
			}
			break;
		case ID_FCM2_VERIFIED_JOIN_START:
			{
				DataStructures::List<RakNet::SystemAddress> addresses;
				DataStructures::List<RakNet::RakNetGUID> guids;
				fullyConnectedMesh2->GetVerifiedJoinRequiredProcessingList(packet->guid, addresses, guids);
				for (unsigned int i=0; i < guids.Size(); i++)
					natPunchthroughClient->OpenNAT(guids[i], facilitatorSystemAddress);
			}
			break;
		case ID_FCM2_VERIFIED_JOIN_FAILED:
			{
				PushMessage(RakNet::RakString("Failed to join game session"));
			}
			break;
		case ID_FCM2_VERIFIED_JOIN_CAPABLE:
			{
				fullyConnectedMesh2->RespondOnVerifiedJoinCapable(packet, true, 0);
			}
			break;
		case ID_FCM2_VERIFIED_JOIN_ACCEPTED:
			{
				DataStructures::List<RakNet::RakNetGUID> systemsAccepted;
				bool thisSystemAccepted;
				fullyConnectedMesh2->GetVerifiedJoinAcceptedAdditionalData(packet, &thisSystemAccepted, systemsAccepted, 0);
				if (thisSystemAccepted)
					PushMessage("Game join request accepted\n");
				else
					PushMessage(RakNet::RakString("System %s joined the mesh\n", systemsAccepted[0].ToString()));
				
				// DataStructures::List<RakNetGUID> participantList;
				// fullyConnectedMesh2->GetParticipantList(participantList);

				for (unsigned int i=0; i < systemsAccepted.Size(); i++)
					replicaManager3->PushConnection(replicaManager3->AllocConnection(rakPeer->GetSystemAddressFromGuid(systemsAccepted[i]), systemsAccepted[i]));
			}
			break;
		case ID_CONNECTION_REQUEST_ACCEPTED:
			{
				PushMessage(RakNet::RakString("Connection request to ") + targetName + RakNet::RakString(" accepted."));
				if (packet->systemAddress==facilitatorSystemAddress)
				{
					isConnectedToNATPunchthroughServer=true;

					// Open UPNP.
					struct UPNPDev * devlist = 0;
					devlist = upnpDiscover(1000, 0, 0, 0);
					if (devlist)
					{
						char lanaddr[64];	/* my ip address on the LAN */
						struct UPNPUrls urls;
						struct IGDdatas data;
						if (UPNP_GetValidIGD(devlist, &urls, &data, lanaddr, sizeof(lanaddr))==1)
						{
							// External port is the port people will be connecting to us on. This is our port as seen by the directory server
							// Internal port is the port RakNet was internally started on
							char eport[32], iport[32];
							natPunchthroughClient->GetUPNPPortMappings(eport, iport, facilitatorSystemAddress);

							int r = UPNP_AddPortMapping(urls.controlURL, data.first.servicetype,
								eport, iport, lanaddr, 0, "UDP", 0);

							if(r==UPNPCOMMAND_SUCCESS)
							{
								// UPNP done
							}

						}
					}
					
					// Query cloud for other running game instances
					RakNet::CloudQuery cloudQuery;
					cloudQuery.keys.Push(RakNet::CloudKey("IrrlichtDemo",0),_FILE_AND_LINE_);
					cloudClient->Get(&cloudQuery, packet->guid);
				}
			}
			break;
		case ID_FCM2_NEW_HOST:
			{
				if (packet->guid==rakPeer->GetMyGUID())
				{
					// Original host dropped. I am the new session host. Upload to the cloud so new players join this system.
					RakNet::CloudKey cloudKey("IrrlichtDemo",0);
					cloudClient->Post(&cloudKey, 0, 0, rakPeer->GetGuidFromSystemAddress(facilitatorSystemAddress));
				}
			}
			break;
		case ID_CLOUD_GET_RESPONSE:
			{
				RakNet::CloudQueryResult cloudQueryResult;
				cloudClient->OnGetReponse(&cloudQueryResult, packet);
				if (cloudQueryResult.rowsReturned.Size()>0)
				{	
					PushMessage(RakNet::RakString("NAT punch to existing game instance"));
					natPunchthroughClient->OpenNAT(cloudQueryResult.rowsReturned[0]->clientGUID, facilitatorSystemAddress);
				}
				else
				{
					PushMessage(RakNet::RakString("Publishing new game instance"));

					// Start as a new game instance because no other games are running
					RakNet::CloudKey cloudKey("IrrlichtDemo",0);
					cloudClient->Post(&cloudKey, 0, 0, packet->guid);
				}

				cloudClient->DeallocateWithDefaultAllocator(&cloudQueryResult);
			}
			break;
		case ID_CONNECTION_ATTEMPT_FAILED:
			{
				PushMessage(RakNet::RakString("Connection attempt to ") + targetName + RakNet::RakString(" failed."));
				if (packet->systemAddress==facilitatorSystemAddress)
					PushMessage("Multiplayer will not work without the NAT punchthrough server!");
			}
			break;
		case ID_NAT_TARGET_NOT_CONNECTED:
			{
				RakNet::RakNetGUID recipientGuid;
				RakNet::BitStream bs(packet->data,packet->length,false);
				bs.IgnoreBytes(sizeof(RakNet::MessageID));
				bs.Read(recipientGuid);
				targetName=recipientGuid.ToString();
				PushMessage(RakNet::RakString("NAT target ") + targetName + RakNet::RakString(" not connected."));
			}
			break;
		case ID_NAT_TARGET_UNRESPONSIVE:
			{
				RakNet::RakNetGUID recipientGuid;
				RakNet::BitStream bs(packet->data,packet->length,false);
				bs.IgnoreBytes(sizeof(RakNet::MessageID));
				bs.Read(recipientGuid);
				targetName=recipientGuid.ToString();
				PushMessage(RakNet::RakString("NAT target ") + targetName + RakNet::RakString(" unresponsive."));
			}
			break;
		case ID_NAT_CONNECTION_TO_TARGET_LOST:
			{
				RakNet::RakNetGUID recipientGuid;
				RakNet::BitStream bs(packet->data,packet->length,false);
				bs.IgnoreBytes(sizeof(RakNet::MessageID));
				bs.Read(recipientGuid);
				targetName=recipientGuid.ToString();
				PushMessage(RakNet::RakString("NAT target connection to ") + targetName + RakNet::RakString(" lost."));
			}
			break;
		case ID_NAT_ALREADY_IN_PROGRESS:
			{
				RakNet::RakNetGUID recipientGuid;
				RakNet::BitStream bs(packet->data,packet->length,false);
				bs.IgnoreBytes(sizeof(RakNet::MessageID));
				bs.Read(recipientGuid);
				targetName=recipientGuid.ToString();
				PushMessage(RakNet::RakString("NAT punchthrough to ") + targetName + RakNet::RakString(" in progress (skipping)."));
			}
			break;

		case ID_NAT_PUNCHTHROUGH_SUCCEEDED:
			{
				if (packet->data[1]==1)
				{
					PushMessage(RakNet::RakString("Connecting to existing game instance"));
					RakNet::ConnectionAttemptResult car = rakPeer->Connect(packet->systemAddress.ToString(false), packet->systemAddress.GetPort(), 0, 0);
					RakAssert(car==RakNet::CONNECTION_ATTEMPT_STARTED);
				}
			}
			break;

		case ID_ADVERTISE_SYSTEM:
			if (packet->guid!=rakPeer->GetGuidFromSystemAddress(RakNet::UNASSIGNED_SYSTEM_ADDRESS))
			{
				char hostIP[32];
				packet->systemAddress.ToString(false,hostIP);
				RakNet::ConnectionAttemptResult car = rakPeer->Connect(hostIP,packet->systemAddress.GetPort(),0,0);
				RakAssert(car==RakNet::CONNECTION_ATTEMPT_STARTED);
			}
			break;
		}
	}

	// Call the Update function for networked game objects added to BaseIrrlichtReplica once the game is ready
	if (currentScene>=1)
	{
		unsigned int idx;
		for (idx=0; idx < replicaManager3->GetReplicaCount(); idx++)
			((BaseIrrlichtReplica*)(replicaManager3->GetReplicaAtIndex(idx)))->Update(curTime);;
	}	
}

bool CDemo::IsKeyDown(EKEY_CODE keyCode) const {return KeyIsDown[keyCode];}
bool CDemo::IsMovementKeyDown(void) const {return KeyIsDown[KEY_UP] | 
KeyIsDown[KEY_DOWN] | 
KeyIsDown[KEY_LEFT] | 
KeyIsDown[KEY_RIGHT] | 
KeyIsDown[KEY_KEY_W] | 
KeyIsDown[KEY_KEY_S] | 
KeyIsDown[KEY_KEY_A] | 
KeyIsDown[KEY_KEY_D];
}
void CDemo::PushMessage(RakNet::RakString rs)
{
	outputMessages.Push(rs,_FILE_AND_LINE_);
	if (whenOutputMessageStarted==0)
	{
		whenOutputMessageStarted=RakNet::GetTimeMS();
	}
}
const char *CDemo::GetCurrentMessage(void)
{
	if (outputMessages.GetSize()==0)
		return "";
	RakNet::TimeMS curTime = RakNet::GetTimeMS();
	if (curTime-whenOutputMessageStarted>2500)
	{
		outputMessages.Pop(_FILE_AND_LINE_);
		whenOutputMessageStarted=curTime;
	}

	
	if (outputMessages.GetSize()==0)
	{
		whenOutputMessageStarted=0;
		return "";
	}
	return outputMessages.Peek().C_String();
}

#ifdef USE_IRRKLANG
void CDemo::startIrrKlang()
{
	irrKlang = irrklang::createIrrKlangDevice();

	if (!irrKlang)
		return;

	// play music

	irrklang::ISound* snd = irrKlang->play2D(IRRLICHT_MEDIA_PATH "IrrlichtTheme.ogg", true, false, true);
	if ( !snd )
		snd = irrKlang->play2D("IrrlichtTheme.ogg", true, false, true);

	if (snd)
	{
		snd->setVolume(0.5f); // 50% volume
		snd->drop();
	}

	// preload both sound effects

	ballSound = irrKlang->getSoundSource(IRRLICHT_MEDIA_PATH "ball.wav");
	impactSound = irrKlang->getSoundSource(IRRLICHT_MEDIA_PATH "impact.wav");
}
#endif


#ifdef USE_SDL_MIXER
void CDemo::startSound()
{
	stream = NULL;
	ballSound = NULL;
	impactSound = NULL;

	SDL_Init(SDL_INIT_AUDIO);

	if (Mix_OpenAudio(22050, AUDIO_S16, 2, 128))
		return;

	stream = Mix_LoadMUS(IRRLICHT_MEDIA_PATH "IrrlichtTheme.ogg");
	if (stream)
		Mix_PlayMusic(stream, -1);

	ballSound = Mix_LoadWAV(IRRLICHT_MEDIA_PATH "ball.wav");
	impactSound = Mix_LoadWAV(IRRLICHT_MEDIA_PATH "impact.wav");
}

void CDemo::playSound(Mix_Chunk *sample)
{
	if (sample)
		Mix_PlayChannel(-1, sample, 0);
}

void CDemo::pollSound(void)
{
	SDL_Event event;

	while (SDL_PollEvent(&event))
		;
}
#endif
