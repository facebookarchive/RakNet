import flash.external.*;
import gfx.controls.TextInput;
import gfx.controls.Button;
import gfx.controls.CheckBox;
import mx.utils.Delegate;
import Screens.RecoverPasswordScreen;

class Screens.LoginScreen extends Screen
{	
	private var resetDatabase:Button;
	private var recoverPassword:Button;
	private var deleteAccountButton:Button;
	private var changeHandleButton:Button;
	private var registerAccount:Button;
	private var login:Button;
	private var disconnectFromServer:Button;
	private var userNameEdit:TextInput;
	private var passwordEdit:TextInput;
	private var changeHandleNewHandleEditBox:TextInput;
	private var savePassword:CheckBox;
	private var saveUsername:CheckBox;
	private var mcForgotUsername:MovieClip;
	private var mcForgotPassword:MovieClip;
	
	private var tfLoginFailed:TextField;
		
	public function LoginScreen() 
	{
		ConsoleWindow.Trace("Constructing LoginScreen");				
		
		mScreenId = ScreenID.LOGIN;		
		mScreenTabId = ScreenTab.ID_LOGIN;
		
		tfLoginFailed._visible = false;
	}
	
	public function VOnFinishedLoading():Void
	{
		var self:LoginScreen = this;
		
		//Add click event for buttons
		resetDatabase.addEventListener("click", this, "f2c_ResetDatabase");
		//mcForgotPassword.onPress = function() { self["recoverPasswordFunc"](); }
		//recoverPassword.addEventListener("click", this, "recoverPasswordFunc");
		deleteAccountButton.addEventListener("click", this, "f2c_DeleteAccount");
		changeHandleButton.addEventListener("click", this, "f2c_ChangeHandle");
		registerAccount.addEventListener("click", this, "f2c_RegisterAccountStateQuery");
		login.addEventListener("click", this, "f2c_LoginToAccount");
		disconnectFromServer.addEventListener("click", this, "f2c_DisconnectFromServer");
		mcForgotPassword.onPress = Delegate.create( this, ShowForgotPassword );
		mcForgotUsername.onPress = Delegate.create( this, ShowForgotUsername );
		
		//Add callbacks for C++
		ExternalInterface.addCallback("c2f_SetStateEnterCDKey", this, c2f_SetStateEnterCDKey);
		ExternalInterface.addCallback("c2f_SetStateRegisterAccount", this, c2f_SetStateRegisterAccount);
		ExternalInterface.addCallback("c2f_DeleteAccountResult", this, c2f_DeleteAccountResult);
		ExternalInterface.addCallback("c2f_ChangeHandleResult", this, c2f_ChangeHandleResult);
		ExternalInterface.addCallback("c2f_NotifyLoginResultFailure", this, c2f_NotifyLoginResultFailure);
		ExternalInterface.addCallback("c2f_NotifyLoginResultSuccess", this, c2f_NotifyLoginResultSuccess);
		
		super.VOnFinishedLoading();
	}
	
	public function GetUsername():String
	{
		return userNameEdit.text;
	}
	
	public function f2c_ResetDatabase():Void
	{
		ExternalInterface.call("f2c_ResetDatabase");
	}
	
	public function recoverPasswordFunc():Void
	{
		//_root.gotoAndPlay("RecoverPassword");
		LobbyInterface.Instance.ShowScreen( ScreenID.RECOVER_PASSWORD );
	}
	
	public function f2c_DeleteAccount():Void
	{
		ExternalInterface.call("f2c_DeleteAccount", userNameEdit.text, passwordEdit.text);
	}
	
	public function f2c_ChangeHandle():Void
	{
		ExternalInterface.call("f2c_ChangeHandle", userNameEdit.text, passwordEdit.text, changeHandleNewHandleEditBox.text);
	}

	public function f2c_RegisterAccountStateQuery():Void
	{
		ExternalInterface.call("f2c_RegisterAccountStateQuery", userNameEdit.text, passwordEdit.text);
		// C++ will call either c2f_SetStateEnterCDKey or c2f_SetStateCreateAccount1
	}
	
	public function f2c_LoginToAccount():Void
	{
		tfLoginFailed._visible=false;
		ExternalInterface.call("f2c_LoginToAccount", userNameEdit.text, passwordEdit.text, savePassword.selected);
		//_root.gotoAndPlay("LoggingIn");
		//LobbyInterface.Instance.ShowScreen( ScreenID.LOGGING_IN );
		if ( LobbyInterface.Instance.IsInFlashMode() )
		{
			c2f_NotifyLoginResultSuccess();
		}
	}
	
