import flash.external.*;
import gfx.controls.TextInput;
import gfx.controls.Button;
import gfx.controls.CheckBox;
import gfx.data.dataProvider;
import gfx.controls.DropdownMenu;
// Please wait
// get account details: Client_GetAccountDetails
// fill in fields
// Enable update account button
// on press, save details


class Screens.UpdateAccountScreen extends Screen
{	
	private var firstName:TextInput;
	private var middleName:TextInput;
	private var lastName:TextInput;
	private var homeAddress1:TextInput;
	private var homeAddress2:TextInput;
	private var homeCity:TextInput;
	private var homeState:TextInput;
	private var homeZipCode:TextInput;
	private var billingAddress1:TextInput;
	private var billingAddress2:TextInput;
	private var billingCity:TextInput;
	private var billingState:TextInput;
	private var billingZipCode:TextInput;
	private var emailAddress:TextInput;
	private var password:TextInput;
	private var passwordRecoveryQuestion:TextInput;
	private var passwordRecoveryAnswer:TextInput;
	private var caption1:TextInput;
	private var caption2:TextInput;
	private var ageInDays:TextInput;
	private var handle:TextInput;	
	
	private var race:TextInput;
	private var homeCountry:TextInput;
	private var billingCountry:TextInput;
	private var sex_male:CheckBox;
	
	private var updateAccountButton:Button;
		
	public function UpdateAccountScreen() 
	{
		ConsoleWindow.Trace("Constructing UpdateAccountScreen");				
		
		mScreenId = ScreenID.UPDATE_ACCOUNT;		
		mScreenTabId = ScreenTab.ID_PROFILE;
	}
	
	public function VOnFinishedLoading():Void
	{		
		//updateAccountButton.visible=false;
		//Add click event for buttons
		updateAccountButton.addEventListener("click", this, "f2c_UpdateAccount");
		
		//Add callbacks for C++
		ExternalInterface.addCallback("c2f_UpdateAccountResult", this, c2f_UpdateAccountResult);
		// Do not change the order the parameters are passed in as
		//ExternalInterface.call("f2c_GetAccountDetails");
		//ExternalInterface.addCallback("c2f_GetAccountDetailsResult", this, c2f_GetAccountDetailsResult);
		
		super.VOnFinishedLoading();
	}
	/*
	function c2f_GetAccountDetailsResult( resultIdentifier:String, firstNameParam:String, middleNameParam:String, lastNameParam:String, raceParam:String, 
										  sex_maleParam:Boolean, homeAddress1Param:String, homeAddress2Param:String, homeCityParam:String, homeStateParam:String,
										  homeCountryParam:String, homeZipCodeParam:String, billingAddress1Param:String, billingAddress2Param:String, 
										  billingCityParam:String, billingStateParam:String, billingCountryParam:String, billingZipCodeParam:String, 
										  emailAddressParam:String, passwordParam:String, passwordRecoveryQuestionParam:String, passwordRecoveryAnswerParam:String,
										  caption1Param:String, caption2Param:String, ageInDaysParam:Number ):Void
	{
		ConsoleWindow.Trace( "UpdateAccountScreen.ServerSetAccountInfo()... " + resultIdentifier );
		switch (resultIdentifier)
		{
			case "SUCCESS":
			{
				firstName.text = firstNameParam; 
				middleName.text = middleNameParam; 
				lastName.text = lastNameParam;
				race.text = raceParam; 
				sex_male.selected = sex_maleParam; 
				homeAddress1.text = homeAddress1Param;
				homeAddress2.text = homeAddress2Param; 
				homeCity.text = homeCityParam; 
				homeState.text = homeStateParam;
				homeCountry.text = homeCountryParam; 
				homeZipCode.text = homeZipCodeParam; 
				billingAddress1.text = billingAddress1Param;
				billingAddress2.text = billingAddress2Param; 
				billingCity.text = billingCityParam; 
				billingState.text = billingStateParam;
				billingCountry.text = billingCountryParam; 
				billingZipCode.text = billingZipCodeParam; 
				emailAddress.text = emailAddressParam;
				password.text = passwordParam; 
				passwordRecoveryQuestion.text = passwordRecoveryQuestionParam; 
				passwordRecoveryAnswer.text = passwordRecoveryAnswerParam;
				caption1.text = caption1Param; 
				caption2.text = caption2Param; 
				ageInDays.text = String(ageInDaysParam);
								
				updateAccountButton.visible=true;
				break;
			}
			case "DATABASE_CONSTRAINT_FAILURE":
				ConsoleWindow.Trace( "Can't find the logged in user in the database for some reason" );
			break;
		}
	}*/

	function f2c_UpdateAccount():Void
	{
		// Do not change the order the parameters are passed in as
		ExternalInterface.call("f2c_UpdateAccount",
		[ firstName.text, middleName.text, lastName.text, race.text, sex_male.selected, homeAddress1.text, homeAddress2.text, homeCity.text, homeState.text,
		  homeCountry.text, homeZipCode.text, billingAddress1.text, billingAddress2.text, billingCity.text, billingState.text, billingCountry.text, billingZipCode.text, 
		  emailAddress.text, password.text, passwordRecoveryQuestion.text, passwordRecoveryAnswer.text, caption1.text, caption2.text, ageInDays.text 
		]);
		// This may take a while, show a waiting screen until we get c2f_UpdateAccountResult
		
		//ExternalInterface.call("f2c_GetAccountDetails");
	}

	function c2f_UpdateAccountResult(resultIdentifier:String)
	{
		switch (resultIdentifier)
		{
			case "DATABASE_CONSTRAINT_FAILURE":
				// Can't find the logged in user in the database for some reason
				break;
			case "SUCCESS":
			//_root.gotoAndPlay("LoggedIn");
				LobbyInterface.Instance.ShowScreen( ScreenID.LOGGED_IN );
			// Done
		}
				
		if ( resultIdentifier != "SUCCESS" )
		{
			LobbyInterface.Instance.CreateMessageBox( resultIdentifier );
		}	
	}
		
}