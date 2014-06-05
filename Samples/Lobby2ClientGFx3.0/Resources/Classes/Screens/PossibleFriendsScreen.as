import flash.external.*;
import gfx.controls.TextInput;
import gfx.controls.Button;
import mx.utils.Delegate;

class Screens.PossibleFriendsScreen extends ScreenWithPageNavigator
{	
	private var targetHandleEditBox:TextInput;	
	private var subjectEditBox:TextInput;	
	private var bodyEditBox:TextInput;	
	private var emailStatusEditBox:TextInput;	
		
	private var sendInviteButton:Button;
	private var acceptInviteButton:Button;
	private var rejectInviteButton:Button;
	
	private var btnMyFriends:Button;
	
	private var mcProfileImage:MovieClip;		
	private var tfUsername:TextField;
	
	private var mcMail:Mail;
	
	public function PossibleFriendsScreen() 
	{
		ConsoleWindow.Trace("Constructing FriendScreen");						
		
		mScreenId = ScreenID.POSSIBLE_FRIENDS;		
		mScreenTabId = ScreenTab.ID_FRIENDS;	
		
		mEntriesPerPage = 10;
		mFirstEntryX = 440;
		mFirstEntryY = -245;
		mDeltaY = 10;
	}
	
	public function VOnFinishedLoading():Void
	{		
		//Add click event for buttons
		sendInviteButton.addEventListener("click", this, "ShowInvitationMail_Outgoing");
		//acceptInviteButton.addEventListener("click", this, "f2c_AcceptInvite");
		//rejectInviteButton.addEventListener("click", this, "f2c_RejectInvite");
		btnMyFriends.addEventListener("click", this, "ShowMyFriends");
						
		//Add callbacks for C++
		ExternalInterface.addCallback("c2f_SendInviteResult", this, c2f_SendInviteResult);
		ExternalInterface.addCallback("c2f_AcceptInviteResult", this, c2f_AcceptInviteResult);
		ExternalInterface.addCallback("c2f_RejectInviteResult", this, c2f_RejectInviteResult);
		ExternalInterface.addCallback("c2f_GetFriendInvites", this, c2f_GetFriendInvites);
		//ExternalInterface.addCallback("c2f_GetFriends", this, c2f_GetFriends);
		
		mcMail._visible = false;
		
		super.VOnFinishedLoading();
	}
	
	public function OnShow():Void
	{
		super.OnShow();
				
		if ( !mcProfileImage.mcImageContainer.mcImage )
		{
			var imageIndex:Number = LobbyInterface.Instance.GetProfileImageIndex();
			mcProfileImage.attachMovie( "ProfileImage" + imageIndex, "mcImage", mcProfileImage.getNextHighestDepth() );			
		}
		
		tfUsername.text = LobbyInterface.Instance.GetUsername();
		
		if ( mMovieClipList.length > 0 )
		{
			CleanUpMoveClipList( mMovieClipList );
		}
		
		mMovieClipList = new Array();
		mCurrentPage = 0;
		GoToPage( 1 );
		f2c_GetFriendInvites();
		//mIsWaitingForResponseFromServer = false;	
	}
	
	public function ShowInvitationMail_Outgoing():Void
	{
		mcMail.Clear();
		mcMail.SetButton( 1, "CANCEL", Delegate.create(this, HideMail) );
		mcMail.SetButton( 2, "SEND", Delegate.create(this, f2c_SendInvite) );
		
		mcMail.SetTitle( "ASK " + targetHandleEditBox.text + " TO BE FRIENDS:" );
		mcMail.SetToDisplayText( "TO:" );
		mcMail.SetToField( targetHandleEditBox.text );
		mcMail.DisableToField();
		mcMail.swapDepths( getNextHighestDepth() );
		mcMail._visible = true;
	}
	
	public function ShowInvitationMail_Incoming( username:String ):Void
	{
		mcMail.Clear();
		mcMail.SetButton( 1, "IGNORE", Delegate.create(this, HideMail) );
		mcMail.SetButton( 2, "BE FRIENDS", Delegate.create(this, f2c_AcceptInvite) );
		mcMail.SetButton( 3, "DECLINE", Delegate.create(this, f2c_RejectInvite) );
		
		mcMail.SetTitle( username + " WANTS TO BE YOUR FRIEND:" );
		mcMail.SetToDisplayText( "FROM:" );
		mcMail.SetToField( username );
		mcMail.DisableToField();
		mcMail.swapDepths( getNextHighestDepth() );
		mcMail._visible = true;
	}
	
	public function HideMail():Void
	{
		mcMail._visible = false;
	}
	
	public function f2c_SendInvite():Void
	{
		ConsoleWindow.Trace("Attempting to send invitation: name = " + mcMail.GetToField() + ", subject = " + mcMail.GetSubjectField() + ", msg = " + mcMail.GetMsgField());
		ExternalInterface.call("f2c_SendInvite", mcMail.GetToField(), mcMail.GetSubjectField(), mcMail.GetMsgField(), "0");
		HideMail();
	}

	public function c2f_SendInviteResult(resultIdentifier:String):Void
	{
		switch (resultIdentifier)
		{
			case "Friends_SendInvite_UNKNOWN_TARGET_HANDLE":
			break;
			case "Friends_SendInvite_CANNOT_PERFORM_ON_SELF":
			break;
			case "DATABASE_CONSTRAINT_FAILURE":
			break;
			case "Friends_SendInvite_ALREADY_SENT_INVITE":
			break;
			case "Friends_SendInvite_ALREADY_FRIENDS":
			break;
			case "SUCCESS":
				ConsoleWindow.Trace("c2f_SendInviteResult: " + resultIdentifier);
			break;
		}
				
		if ( resultIdentifier != "SUCCESS" )
		{
			LobbyInterface.Instance.CreateMessageBox( resultIdentifier );
		}
	}

