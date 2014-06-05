import flash.external.*;
import gfx.controls.TextInput;
import gfx.controls.TextArea;
import gfx.controls.Button;
import mx.utils.Delegate;
import gfx.controls.CheckBox;

class Screens.ClanInvitedRequestedScreen extends ScreenWithPageNavigator
{		
	private var btnBack:Button;
	private var tfClanName:TextField;
	private var tfClanDescription:TextField;
	
	private var mcMail:Mail;
	public var mcSelectedEntry:MovieClip;
	
	public function ClanInvitedRequestedScreen() 
	{
		ConsoleWindow.Trace("Constructing CreateClan");						
		
		mScreenId = ScreenID.CLAN_INVITED_REQUESTED;		
		mScreenTabId = ScreenTab.ID_CLANS;	
		
		mEntriesPerPage = 10;
		mFirstEntryX = 500;
		mFirstEntryY = -200;
		mDeltaY = 10;
	}
	
	public function VOnFinishedLoading():Void
	{		
		//Add click event for buttons
		btnBack.addEventListener("click", this, "Back");
						
		//Add callbacks for C++
		ExternalInterface.addCallback("c2f_Clans_DownloadInvitationList", this, c2f_Clans_DownloadInvitationList);
		ExternalInterface.addCallback("c2f_Clans_DownloadRequestList", this, c2f_Clans_DownloadRequestList);
		ExternalInterface.addCallback("c2f_Clans_AcceptJoinRequest", this, c2f_Clans_AcceptJoinRequest);
		ExternalInterface.addCallback("c2f_Clans_RejectJoinRequest", this, c2f_Clans_RejectJoinRequest);
		ExternalInterface.addCallback("c2f_Clans_WithdrawJoinRequest", this, c2f_Clans_WithdrawJoinRequest);
		ExternalInterface.addCallback("c2f_Clans_AcceptJoinInvitation", this, c2f_Clans_AcceptJoinInvitation);
		ExternalInterface.addCallback("c2f_Clans_RejectJoinInvitation", this, c2f_Clans_RejectJoinInvitation);
				
		super.VOnFinishedLoading();
	}
	
	public function OnShow():Void
	{				
		super.OnShow();
		mcMail._visible = false;
		
		tfClanName.text = "";
		tfClanDescription.text = "";
		
		RefreshScreen();
	}
	
	public function RefreshScreen():Void
	{				
		if ( mMovieClipList.length > 0 )
		{
			CleanUpMoveClipList( mMovieClipList );
		}
		
		mMovieClipList = new Array();
		mCurrentPage = 0;
		GoToPage( 1 );		
		DownloadLists();		
	}
	
	public function Back():Void
	{
		LobbyInterface.Instance.ShowScreen( ScreenID.CLAN_ROOT );
	}
	
	private function DownloadLists():Void
	{
		ConsoleWindow.Trace("DownloadLists");
		ExternalInterface.call("f2c_Clans_DownloadInvitationList");
		ExternalInterface.call("f2c_Clans_DownloadRequestList");		
	}
	
	public function c2f_Clans_DownloadInvitationList():Void
	{
		var resultIdentifier:String = arguments[0];
		ConsoleWindow.Trace("c2f_Clans_DownloadInvitationList... resultIdentifier: " + resultIdentifier);
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
		
		var numResults:Number = arguments.length-1;
		for (var i:Number=0; i < numResults; i++)
		{
			var clanHandle:String = arguments[1 + i];
			AddListEntryIncomingInvite( clanHandle, 0 );
			//AddListEntry( clanHandle, "INVITED", 0, false );
		}
		
		ConsoleWindow.Trace("c2f_Clans_DownloadInvitationList(), arguments = " + arguments);
		
		//debug data:
		//for (var i:Number=0; i < 15; i++)
		{
		//	AddListEntry( "test clan" + i, "INVITED"  );
		}
		
		OnMoveClipListUpdated();	
		ShowPage(1);
	}

