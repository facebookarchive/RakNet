import flash.external.*;
import gfx.controls.TextInput;
import gfx.controls.Button;
import gfx.controls.CheckBox;
import gfx.data.dataProvider;
import gfx.controls.DropdownMenu;
import Screens.ProfileScreen;
import Screens.RegisterAccountScreen;
import Screens.RegisterAccountScreen2;

import mx.utils.Delegate;

//Login Infomation
class Screens.RegisterAccountScreen3 extends Screen
{	
	private static var mInstance:RegisterAccountScreen3;
	
	private var password:TextInput;
	private var passwordConfirm:TextInput;
	private var passwordRecoveryQuestion:DropdownMenu;
	private var passwordRecoveryAnswer:TextInput;
	private var ageInDays:TextInput;
	private var handle:TextInput;	
	private var mcCheckUsername:MovieClip;
	private var mcCheckmark:MovieClip;
	private var tfRegistrationStatus:TextField;
		
	private var validateHandleButton:Button;
	private var registerAccountButton:Button;
	private var updateAccountButton:Button;
	private var cancelButton:Button;
	
	private var SecretQuestionList:Array = [ "Your city of birth?", "Your mother's middle name?", "Your first job title?", "Your favorite college restaurant?" ];
									 
		
	public function RegisterAccountScreen3() 
	{
		mInstance = this;
		ConsoleWindow.Trace("Constructing RegisterAccountScreen3");				
		
		mScreenId = ScreenID.REGISTER_ACCOUNT_LOGIN;
		mScreenTabId = ScreenTab.ID_REGISTRATION;
	}
	
	public static function get Instance():RegisterAccountScreen3 { return mInstance; }
	
	public function VOnFinishedLoading():Void
	{		
		passwordRecoveryQuestion.dataProvider = SecretQuestionList;
		mcCheckUsername.onPress = Delegate.create( this, CheckUsername );
		
		//Add click event for buttons
		validateHandleButton.addEventListener("click", this, "f2c_ValidateHandle"); 
		cancelButton.addEventListener("click", this, "Back");
		registerAccountButton.addEventListener("click", this, "f2c_RegisterAccount");
		updateAccountButton.addEventListener("click", this, "f2c_UpdateAccount");
		
		//Add callbacks for C++
		ExternalInterface.addCallback("c2f_ValidateHandleResult", this, c2f_ValidateHandleResult);
		ExternalInterface.addCallback("c2f_RegisterAccountResult", this, c2f_RegisterAccountResult);
		ExternalInterface.addCallback("c2f_UpdateAccountResult", this, c2f_UpdateAccountResult);
		
		super.VOnFinishedLoading();
	}
	
	public function OnShow():Void
	{		
		if ( LobbyInterface.Instance.IsLoggedIn() )
		{
			gotoAndStop("Update");	
			PopulateFromAccountInfo();
			registerAccountButton._visible = false;
			updateAccountButton._visible = true;
		}
		else
		{
			gotoAndStop("New");
			registerAccountButton._visible = true;
			updateAccountButton._visible = false;
		}
		
		mcCheckmark.gotoAndStop("Hide");
		tfRegistrationStatus._visible = false;		
	}
	
	public function CheckUsername():Void
	{
		ConsoleWindow.Trace( "checking username: " + handle.text);
		f2c_ValidateHandle();
	}
	
	public function Back():Void
	{
		if ( LobbyInterface.Instance.IsLoggedIn() )
		{	
			SaveToAccountInfo();
		}
		LobbyInterface.Instance.ShowScreen( ScreenID.REGISTER_ACCOUNT_BILLING );		
	}
	
