import flash.external.*;
import gfx.controls.TextInput;
import gfx.controls.Button;

class Screens.ConnectingToServerScreen extends Screen
{	
	private var failureBackButton:Button;		
		
	public function ConnectingToServerScreen() 
	{
		ConsoleWindow.Trace("Constructing ConnectingToServerScreen");						
		
		mScreenId = ScreenID.CONNECTING_TO_SERVER;	
		mScreenTabId = ScreenTab.ID_CONNECTION;
	}
	
	public function VOnFinishedLoading():Void
	{		
		failureBackButton.visible=false;

		//Add click event for buttons
		failureBackButton.addEventListener("click", this, "onPressedFailureBack");
				
		
		super.VOnFinishedLoading();
	}
	

	function onPressedFailureBack():Void
	{
		//_root.gotoAndPlay("Disconnected");
		LobbyInterface.Instance.ShowScreen( ScreenID.CONNECTION );
	}

}