import flash.external.*;
import gfx.controls.TextInput;
import gfx.controls.Button;
import gfx.controls.CheckBox;
import Screens.ClanBlockListScreen;
import Screens.ClanRootScreen;
import mx.utils.Delegate;

class Screens.ClanGeneralScreen extends ScreenWithPageNavigator
{	
	private var clanHandle_EditBox:TextInput;	
	private var clans_Create_description_EditBox:TextInput;	
	private var clans_Create_emailStatus_EditBox:TextInput;	
	private var clans_Description_EditBox:TextInput;	
	private var clans_targetHandle_EditBox:TextInput;	
	private var clans_SetSubleaderStatus_targetHandle_EditBox:TextInput;	
	private var clans_SetMemberRank_targetHandle_EditBox:TextInput;	
	private var clans_SetMemberRank_newRank_EditBox:TextInput;	
	private var clans_ChangeHandle_newClanHandle_EditBox:TextInput;	
	
	private var clans_Create_failIfAlreadyInClan_Checkbox:CheckBox;
	private var clans_Create_requiresInvitationsToJoin_Checkbox:CheckBox;
	private var clans_SetSubleaderStatus_setToSubleader_Checkbox:CheckBox;
	
	private var clans_Create_Button:Button;		
	private var clans_SetProperties_Button:Button;		
	private var clans_SetMyMemberProperties_Button:Button;		
	private var clans_GrantLeader_Button:Button;		
	private var clans_SetSubleaderStatus_Button:Button;		
	private var clans_SetMemberRank_Button:Button;		
	private var clans_ChangeHandle_Button:Button;		
	
	private var btnBack:Button;
	private var btnLeaveClan:Button;
	private var btnBlockedUsers:Button;
	private var btnAcceptInvite:Button;
	private var btnRequest:Button;
	private var btnInvite:Button;
	private var tfUserToAdd:TextInput;
	
	private var tfClanName:TextField;
	private var tfClanDescription:TextField;
	
	private var mIsMyClan:Boolean;
	private var mIsClanLeader:Boolean;
	private var mcMail:Mail;
		
	private static var mInstance:ClanGeneralScreen;
	
	public function ClanGeneralScreen() 
	{
		ConsoleWindow.Trace("Constructing ClanGeneralScreen");				
		
		mScreenId = ScreenID.CLAN_GENERAL;		
		mScreenTabId = ScreenTab.ID_CLANS;
		
		mEntriesPerPage = 9;
		mFirstEntryX = 435;
		mFirstEntryY = -200;
		mDeltaY = 10;
		mIsMyClan = false;
		mIsClanLeader = false;
		
		mInstance = this;
	}
	
	public static function get Instance():ClanGeneralScreen
	{
		return mInstance;
	}
	
	public function VOnFinishedLoading():Void
	{		
		//Add click event for buttons
		/*clans_Create_Button.addEventListener("click", this, "f2c_Clans_Create");
		clans_SetProperties_Button.addEventListener("click", this, "f2c_Clans_SetProperties");
		clans_SetMyMemberProperties_Button.addEventListener("click", this, "f2c_Clans_SetMyMemberProperties");
		clans_GrantLeader_Button.addEventListener("click", this, "f2c_Clans_GrantLeader");
		clans_SetSubleaderStatus_Button.addEventListener("click", this, "f2c_Clans_SetSubleaderStatus");
		clans_SetMemberRank_Button.addEventListener("click", this, "f2c_Clans_SetMemberRank");
		clans_ChangeHandle_Button.addEventListener("click", this, "f2c_Clans_ChangeHandle");*/
		btnBack.addEventListener("click", this, "Back");
		btnBlockedUsers.addEventListener("click", this, "ShowBlockedUsers");
		btnLeaveClan.addEventListener("click", this, "ShowLeaveClanMail");
		btnRequest.addEventListener("click", this, "SendJoinRequestEmail");
		btnInvite.addEventListener("click", this, "ShowSendJoinInvitationMail");
						
		//Add callbacks for C++
		ExternalInterface.addCallback("c2f_Clans_GetProperties", this, c2f_Clans_GetProperties);
		
		ExternalInterface.addCallback("c2f_Clans_SetProperties", this, c2f_Clans_SetProperties);
		ExternalInterface.addCallback("c2f_Clans_SetMyMemberProperties", this, c2f_Clans_SetMyMemberProperties);
		ExternalInterface.addCallback("c2f_Clans_GrantLeader", this, c2f_Clans_GrantLeader);
		ExternalInterface.addCallback("c2f_Clans_SetSubleaderStatus", this, c2f_Clans_SetSubleaderStatus);
		ExternalInterface.addCallback("c2f_Clans_SetMemberRank", this, c2f_Clans_SetMemberRank);
		ExternalInterface.addCallback("c2f_Clans_ChangeHandle", this, c2f_Clans_ChangeHandle);
		ExternalInterface.addCallback("c2f_Clans_GetMembers", this, c2f_Clans_GetMembers);
		
		ExternalInterface.addCallback("c2f_Clans_Leave", this, c2f_Clans_Leave);
		ExternalInterface.addCallback("c2f_Clans_SendJoinRequest", this, c2f_Clans_SendJoinRequest);
		ExternalInterface.addCallback("c2f_Clans_SendJoinInvitation", this, c2f_Clans_SendJoinInvitation);
		
		super.VOnFinishedLoading();
	}
		