	public function f2c_RegisterAccount():Void
	{
		if ( password.text != passwordConfirm.text )
		{
			LobbyInterface.Instance.CreateMessageBox( "ERROR: Bad Confirm Password field." );
		}
		else
		{
			tfRegistrationStatus._visible = false;
			
			var registrationScreen1 = LobbyInterface.Instance.GetScreen( ScreenID.REGISTER_ACCOUNT_PERSONAL );	
			var registrationScreen2 = LobbyInterface.Instance.GetScreen( ScreenID.REGISTER_ACCOUNT_BILLING );	
					
			// Do not change the order the parameters are passed in as
			ExternalInterface.call("f2c_RegisterAccount",
			[
			 registrationScreen1.GetFirstName(), registrationScreen1.GetMiddleInitial(), registrationScreen1.GetLastName(),
			 registrationScreen1.GetSelectedRace(), registrationScreen1.GetIsMale(), registrationScreen1.GetStreetAddress(),
			 registrationScreen1.GetStreetAddress2(), registrationScreen1.GetCity(), registrationScreen1.GetSelectedState(),
			 "United States",//countryList[homeCountry.selectedIndex]
			 registrationScreen1.GetZipCode(), 
			 registrationScreen2.GetStreetAddress1(),
			 registrationScreen2.GetStreetAddress2(), registrationScreen2.GetCity(), registrationScreen2.GetSelectedState(),
			 "United States",// countryList[billingCountry.selectedIndex], 
			 registrationScreen2.GetZipCode(), registrationScreen1.GetEmail(),
			 password.text, GetSelectedSecretQuestion(), passwordRecoveryAnswer.text,
			 "", "", 
			 String(registrationScreen1.GetAgeInDays()),
			 handle.text,
			 "about me....",
			 "activities...",
			 "interests...",
			 "fav games...",
			 "fav movies...",
			 "fav books...",
			 "fav quotes..."
			 ]);		 
		}
		 /*
		ExternalInterface.call("f2c_RegisterAccount",
		[
		 "first name", "m", "lastname",
		 registrationScreen1.GetSelectedRace(), registrationScreen1.GetIsMale(), registrationScreen1.GetStreetAddress(),
		 registrationScreen2.GetStreetAddress(), registrationScreen1.GetCity(), registrationScreen1.GetSelectedState(),
		 "Unknown Country",//countryList[homeCountry.selectedIndex]
		 registrationScreen1.GetZipCode(), 
		 registrationScreen2.GetStreetAddress1(),
		 registrationScreen2.GetStreetAddress2(), registrationScreen2.GetCity(), registrationScreen2.GetSelectedState(),
		 "Unknown Country",// countryList[billingCountry.selectedIndex], 
		 registrationScreen2.GetZipCode(), registrationScreen1.GetEmail(),
		 password.text, GetSelectedSecretQuestion(), passwordRecoveryAnswer.text,
		 "", "", "Age in days?",
		 handle.text
		 ]);*/
	}
	
