/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

#include "App3D.h"

#include "Ogre.h"
#include "RakAssert.h"
#include "FormatString.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "OgreRenderSystemCapabilities.h"

#if defined(__GNUC__)
#define _vsnprintf vsnprintf
#endif

static const char *defaultCameraName = "DefaultCamera";
static const char *defaultSceneManagerName = "DefaultSceneManager";

App3D::App3D()
{
	root=0;
	camera=0;
	sceneManager=0;
	window=0;
	viewport=0;
	math=0;
	isVisible=true;
	//statsOverlay=0;
}
App3D::~App3D()
{
}
void App3D::Update(AppTime curTimeMS,AppTime elapsedTimeMS)
{
	AppInterface::Update(curTimeMS, elapsedTimeMS);
	UpdateDefault(curTimeMS, elapsedTimeMS);
}
void App3D::Render(AppTime curTimeMS)
{
	if (isVisible)
	{
		///
		/// SINCE WE ARE CALLING OGRE EXCEPTIONS ARE POSSIBLE HERE
		///
#ifdef _DEBUG
		try
		{
#endif
			if (window->isActive())
			{
				root->renderOneFrame();
			}
			else if (window->isVisible()) // Need this isVisible check or it crashes like hell when alt-tabbing
			{
				window->update();
			}
#ifdef _DEBUG
		}
		catch (Ogre::Exception &e)
		{
			RakAssert(0 && "Something has happened to the Ogre rendering state. Recovery is possible. The cause needs to be fixed.");
		}
#endif
	}	
}
void App3D::SetVisible(bool _isVisible)
{
	isVisible=_isVisible;
}
bool App3D::IsVisible(void) const
{
	return isVisible;
}
void App3D::UpdateDefault(AppTime curTimeMS,AppTime elapsedTimeMS)
{
}
void App3D::PreConfigure(void)
{
#ifdef WIN32
	::GetCurrentDirectory(sizeof(workingDirectory)-1, workingDirectory);
#else
	// TODO - Get the current directory in linux
	RakAssert(0);
#endif

	try
	{
		// If it throws an exception, change project properties / configuration properties / debugging / working directory to $(ProjectDir)
#ifdef WIN32
	#ifdef _DEBUG
		root = new Ogre::Root("PluginsDebug.cfg", "Graphics.cfg", "Graphics.log");
	#else
		root = new Ogre::Root("Plugins.cfg", "Graphics.cfg", "Graphics.log");
	#endif
	#else
	#ifdef _DEBUG
		root = new Ogre::Root("PluginsDebugL.cfg", "Graphics.cfg", "Graphics.log");
	#else
		root = new Ogre::Root("PluginsL.cfg", "Graphics.cfg", "Graphics.log");
	#endif
#endif
	}
	catch (Ogre::Exception* e)
	{
		e->getFullDescription();
	}
	

}
bool App3D::Configure(void)
{
	try
	{
		if(root->restoreConfig() || root->showConfigDialog())
		{
			// If returned true, user clicked OK so initialise
			// Here we choose to let the system create a default rendering window by passing 'true'
			window = root->initialise(true,  GetWindowTitle());
			return true;
		}
		else 
		{
			return false;
		}
	}
	catch (Ogre::InvalidParametersException &e)
	{
		return false;
		
	}
}
void App3D::PostConfigure(const char *defaultResourceConfigurationPath, bool recursive)
{
	// Instantiate the math class to build the trig tables.
	math = new Ogre::Math;

	// Load resource paths from config file
	Ogre::ConfigFile cf;
	cf.load(defaultResourceConfigurationPath);

	// Go through all sections & settings in the file
	Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();

	Ogre::String secName, typeName, archName;
	while (seci.hasMoreElements())
	{
		secName = seci.peekNextKey();
		// Seems like General is created automatically going by ogre.log
		if (secName!="General")
		{			
			try
			{
				Ogre::ResourceGroupManager::getSingleton().createResourceGroup(secName);
			}
			catch (Ogre::Exception& e)
			{
				e;
			}
		}
		
		Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
		Ogre::ConfigFile::SettingsMultiMap::iterator i;
		for (i = settings->begin(); i != settings->end(); ++i)
		{
			typeName = i->first;
			archName = i->second;
 
			Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
				archName, typeName, secName, recursive);
		}
	}

	// Window should have been created in Configure, or created here in a derived class
	Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup("Bootstrap");
	Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup("General");

	const Ogre::RenderSystemCapabilities* cap = root->getRenderSystem()->getCapabilities();
	// + (*cap).mMaxFragmentProgramVersion "ps_2_b" std::basic_string<char,std::char_traits<char>,std::allocator<char> >
//	if (cap->getMaxFragmentProgramVersion().c_str()[3]>='2')
		hasPixelShader2=true;
//	else
//		hasPixelShader2=false;
}
void App3D::InitSceneManager(Ogre::SceneManager *sm)
{
	if (sm)
		sceneManager=sm;
	else
		sceneManager = root->createSceneManager(Ogre::ST_GENERIC, defaultSceneManagerName);

	// Normal object visibility is mask 1.
	Ogre::MovableObject::setDefaultVisibilityFlags(1);
	sceneManager->setVisibilityMask(1);
}
// Must be called after InitSceneManager
void App3D::InitGUIManager(void)
{
	
}
void App3D::InitCamera(Ogre::Camera *cam)
{
	RakAssert(sceneManager);

	if (cam)
		camera=cam;
	else
		camera = sceneManager->createCamera(defaultCameraName);

	camera->setFOVy(Ogre::Radian(3.1415927/4.0f));

}
void App3D::InitViewport(Ogre::Viewport *vp)
{
	// Create one viewport, entire window
	if (vp==0)
	{
		if (viewport==0)
		{
			viewport = window->addViewport(camera);
			viewport->setBackgroundColour(Ogre::ColourValue(0,0,0));
		}		
	}
	else
		viewport=vp;

	// Alter the camera aspect ratio to match the viewport
	camera->setAspectRatio(
		(float)viewport->getActualWidth() / (float)viewport->getActualHeight());
}

void App3D::OnAppShutdown(void)
{
	if (window)
		window->removeAllViewports();

	delete root;
	delete math;

	camera=0;
	sceneManager=0;
}
bool App3D::ShouldQuit(void) const
{
	return window->isClosed();
}
void App3D::SetState(int stateType, RunnableState* state)
{
	AppInterface::SetState(stateType, state);
}
Ogre::SceneManager* App3D::GetSceneManager(void) const
{
	return sceneManager;
}
const char* App3D::GetWorkingDirectory(void) const
{
	return workingDirectory;
}
bool App3D::HasPixelShader2(void) const
{
	return hasPixelShader2;
}
const char * App3D::TakeScreenshot(const char *prefix, const char *suffix)
{
	time_t aclock;
	time( &aclock );   // Get time in seconds
	tm *newtime = localtime( &aclock );   // Convert time to struct tm form 
	char text[1024];
	strcpy(text, asctime( newtime ));
	text[strlen(text)-1]=0;

	// ':' character is not allowed for file names
	size_t len=strlen(text);
	for (size_t i=0;i<len;i++)
	{
		if (text[i]==':') text[i]='_';
	}

	char *str = FormatString("%s%s%s", prefix, text, suffix);
	window->writeContentsToFile(str);
	return str;
}
