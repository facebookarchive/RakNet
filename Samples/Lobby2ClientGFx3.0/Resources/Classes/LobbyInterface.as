import flash.external.*;

class LobbyInterface extends MovieClip
{
	private static var mSingletonInstance:LobbyInterface;
	
	private var mScreens:Array;
	private var mCurScreenID:Number;	
	private var mLastScreenID:Number;
	private var mScreensRegistered:Number;
	private var mIsLoggedIn:Boolean;
	private var mScreenTabManager:ScreenTabManager;
	private var mcNavigationPanel:NavigationMenu;
	
	private var mFlashMode:Boolean = false;
	
	//====================================================
	//User data
	private var mProfileImageIndex:Number;
	private var mAccountInfo:AccountInfo;
	
	public function LobbyInterface()
	{
		ConsoleWindow.Trace("Constructing LobbyInterface");
		
		mSingletonInstance = this;
		mScreens = new Array();
		mCurScreenID = 0;
		mScreensRegistered = 0;
		mProfileImageIndex = 1;  //TODO: get this from c++
		
		mScreenTabManager = new ScreenTabManager();
		
		onLoad = Initialize;
	}
	
	//All screens should be constructed by now, but not neccessarily loaded
	public function Initialize():Boolean
	{
		ConsoleWindow.Trace("LobbyInterface.Initialize()....");
		mAccountInfo = new AccountInfo();
		
		ExternalInterface.addCallback("c2f_NotifyConnectionLost", this, c2f_NotifyConnectionLost);
		ExternalInterface.addCallback("c2f_GetAccountDetailsResult", mAccountInfo, ServerSetAccountInfo);		
		return true;
	}	
	
	public function blah():Void
	{
		ConsoleWindow.Trace("blah..");
	}
		
	public static function get Instance():LobbyInterface
	{
		return mSingletonInstance;
	}
	
	public function GetCurrentScreen():Screen
	{
		return mScreens[mCurScreenID];
	}
	
	public function GetScreen( _screenID:Number ):Screen
	{
		return mScreens[_screenID];
	}
	
	//TODO: get this from c++
	//player might get to a page that has a profile image before seeing this screen so we have to make sure this is loaded from c++ first
	public function GetProfileImageIndex():Number
	{
		return mProfileImageIndex;
	}
	
	public function GetUsername():String
	{
		if ( mIsLoggedIn )
		{
			return mScreens[ScreenID.LOGIN].GetUsername();
		}
		
		return null;
	}
	
	public function SetLoginState( _state:Boolean ):Void
	{
		if ( mIsLoggedIn != _state )
		{
			mIsLoggedIn = _state;
			
			if ( mIsLoggedIn )
			{
				mcNavigationPanel.OnPlayerLoggedIn();
			}			
			else
			{
				mcNavigationPanel.OnPlayerLoggedOut();
			}
		}
	
	}
	
	public function IsLoggedIn():Boolean
	{
		return mIsLoggedIn;
	}
		
	public function RegisterScreen( _screenID:Number, _screen ):Void
	{
		mScreens[_screenID] = _screen;		
		mScreensRegistered++;
		//trace("Registering screen: " + this[ ScreenID.GetScreenName(_screenID) ] );
		ConsoleWindow.Trace("Registering screen: " + mScreens[_screenID] );
		
		//All screens are done loading
		if ( mScreensRegistered == ScreenID.TOTAL_SCREENS )
		{
			ConsoleWindow.Trace( "Finished registering all " + mScreensRegistered + " screens!" );
			
			ShowScreen( ScreenID.CONNECTION );			
			//GetCurrentScreen()["f2c_Connect"]();
		}
	}
	
	public function ShowScreen( _id:Number ):Void
	{
		//mScreens[mCurScreenID].Hide();
		
		mScreenTabManager.ShowScreen( _id );
		
		mLastScreenID = mCurScreenID;
		mCurScreenID = _id;
		//mScreens[mCurScreenID].Show();
	}
	
	public function GetLastScreenId():Number
	{
		return mLastScreenID;
	}
	
	public function ShowLastScreen():Void
	{
		ShowScreen( mLastScreenID );
	}
	
	public function CreateMessageBox( message:String, callback:Function ):Void
	{		
		var messageBox = attachMovie("MessageBox", "MessageBox" + getNextHighestDepth(), getNextHighestDepth() );
		messageBox.SetMessage( message, callback );
	}
	
	public function IsInFlashMode():Boolean		{ return mFlashMode; }
	
	//===============================================================================================================================
	//Functions called by C++
	public function c2f_NotifyConnectionLost(resultIdentifier:String):Void
	{
		switch (resultIdentifier)
		{
			case "LCR_CLOSED_BY_USER":
				// Programmer closed the connection to the server intentionally
			break;
			case "LCR_DISCONNECTION_NOTIFICATION":
				// Server disconnected us intentionally
			break;
			case "LCR_CONNECTION_LOST":
				// Lost connection to the server
			break;
		}

		// If the user is in any state where you have to be connected (anything other than Disconnected and ConnectingToServer)
		// And you get this callback, then show an error dialog, immediately leave whatever screen they are on once they click ok,
		// and go to the disconnected state
		//gotoAndStop("Disconnected");
		ShowScreen(ScreenID.CONNECTION);
	}
}