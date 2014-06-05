import flash.external.*;
import gfx.controls.TextInput;
import gfx.controls.TextArea;
import gfx.controls.Button;
import mx.utils.Delegate;
import gfx.controls.CheckBox;

class Screens.CreateClan extends Screen
{		
	private var mcProfileImage:MovieClip;	
	private var tiName:TextInput;
	private var taDescription:TextArea;
	private var cbInvitationOnly:CheckBox;
	private var btnCancel:Button;
	private var btnCreate:Button;
	private var btnChangeLogo:Button;
	
	public function CreateClan() 
	{
		ConsoleWindow.Trace("Constructing CreateClan");						
		
		mScreenId = ScreenID.CLAN_CREATE;		
		mScreenTabId = ScreenTab.ID_CLANS;	
	}
	
	public function VOnFinishedLoading():Void
	{		
		//Add click event for buttons
		btnCancel.addEventListener("click", this, "Cancel");
		btnCreate.addEventListener("click", this, "Create");
		btnChangeLogo.addEventListener("click", this, "ChangeLogo");
						
		//Add callbacks for C++
		ExternalInterface.addCallback("c2f_Clans_Create", this, c2f_Clans_Create);
				
		super.VOnFinishedLoading();
	}
	
	public function OnShow():Void
	{
	}
	
	public function Cancel():Void
	{
		LobbyInterface.Instance.ShowScreen( ScreenID.CLAN_ROOT );
	}
	
	public function Create():Void
	{		
		ExternalInterface.call("f2c_Clans_Create", tiName.text,
											   false,
											   cbInvitationOnly.selected,
											   taDescription.text);
											   
		tiName.text = "";
		taDescription.text = "";
		cbInvitationOnly.selected = false;
	}
	
	public function c2f_Clans_Create(resultIdentifier:String):Void
	{
		switch (resultIdentifier)
		{
			case "PROFANITY_FILTER_CHECK_FAILED":
			break;
			case "Clans_Create_CLAN_HANDLE_IN_USE":
			break;
			case "DATABASE_CONSTRAINT_FAILURE":
			break;
			case "Clans_Create_ALREADY_IN_A_CLAN":
			break;
			case "SUCCESS":
				LobbyInterface.Instance.ShowScreen( ScreenID.CLAN_MY_CLANS );
			break;
			default:		
			break;
		}
		
		if ( resultIdentifier != "SUCCESS" )
		{
			LobbyInterface.Instance.CreateMessageBox( resultIdentifier );
		}
	}
	
	public function ChangeLogo():Void
	{
		LobbyInterface.Instance.ShowScreen( ScreenID.CHANGE_PHOTO );		
	}
}