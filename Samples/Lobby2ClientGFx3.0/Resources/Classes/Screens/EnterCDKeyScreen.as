import flash.external.*;
import gfx.controls.TextInput;
import gfx.controls.Button;

class Screens.EnterCDKeyScreen extends Screen
{	
	private var enterCdKeyOKButton:Button;
	private var cancelButton:Button;
	private var cdKeyEditBox:TextInput;
	private var tfFailedMsg:TextField;
		
	public function EnterCDKeyScreen() 
	{
		ConsoleWindow.Trace("Constructing EnterCDKeyScreen");				
		
		mScreenId = ScreenID.ENTER_CD_KEY;		
		mScreenTabId = ScreenTab.ID_CD_KEY;
		tfFailedMsg._visible = false;
	}
	
	public function VOnFinishedLoading():Void
	{
		//Add click event for buttons
		enterCdKeyOKButton.addEventListener("click", this, "f2c_CheckCDKey");
		cancelButton.addEventListener("click", this, "Cancel");
		
		//Add callbacks for C++
		ExternalInterface.addCallback("c2f_CheckCDKeyResult", this, c2f_CheckCDKeyResult);
		
		super.VOnFinishedLoading();
	}
		
	public function f2c_CheckCDKey():Void
	{
		tfFailedMsg._visible = false;
		ConsoleWindow.Trace("cdKeyEditBox.text = " + cdKeyEditBox.text);
		ExternalInterface.call("f2c_CheckCDKey", cdKeyEditBox.text);
	}
	
	public function Cancel():Void
	{
		ConsoleWindow.Trace("EnterCDKeyScreen calling login screen");
		LobbyInterface.Instance.ShowScreen( ScreenID.LOGIN );		
	}

	public function c2f_CheckCDKeyResult(reasonIdentifier:String, userUsedBy:String, activationDate:String, wasStolen:Boolean, usable:Boolean):Void
	{
		switch (reasonIdentifier)
		{
			case "SUCCESS":
				if (usable==true)
				{					
					// if usable==true, we're ok
					ConsoleWindow.Trace("RegisterAccount?");
					//_root.gotoAndPlay("RegisterAccount");
					LobbyInterface.Instance.ShowScreen( ScreenID.REGISTER_ACCOUNT_PERSONAL );
				}
				else if (wasStolen==true)
				{
					ConsoleWindow.Trace("account stolen?");
					// If wasStolen==true, this cd key was stolen. Display appropriate error message and ask user to reenter
				}
				else
				{
					ConsoleWindow.Trace("cd key already in used");
					// CD key already used. Display appropriate error message and ask user to reenter
				}
				break;
			case "CDKey_GetStatus_TITLE_NOT_IN_USE":
				// Internal error, just display bad title
				break;
			case "CDKey_GetStatus_UNKNOWN_CD_KEY":
				tfFailedMsg.text = "Unknown CD KEY";
				//tfFailedMsg._visible = true;
				// Most common case, CD key was mistyped. Display appropriate error message and ask user to reenter
				break;
			case "REQUIRED_TEXT_IS_EMPTY":
				tfFailedMsg.text = "PLEASE ENTER CD KEY";
				//tfFailedMsg._visible = true;
				// Field was blank
				break;
		}
		
		if ( reasonIdentifier != "SUCCESS" )
		{
			LobbyInterface.Instance.CreateMessageBox( reasonIdentifier );
		}
	}
}