	public function c2f_Clans_DownloadRequestList():Void
	{
		var resultIdentifier:String = arguments[0];
		ConsoleWindow.Trace("c2f_Clans_DownloadRequestList... resultIdentifier: " + resultIdentifier);
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
		
		var joinRequestsToMyClan:Number = arguments[1];
		var joinRequestsFromMe:Number = arguments[2];
		for (var i:Number=0; i < joinRequestsToMyClan; i++)
		{
			var targetClan:String = arguments[3+i*3+0];
			var dateSent:String = arguments[3+i*3+1];
			var joinRequestSender:String = arguments[3 + i * 3 + 2];
			AddListEntryIncomingRequest( targetClan, 0, joinRequestSender );
			//AddListEntry( targetClan, "REQUESTED", 0, false, joinRequestSender );
		}
		
		for (var i:Number=0; i < joinRequestsFromMe; i++)
		{
			var targetClan:String = arguments[3+i*3+joinRequestsToMyClan*3+0];
			var dateSent:String = arguments[3+i*3+joinRequestsToMyClan*3+1];
			var joinRequestSender:String = arguments[3 + i * 3 + joinRequestsToMyClan * 3 + 2];
			AddListEntryOutgoingRequest( targetClan, 0, joinRequestSender );
			//AddListEntry( targetClan, "REQUESTED", 0, true, joinRequestSender );
		}
		
		OnMoveClipListUpdated();	
	}
	
	private function AddListEntry( clanToAdd:String, invitedOrRequested:String, _profileImageIndex:Number, userName:String ):MovieClip
	{
		var clanEntry:MovieClip = attachMovie("ClanInviteListEntry", "mcClanEntry" + mMovieClipList.length, getNextHighestDepth() );
		clanEntry._visible = false;
		clanEntry.tfClanName.text = clanToAdd;
		clanEntry.tfInvitation.text = invitedOrRequested;
		clanEntry.username = userName;
		mMovieClipList.push( clanEntry );	
		ConsoleWindow.Trace("Adding entry: " + clanEntry);
		
		var self:ClanInvitedRequestedScreen = this;		
		clanEntry.mcDelete.onPress = function() { self.RemoveEntry( this._parent ); }	
		
		return clanEntry;
	}
	
	private function AddListEntryIncomingInvite( clan:String, clanProfileImageIndex:Number )
	{
		var entry:MovieClip = AddListEntry( clan, "INVITE", clanProfileImageIndex, LobbyInterface.Instance.GetUsername() );
		
		var self:ClanInvitedRequestedScreen = this;		
		entry.tfInvitation.onPress = entry.mcImage.onPress = function() { self.ShowJoinInvitationMail( this._parent ); }	
	}
	
	private function AddListEntryIncomingRequest( clan:String, clanProfileImageIndex:Number, senderName:String ):Void
	{
		var entry:MovieClip = AddListEntry( clan, "REQUESTED", clanProfileImageIndex, senderName  );		
		
		var self:ClanInvitedRequestedScreen = this;		
		entry.tfInvitation.onPress = entry.mcImage.onPress = function() { self.ShowJoinRequestMail( this._parent ); }	
	}
	
	private function AddListEntryOutgoingRequest( clan:String, clanProfileImageIndex:Number ):Void
	{
		var entry:MovieClip = AddListEntry( clan, "REQUESTED", clanProfileImageIndex, LobbyInterface.Instance.GetUsername()  );		
		
		var self:ClanInvitedRequestedScreen = this;		
		entry.tfInvitation.onPress = entry.mcImage.onPress = function() { self.ShowWithdrawJoinRequestMail( this._parent ); }		
	}
	
	private function OnShowMovieClipEntry( mcEntry:MovieClip, index:Number ):Void 
	{
		super.OnShowMovieClipEntry( mcEntry, index );		
	}
	
