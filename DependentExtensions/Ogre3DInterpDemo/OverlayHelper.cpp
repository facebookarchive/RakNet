/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

#include "OverlayHelper.h"
#include "OgreTextAreaOverlayElement.h"
#include "OgrePanelOverlayElement.h"
#include "OgreOverlayManager.h"
#include "OgreOverlayElementFactory.h"


using namespace Ogre;

OverlayHelper::TimedOverlay::TimedOverlay()
{
}
OverlayHelper::TimedOverlay::~TimedOverlay()
{
}
OverlayHelper::TimedOverlay::TimedOverlay(OverlayElement *overlayElement, unsigned int totalTime, unsigned int fadeTimeMS, float finalAlpha, bool deleteAfterFade)
{
	startFadeAlpha=-1.0f;
	this->overlayElement=overlayElement;
	this->remainingTimeMS=totalTime;
	this->fadeTimeMS=fadeTimeMS;
	this->finalAlpha=finalAlpha;
	this->deleteAfterFade=deleteAfterFade;
}
OverlayHelper::OverlayHelper()
{
	globalOverlay=0;
}
OverlayHelper::~OverlayHelper()
{

}
void OverlayHelper::Startup(void)
{
	globalOverlay = OverlayManager::getSingleton().create("OverlayHelperRoot");
	globalOverlay->show();
}
void OverlayHelper::Shutdown(void)
{
	timedOverlays.Clear(false, _FILE_AND_LINE_ );
	if (globalOverlay)
		OverlayManager::getSingleton().destroy(globalOverlay);
}
void OverlayHelper::Update(unsigned int elapsedTimeMS)
{
	unsigned i;
	i=0;
	while (i < timedOverlays.Size())
	{
		if (timedOverlays[i].remainingTimeMS < elapsedTimeMS)
		{
			if (timedOverlays[i].deleteAfterFade)
			{
				SafeDestroyOverlayElement(timedOverlays[i].overlayElement);
			}
			else
			{
				const ColourValue &color = timedOverlays[i].overlayElement->getColour();
				ColourValue newColor = color;
				newColor.a=timedOverlays[i].finalAlpha;
				timedOverlays[i].overlayElement->setColour(newColor);
				timedOverlays.RemoveAtIndex(i);
			}			
		}
		else
		{
			timedOverlays[i].remainingTimeMS-=elapsedTimeMS;
			if (timedOverlays[i].remainingTimeMS < timedOverlays[i].fadeTimeMS)
			{
				const ColourValue &color = timedOverlays[i].overlayElement->getColour();
				if (timedOverlays[i].startFadeAlpha==-1.0f)
					timedOverlays[i].startFadeAlpha=color.a;
				ColourValue newColor = color;
				newColor.a=timedOverlays[i].finalAlpha - (timedOverlays[i].finalAlpha-timedOverlays[i].startFadeAlpha) * (float) timedOverlays[i].remainingTimeMS / (float) timedOverlays[i].fadeTimeMS;
				timedOverlays[i].overlayElement->setColour(newColor);
			}
			i++;
		}		
	}
}
Overlay* OverlayHelper::GetGlobalOverlay(void) const
{
	return globalOverlay;
}
void OverlayHelper::FadeOverlayElement(OverlayElement* element, unsigned int totalTime, unsigned int fadeTimeMS, float finalAlpha, bool deleteAfterFade)
{
	timedOverlays.Insert(TimedOverlay(element, totalTime, fadeTimeMS, finalAlpha,deleteAfterFade), _FILE_AND_LINE_ );
}
OverlayContainer* OverlayHelper::CreatePanel(const char *instanceName, bool addToGlobalOverlay)
{
	OverlayContainer* element = (OverlayContainer*) OverlayManager::getSingleton().createOverlayElement("Panel", instanceName);
	if (addToGlobalOverlay)
		globalOverlay->add2D(element);
	return element;
}
TextAreaOverlayElement *OverlayHelper::CreateTextArea(const char *instanceName, const char *fontName, OverlayContainer* parent)
{
	TextAreaOverlayElement *element = (TextAreaOverlayElement *) OverlayManager::getSingleton().createOverlayElement("TextArea", instanceName);
	if (parent)
		parent->addChild(element);
	element->setFontName(fontName);
	return element;
}
BorderPanelOverlayElement *OverlayHelper::CreateBorderPanel(const char *instanceName, OverlayContainer* parent)
{
	BorderPanelOverlayElement *element = (BorderPanelOverlayElement *) OverlayManager::getSingleton().createOverlayElement("BorderPanel", instanceName);
	if (parent)
		parent->addChild(element);
	return element;
}
void OverlayHelper::SafeDestroyOverlayElement(OverlayElement *item)
{
	if (item->isContainer())
	{
		OverlayContainer *container = (OverlayContainer*) item;

		// Arrggghh the variable is protected
		// ((OverlayContainer*)item)->mOverlay->remove2D((OverlayContainer*)item);
		OverlayManager::OverlayMapIterator iter1 = OverlayManager::getSingleton().getOverlayIterator();
		while (iter1.hasMoreElements())
		{
			iter1.getNext()->remove2D(container);
		}

		OverlayContainer::ChildIterator iter2 = container->getChildIterator();
		while (iter2.hasMoreElements())
		{
			iter2.getNext()->_setParent(0);
		}

		OverlayContainer::ChildContainerIterator iter3 = container->getChildContainerIterator();
		while (iter3.hasMoreElements())
		{
			iter3.getNext()->_setParent(0);
		}
	}

	if (item->getParent())
		item->getParent()->removeChild(item->getName());
	OverlayManager::getSingleton().destroyOverlayElement(item);
	unsigned i;
	i=0;
	while (i < timedOverlays.Size())
	{
		if (timedOverlays[i].overlayElement==item)
			timedOverlays.RemoveAtIndex(i);
		else
			i++;
	}
}
