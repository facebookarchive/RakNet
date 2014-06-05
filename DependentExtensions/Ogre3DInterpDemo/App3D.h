/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

#ifndef __APP_3D_H
#define __APP_3D_H

#include "AppInterface.h"

namespace Ogre
{
	class Root;
	class Camera;
	class SceneManager;
	class RenderWindow;
	class Viewport;
	class SceneNode;
	class Math;
	class Overlay;
	class Viewport;
}

class FadingTextList;

class App3D : public AppInterface
{
public:
	App3D();
	virtual ~App3D();
	virtual void PreConfigure(void);
	//virtual void StartupSound(int maxchannels, unsigned int flags, void *extradriverdata, unsigned logType, bool logActive=false);
	virtual bool Configure(void);
	// Note that Ogre is bugged - setting recursive to true just crashes
	virtual void PostConfigure(const char *defaultResourceConfigurationPath, bool recursive);
	virtual void InitSceneManager(Ogre::SceneManager *sm=0);
	virtual void InitGUIManager(void);
	virtual void InitCamera(Ogre::Camera *cam=0);
	virtual void InitViewport(Ogre::Viewport *vp=0);
	virtual void Update(AppTime curTimeMS,AppTime elapsedTimeMS);
	virtual void Render(AppTime curTimeMS);
	virtual void SetVisible(bool _isVisible);
	virtual bool IsVisible(void) const;
	virtual bool ShouldQuit(void) const;
	virtual void OnAppShutdown(void);
	virtual void SetState(int stateType, RunnableState* state);
	bool HasPixelShader2(void) const;
	// Returns filename written
	const char * TakeScreenshot(const char *prefix, const char *suffix);
	
	Ogre::SceneManager* GetSceneManager(void) const;
	const char* GetWorkingDirectory(void) const;
	
	Ogre::Root *root;
	Ogre::Camera* camera;
		Ogre::RenderWindow* window;
	Ogre::Viewport* viewport;
	// This is here because the math trig tables don't get initialize until Math is instantiated, even though the tables are static.
	Ogre::Math *math;
	//Ogre::SceneNode *gameplayNode;

protected:

	Ogre::SceneManager* sceneManager;
	void UpdateDefault(AppTime curTimeMS,AppTime elapsedTimeMS);
	virtual char * GetWindowTitle(void) const=0;

	bool isVisible;
	char workingDirectory[512];
	bool hasPixelShader2;
};

#endif