	public function OnShow():Void
	{				
		super.OnShow();
		
		mcMail._visible = false;
		
		if ( mMovieClipList.length > 0 )
		{
			CleanUpMoveClipList( mMovieClipList );
		}
		
		btnRequest._visible = false;
		btnBlockedUsers._visible = false;
		btnAcceptInvite._visible = false;
		btnLeaveClan._visible = false;
		btnInvite._visible = false;
		tfUserToAdd._visible = false;
		
		//tfClanName.text = "";
		//tfClanDescription.text = "";
		
		mMovieClipList = new Array();
		mCurrentPage = 0;
		GoToPage( 1 );
		
		if ( tfClanName.text.length > 0 )
		{
			ExternalInterface.call("f2c_Clans_GetProperties", tfClanName.text);		
			ExternalInterface.call("f2c_Clans_GetMembers", tfClanName.text);
		}		
	}
	
	public function Back():Void
	{
		LobbyInterface.Instance.ShowLastScreen();
	}
	
	public function SetClanName( name:String ):Void
	{
		ConsoleWindow.Trace("Setting clan name to " + name);
		tfClanName.text = name;
	}
		
	private function AddListEntry( username:String, rank:String, _profileImageIndex:Number ):MovieClip
	{
		var clanEntry:MovieClip = attachMovie("ClanMemberListEntry", "mcClanEntry" + mMovieClipList.length, getNextHighestDepth() );
		clanEntry._visible = false;
		clanEntry.tfUserName.text = username;
		clanEntry.tfRank.text = rank;
		mMovieClipList.push( clanEntry );	
		//ConsoleWindow.Trace("Adding entry: " + clanEntry);
		
		var self:ClanGeneralScreen = this;		
		//clanEntry.onPress = function() { self.OnSelectClan( this ); }		
		
		return clanEntry;
	}
	
	public function c2f_Clans_GetProperties(resultIdentifier:String, clanDescription:String):Void
	{
		switch (resultIdentifier)
		{
			case "Clans_GetProperties_UNKNOWN_CLAN":
			break;
			case "DATABASE_CONSTRAINT_FAILURE":
			break;
			case "SUCCESS":
			// clanDescription should be a valid string
				tfClanDescription.text = clanDescription;
			break;
		}
		
		if ( resultIdentifier != "SUCCESS" )
		{
			LobbyInterface.Instance.CreateMessageBox( resultIdentifier );
		}
	}
	

	public function c2f_Clans_GetMembers():Void
	{
		var success:Boolean = false;
		var resultIdentifier:String = arguments[0];
		switch (resultIdentifier)
		{
			case "Clans_GetMembers_UNKNOWN_CLAN":
			break;
			case "DATABASE_CONSTRAINT_FAILURE":
			break;
			case "SUCCESS":
				success = true;
			break;
		}
		
		ConsoleWindow.Trace("ClanGeneralScreen.c2f_Clans_GetMembers(), resultIdentifier = " + resultIdentifier);
		
		if ( success )
		{
			mIsMyClan = false;
			var username:String = LobbyInterface.Instance.GetUsername();
			var clanLeader:String = arguments[1];
			mIsClanLeader = false;
			if ( clanLeader == username )
			{
				mIsMyClan = true;		
				mIsClanLeader = true;
			}
			AddListEntry( clanLeader, "LEADER", 1 );
			var clanMembersOtherThanLeader:Number = arguments.length - 2;
			for (var i:Number=0; i < clanMembersOtherThanLeader; i++)
			{
				var clanMember:String = arguments[2+i];
				AddListEntry( clanMember, "MEMBER", 1 );
				if ( clanMember == username )
				{
					mIsMyClan = true;				
				}
			}
			
			if ( mIsMyClan )
			{
				btnLeaveClan._visible 	= true;
				btnBlockedUsers._visible = true;
				btnAcceptInvite._visible = false;
				btnRequest._visible 	= false;		
				btnInvite._visible 		= true;
				tfUserToAdd._visible	= true;
			}
			else
			{
				btnLeaveClan._visible 	= false;
				btnBlockedUsers._visible = false;
				btnAcceptInvite._visible = false;
				btnRequest._visible 	= true;					
			}
					
			OnMoveClipListUpdated();		
			ShowPage(1);
		}
		else
		{
			LobbyInterface.Instance.CreateMessageBox( resultIdentifier );
		}
	}
		
