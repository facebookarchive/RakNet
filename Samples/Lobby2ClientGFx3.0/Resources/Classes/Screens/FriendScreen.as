import flash.external.*;
import gfx.controls.TextInput;
import gfx.controls.Button;
import mx.utils.Delegate;

class Screens.FriendScreen extends ScreenWithPageNavigator
{	
	private var targetHandleEditBox:TextInput;	
	private var subjectEditBox:TextInput;	
	private var bodyEditBox:TextInput;	
	private var emailStatusEditBox:TextInput;	
		
	private var sendInviteButton:Button;
	private var acceptInviteButton:Button;
	private var rejectInviteButton:Button;
	private var removeFriendButton:Button;
	private var getInvitesButton:Button;
	private var getFriendsButton:Button;
	
	private var btnPossibleFriends:Button;
	
	private var mcProfileImage:MovieClip;		
	private var tfUsername:TextField;
	
	private var mcMail:Mail;
	
	public function FriendScreen() 
	{
		ConsoleWindow.Trace("Constructing FriendScreen");						
		
		mScreenId = ScreenID.FRIENDS;		
		mScreenTabId = ScreenTab.ID_FRIENDS;	
		
		mEntriesPerPage = 10;
		mFirstEntryX = 440;
		mFirstEntryY = -245;
		mDeltaY = 10;
	}
	
	public function VOnFinishedLoading():Void
	{		
		//Add click event for buttons
		sendInviteButton.addEventListener("click", this, "ShowInvitationMail");
		acceptInviteButton.addEventListener("click", this, "f2c_AcceptInvite");
		rejectInviteButton.addEventListener("click", this, "f2c_RejectInvite");
		removeFriendButton.addEventListener("click", this, "f2c_RemoveFriend");
		getInvitesButton.addEventListener("click", this, "f2c_GetFriendInvites");
		//getFriendsButton.addEventListener("click", this, "f2c_GetFriends");
		btnPossibleFriends.addEventListener("click", this, "ShowPossibleFriends");
						
		//Add callbacks for C++
		ExternalInterface.addCallback("c2f_SendInviteResult", this, c2f_SendInviteResult);
		ExternalInterface.addCallback("c2f_AcceptInviteResult", this, c2f_AcceptInviteResult);
		ExternalInterface.addCallback("c2f_RejectInviteResult", this, c2f_RejectInviteResult);
		ExternalInterface.addCallback("c2f_RemoveFriendResult", this, c2f_RemoveFriendResult);
		//ExternalInterface.addCallback("c2f_GetFriendInvites", this, c2f_GetFriendInvites);
		ExternalInterface.addCallback("c2f_GetFriends", this, c2f_GetFriends);
		
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
		f2c_GetFriends();
		//mIsWaitingForResponseFromServer = false;	
		if ( LobbyInterface.Instance.IsInFlashMode() )
		{
			c2f_GetFriends();			
		}
	}
	
	public function ShowInvitationMail():Void
	{
		mcMail.Clear();
		mcMail.SetButton( 1, "CANCEL", Delegate.create(this, HideMail) );
		mcMail.SetButton( 2, "SEND", Delegate.create(this, f2c_SendInvite) );
		
		mcMail.SetTitle( "ASK " + targetHandleEditBox.text + " TO BE FRIENDS:" );
		mcMail.SetToField( targetHandleEditBox.text );
		mcMail.DisableToField();
		mcMail.swapDepths( getNextHighestDepth() );
		mcMail._visible = true;
	}
		
	public function ShowFriendMail( friendName:String ):Void
	{
		mcMail.Clear();
		mcMail.SetButton( 1, "CANCEL", Delegate.create(this, HideMail) );
		mcMail.SetButton( 2, "SEND", Delegate.create(this, f2c_SendInvite) );
		
		mcMail.SetTitle( "COMPOSE MESSAGE" );
		mcMail.SetToField( friendName );
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
		ExternalInterface.call("f2c_AcceptInvite", targetHandleEditBox.text, subjectEditBox.text, bodyEditBox.text, emailStatusEditBox.text);
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
			break;
		}
		
