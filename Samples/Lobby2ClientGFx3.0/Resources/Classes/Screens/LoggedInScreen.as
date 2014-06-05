import flash.external.*;
import gfx.controls.TextInput;
import gfx.controls.Button;

class Screens.LoggedInScreen extends Screen
{	
	private var logoffButton:Button;
	private var updateAccountButton:Button;
	private var ignoreFunctions:Button;
	private var friendFunctions:Button;
	private var emailFunctions:Button;
	private var clanFunctions:Button;
		
	public function LoggedInScreen() 
	{
		ConsoleWindow.Trace("Constructing LoggedInScreen");						
		
		mScreenId = ScreenID.LOGGED_IN;	
		mScreenTabId = ScreenTab.ID_LOGIN;
	}
	
	public function VOnFinishedLoading():Void
	{		
		//Add click event for buttons
		logoffButton.addEventListener("click", this, "f2c_Logoff");
		updateAccountButton.addEventListener("click", this, "GotoUpdateAccountState");
		ignoreFunctions.addEventListener("click", this, "GotoIgnoreFunctionsState");
		friendFunctions.addEventListener("click", this, "GotoFriendFunctionsState");
		emailFunctions.addEventListener("click", this, "GotoEmailFunctionsState");
		clanFunctions.addEventListener("click", this, "GotoClanFunctionsState");
				
		//Add callbacks for C++
		
		super.VOnFinishedLoading();
	}	
	
	public function f2c_Logoff():Void
	{
		ConsoleWindow.Trace("LoggedinScreen calling login");
		ExternalInterface.call("f2c_Logoff");
		//_root.gotoAndPlay("ConnectedToServer");
		LobbyInterface.Instance.SetLoginState( false );
		LobbyInterface.Instance.ShowScreen( ScreenID.LOGIN );
	}

	public function GotoUpdateAccountState():Void
	{
		//_root.gotoAndPlay("UpdateAccount");
		//LobbyInterface.Instance.ShowScreen( ScreenID.LOGIN ); //need to hook up update profile screen
		LobbyInterface.Instance.ShowScreen( ScreenID.UPDATE_ACCOUNT );
	}

	public function GotoIgnoreFunctionsState():Void
	{
		//_root.gotoAndPlay("IgnoreFunctions");
		LobbyInterface.Instance.ShowScreen( ScreenID.BLOCK_LIST );
	}

	public function GotoFriendFunctionsState():Void
	{
		//_root.gotoAndPlay("FriendFunctions");
		LobbyInterface.Instance.ShowScreen( ScreenID.FRIENDS );
	}

	public function GotoEmailFunctionsState():Void
	{
		//_root.gotoAndPlay("EmailFunctions");
		LobbyInterface.Instance.ShowScreen( ScreenID.EMAIL );
	}

	public function GotoClanFunctionsState():Void
	{
		//_root.gotoAndPlay("ClanFunctions");
		LobbyInterface.Instance.ShowScreen( ScreenID.CLAN_ROOT );
	}
}