	public function ShowBlockedUsers():Void
	{
		ClanBlockListScreen.Instance.SetClanName( tfClanName.text );
		LobbyInterface.Instance.ShowScreen( ScreenID.CLAN_BLOCK_LIST );
	}
	
	public function f2c_Clans_Create():Void
	{
	}

	public function f2c_Clans_SetProperties():Void
	{
		ExternalInterface.call("f2c_Clans_SetProperties", clanHandle_EditBox.text,clans_Description_EditBox.text);
	}

	public function f2c_Clans_SetMyMemberProperties():Void
	{
		ExternalInterface.call("f2c_Clans_SetMyMemberProperties", clanHandle_EditBox.text,clans_Description_EditBox.text);
	}

	public function f2c_Clans_GrantLeader():Void
	{
		ExternalInterface.call("f2c_Clans_GrantLeader", clanHandle_EditBox.text,clans_targetHandle_EditBox.text);
	}

	public function f2c_Clans_SetSubleaderStatus():Void
	{
		ExternalInterface.call("f2c_Clans_SetSubleaderStatus", clanHandle_EditBox.text,
						  clans_SetSubleaderStatus_targetHandle_EditBox.text,
														   clans_SetSubleaderStatus_setToSubleader_Checkbox.selected);
	}

	public function f2c_Clans_SetMemberRank():Void
	{
		ExternalInterface.call("f2c_Clans_SetMemberRank", clanHandle_EditBox.text,clans_SetMemberRank_targetHandle_EditBox.text,
													  clans_SetMemberRank_newRank_EditBox.text);
	}

	public function f2c_Clans_ChangeHandle():Void
	{
		ExternalInterface.call("f2c_Clans_ChangeHandle", clanHandle_EditBox.text,clans_ChangeHandle_newClanHandle_EditBox.text);
	}