		if ( resultIdentifier != "SUCCESS" )
		{
			LobbyInterface.Instance.CreateMessageBox( resultIdentifier );
		}
	}

	public function f2c_RejectInvite():Void
	{
		ExternalInterface.call("f2c_RejectInvite", targetHandleEditBox.text, subjectEditBox.text, bodyEditBox.text, emailStatusEditBox.text);
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
			break;
		}
		
		if ( resultIdentifier != "SUCCESS" )
		{
			LobbyInterface.Instance.CreateMessageBox( resultIdentifier );
		}
	}

	public function f2c_RemoveFriend( _username:String ):Void
	{
		ExternalInterface.call("f2c_RemoveFriend", _username, "", "", "");
	}

	public function c2f_RemoveFriendResult(resultIdentifier:String):Void
	{
		switch (resultIdentifier)
		{
			case "Friends_Remove_UNKNOWN_TARGET_HANDLE":
			break;
			case "Friends_Remove_CANNOT_PERFORM_ON_SELF":
			break;
			case "DATABASE_CONSTRAINT_FAILURE":
			break;
			case "Friends_Remove_NOT_FRIENDS":
			break;
			case "SUCCESS":		
				ConsoleWindow.Trace("c2f_RemoveFriendResult: " + resultIdentifier);
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

	public function f2c_GetFriends():Void
	{
		ExternalInterface.call("f2c_GetFriends");
	}

	public function c2f_GetFriendInvites():Void
	{
		var invitesSent:Number = arguments[0];
		var invitesReceived:Number = arguments[1];
		for (var i:Number=0; i<invitesSent; i++) {
			var handleOfUserInviteSentTo:String = arguments[2+i];
		}
		
		for (var i:Number=0; i<invitesReceived; i++) {
			var handleOfUserInviteReceivedFrom:String = arguments[2+i+invitesSent];
		}	
	}

	public function c2f_GetFriends():Void
	{
		ConsoleWindow.Trace("c2f_GetFriends...");
		for (var i:Number=0; i<arguments.length; i++)
		{
			var friendName:String = arguments[i];
			ConsoleWindow.Trace(friendName);
			AddListEntry( friendName );
		}
		
		//TODO: remove this once c++ sends down proper data
		for ( var n:Number = 0; n < 7; n++ )
		{
			AddListEntry( "friend" + mMovieClipList.length );
		}		
		
		OnMoveClipListUpdated();		
		ShowPage(1);
	}
	
	public function InviteFriend( _username:String, _profileImageIndex:Number ):Void
	{
		AddListEntry( _username, _profileImageIndex );
		
		RefreshPage();
		OnMoveClipListUpdated();
	}	
	
	public function RemoveFriend( friendEntry:MovieClip ):Void
	{
		f2c_RemoveFriend( friendEntry.tfUsername.text );
		RemoveEntryFromList( friendEntry, mMovieClipList );
	}
	
	private function AddListEntry( usernameToAdd:String, _profileImageIndex:Number ):MovieClip
	{
		var userEntry:MovieClip = attachMovie("MyFriendsListEntry", "mcFriendEntry" + mMovieClipList.length, getNextHighestDepth() );
		userEntry._visible = false;
		userEntry.tfUsername.text = usernameToAdd;
		mMovieClipList.push( userEntry );	
		
		var self:FriendScreen = this;		
		userEntry.mcDelete.onPress = function() { self.RemoveFriend( this._parent ); }	
		userEntry.mcMail.onPress = function() { self.ShowFriendMail( this._parent.tfUsername.text ); }	
		return userEntry;
	}
	
	public function ShowPossibleFriends():Void
	{
		LobbyInterface.Instance.ShowScreen( ScreenID.POSSIBLE_FRIENDS );
	}
	
	private function OnShowMovieClipEntry( mcEntry:MovieClip, index:Number ):Void 
	{
		super.OnShowMovieClipEntry( mcEntry, index );
		mcEntry.tfInvitation._visible = false;
	}

}