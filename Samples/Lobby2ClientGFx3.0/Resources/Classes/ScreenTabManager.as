/**
* October 21, 2009
* @author Dezhu Chen
*/

import mx.transitions.easing.None;
import mx.transitions.Tween;

class ScreenTabManager
{	
	private static var mSingletonInstance:ScreenTabManager;
	
	private var mScreenTabs:Array;
	
	public function ScreenTabManager() 
	{
		mSingletonInstance = this;
		
		mScreenTabs = new Array();
	}	
				
	public static function get Instance():ScreenTabManager
	{
		return mSingletonInstance;
	}
	
	public function AddScreenTab( tab:ScreenTab ):Void
	{
		mScreenTabs[tab.tabID] = tab;
	}
	
	public function ShowScreen( _newScreenId:Number ):Void
	{
		var curScreen:Screen = LobbyInterface.Instance.GetCurrentScreen();
		var newScreen:Screen = LobbyInterface.Instance.GetScreen( _newScreenId );
		
		var curTab:ScreenTab = mScreenTabs[curScreen.GetScreenTabId()];
		var newTab:ScreenTab = mScreenTabs[newScreen.GetScreenTabId()];
		
		if ( curTab != newTab )
		{
			newTab._visible = true;			
			ConsoleWindow.Trace("Hiding " + curTab.tabID + ", showing " + newTab.tabID);
			
			new Tween( curTab, "_x", None.easeNone, curTab._x, curTab.GetStartingPos(), .5, true );
			curScreen.Hide( true, curTab.GetStartingPos() );
			new Tween( newTab, "_x", None.easeNone, newTab._x, ScreenTab.cExtendedPosX, .5, true );
			newScreen.Show( true, newTab._x, ScreenTab.cExtendedPosX );
		}
		else if ( curScreen.GetScreenId != _newScreenId )
		{
			curScreen.Hide( false, curTab.GetStartingPos() );
			newScreen.Show( false );
		}
	}
}