// This is a Demo of the Irrlicht Engine (c) 2005-2008 by N.Gebhardt.
// This file is not documented.

#include "CMainMenu.h"


//! we want the lights follow the model when it's moving
class CSceneNodeAnimatorFollowBoundingBox : public irr::scene::ISceneNodeAnimator
{
public:

	//! constructor
	CSceneNodeAnimatorFollowBoundingBox(irr::scene::ISceneNode* tofollow,
			const core::vector3df &offset, u32 frequency, s32 phase)
		: Offset(offset), ToFollow(tofollow), Frequency(frequency), Phase(phase)
	{
		if (ToFollow)
			ToFollow->grab();
	}

	//! destructor
	virtual ~CSceneNodeAnimatorFollowBoundingBox()
	{
		if (ToFollow)
			ToFollow->drop();
	}

	//! animates a scene node
	virtual void animateNode(irr::scene::ISceneNode* node, u32 timeMs)
	{
		if (0 == node || node->getType() != irr::scene::ESNT_LIGHT)
			return;

		irr::scene::ILightSceneNode* l = (irr::scene::ILightSceneNode*) node;

		if (ToFollow)
		{
			core::vector3df now = l->getPosition();
			now += ToFollow->getBoundingBox().getCenter();
			now += Offset;
			l->setPosition(now);
		}

		irr::video::SColorHSL color;
		irr::video::SColorf rgb(0);
		color.Hue = ( (timeMs + Phase) % Frequency  ) * ( 2.f * irr::core::PI / Frequency );
		color.Saturation = 1.f;
		color.Luminance = 0.5f;
		color.toRGB(rgb);

		video::SLight light = l->getLightData();
		light.DiffuseColor = rgb;
		l->setLightData(light);
	}

	virtual scene::ISceneNodeAnimator* createClone(scene::ISceneNode* node, scene::ISceneManager* newManager=0) {return 0;}
private:

	core::vector3df Offset;
	irr::scene::ISceneNode* ToFollow;
	s32 Frequency;
	s32 Phase;
};


CMainMenu::CMainMenu()
: startButton(0), MenuDevice(0), selected(2), start(false),
	shadows(false), additive(false), transparent(true), vsync(false), aa(false),
#ifdef _DEBUG
	fullscreen(false), music(false)
#else
	fullscreen(true), music(true)
#endif
{
}