	public function c2f_Clans_SetProperties(resultIdentifier:String):Void
	{
		switch (resultIdentifier)
		{
			case "Clans_SetProperties_UNKNOWN_CLAN":
			break;
			case "Clans_SetProperties_MUST_BE_LEADER":
			break;
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

	public function c2f_Clans_SetMyMemberProperties(resultIdentifier:String):Void
	{
		switch (resultIdentifier)
		{
			case "Clans_SetMyMemberProperties_UNKNOWN_CLAN":
			break;
			case "Clans_SetMyMemberProperties_NOT_IN_CLAN":
			break;
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

	public function c2f_Clans_GrantLeader(resultIdentifier:String):Void
	{
		switch (resultIdentifier)
		{
			case "Clans_GrantLeader_UNKNOWN_CLAN":
			break;
			case "Clans_GrantLeader_MUST_BE_LEADER":
			break;
			case "Clans_GrantLeader_UNKNOWN_TARGET_HANDLE":
			break;
			case "Clans_GrantLeader_CANNOT_PERFORM_ON_SELF":
			break;
			case "Clans_GrantLeader_TARGET_NOT_IN_CLAN":
			break;
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

	public function c2f_Clans_SetSubleaderStatus(resultIdentifier:String):Void
	{
		switch (resultIdentifier)
		{
			case "Clans_SetSubleaderStatus_UNKNOWN_CLAN":
			break;
			case "Clans_SetProperties_MUST_BE_LEADER":
			break;
			case "Clans_SetSubleaderStatus_UNKNOWN_TARGET_HANDLE":
			break;
			case "Clans_SetSubleaderStatus_CANNOT_PERFORM_ON_SELF":
			break;
			case "Clans_SetSubleaderStatus_TARGET_NOT_IN_CLAN":
			break;
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


	public function c2f_Clans_SetMemberRank(resultIdentifier:String):Void
	{
		switch (resultIdentifier)
		{
			case "Clans_SetMemberRank_UNKNOWN_CLAN":
			break;
			case "Clans_SetMemberRank_MUST_BE_LEADER":
			break;
			case "Clans_SetMemberRank_UNKNOWN_TARGET_HANDLE":
			break;
			case "Clans_SetMemberRank_CANNOT_PERFORM_ON_SELF":
			break;
			case "Clans_SetMemberRank_TARGET_NOT_IN_CLAN":
			break;
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

	public function c2f_Clans_ChangeHandle(resultIdentifier:String):Void
	{
		switch (resultIdentifier)
		{
			case "Clans_ChangeHandle_UNKNOWN_CLAN":
			break;
			case "Clans_ChangeHandle_MUST_BE_LEADER":
			break;
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
	
	public function HideMail():Void
	{
		mcMail._visible = false;
	}
	
	public function ShowLeaveClanMail():Void
	{
		mcMail.Clear();
		mcMail.SetButton( 1, "CANCEL", Delegate.create(this, HideMail) );
		mcMail.SetButton( 2, "LEAVE", Delegate.create(this, LeaveClan) );
		
		mcMail.SetTitle( "LEAVE CLAN" );
		mcMail.SetToField( tfClanName.text );
		mcMail.DisableToField();
		mcMail.SetCheckBox( "DISBAND CLAN" );
		mcMail.swapDepths( getNextHighestDepth() );
		mcMail._visible = true;		
	}
	
	public function LeaveClan():Void
	{		
		//emailStatus_EditBox.text (4th param), Kevin - The email system supports a numerical status for emails that is game-specific. 
		//	For example, you might have 23 represent an email reporting match statistics.
		ExternalInterface.call("f2c_Clans_Leave", [
											  tfClanName.text, mcMail.GetSubjectField(), mcMail.GetMsgField(), "0",
											  mcMail.IsCheckBoxOn()], _root);
		HideMail();
		Back();
	}
	
	public function c2f_Clans_Leave(resultIdentifier:String, wasDissolved:Boolean, newClanLeader:String):Void
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
				ConsoleWindow.Trace("c2f_Clans_Leave(), resultIdentifier = " + resultIdentifier );
			break;
		}
		
		if ( resultIdentifier != "SUCCESS" )
		{
			LobbyInterface.Instance.CreateMessageBox( resultIdentifier );
		}
		
		//ConsoleWindow.Trace("c2f_Clans_Leave(), resultIdentifier = " + resultIdentifier );
	}
	
	public function SendJoinRequestEmail():Void
	{
		mcMail.Clear();
		mcMail.SetButton( 1, "CANCEL", Delegate.create(this, HideMail) );
		mcMail.SetButton( 2, "SEND REQUEST", Delegate.create(this, SendJoinRequest) );
		
		mcMail.SetTitle( "JOIN CLAN" );
		mcMail.SetToField( tfClanName.text );
		mcMail.DisableToField();
		mcMail.swapDepths( getNextHighestDepth() );
		mcMail._visible = true;				
	}
	
	public function SendJoinRequest():Void
	{
		ExternalInterface.call("f2c_Clans_SendJoinRequest", 
											  tfClanName.text, mcMail.GetSubjectField(), mcMail.GetMsgField(), "0"
												);
	}
	

	public function c2f_Clans_SendJoinRequest(resultIdentifier:String, clanJoined:Boolean):Void
	{
		ConsoleWindow.Trace("c2f_Clans_SendJoinRequest() resultIdentifier = " + resultIdentifier); 
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
				HideMail();
			break;
		}
		
		if ( resultIdentifier != "SUCCESS" )
		{
			LobbyInterface.Instance.CreateMessageBox( resultIdentifier );
		}
	}
	
	public function ShowSendJoinInvitationMail():Void
	{
		mcMail.Clear();
		mcMail.SetButton( 1, "CANCEL", Delegate.create(this, HideMail) );
		mcMail.SetButton( 2, "SEND INVITE", Delegate.create(this, SendJoinInvitation) );
		
		mcMail.SetTitle( "INVITE USER TO CLAN" );
		mcMail.SetToField( tfUserToAdd.text );
		mcMail.DisableToField();
		mcMail.swapDepths( getNextHighestDepth() );
		mcMail._visible = true;				
	}
	
	public function SendJoinInvitation():Void
	{		
		ExternalInterface.call("f2c_Clans_SendJoinInvitation", tfClanName.text, mcMail.GetSubjectField(), mcMail.GetMsgField(), "0",
														   tfUserToAdd.text);
	}
	
	public function c2f_Clans_SendJoinInvitation(resultIdentifier:String):Void
	{
		switch (resultIdentifier)
		{
			case "DATABASE_CONSTRAINT_FAILURE":
			break;
			case "SUCCESS":
				ConsoleWindow.Trace("c2f_Clans_SendJoinInvitation(), success");
				HideMail();
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
	}
}