	public function f2c_UpdateAccount():Void
	{		
		if ( password.text != passwordConfirm.text )
		{
			LobbyInterface.Instance.CreateMessageBox( "ERROR: Bad Confirm Password field." );
		}
		else
		{
			tfRegistrationStatus._visible = false;
			
			var registrationScreen1 = LobbyInterface.Instance.GetScreen( ScreenID.REGISTER_ACCOUNT_PERSONAL );	
			var registrationScreen2 = LobbyInterface.Instance.GetScreen( ScreenID.REGISTER_ACCOUNT_BILLING );	
			
		/*	ConsoleWindow.Trace("update account : ");
			ConsoleWindow.Trace("first name = " + registrationScreen1.GetFirstName());
			ConsoleWindow.Trace("GetMiddleInitial name = " + registrationScreen1.GetMiddleInitial());
			ConsoleWindow.Trace("GetLastName = " + registrationScreen1.GetLastName());
			ConsoleWindow.Trace("GetSelectedRace = " + registrationScreen1.GetSelectedRace());
			ConsoleWindow.Trace("GetIsMale = " + registrationScreen1.GetIsMale());
			ConsoleWindow.Trace("GetStreetAddress = " + registrationScreen1.GetStreetAddress());
			ConsoleWindow.Trace("GetStreetAddress2 = " + registrationScreen1.GetStreetAddress2());
			ConsoleWindow.Trace("GetCity = " + registrationScreen1.GetCity());
			ConsoleWindow.Trace("GetSelectedState = " + registrationScreen1.GetSelectedState());
			ConsoleWindow.Trace("United States = ");
			ConsoleWindow.Trace("GetZipCode = " + registrationScreen1.GetZipCode());
			ConsoleWindow.Trace("billing GetStreetAddress1 = " + registrationScreen2.GetStreetAddress1());
			ConsoleWindow.Trace("billing GetStreetAddress2 = " + registrationScreen2.GetStreetAddress2());
			ConsoleWindow.Trace("billing GetCity = " + registrationScreen2.GetCity());
			ConsoleWindow.Trace("billing GetSelectedState = " + registrationScreen2.GetSelectedState());
			ConsoleWindow.Trace("United States ");
			ConsoleWindow.Trace("billing GetZipCode = " + registrationScreen2.GetZipCode());
			ConsoleWindow.Trace("GetEmail = " + registrationScreen1.GetEmail());
			ConsoleWindow.Trace("password.text = " + password.text);
			ConsoleWindow.Trace("GetSelectedSecretQuestion = " + GetSelectedSecretQuestion());
			ConsoleWindow.Trace("passwordRecoveryAnswer.text = " + passwordRecoveryAnswer.text);
			*/
			// Do not change the order the parameters are passed in as
			ExternalInterface.call("f2c_UpdateAccount",
			[
			 registrationScreen1.GetFirstName(), registrationScreen1.GetMiddleInitial(), registrationScreen1.GetLastName(),
			 registrationScreen1.GetSelectedRace(), registrationScreen1.GetIsMale(), registrationScreen1.GetStreetAddress(),
			 registrationScreen1.GetStreetAddress2(), registrationScreen1.GetCity(), registrationScreen1.GetSelectedState(),
			 "United States",//countryList[homeCountry.selectedIndex]
			 registrationScreen1.GetZipCode(), 
			 registrationScreen2.GetStreetAddress1(),
			 registrationScreen2.GetStreetAddress2(), registrationScreen2.GetCity(), registrationScreen2.GetSelectedState(),
			 "United States",// countryList[billingCountry.selectedIndex], 
			 registrationScreen2.GetZipCode(), registrationScreen1.GetEmail(),
			 password.text, GetSelectedSecretQuestion(), passwordRecoveryAnswer.text,
			 "", "", 
			 String(registrationScreen1.GetAgeInDays()),
			 ProfileScreen.Instance.GetAboutMe(),
			 ProfileScreen.Instance.GetActivities(),
			 ProfileScreen.Instance.GetInterests(),
			 ProfileScreen.Instance.GetFavoriteGames(),
			 ProfileScreen.Instance.GetFavoriteMovies(),
			 ProfileScreen.Instance.GetFavoriteBooks(),
			 ProfileScreen.Instance.GetFavoriteQuotes()
			 ]);
					 
		//	AccountInfo.Instance.GetAccountInfoFromServer();
			//LobbyInterface.Instance.ShowScreen( ScreenID.PROFILE );
		}
	}
	
	public function GetSelectedSecretQuestion():String
	{
		return passwordRecoveryQuestion.dataProvider[passwordRecoveryQuestion.selectedIndex];
	}

	public function f2c_ValidateHandle():Void
	{
		mcCheckmark.gotoAndStop("Hide");
		tfRegistrationStatus._visible = false;
		ExternalInterface.call("f2c_ValidateHandle", handle.text);
	}

	public function c2f_ValidateHandleResult(resultIdentifier:String):Void
	{
		//tfRegistrationStatus._visible = true;
		switch (resultIdentifier)
		{
			case "SUCCESS":
				// Handle is ok
				ConsoleWindow.Trace("Handle is OK");
				mcCheckmark.gotoAndPlay("Show");
				tfRegistrationStatus._visible = false;
			break;
			case "PROFANITY_FILTER_CHECK_FAILED":
				// Handle has profanity
				//ConsoleWindow.Trace("Handle has profanity");
				//tfRegistrationStatus.text = "Handle has profanity";
			break;
			case "Client_ValidateHandle_HANDLE_ALREADY_IN_USE":
				// Handle is already in use
				//ConsoleWindow.Trace("Handle is already in use");
				//tfRegistrationStatus.text = "Handle is already in use";
			break;
			case "HANDLE_IS_EMPTY":
				//ConsoleWindow.Trace("Handle is empty");
				//tfRegistrationStatus.text = "Username field is empty.";				
			break;
			default:
				ConsoleWindow.Trace("Unknown handle result: " + resultIdentifier);
			break;
		}		
				
		if ( resultIdentifier != "SUCCESS" )
		{
			LobbyInterface.Instance.CreateMessageBox( resultIdentifier );
		}			
	}