	private function OnHideMovieClipEntry( mcEntry:MovieClip ):Void 
	{
		mcEntry.mcDelete.onPress = null;
		mcEntry.mcImage.onPress = null;
		mcEntry.tfInvitation.onPress = null;
	}
	
/*	public function OnSelectClan( clanEntry:MovieClip ):Void
	{
		//tfClanName.text = clanEntry.tfClanName.text;
		//tfClanDescription.text = clanEntry.tfClanName.text + "'s description...";
		//ConsoleWindow.Trace("from me? " + clanEntry.fromMe);
		//ConsoleWindow.Trace("sender name = " + clanEntry.sender);
		mcSelectedEntry = clanEntry;
		if ( !clanEntry.fromMe )
		{
			ShowJoinRequestMail(clanEntry.sender);
		}
		else
		{
			ShowWithdrawJoinRequestMail( clanEntry.tfClanName.text );
		}
	}*/
		
	public function RemoveEntry( clanEntry:MovieClip ):Void
	{
		//f2c_RemoveFriend( clanEntry.tfUsername.text );
		RemoveEntryFromList( clanEntry, mMovieClipList );
	}
	
	public function ShowJoinRequestMail( clanEntry:MovieClip ):Void
	{
		mcSelectedEntry = clanEntry;
		mcMail.Clear();
		mcMail.SetButton( 1, "ACCEPT", Delegate.create(this, SendAcceptJoinRequest) );
		mcMail.SetButton( 2, "REJECT", Delegate.create(this, SendRejectJoinRequest) );
		mcMail.SetButton( 3, "CANCEL", Delegate.create(this, HideMail) );
		
		mcMail.SetTitle( clanEntry.username + " WANTS TO JOIN YOUR CLAN" );
		mcMail.SetToField( clanEntry.username );
		mcMail.DisableToField();
		mcMail.SetCheckBox( "FAIL IF ALREADY IN CLAN" );
		mcMail.swapDepths( getNextHighestDepth() );
		mcMail._visible = true;		
	}
	
	public function HideMail():Void
	{
		mcMail._visible = false;
	}
	
	public function SendAcceptJoinRequest():Void
	{
		ConsoleWindow.Trace( "clan name = " + mcSelectedEntry.tfClanName.text + ", subject = " +  mcMail.GetSubjectField() + ", msg = " +  mcMail.GetMsgField() + 
														", sender = " + 
														   mcSelectedEntry.username + ", check box = " +  mcMail.IsCheckBoxOn() );
		ExternalInterface.call("f2c_Clans_AcceptJoinRequest", mcSelectedEntry.tfClanName.text, mcMail.GetSubjectField(), mcMail.GetMsgField(), "0",
														   mcSelectedEntry.username, mcMail.IsCheckBoxOn());
		//HideMail();
	}
	
	public function SendRejectJoinRequest():Void
	{
		//ConsoleWindow.Trace( "clan name = " + mcSelectedEntry.tfClanName.text + ", subject = " +  mcMail.GetSubjectField() + ", msg = " +  mcMail.GetMsgField() + 
		//												", sender = " + 
		//												   mcSelectedEntry.username + ", check box = " +  mcMail.IsCheckBoxOn() );
		ExternalInterface.call("f2c_Clans_RejectJoinRequest", mcSelectedEntry.tfClanName.text,  mcMail.GetSubjectField(), mcMail.GetMsgField(), "0",
														   mcSelectedEntry.username);
		//HideMail();
	}
		
	public function c2f_Clans_AcceptJoinRequest(resultIdentifier:String):Void
	{
		ConsoleWindow.Trace("c2f_Clans_AcceptJoinRequest(), resultIdentifier = " + resultIdentifier);
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
				HideMail();
				RefreshScreen();
			break;
		}
		
		if ( resultIdentifier != "SUCCESS" )
		{
			LobbyInterface.Instance.CreateMessageBox( resultIdentifier );
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
				HideMail();
				RefreshScreen();
				ConsoleWindow.Trace("c2f_Clans_RejectJoinRequest(), resultIdentifier = " + resultIdentifier);
			break;
		}
		