	public function f2c_AcceptInvite():Void
	{
		ConsoleWindow.Trace("f2c_AcceptInvite: name = " + mcMail.GetToField() + ", subject = " + mcMail.GetSubjectField() + ", msg = " + mcMail.GetMsgField());
		ExternalInterface.call("f2c_AcceptInvite", mcMail.GetToField(), mcMail.GetSubjectField(), mcMail.GetMsgField(), "0");
		HideMail();
	}

	public function c2f_AcceptInviteResult(resultIdentifier:String):Void
	{
		switch (resultIdentifier)
		{
			case "Friends_AcceptInvite_UNKNOWN_TARGET_HANDLE":
			break;
			case "Friends_AcceptInvite_CANNOT_PERFORM_ON_SELF":
			break;
			case "DATABASE_CONSTRAINT_FAILURE":
			break;
			case "Friends_AcceptInvite_NO_INVITE":
			break;
			case "Friends_AcceptInvite_NO_INVITE":
			break;
			case "SUCCESS":
				ConsoleWindow.Trace("c2f_AcceptInviteResult: " + resultIdentifier);
			break;
		}
				
		if ( resultIdentifier != "SUCCESS" )
		{
			LobbyInterface.Instance.CreateMessageBox( resultIdentifier );
		}		
	}

	public function f2c_RejectInvite():Void
	{
		ConsoleWindow.Trace("f2c_RejectInvite: name = " + mcMail.GetToField() + ", subject = " + mcMail.GetSubjectField() + ", msg = " + mcMail.GetMsgField());
		ExternalInterface.call("f2c_RejectInvite", mcMail.GetToField(), mcMail.GetSubjectField(), mcMail.GetMsgField(), "0");
		HideMail();
	}

	public function c2f_RejectInviteResult(resultIdentifier:String):Void
	{
		switch (resultIdentifier)
		{
			case "Friends_RejectInvite_UNKNOWN_TARGET_HANDLE":
			break;
			case "Friends_RejectInvite_CANNOT_PERFORM_ON_SELF":
			break;
			case "DATABASE_CONSTRAINT_FAILURE":
			break;
			case "Friends_RejectInvite_NO_INVITE":
			break;
			case "SUCCESS":
				ConsoleWindow.Trace("c2f_RejectInviteResult = " + c2f_RejectInviteResult);
			break;
		}
				
		if ( resultIdentifier != "SUCCESS" )
		{
			LobbyInterface.Instance.CreateMessageBox( resultIdentifier );
		}			
	}

	public function f2c_GetFriendInvites():Void
	{
		ExternalInterface.call("f2c_GetFriendInvites");
	}
	
	public function c2f_GetFriendInvites():Void
	{
		ConsoleWindow.Trace("c2f_GetFriendInvites...");
		var invitesSent:Number = arguments[0];
		var invitesReceived:Number = arguments[1];
		for (var i:Number = 0; i < invitesSent; i++) 
		{
			var handleOfUserInviteSentTo:String = arguments[2+i];
			ConsoleWindow.Trace("invite sent to: " + handleOfUserInviteSentTo);
			AddListEntry( handleOfUserInviteSentTo, "INVITED" );
		}
		
		for (var i:Number = 0; i < invitesReceived; i++) 
		{
			var handleOfUserInviteReceivedFrom:String = arguments[2+i+invitesSent];
			ConsoleWindow.Trace("invite received from: " + handleOfUserInviteReceivedFrom);
			AddListEntry( handleOfUserInviteReceivedFrom, "REQUESTED" );
		}	
		
		//TODO: remove this once c++ sends down proper data
		for ( var n:Number = 0; n < 5; n++ )
		{
			AddListEntry( "possible friend" + mMovieClipList.length, "REQUESTED" );
		}		
		
		OnMoveClipListUpdated();		
		ShowPage(1);
	}
	
	public function InviteFriend( _username:String, _profileImageIndex:Number ):Void
	{
		AddListEntry( _username );
		RefreshPage();
		OnMoveClipListUpdated();
	}	
	
	public function RemovePendingEntry( friendEntry:MovieClip ):Void
	{
		//TODO: should we notify c++ that en entry is removed here?
		RemoveEntryFromList( friendEntry, mMovieClipList );
	}
	
	private function AddListEntry( usernameToAdd:String, inviteText:String ):MovieClip
	{
		var userEntry:MovieClip = attachMovie("MyFriendsListEntry", "mcFriendEntry" + mMovieClipList.length, getNextHighestDepth() );
		userEntry._visible = false;
		userEntry.tfUsername.text = usernameToAdd;
		userEntry.tfInvitation.text = inviteText;
		mMovieClipList.push( userEntry );
		
		var self:PossibleFriendsScreen = this;
		userEntry.mcDelete.onPress = function() { self.RemovePendingEntry( this._parent ); }
		if ( inviteText == "REQUESTED" )
		{
			userEntry.mcMail.onPress = function() { self.ShowInvitationMail_Incoming( this._parent.tfUsername.text ); }
		}
		return userEntry;
	}
	
	private function ShowMyFriends():Void
	{
		LobbyInterface.Instance.ShowScreen( ScreenID.FRIENDS );
	}
	
	private function OnShowMovieClipEntry( mcEntry:MovieClip, index:Number ):Void 
	{
		super.OnShowMovieClipEntry( mcEntry, index );
		//mcEntry.tfInvitation._visible = false;
	}

}