	public function f2c_DisconnectFromServer():Void
	{
		ExternalInterface.call("f2c_DisconnectFromServer");
	}
	
	public function c2f_SetStateEnterCDKey():Void
	{
		//_root.gotoAndPlay("EnterCDKey");
		LobbyInterface.Instance.ShowScreen( ScreenID.ENTER_CD_KEY );
	}
		
	public function c2f_SetStateRegisterAccount():Void
	{
		//_root.gotoAndPlay("RegisterAccount");
		LobbyInterface.Instance.ShowScreen( ScreenID.REGISTER_ACCOUNT_PERSONAL );
	}
	
	public function c2f_DeleteAccountResult(resultIdentifier:String):Void
	{
		switch (resultIdentifier)
		{
			case "UNKNOWN_USER":
			// Unknown username
			break;
			case "SUCCESS":
			// OK
			break;
			case "DATABASE_CONSTRAINT_FAILURE":
			// Database error, maybe account was already deleted
			break;
			case "System_DeleteAccount_INVALID_PASSWORD":
			// Bad password to delete this account
			break;
		}
		
		if ( resultIdentifier != "SUCCESS" )
		{
			LobbyInterface.Instance.CreateMessageBox( resultIdentifier );
		}
	}
		
	public function c2f_ChangeHandleResult(resultIdentifier:String, oldHandle:String, newHandle:String):Void
	{
		switch (resultIdentifier)
		{
			case "UNKNOWN_USER":
			// Unknown username
			break;
			case "SUCCESS":
			{
				// OK
				userNameEdit.text=newHandle;
				break;
			}
			case "DATABASE_CONSTRAINT_FAILURE":
			// Database error, maybe account was already deleted
			break;
			case "Client_ChangeHandle_NEW_HANDLE_ALREADY_IN_USE":
			break;
			case "Client_ChangeHandle_INVALID_PASSWORD":
			// Bad password to delete this account
			break;
		}
		
		if ( resultIdentifier != "SUCCESS" )
		{
			LobbyInterface.Instance.CreateMessageBox( resultIdentifier );
		}
	}
	
	public function c2f_NotifyLoginResultFailure(reasonIdentifier:String, bannedReason:String, whenBanned:String, bannedExpiration:String ):Void
	{
		switch (reasonIdentifier)
		{
			case "Client_Login_HANDLE_NOT_IN_USE_OR_BAD_SECRET_KEY":
				tfLoginFailed.text = "Handle not in use or bad secret key.";
			break;
			case "Client_Login_CANCELLED":
				tfLoginFailed.text = "Login cancelled.";
			break;
			case "Client_Login_CABLE_NOT_CONNECTED":
				tfLoginFailed.text = "Cable not connected.";
			break;
			case "Client_Login_NET_NOT_CONNECTED":
				tfLoginFailed.text = "Net not connected.";				
			break;
			case "Client_Login_BANNED":
				tfLoginFailed.text = "User banned.";
			// banned parameters used here, not otherwise
			break;
			case "Client_Login_CDKEY_STOLEN":
				tfLoginFailed.text = "CDKEY stolen";
			break;
			case "Client_Login_EMAIL_ADDRESS_NOT_VALIDATED":
				tfLoginFailed.text = "Email address not validated.";
			break;
			case "Client_Login_BAD_TITLE_OR_TITLE_SECRET_KEY":
				tfLoginFailed.text = "Bad title or title secret key.";
			break;
		}
				
		//tfLoginFailed._visible = true;
		
		LobbyInterface.Instance.CreateMessageBox(reasonIdentifier);
	}

	public function c2f_NotifyLoginResultSuccess( ):Void
	{
		LobbyInterface.Instance.SetLoginState( true );
		LobbyInterface.Instance.ShowScreen( ScreenID.PROFILE );
		ConsoleWindow.Trace("AccountInfo.Instance = " + AccountInfo.Instance);
		AccountInfo.Instance.GetAccountInfoFromServer();
	}
	
	public function ShowForgotUsername():Void
	{
		ConsoleWindow.Trace("show forgot username");
		RecoverPasswordScreen.Instance.SetMode( false );
		LobbyInterface.Instance.ShowScreen( ScreenID.RECOVER_PASSWORD );
	}
	
	public function ShowForgotPassword():Void
	{
		ConsoleWindow.Trace("show forgot password");
		RecoverPasswordScreen.Instance.SetMode( true );
		LobbyInterface.Instance.ShowScreen( ScreenID.RECOVER_PASSWORD );		
	}
}