		if ( resultIdentifier != "SUCCESS" )
		{
			LobbyInterface.Instance.CreateMessageBox( resultIdentifier );
		}
	}
	
	public function ShowWithdrawJoinRequestMail( clanEntry:MovieClip ):Void
	{
		mcSelectedEntry = clanEntry;
		mcMail.Clear();
		mcMail.SetButton( 1, "WITHDRAW", Delegate.create(this, WithdrawJoinRequest) );
		mcMail.SetButton( 2, "CANCEL", Delegate.create(this, HideMail) );
		
		mcMail.SetTitle( "YOU HAVE A PENDING INVITATION TO JOIN CLAN " + clanEntry.tfClanName.text );
		mcMail.SetToField( clanEntry.tfClanName.text );
		mcMail.DisableToField();
		mcMail.swapDepths( getNextHighestDepth() );
		mcMail._visible = true;				
	}
	
	public function WithdrawJoinRequest():Void
	{		
		ExternalInterface.call("f2c_Clans_WithdrawJoinRequest", mcSelectedEntry.tfClanName.text,  mcMail.GetSubjectField(), mcMail.GetMsgField(), "0"
														  );
	}
	
	public function c2f_Clans_WithdrawJoinRequest(resultIdentifier:String):Void
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
				HideMail();
				RefreshScreen();			
				ConsoleWindow.Trace("c2f_Clans_WithdrawJoinRequest(), resultIdentifier = " + resultIdentifier);
			break;
		}
		
		if ( resultIdentifier != "SUCCESS" )
		{
			LobbyInterface.Instance.CreateMessageBox( resultIdentifier );
		}
	}	
	
	public function ShowJoinInvitationMail( clanEntry:MovieClip ):Void
	{
		mcSelectedEntry = clanEntry;
		mcMail.Clear();
		mcMail.SetButton( 1, "ACCEPT", Delegate.create(this, AcceptJoinInvitation) );
		mcMail.SetButton( 2, "REJECT", Delegate.create(this, RejectJoinInvitation) );
		mcMail.SetButton( 3, "CANCEL", Delegate.create(this, HideMail) );
		
		mcMail.SetTitle( "YOU ARE INVITED TO JOIN CLAN " + clanEntry.tfClanName.text );
		mcMail.SetToField( clanEntry.tfClanName.text );
		mcMail.DisableToField();
		mcMail.SetCheckBox( "FAIL IF ALREADY IN CLAN" );
		mcMail.swapDepths( getNextHighestDepth() );
		mcMail._visible = true;				
	}
	
	public function AcceptJoinInvitation():Void
	{		
		ConsoleWindow.Trace("trying to accept join invitation... clan name = " + mcMail.GetToField());
		ExternalInterface.call("f2c_Clans_AcceptJoinInvitation", mcMail.GetToField(), mcMail.GetSubjectField(), mcMail.GetMsgField(), "0",
														   mcMail.IsCheckBoxOn());
	}
	
	public function c2f_Clans_AcceptJoinInvitation(resultIdentifier:String):Void
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
				HideMail();
				RefreshScreen();
			break;
		}
		
		if ( resultIdentifier != "SUCCESS" )
		{
			LobbyInterface.Instance.CreateMessageBox( resultIdentifier );
		}
	}
	
	public function RejectJoinInvitation():Void
	{
		ConsoleWindow.Trace("RejectJoinInvitation(), clan = " + mcMail.GetToField());
		ExternalInterface.call("f2c_Clans_RejectJoinInvitation", mcMail.GetToField, mcMail.GetSubjectField(), mcMail.GetMsgField(), "0"
														   );		
	}
	
	public function c2f_Clans_RejectJoinInvitation(resultIdentifier:String):Void
	{
		switch (resultIdentifier)
		{
			case "Clans_RejectJoinInvitation_UNKNOWN_CLAN":
			break;
			case "Clans_RejectJoinInvitation_NO_SUCH_INVITATION_EXISTS":
			break;
			case "SUCCESS":
				HideMail();
				RefreshScreen();
			break;
		}
		
		if ( resultIdentifier != "SUCCESS" )
		{
			LobbyInterface.Instance.CreateMessageBox( resultIdentifier );
		}
	}
}