	public function c2f_RegisterAccountResult(resultIdentifier:String, username:String, password:String):Void
	{
		switch (resultIdentifier)
		{
			case "Client_RegisterAccount_INVALID_COUNTRY":
				//tfRegistrationStatus.text = "INVALID COUNTRY";
			break;
			case "Client_RegisterAccount_INVALID_STATE":
				//tfRegistrationStatus.text = "INVALID STATE";
			break;
			case "Client_RegisterAccount_INVALID_RACE":
				//tfRegistrationStatus.text = "INVALID RACE";
			break;
			case "Client_RegisterAccount_REQUIRED_AGE_NOT_MET":
				// User is not old enough to register an account with the game being registered
				//tfRegistrationStatus.text = "USER IS NOT OLD ENOUGH TO REGISTER.";
			break;
			case "PROFANITY_FILTER_CHECK_FAILED":
				// Username has profanity
				//tfRegistrationStatus.text = "USERNAME HAS PROFANITY.";
			break;
			case "Client_RegisterAccount_HANDLE_ALREADY_IN_USE":
				//tfRegistrationStatus.text = "HANDLE ALREADY IN USE.";
			break;
			case "Client_RegisterAccount_REQUIRES_CD_KEY":
				// Required that you entered a CD key, but you didn't
				//tfRegistrationStatus.text = "REQUIRES THAT YOU ENTER A CD KEY.";
			break;
			case "Client_RegisterAccount_CD_KEY_NOT_USABLE":
				//tfRegistrationStatus.text = "BAD CD KEY.";
			break;
			case "Client_RegisterAccount_CD_KEY_STOLEN":
			//	tfRegistrationStatus.text = "CD KEY IS STOLEN.";
			break;
			case "Client_RegisterAccount_CD_KEY_ALREADY_USED":
			//	tfRegistrationStatus.text = "CD KEY IS ALREADY IN USED.";
			break;
			case "DATABASE_CONSTRAINT_FAILURE":
				// Database messed up, user should try again
				//tfRegistrationStatus.text = "DATABASE FAILURE, PLEASE TRY AGAIN.";
			break;
			case "PASSWORD_IS_TOO_SHORT":
				//tfRegistrationStatus.text = "PASSWORD ID TOO SHORT";				
			break;
			case "EMAIL_ADDRESS_IS_INVALID":
				//tfRegistrationStatus.text = "EMAIL ADDRESS IS INVALID.";				
			break;
			case "SUCCESS":
				// Account created
				// Show a dialog telling the user about this
				// Go to ConnectedToServer, fill in fields there
				//_root.gotoAndPlay("ConnectedToServer");
				LobbyInterface.Instance.ShowScreen( ScreenID.LOGIN );
			break;
			
			default:
				//ConsoleWindow.Trace( "UNKNOWN ERROR: " + resultIdentifier );
			break;
		}
		
		//tfRegistrationStatus._visible = true;
				
		if ( resultIdentifier != "SUCCESS" )
		{
			LobbyInterface.Instance.CreateMessageBox( resultIdentifier );
		}			
	}
			
	private function c2f_UpdateAccountResult(resultIdentifier:String):Void
	{
		ConsoleWindow.Trace("c2f_UpdateAccountResult: " + resultIdentifier)
		switch( resultIdentifier )
		{			
			case "SUCCESS":
				LobbyInterface.Instance.ShowScreen( ScreenID.PROFILE );
			break;
			
			default:
			//	tfRegistrationStatus.text = resultIdentifier;					
			break;
		}
		
		//tfRegistrationStatus._visible = true;
				
		if ( resultIdentifier != "SUCCESS" )
		{
			LobbyInterface.Instance.CreateMessageBox( resultIdentifier );
		}	
	}
	
	private function PopulateFromAccountInfo():Void
	{
		ConsoleWindow.Trace("received account info... password.text = " + password.text);
		handle.text = AccountInfo.Instance.GetUserName();
		password.text = AccountInfo.Instance.GetPassword();
		passwordConfirm.text = AccountInfo.Instance.GetPassword();
		passwordRecoveryQuestion.selectedIndex = GetIndexFromElement( AccountInfo.Instance.GetPasswordRecoveryQuestion(), SecretQuestionList );
		passwordRecoveryAnswer.text = AccountInfo.Instance.GetPasswordRecoveryAnswer();
	}
	
	//temporary save, data will get wiped if user exits edit account
	private function SaveToAccountInfo():Void
	{
	}
}