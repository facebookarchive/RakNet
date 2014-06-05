import flash.external.*;
import gfx.controls.TextInput;
import gfx.controls.Button;

class Screens.RecoverPasswordScreen extends Screen
{	
	private var userNameEdit:TextInput;	
	private var recoverPasswordByUsername:Button;		
	private var goBackToConnectedToServer:Button;
	
	private var bForgotPassword:Boolean;
	private var btnBack:Button;
	
	private var btnSendEmail:Button;
	private var btnSendUsername:Button;
	private var btnSendAnswer:Button;
	
	private static var mInstance:RecoverPasswordScreen;
		
	public function RecoverPasswordScreen() 
	{
		ConsoleWindow.Trace("Constructing RecoverPasswordScreen");							
		
		mScreenId = ScreenID.RECOVER_PASSWORD;			
		mScreenTabId = ScreenTab.ID_LOGIN;
		
		mInstance = this;
	}
	
	public static function get Instance():RecoverPasswordScreen
	{
		return mInstance;
	}
	
	public function VOnFinishedLoading():Void
	{		
		//Add click event for buttons
		goBackToConnectedToServer.addEventListener("click", this, "goBackToConnectedToServerFunc");
		btnBack.addEventListener("click", this, "Back");
		btnSendUsername.addEventListener("click", this, "f2c_RecoverPasswordByUsername");
		btnSendAnswer.addEventListener("click", this, "f2c_GetPasswordByPasswordRecoveryAnswer");
				
		//Add callbacks for C++
		ExternalInterface.addCallback("c2f_RecoverPasswordByUsername", this, c2f_RecoverPasswordByUsername);
		ExternalInterface.addCallback("c2f_GetPasswordByPasswordRecoveryAnswer", this, c2f_GetPasswordByPasswordRecoveryAnswer);
		
		super.VOnFinishedLoading();
	}
	
	public function SetMode( forgotPassword:Boolean ):Void
	{
		ConsoleWindow.Trace("SetMode..." + forgotPassword);
		bForgotPassword = forgotPassword;
	}
	
	public function OnShow():Void
	{
		if ( bForgotPassword )
		{
			btnSendEmail._visible = false;
			btnSendUsername._visible = true;
			btnSendAnswer._visible = false;
			gotoAndStop("Password1");
		}
		else
		{
			btnSendEmail._visible = true;
			btnSendUsername._visible = false;
			btnSendAnswer._visible = false;
			gotoAndStop("Username");
		}
	}
	
	//gets user recovery question 
	private var mTempUsername:String;
	public function f2c_RecoverPasswordByUsername():Void
	{
		mTempUsername = this["tiUserName"].text;
		//ConsoleWindow.Trace("f2c_RecoverPasswordByUsername.... username = " + );
		ExternalInterface.call("f2c_RecoverPasswordByUsername", mTempUsername);
	}
	
	//try to get password by answering the secret question
	public function f2c_GetPasswordByPasswordRecoveryAnswer():Void
	{
		ConsoleWindow.Trace("calling f2c_GetPasswordByPasswordRecoveryAnswer.. answer = " + this["tiAnswer"].text);
		ExternalInterface.call("f2c_GetPasswordByPasswordRecoveryAnswer", mTempUsername, this["tiAnswer"].text);
	}

	public function goBackToConnectedToServerFunc():Void
	{
		//_root.gotoAndPlay("ConnectedToServer");
		LobbyInterface.Instance.ShowScreen( ScreenID.LOGIN );
	}

	public function c2f_RecoverPasswordByUsername(resultIdentifier:String, username:String, email:String, secretQuestion:String ):Void
	{
		ConsoleWindow.Trace("c2f_RecoverPasswordByUsername..." + arguments);
		
		switch (resultIdentifier)
		{
			case "SUCCESS":
				gotoAndStop("Password2");
				btnSendEmail._visible = false;
				btnSendUsername._visible = false;
				btnSendAnswer._visible = true;
				this["tfSecretQuestion"].text = secretQuestion;
			break;
		}
		
		if ( resultIdentifier != "SUCCESS" )
		{
			LobbyInterface.Instance.CreateMessageBox( resultIdentifier );
		}
	}

	public function c2f_GetPasswordByPasswordRecoveryAnswer(resultIdentifier:String, username:String, secretAnswer:String, password:String ):Void
	{
		ConsoleWindow.Trace("c2f_GetPasswordByPasswordRecoveryAnswer..." + arguments);
		
		switch (resultIdentifier)
		{
			case "SUCCESS":
			break;
		}
		
		if ( resultIdentifier != "SUCCESS" )
		{
			LobbyInterface.Instance.CreateMessageBox( resultIdentifier );
		}
	}
	
	public function Back():Void
	{
		LobbyInterface.Instance.ShowScreen( ScreenID.LOGIN );
	}
}