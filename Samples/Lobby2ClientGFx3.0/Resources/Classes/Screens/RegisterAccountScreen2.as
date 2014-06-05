import flash.external.*;
import gfx.controls.TextInput;
import gfx.controls.Button;
import gfx.controls.CheckBox;
import gfx.data.dataProvider;
import gfx.controls.DropdownMenu;
import Screens.RegisterAccountScreen;

//Billiing Infomation
class Screens.RegisterAccountScreen2 extends Screen
{	
	private var billingAddress1:TextInput;
	private var billingAddress2:TextInput;
	private var billingCity:TextInput;
	private var billingState:DropdownMenu;
	private var billingZipCode:TextInput;
	private var cbSameAsHomeAddress:CheckBox;
	
	private var billingCountry:DropdownMenu;										 

	private var cancelButton:Button;
	private var nextButton:Button;
		
	public function RegisterAccountScreen2() 
	{
		ConsoleWindow.Trace("Constructing RegisterAccountScreen2");				
		
		mScreenId = ScreenID.REGISTER_ACCOUNT_BILLING;		
		mScreenTabId = ScreenTab.ID_REGISTRATION;
	}
	
	public function VOnFinishedLoading():Void
	{		
		//Add click event for buttons
		cancelButton.addEventListener("click", this, "Back");
		nextButton.addEventListener("click", this, "Next");	
		cbSameAsHomeAddress.addEventListener("select", this, "OnToggleHomeAddress");	
		
		super.VOnFinishedLoading();
	}	
	
	public function OnShow():Void
	{		
		billingState.dataProvider = LobbyInterface.Instance.GetScreen( ScreenID.REGISTER_ACCOUNT_PERSONAL )["GetStateList"]();
		cbSameAsHomeAddress.selected = false;
		if ( LobbyInterface.Instance.IsLoggedIn() )
		{
			gotoAndStop("Update");			
			PopulateFromAccountInfo();
		}
		else
		{
			gotoAndStop("New");
		}
	}
	
	public function Next():Void
	{
		if ( LobbyInterface.Instance.IsLoggedIn() )
		{	
			SaveToAccountInfo();
		}
		LobbyInterface.Instance.ShowScreen( ScreenID.REGISTER_ACCOUNT_LOGIN );
	}
	
	public function Back():Void
	{
		if ( LobbyInterface.Instance.IsLoggedIn() )
		{	
			SaveToAccountInfo();
		}
		LobbyInterface.Instance.ShowScreen( ScreenID.REGISTER_ACCOUNT_PERSONAL );		
	}
	
	public function OnToggleHomeAddress():Void
	{
		if ( cbSameAsHomeAddress.selected )
		{
			var registrationScreen = LobbyInterface.Instance.GetScreen( ScreenID.REGISTER_ACCOUNT_PERSONAL );		
			billingAddress1.text = registrationScreen.GetStreetAddress();
			billingAddress2.text = registrationScreen.GetStreetAddress2();
			billingCity.text = registrationScreen.GetCity();
			billingState.selectedIndex = registrationScreen.GetState();
			billingZipCode.text = registrationScreen.GetZipCode();
		}
	}
	
	public function GetStreetAddress1():String
	{
		return billingAddress1.text;
	}
	
	public function GetStreetAddress2():String
	{
		return billingAddress2.text;
	}
	
	public function GetCity():String
	{
		return billingCity.text;
	}
	
	public function GetSelectedState():String
	{
		return LobbyInterface.Instance.GetScreen( ScreenID.REGISTER_ACCOUNT_PERSONAL )["GetStateList"]()[billingState.selectedIndex];
	}
	
	public function GetZipCode():String
	{
		return billingZipCode.text;
	}
	
	private function PopulateFromAccountInfo():Void
	{
		billingAddress1.text = AccountInfo.Instance.GetBillingAddress1();
		billingAddress2.text = AccountInfo.Instance.GetBillingAddress2();
		billingCity.text = AccountInfo.Instance.GetBillingCity();
		billingState.selectedIndex = GetIndexFromElement( AccountInfo.Instance.GetBillingState(), RegisterAccountScreen.stateList );
		billingZipCode.text = AccountInfo.Instance.GetBillingZipCode();
	}
	
	//temporary save, data will get wiped if user exits edit account
	private function SaveToAccountInfo():Void
	{
		AccountInfo.Instance.SetBillingAddress1( billingAddress1.text );
		AccountInfo.Instance.SetBillingAddress2( billingAddress2.text );
		AccountInfo.Instance.SetBillingCity( billingCity.text );
		AccountInfo.Instance.SetBillingState( GetSelectedState() );
		AccountInfo.Instance.SetBillingZipCode( billingZipCode.text );
	}
}
