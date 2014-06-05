import flash.external.*;
import gfx.controls.TextInput;
import gfx.controls.Button;
import gfx.controls.CheckBox;

class Screens.ClanEmailScreen extends Screen
{	
	private var clanHandle_EditBox:TextInput;	
	private var subject_EditBox:TextInput;	
	private var body_EditBox:TextInput;	
	private var emailStatus_EditBox:TextInput;	
	private var clans_targetHandle_EditBox:TextInput;	
	private var clans_AcceptJoinRequest_requestingUserHandle_EditBox:TextInput;	
	private var clans_RejectJoinRequest_requestingUserHandle_EditBox:TextInput;	
	private var clans_KickAndBlacklistUser_targetHandle_EditBox:TextInput;	
	private var clans_KickAndBlacklistUser_reason_EditBox:TextInput;	
	
	private var clans_Leave_dissolveIfClanLeader_Checkbox:CheckBox;
	private var clans_AcceptJoinInvitation_failIfAlreadyInClan_Checkbox:CheckBox;
	private var clans_AcceptJoinRequest_failIfAlreadyInClan_Checkbox:CheckBox;
	private var clans_KickAndBlacklistUser_blacklist_Checkbox:CheckBox;
	private var clans_KickAndBlacklistUser_kick_Checkbox:CheckBox;
	
	private var clans_RejectJoinInvitation_Button:Button;			
	private var clans_SendJoinRequest_Button:Button;			
	private var clans_WithdrawJoinRequest_Button:Button;			
	private var clans_Leave_Button:Button;			
	private var clans_AcceptJoinInvitation_Button:Button;			
	private var clans_SendJoinInvitation_Button:Button;			
	private var clans_WithdrawJoinInvitation_Button:Button;			
	private var clans_UnblacklistUser_Button:Button;			
	private var clans_AcceptJoinRequest_Button:Button;			
	private var clans_RejectJoinRequest_Button:Button;			
	private var clans_KickAndBlacklistUser_Button:Button;			
		
	public function ClanEmailScreen() 
	{
		ConsoleWindow.Trace("Constructing ClanEmailScreen");						
		
		mScreenId = ScreenID.CLAN_EMAIL
		mScreenTabId = ScreenTab.ID_CLANS;;	
	}
	
	public function VOnFinishedLoading():Void
	{		
		//Add click event for buttons
	//	clans_RejectJoinInvitation_Button.addEventListener("click", this, "f2c_Clans_RejectJoinInvitation");
	//	clans_SendJoinInvitation_Button.addEventListener("click", this, "f2c_Clans_SendJoinInvitation");
		clans_WithdrawJoinInvitation_Button.addEventListener("click", this, "f2c_Clans_WithdrawJoinInvitation");
	//	clans_Leave_Button.addEventListener("click", this, "f2c_Clans_Leave");
	//	clans_AcceptJoinInvitation_Button.addEventListener("click", this, "f2c_Clans_AcceptJoinInvitation");
	//	clans_SendJoinRequest_Button.addEventListener("click", this, "f2c_Clans_SendJoinRequest");
	//	clans_WithdrawJoinRequest_Button.addEventListener("click", this, "f2c_Clans_WithdrawJoinRequest");
	//	clans_UnblacklistUser_Button.addEventListener("click", this, "f2c_Clans_UnblacklistUser");
	//	clans_AcceptJoinRequest_Button.addEventListener("click", this, "f2c_Clans_AcceptJoinRequest");
	//	clans_RejectJoinRequest_Button.addEventListener("click", this, "f2c_Clans_RejectJoinRequest");
	//	clans_KickAndBlacklistUser_Button.addEventListener("click", this, "f2c_Clans_KickAndBlacklistUser");
				
		//Add callbacks for C++
	//	ExternalInterface.addCallback("c2f_Clans_Leave", this, c2f_Clans_Leave);
	//	ExternalInterface.addCallback("c2f_Clans_SendJoinInvitation", this, c2f_Clans_SendJoinInvitation);
		ExternalInterface.addCallback("c2f_Clans_WithdrawJoinInvitation", this, c2f_Clans_WithdrawJoinInvitation);
	//	ExternalInterface.addCallback("c2f_Clans_AcceptJoinInvitation", this, c2f_Clans_AcceptJoinInvitation);
	//	ExternalInterface.addCallback("c2f_Clans_RejectJoinInvitation", this, c2f_Clans_RejectJoinInvitation);
	//	ExternalInterface.addCallback("c2f_Clans_SendJoinRequest", this, c2f_Clans_SendJoinRequest);
	//	ExternalInterface.addCallback("c2f_Clans_WithdrawJoinRequest", this, c2f_Clans_WithdrawJoinRequest);
	//	ExternalInterface.addCallback("c2f_Clans_AcceptJoinRequest", this, c2f_Clans_AcceptJoinRequest);
	//	ExternalInterface.addCallback("c2f_Clans_RejectJoinRequest", this, c2f_Clans_RejectJoinRequest);
	//	ExternalInterface.addCallback("c2f_Clans_KickAndBlacklistUser", this, c2f_Clans_KickAndBlacklistUser);
	//	ExternalInterface.addCallback("c2f_Clans_UnblacklistUser", this, c2f_Clans_UnblacklistUser);
		
		super.VOnFinishedLoading();
	}
	
/*	public function f2c_Clans_Leave():Void
	{
		ExternalInterface.call("f2c_Clans_Leave", ternalInterface.call("f2c_Clans_Leave", [
											  clanHandle_EditBox.text, subject_EditBox.text, body_EditBox.text, emailStatus_EditBox.text,
											  clans_Le);
	}*/

/*	public function f2c_Clans_SendJoinInvitation():Void
	{
		ExternalInterface.call("f2c_Clans_SendJoinInvitation", clanHandle_EditBox.text, subject_EditBox.text, body_EditBox.text, emailStatus_EditBox.text,
														   clans_targetHandle_EditBox.text);
	}*/

