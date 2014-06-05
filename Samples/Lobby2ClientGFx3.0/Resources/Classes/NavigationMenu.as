
import gfx.controls.Button;
import mx.transitions.easing.None;
import mx.transitions.Tween;


class NavigationMenu extends MovieClip
{	
	private var mcLogIn:Button;
	private var mcLogOut:Button;
	private var mcProfile:Button;
	private var mcFriends:Button;
	private var mcEmail:Button;
	private var mcClan:Button;
	private var mcExit:Button;
	
	private var mButtonAnimCt:Number;
	private var mButtons:Array;  //store buttons in array for sequencial animation
	
	public function NavigationMenu()
	{
		mButtons = new Array;
	}
	
	public function onLoad()
	{		
		mcLogIn.addEventListener("click", this, "OnClickedLogInButton");
		mcLogOut.addEventListener("click", this, "OnClickedLogoutButton");
		mcProfile.addEventListener("click", this, "OnClickedProfileButton");
		mcFriends.addEventListener("click", this, "OnClickedFriendsButton");
		mcEmail.addEventListener("click", this, "OnClickedEmailButton");
		mcClan.addEventListener("click", this, "OnClickedClanButton");
		mcExit.addEventListener("click", this, "OnClickedExitButton");
		
		mButtons.push( mcLogOut );
		mButtons.push( mcProfile );
		mButtons.push( mcFriends );
		mButtons.push( mcEmail );
		mButtons.push( mcClan );
		mButtons.push( mcExit );
		OnPlayerLoggedOut();
	}
	
	public function OnClickedLogInButton():Void
	{		
	}
	
	public function OnClickedLogoutButton():Void
	{
		if ( LobbyInterface.Instance.IsLoggedIn() )
		{
			LobbyInterface.Instance.GetScreen(ScreenID.LOGGED_IN)["f2c_Logoff"]();
		}
	}
	
	public function OnClickedProfileButton():Void
	{
		if ( LobbyInterface.Instance.IsLoggedIn() )
		{
			LobbyInterface.Instance.ShowScreen( ScreenID.PROFILE );
		}
	}
	
	public function OnClickedFriendsButton():Void
	{
		if ( LobbyInterface.Instance.IsLoggedIn() )
		{
			LobbyInterface.Instance.ShowScreen( ScreenID.FRIENDS );
		}
	}
	
	public function OnClickedEmailButton():Void
	{
		if ( LobbyInterface.Instance.IsLoggedIn() )
		{
			LobbyInterface.Instance.ShowScreen( ScreenID.EMAIL );
		}
	}
	
	public function OnClickedClanButton():Void
	{
		if ( LobbyInterface.Instance.IsLoggedIn() )
		{
			LobbyInterface.Instance.ShowScreen( ScreenID.CLAN_ROOT );
		}
	}
	
	public function OnClickedExitButton():Void
	{
		if ( LobbyInterface.Instance.IsLoggedIn() )
		{
			LobbyInterface.Instance.GetScreen(ScreenID.LOGGED_IN)["f2c_Logoff"]();		
		}
	}
	
	private var mLastTime:Number;
	public function OnPlayerLoggedIn():Void
	{
		mButtonAnimCt = 0;
		onEnterFrame = PlayIntroAnimation;
		mLastTime = getTimer();
		/*
		mcLogIn.visible = false;
		mcLogOut.visible = true;
		mcProfile.visible = true;
		mcFriends.visible = true;
		mcClan.visible = true;
		mcEmail.visible = true;
		mcFriends.visible = true;	
		mcExit.visible = true;	*/
		PlayIntroAnimation();
	}
	
	public function PlayIntroAnimation():Void
	{
		var now:Number = getTimer();
		var delta:Number = now - mLastTime;
		if ( delta > 100 )
		{
			mLastTime = now;
			//mButtons[mButtonAnimCt]._visible = true;
			new Tween( mButtons[mButtonAnimCt], "_alpha", None.easeNone, 0, 100, .5, true );
		//	new Tween( this, "_alpha", None.easeNone, 0, 100, .5, true );
			mButtonAnimCt++;
			if ( mButtonAnimCt >= mButtons.length )
			{
				onEnterFrame = null;
			}
		}
	}
	
	public function OnPlayerLoggedOut():Void
	{
		/*mcLogOut._alpha = 0;
		mcProfile._alpha = 0;
		mcFriends._alpha = 0;
		mcClan._alpha = 0;
		mcEmail._alpha = 0;
		mcFriends._alpha = 0;
		mcExit._alpha = 0;*/
		for ( var i:Number = 0; i < mButtons.length; i++ )
		{
			new Tween( mButtons[i], "_alpha", None.easeNone, 100, 0, .4, true );
		}
	/*	mcLogIn._alpha = true;
		mcLogOut.visible = false;
		mcProfile.visible = false;
		mcFriends.visible = false;
		mcClan.visible = false;
		mcEmail.visible = false;
		mcFriends.visible = false;
		mcExit.visible = false;*/
	}
}