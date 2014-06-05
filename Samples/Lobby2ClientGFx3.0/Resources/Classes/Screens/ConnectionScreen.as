import flash.external.*;
import gfx.controls.TextInput;
import gfx.controls.Button;
import gfx.controls.ProgressBar;
import mx.transitions.easing.None;
import mx.transitions.Tween;

class Screens.ConnectionScreen extends Screen
{
	private var mcConnect:Button;
	private var mcCancel:Button;
	//private var mcProgress:ProgressBar;
	private var mcProgressBar:MovieClip;
	
	private var tfConnectionFailed:TextField;
	
	private var tfAddress:TextField;
	private var tfPort:TextField;
	
	public function ConnectionScreen() 
	{
		ConsoleWindow.Trace("Constructing ConnectionScreen");							
		
		mScreenId = ScreenID.CONNECTION;
		mScreenTabId = ScreenTab.ID_CONNECTION;
		tfConnectionFailed._visible = false;			
	}
	
	public function VOnFinishedLoading():Void
	{				
		mcConnect.addEventListener("click", this, "f2c_Connect");
		
		//Add callbacks for C++
		ExternalInterface.addCallback("c2f_NotifyConnectingToServer", this, c2f_NotifyConnectingToServer);			
		ExternalInterface.addCallback("c2f_NotifyConnectionResultFailure", this, c2f_NotifyConnectionResultFailure);
		ExternalInterface.addCallback("c2f_NotifyConnectionResultSuccess", this, c2f_NotifyConnectionResultSuccess);
				
		super.VOnFinishedLoading();
	}
	
	public function OnShow():Void
	{
		//ConsoleWindow.Trace( mcProgressBar.mcBar);
		mcProgressBar.mcBar._xscale = 0;
	}
		
	public function f2c_Connect():Void
	{		
		new Tween( mcProgressBar.mcBar, "_xscale", None.easeNone, 0, 100, 6, true );	
		//mcProgress.setProgress( 2, 6 );
		ConsoleWindow.Trace("connecting to server");
		//ExternalInterface.call("f2c_Connect", "127.0.0.1", "60481");
		ExternalInterface.call("f2c_Connect", tfAddress.text, tfPort.text);
		
		if ( LobbyInterface.Instance.IsInFlashMode() )
		{
			LobbyInterface.Instance.ShowScreen( ScreenID.LOGIN );
		}
	}

	public function c2f_NotifyConnectingToServer():Void
	{
		//_root.gotoAndPlay("ConnectingToServer");
		//LobbyInterface.Instance.ShowScreen( ScreenID.CONNECTING_TO_SERVER );
	}
	
	function c2f_NotifyConnectionResultFailure(resultIdentifier:String):Void
	{
		switch (resultIdentifier)
		{
			case "CONNECTION_ATTEMPT_FAILED":
			break;
			case "ALREADY_CONNECTED":
			break;
			case "NO_FREE_INCOMING_CONNECTIONS":
			break;
			case "RSA_PUBLIC_KEY_MISMATCH":
			break;
			case "CONNECTION_BANNED":
			break;
			case "INVALID_PASSWORD":
			break;
		}
		
		tfConnectionFailed.text=resultIdentifier;
		tfConnectionFailed._visible=true;
	}
	
	function c2f_NotifyConnectionResultSuccess():Void
	{
		ConsoleWindow.Trace("ConnectionScreen... calling login");
		LobbyInterface.Instance.ShowScreen( ScreenID.LOGIN );
	}
}