bool CMainMenu::run(bool& outFullscreen, bool& outMusic, bool& outShadows,
			bool& outAdditive, bool& outVSync, bool& outAA,
			video::E_DRIVER_TYPE& outDriver, core::stringw &playerName)
{
	//video::E_DRIVER_TYPE driverType = video::EDT_DIRECT3D9;
	//video::E_DRIVER_TYPE driverType = video::EDT_OPENGL;
	video::E_DRIVER_TYPE driverType = video::EDT_BURNINGSVIDEO;
	//video::E_DRIVER_TYPE driverType = video::EDT_SOFTWARE;

	MenuDevice = createDevice(driverType,
		core::dimension2d<u32>(512, 384), 16, false, false, false, this);

	if (MenuDevice->getFileSystem()->existFile("irrlicht.dat"))
		MenuDevice->getFileSystem()->addFileArchive("irrlicht.dat", true, true, io::EFAT_ZIP);
	else
		MenuDevice->getFileSystem()->addFileArchive("../../media/irrlicht.dat", true, true, io::EFAT_ZIP);

	video::IVideoDriver* driver = MenuDevice->getVideoDriver();
	scene::ISceneManager* smgr = MenuDevice->getSceneManager();
	gui::IGUIEnvironment* guienv = MenuDevice->getGUIEnvironment();

	core::stringw str = "Irrlicht Engine Demo v";
	str += MenuDevice->getVersion();
	MenuDevice->setWindowCaption(str.c_str());

	// set new Skin
	gui::IGUISkin* newskin = guienv->createSkin(gui::EGST_BURNING_SKIN);
	guienv->setSkin(newskin);
	newskin->drop();

	// load font
	gui::IGUIFont* font = guienv->getFont("../../media/fonthaettenschweiler.bmp");
	if (font)
		guienv->getSkin()->setFont(font);

	// add images

	const s32 leftX = 260;

	// add tab control
	gui::IGUITabControl* tabctrl = guienv->addTabControl(core::rect<int>(leftX,10,512-10,384-10),
		0, true, true);
	gui::IGUITab* optTab = tabctrl->addTab(L"Demo");
	gui::IGUITab* aboutTab = tabctrl->addTab(L"About");

	// add list box

	gui::IGUIListBox* box = guienv->addListBox(core::rect<int>(10,10,220,120), optTab, 1);
	box->addItem(L"OpenGL 1.5");
	box->addItem(L"Direct3D 8.1");
	box->addItem(L"Direct3D 9.0c");
	box->addItem(L"Burning's Video 0.39");
	box->addItem(L"Irrlicht Software Renderer 1.0");
	box->setSelected(selected);

	// add button

	startButton = guienv->addButton(core::rect<int>(30,295,200,324), optTab, 2, L"Start Demo");

	// add checkbox

	const s32 d = 50;

	guienv->addCheckBox(fullscreen, core::rect<int>(20,85+d,130,110+d),
		optTab, 3, L"Fullscreen");
	guienv->addCheckBox(music, core::rect<int>(135,85+d,245,110+d),
		optTab, 4, L"Music & Sfx");
	guienv->addCheckBox(shadows, core::rect<int>(20,110+d,135,135+d),
		optTab, 5, L"Realtime shadows");
	guienv->addCheckBox(additive, core::rect<int>(20,135+d,230,160+d),
		optTab, 6, L"Old HW compatible blending");
	guienv->addCheckBox(vsync, core::rect<int>(20,160+d,230,185+d),
		optTab, 7, L"Vertical synchronisation");
	guienv->addCheckBox(aa, core::rect<int>(135,110+d,245,135+d),
		optTab, 8, L"Antialiasing");

	// RakNet: Add edit box
	nameEditBox=guienv->addEditBox(L"Your name here", core::rect<int>(20,185+d,230,210+d), true, optTab, 9);


	// add about text

	wchar_t* text2 = L"This is the tech demo of the Irrlicht engine. To start, "\
		L"select a video driver which works best with your hardware and press 'Start Demo'.\n"\
		L"What you currently see is displayed using the Burning Software Renderer (Thomas Alten).\n"\
		L"The Irrlicht Engine was written by me, Nikolaus Gebhardt. The models, "\
		L"maps and textures were placed at my disposal by B.Collins, M.Cook and J.Marton. The music was created by "\
		L"M.Rohde and is played back by irrKlang.\n"\
		L"For more informations, please visit the homepage of the Irrlicht engine:\nhttp://irrlicht.sourceforge.net\n"\
		L"\n*** MULTIPLAYER UPDATE ***\n"\
		L"Peer to peer multiplayer added in two days using RakNet.\n"\
		L"For a description of the networking design, see included readme.txt .\n";

	guienv->addStaticText(text2, core::rect<int>(10, 10, 230, 320),
		true, true, aboutTab);

	// add md2 model

	scene::IAnimatedMesh* mesh = smgr->getMesh("../../media/faerie.md2");
	scene::IAnimatedMeshSceneNode* modelNode = smgr->addAnimatedMeshSceneNode(mesh);
	if (modelNode)
	{
		modelNode->setPosition( core::vector3df(0.f, 0.f, -5.f) );
		modelNode->setMaterialTexture(0, driver->getTexture("../../media/faerie2.bmp"));
		modelNode->setMaterialFlag(video::EMF_LIGHTING, true);
		modelNode->getMaterial(0).Shininess = 28.f;
		modelNode->getMaterial(0).NormalizeNormals = true;
		modelNode->setMD2Animation(scene::EMAT_STAND);
	}

	// set ambient light (no sun light in the catacombs)
	smgr->setAmbientLight( video::SColorf(0.f, 0.f, 0.f) );

	scene::ISceneNodeAnimator* anim;
	scene::ISceneNode* bill;

	// add light 1 (sunset orange)
	scene::ILightSceneNode* light1 =
		smgr->addLightSceneNode(0, core::vector3df(10.f,10.f,0),
		video::SColorf(0.86f, 0.38f, 0.05f), 200.0f);

	// add fly circle animator to light 1
	anim = smgr->createFlyCircleAnimator(core::vector3df(0,0,0),30.0f, -0.004f, core::vector3df(0.41f, 0.4f, 0.0f));
	light1->addAnimator(anim);
	anim->drop();

	// let the lights follow the model...
	anim = new CSceneNodeAnimatorFollowBoundingBox(modelNode, core::vector3df(0,16,0), 4000, 0);
	//light1->addAnimator(anim);
	anim->drop();

	// attach billboard to the light
	bill = smgr->addBillboardSceneNode(light1, core::dimension2d<f32>(10, 10));
	bill->setMaterialFlag(video::EMF_LIGHTING, false);
	bill->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
	bill->setMaterialTexture(0, driver->getTexture("../../media/particlered.bmp"));

#if 1
	// add light 2 (nearly red)
	scene::ILightSceneNode* light2 =
		smgr->addLightSceneNode(0, core::vector3df(0,1,0),
		video::SColorf(0.9f, 1.0f, 0.f, 0.0f), 200.0f);

	// add fly circle animator to light 1
	anim = smgr->createFlyCircleAnimator(core::vector3df(0,0,0),30.0f, 0.004f, core::vector3df(0.41f, 0.4f, 0.0f));
	light2->addAnimator(anim);
	anim->drop();

	// let the lights follow the model...
	anim = new CSceneNodeAnimatorFollowBoundingBox( modelNode, core::vector3df(0,-8,0), 2000, 0 );
	//light2->addAnimator(anim);
	anim->drop();


	// attach billboard to the light
	bill = smgr->addBillboardSceneNode(light2, core::dimension2d<f32>(10, 10));
	bill->setMaterialFlag(video::EMF_LIGHTING, false);
	bill->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
	bill->setMaterialTexture(0, driver->getTexture("../../media/particlered.bmp"));

	// add light 3 (nearly blue)
	scene::ILightSceneNode* light3 =
		smgr->addLightSceneNode(0, core::vector3df(0,-1,0),
		video::SColorf(0.f, 0.0f, 0.9f, 0.0f), 40.0f);

	// add fly circle animator to light 2
	anim = smgr->createFlyCircleAnimator(core::vector3df(0,0,0),40.0f, 0.004f, core::vector3df(-0.41f, -0.4f, 0.0f));
	light3->addAnimator(anim);
	anim->drop();

	// let the lights follow the model...
	anim = new CSceneNodeAnimatorFollowBoundingBox(modelNode, core::vector3df(0,8,0), 8000, 0);
	//light3->addAnimator(anim);
	anim->drop();

	// attach billboard to the light
	bill = smgr->addBillboardSceneNode(light3, core::dimension2d<f32>(10, 10));
	if (bill)
	{
		bill->setMaterialFlag(video::EMF_LIGHTING, false);
		bill->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
		bill->setMaterialTexture(0, driver->getTexture("../../media/portal1.bmp"));
	}
#endif

	// create a fixed camera
	smgr->addCameraSceneNode(0, core::vector3df(45,0,0), core::vector3df(0,0,10));

	// irrlicht logo and background
	// add irrlicht logo
	bool oldMipMapState = driver->getTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS);
	driver->setTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS, false);

	guienv->addImage(driver->getTexture("../../media/irrlichtlogo2.png"),
		core::position2d<s32>(5,5));

	video::ITexture* irrlichtBack = driver->getTexture("../../media/demoback.jpg");

	driver->setTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS, oldMipMapState);

	// query original skin color
	getOriginalSkinColor();

	// set transparency
	setTransparency();

	// draw all

	while(MenuDevice->run())
	{
		if (MenuDevice->isWindowActive())
		{
			driver->beginScene(false, true, video::SColor(0,0,0,0));

			if (irrlichtBack)
				driver->draw2DImage(irrlichtBack,
						core::position2d<int>(0,0));

			smgr->drawAll();
			guienv->drawAll();
			driver->endScene();
		}
	}

	playerName=nameEditBox->getText();
	MenuDevice->drop();

	outFullscreen = fullscreen;
	outMusic = music;
	outShadows = shadows;
	outAdditive = additive;
	outVSync = vsync;
	outAA = aa;

	switch(selected)
	{
	case 0:	outDriver = video::EDT_OPENGL; break;
	case 1:	outDriver = video::EDT_DIRECT3D8; break;
	case 2:	outDriver = video::EDT_DIRECT3D9; break;
	case 3:	outDriver = video::EDT_BURNINGSVIDEO; break;
	case 4:	outDriver = video::EDT_SOFTWARE; break;
	}

	return start;
}


