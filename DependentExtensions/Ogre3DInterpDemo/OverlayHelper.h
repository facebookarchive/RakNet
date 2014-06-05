/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

#ifndef __OVERLAY_HELPER_H
#define __OVERLAY_HELPER_H

#include "DS_List.h"

namespace Ogre
{
	class RenderWindow;
	class SceneManager;
	class OverlayContainer;
	class TextAreaOverlayElement;
	class BorderPanelOverlayElement;
	class OverlayElement;
	class Overlay;
}

// This classe makes it easier to use Ogre's overlay system.  It provides the ability to fade overlays in and out and to automatically delete them
// after time
class OverlayHelper
{
public:
	OverlayHelper();
	~OverlayHelper();
	void Startup(void);
	void Shutdown(void);
	void Update(unsigned int elapsedTimeMS);

	// Just returns a global overlay that I store.  Useful functions on it are hide() and show()
	Ogre::Overlay* GetGlobalOverlay(void) const;

	// Fades an overlay element to some designated final alpha.  You can autodelete the overlay after fading as well.
	void FadeOverlayElement(Ogre::OverlayElement* element, unsigned int totalTime, unsigned int fadeTimeMS, float finalAlpha, bool deleteAfterFade);

	// Equivalent to Ogre's function.  All OverlayElements must be a child of a panel.
	Ogre::OverlayContainer* CreatePanel(const char *instanceName, bool addToGlobalOverlay=true);

	// Displays a single line of text.  Doesn't handle text clipping or wrapping.
	Ogre::TextAreaOverlayElement *CreateTextArea(const char *instanceName, const char *fontName, Ogre::OverlayContainer* parent);

	// Equivalent to Ogre's function.
	Ogre::BorderPanelOverlayElement *CreateBorderPanel(const char *instanceName, Ogre::OverlayContainer* parent);

	// Destroy any overlay created with the above.
	// Safer because it removes the element from its children and parent.
	void SafeDestroyOverlayElement(Ogre::OverlayElement *item);

	// For internal use
	struct TimedOverlay
	{
		TimedOverlay();
		~TimedOverlay();
		TimedOverlay(Ogre::OverlayElement *overlayElement, unsigned int totalTime, unsigned int fadeTimeMS, float finalAlpha, bool deleteAfterFade);
		Ogre::OverlayElement *overlayElement;
		unsigned int remainingTimeMS;
		unsigned int fadeTimeMS;
		float finalAlpha;
		float startFadeAlpha;
		bool deleteAfterFade;
	};

protected:
	// A list of timed text elements with fade.
	// setColour
	DataStructures::List<TimedOverlay> timedOverlays;
	Ogre::Overlay* globalOverlay;
	unsigned int fadeTimeMSMS;
};

#endif