	public function f2c_Clans_WithdrawJoinInvitation():Void
	{
		ExternalInterface.call("f2c_Clans_WithdrawJoinInvitation", clanHandle_EditBox.text, subject_EditBox.text, body_EditBox.text, emailStatus_EditBox.text,
														   clans_targetHandle_EditBox.text);
	}

/*	public function f2c_Clans_AcceptJoinInvitation():Void
	{
		ExternalInterface.call("f2c_Clans_AcceptJoinInvitation", clanHandle_EditBox.text, subject_EditBox.text, body_EditBox.text, emailStatus_EditBox.text,
														   clans_AcceptJoinInvitation_failIfAlreadyInClan_Checkbox.selected);
	}*/
/*
	public function f2c_Clans_RejectJoinInvitation():Void
	{
		ExternalInterface.call("f2c_Clans_RejectJoinInvitation", clanHandle_EditBox.text, subject_EditBox.text, body_EditBox.text, emailStatus_EditBox.text
														   );
	}*/


/*	public function f2c_Clans_SendJoinRequest():Void
	{
		ExternalInterface.call("f2c_Clans_SendJoinRequest", clanHandle_EditBox.text, subject_EditBox.text, body_EditBox.text, emailStatus_EditBox.text
														   );
	}*/

/*	public function f2c_Clans_WithdrawJoinRequest():Void
	{
		ExternalInterface.call("f2c_Clans_WithdrawJoinRequest", clanHandle_EditBox.text, subject_EditBox.text, body_EditBox.text, emailStatus_EditBox.text
														   );
	}*/

/*	public function f2c_Clans_AcceptJoinRequest():Void
	{
		ExternalInterface.call("f2c_Clans_AcceptJoinRequest", clanHandle_EditBox.text, subject_EditBox.text, body_EditBox.text, emailStatus_EditBox.text,
														   clans_AcceptJoinRequest_requestingUserHandle_EditBox.text, 
														   clans_AcceptJoinRequest_failIfAlreadyInClan_Checkbox.selected);
	}

	public function f2c_Clans_RejectJoinRequest():Void
	{
		ExternalInterface.call("f2c_Clans_RejectJoinRequest", clanHandle_EditBox.text, subject_EditBox.text, body_EditBox.text, emailStatus_EditBox.text,
														   clans_RejectJoinRequest_requestingUserHandle_EditBox.text);
	}*/

/*	public function f2c_Clans_KickAndBlacklistUser():Void
	{
		ExternalInterface.call("f2c_Clans_KickAndBlacklistUser", clanHandle_EditBox.text, subject_EditBox.text, body_EditBox.text, emailStatus_EditBox.text,
														   clans_KickAndBlacklistUser_targetHandle_EditBox.text,
														   clans_KickAndBlacklistUser_kick_Checkbox.selected,
														   clans_KickAndBlacklistUser_blacklist_Checkbox.selected,
														   clans_KickAndBlacklistUser_reason_EditBox.text
														   );
	}*/

/*	public function f2c_Clans_UnblacklistUser():Void
	{
		ExternalInterface.call("f2c_Clans_UnblacklistUser", clanHandle_EditBox.text, subject_EditBox.text, body_EditBox.text, emailStatus_EditBox.text,
														   clans_targetHandle_EditBox.text);
	}*/


/*	public function c2f_Clans_Leave(resultIdentifier:String, wasDissolved:Boolean, newClanLeader:String):Void
	{
		switch (resultIdentifier)
		{
			case "Clans_Leave_UNKNOWN_CLAN":
			break;
			case "Clans_Leave_NOT_IN_CLAN":
			break;
			case "DATABASE_CONSTRAINT_FAILURE":
			break;
			case "SUCCESS":
			break;
		}
	}*/

/*	public function c2f_Clans_SendJoinInvitation(resultIdentifier:String):Void
	{
		switch (resultIdentifier)
		{
			case "DATABASE_CONSTRAINT_FAILURE":
			break;
			case "SUCCESS":
			break;
		}
		
		if ( resultIdentifier != "SUCCESS" )
		{
			LobbyInterface.Instance.CreateMessageBox( resultIdentifier );
		}
	}
	
	public function c2f_Clans_WithdrawJoinInvitation(resultIdentifier:String):Void
	{
		switch (resultIdentifier)
		{
			case "Clans_WithdrawJoinInvitation_UNKNOWN_CLAN":
			break;
			case "Clans_WithdrawJoinInvitation_UNKNOWN_TARGET_HANDLE":
			break;
			case "Clans_WithdrawJoinInvitation_CANNOT_PERFORM_ON_SELF":
			break;
			case "Clans_WithdrawJoinInvitation_NO_SUCH_INVITATION_EXISTS":
			break;
			case "Clans_WithdrawJoinInvitation_MUST_BE_LEADER_OR_SUBLEADER":
			break;
			case "SUCCESS":
			break;
		}
		
		if ( resultIdentifier != "SUCCESS" )
		{
			LobbyInterface.Instance.CreateMessageBox( resultIdentifier );
		}
	}*/

/*	public function c2f_Clans_AcceptJoinInvitation(resultIdentifier:String):Void
	{
		switch (resultIdentifier)
		{
			case "Clans_AcceptJoinInvitation_UNKNOWN_CLAN":
			break;
			case "Clans_AcceptJoinInvitation_NO_SUCH_INVITATION_EXISTS":
			break;
			case "Clans_AcceptJoinInvitation_ALREADY_IN_DIFFERENT_CLAN":
			break;
			case "SUCCESS":
			break;
		}
		
		if ( resultIdentifier != "SUCCESS" )
		{
			LobbyInterface.Instance.CreateMessageBox( resultIdentifier );
		}
	}*/

/*	public function c2f_Clans_RejectJoinInvitation(resultIdentifier:String):Void
	{
		switch (resultIdentifier)
		{
			case "Clans_RejectJoinInvitation_UNKNOWN_CLAN":
			break;
			case "Clans_RejectJoinInvitation_NO_SUCH_INVITATION_EXISTS":
			break;
			case "SUCCESS":
			break;
		}
		
		if ( resultIdentifier != "SUCCESS" )
		{
			LobbyInterface.Instance.CreateMessageBox( resultIdentifier );
		}
	}*/

/*	public function c2f_Clans_SendJoinRequest(resultIdentifier:String, clanJoined:Boolean):Void
	{
		switch (resultIdentifier)
		{
			case "Clans_SendJoinRequest_UNKNOWN_CLAN":
			break;
			case "Clans_SendJoinRequest_ALREADY_IN_CLAN":
			break;
			case "Clans_SendJoinRequest_BANNED":
			break;
			case "Clans_SendJoinRequest_REQUEST_ALREADY_PENDING":
			break;
			case "Clans_SendJoinRequest_ALREADY_INVITED":
			break;
			case "SUCCESS":
			break;
		}
	}*/

/*	public function c2f_Clans_WithdrawJoinRequest(resultIdentifier:String):Void
	{
		switch (resultIdentifier)
		{
			case "Clans_WithdrawJoinRequest_UNKNOWN_CLAN":
			break;
			case "Clans_WithdrawJoinRequest_ALREADY_IN_CLAN":
			break;
			case "Clans_WithdrawJoinRequest_NO_SUCH_INVITATION_EXISTS":
			break;
			case "DATABASE_CONSTRAINT_FAILURE":
			break;
			case "SUCCESS":
			break;
		}
	}*/

/*	public function c2f_Clans_AcceptJoinRequest(resultIdentifier:String):Void
	{
		switch (resultIdentifier)
		{
			case "Clans_AcceptJoinRequest_UNKNOWN_CLAN":
			break;
			case "Clans_AcceptJoinRequest_NOT_IN_CLAN":
			break;
			case "Clans_AcceptJoinRequest_MUST_BE_LEADER_OR_SUBLEADER":
			break;
			case "Clans_AcceptJoinRequest_UNKNOWN_TARGET_HANDLE":
			break;
			case "Clans_AcceptJoinRequest_CANNOT_PERFORM_ON_SELF":
			break;
			case "Clans_AcceptJoinRequest_TARGET_ALREADY_IN_CLAN":
			break;
			case "Clans_AcceptJoinRequest_TARGET_IS_BANNED":
			break;
			case "Clans_AcceptJoinRequest_REQUEST_NOT_PENDING":
			break;
			case "Clans_AcceptJoinRequest_TARGET_ALREADY_IN_DIFFERENT_CLAN":
			break;
			case "SUCCESS":
			break;
		}
	}

	public function c2f_Clans_RejectJoinRequest(resultIdentifier:String):Void
	{
		switch (resultIdentifier)
		{
			case "Clans_RejectJoinRequest_UNKNOWN_CLAN":
			break;
			case "Clans_RejectJoinRequest_NOT_IN_CLAN":
			break;
			case "Clans_RejectJoinRequest_MUST_BE_LEADER_OR_SUBLEADER":
			break;
			case "Clans_RejectJoinRequest_REQUESTING_USER_HANDLE_UNKNOWN":
			break;
			case "Clans_RejectJoinRequest_NO_SUCH_INVITATION_EXISTS":
			break;
			case "SUCCESS":
			break;
		}
	}*/

/*	public function c2f_Clans_KickAndBlacklistUser(resultIdentifier:String):Void
	{
		switch (resultIdentifier)
		{
			case "Clans_KickAndBlacklistUser_UNKNOWN_CLAN":
			break;
			case "Clans_KickAndBlacklistUser_NOT_IN_CLAN":
			break;
			case "Clans_KickAndBlacklistUser_MUST_BE_LEADER_OR_SUBLEADER":
			break;
			case "Clans_KickAndBlacklistUser_UNKNOWN_TARGET_HANDLE":
			break;
			case "Clans_KickAndBlacklistUser_CANNOT_PERFORM_ON_SELF":
			break;
			case "Clans_KickAndBlacklistUser_CANNOT_PERFORM_ON_LEADER":
			break;
			case "Clans_KickAndBlacklistUser_ALREADY_BLACKLISTED":
			break;
			case "SUCCESS":
			break;
		}
		
		if ( resultIdentifier != "SUCCESS" )
		{
			LobbyInterface.Instance.CreateMessageBox( resultIdentifier );
		}
	}

	public function c2f_Clans_UnblacklistUser(resultIdentifier:String):Void
	{
		switch (resultIdentifier)
		{
			case "Clans_UnblacklistUser_UNKNOWN_CLAN":
			break;
			case "Clans_UnblacklistUser_NOT_IN_CLAN":
			break;
			case "Clans_UnblacklistUser_MUST_BE_LEADER_OR_SUBLEADER":
			break;
			case "Clans_UnblacklistUser_UNKNOWN_TARGET_HANDLE":
			break;
			case "Clans_UnblacklistUser_NOT_BLACKLISTED":
			break;
			case "SUCCESS":
			break;
		}
		
		if ( resultIdentifier != "SUCCESS" )
		{
			LobbyInterface.Instance.CreateMessageBox( resultIdentifier );
		}
	}*/
	
}