bool CMainMenu::OnEvent(const SEvent& event)
{
	if (event.EventType == EET_KEY_INPUT_EVENT &&
		event.KeyInput.Key == KEY_F9 &&
		event.KeyInput.PressedDown == false)
	{
		video::IImage* image = MenuDevice->getVideoDriver()->createScreenShot();
		if (image)
		{
			MenuDevice->getVideoDriver()->writeImageToFile(image, "screenshot_main.jpg");
			image->drop();
		}
	}
	else
	if (event.EventType == irr::EET_MOUSE_INPUT_EVENT &&
		event.MouseInput.Event == EMIE_RMOUSE_LEFT_UP )
	{
		core::rect<s32> r(event.MouseInput.X, event.MouseInput.Y, 0, 0);
		gui::IGUIContextMenu* menu = MenuDevice->getGUIEnvironment()->addContextMenu(r, 0, 45);
		menu->addItem(L"transparent menus", 666, transparent == false);
		menu->addItem(L"solid menus", 666, transparent == true);
		menu->addSeparator();
		menu->addItem(L"Cancel");
	}
	else
	if (event.EventType == EET_GUI_EVENT)
	{
		s32 id = event.GUIEvent.Caller->getID();
		switch(id)
		{
		case 45: // context menu
			if (event.GUIEvent.EventType == gui::EGET_MENU_ITEM_SELECTED)
			{
				s32 s = ((gui::IGUIContextMenu*)event.GUIEvent.Caller)->getSelectedItem();
				if (s == 0 || s == 1)
				{
					transparent = !transparent;
					setTransparency();
				}
			}
			break;
		case 1:
			if (event.GUIEvent.EventType == gui::EGET_LISTBOX_CHANGED ||
				event.GUIEvent.EventType == gui::EGET_LISTBOX_SELECTED_AGAIN)
			{
				selected = ((gui::IGUIListBox*)event.GUIEvent.Caller)->getSelected();
				//startButton->setEnabled(selected != 4);
				startButton->setEnabled(true);
			}
			break;
		case 2:
			if (event.GUIEvent.EventType == gui::EGET_BUTTON_CLICKED )
			{
				MenuDevice->closeDevice();
				start = true;
			}
		case 3:
			if (event.GUIEvent.EventType == gui::EGET_CHECKBOX_CHANGED )
				fullscreen = ((gui::IGUICheckBox*)event.GUIEvent.Caller)->isChecked();
			break;
		case 4:
			if (event.GUIEvent.EventType == gui::EGET_CHECKBOX_CHANGED )
				music = ((gui::IGUICheckBox*)event.GUIEvent.Caller)->isChecked();
			break;
		case 5:
			if (event.GUIEvent.EventType == gui::EGET_CHECKBOX_CHANGED )
				shadows = ((gui::IGUICheckBox*)event.GUIEvent.Caller)->isChecked();
			break;
		case 6:
			if (event.GUIEvent.EventType == gui::EGET_CHECKBOX_CHANGED )
				additive = ((gui::IGUICheckBox*)event.GUIEvent.Caller)->isChecked();
			break;
		case 7:
			if (event.GUIEvent.EventType == gui::EGET_CHECKBOX_CHANGED )
				vsync = ((gui::IGUICheckBox*)event.GUIEvent.Caller)->isChecked();
			break;
		case 8:
			if (event.GUIEvent.EventType == gui::EGET_CHECKBOX_CHANGED )
				aa = ((gui::IGUICheckBox*)event.GUIEvent.Caller)->isChecked();
			break;
		}
	}

	return false;
}


void CMainMenu::getOriginalSkinColor()
{
	irr::gui::IGUISkin * skin = MenuDevice->getGUIEnvironment()->getSkin();
	for (s32 i=0; i<gui::EGDC_COUNT ; ++i)
	{
		SkinColor[i] = skin->getColor( (gui::EGUI_DEFAULT_COLOR)i );
	}

}


void CMainMenu::setTransparency()
{
	irr::gui::IGUISkin * skin = MenuDevice->getGUIEnvironment()->getSkin();

	for (u32 i=0; i<gui::EGDC_COUNT ; ++i)
	{
		video::SColor col = SkinColor[i];

		if (false == transparent)
			col.setAlpha(255);

		skin->setColor((gui::EGUI_DEFAULT_COLOR)i, col);
	}
}

