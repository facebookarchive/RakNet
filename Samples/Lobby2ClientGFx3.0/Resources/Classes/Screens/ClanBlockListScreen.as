import flash.external.*;
import gfx.controls.TextInput;
import gfx.controls.Button;
import gfx.controls.ScrollBar;
import mx.utils.Delegate;

class Screens.ClanBlockListScreen extends ScreenWithPageNavigator
{	
	private var ignoreUsernameEditBox:TextInput;	
	private var startIgnoreButton:Button;
	private var btnBack:Button;		
	private var tfClanName:TextField;
		
	private var mIsWaitingForResponseFromServer:Boolean;	//true when user hit ignore or stop ignore to wait for server to verify the action
	private var mUsernameOnHold:String;	
	private var mcMail:Mail;
	private var mUserToUnblock:MovieClip;
	
	private static var mInstance:ClanBlockListScreen;
			
	public function ClanBlockListScreen() 
	{
		ConsoleWindow.Trace("Constructing ClanBlockListScreen");				
		
		mScreenId = ScreenID.CLAN_BLOCK_LIST;
		mScreenTabId = ScreenTab.ID_CLANS;
		mIsWaitingForResponseFromServer = false;
		
		mEntriesPerPage = 10;
		mFirstEntryX = 440;
		mFirstEntryY = -245;
		mDeltaY = 10;
		
		mInstance = this;
	}
	
	public static function get Instance():ClanBlockListScreen
	{
		return mInstance;
	}
	
	public function VOnFinishedLoading():Void
	{					
		//Add click event for buttons
		startIgnoreButton.addEventListener("click", this, "ShowKickAndBlockMail");
		//stopIgnoreButton.addEventListener("click", this, "f2c_StopIgnore");
		btnBack.addEventListener("click", this, "Back");
				
		//Add callbacks for C++
	//	ExternalInterface.addCallback("c2f_StartIgnore", this, c2f_StartIgnore);
	//	ExternalInterface.addCallback("c2f_StopIgnore", this, c2f_StopIgnore);
	//	ExternalInterface.addCallback("c2f_GetIgnoreListResult", this, c2f_GetIgnoreListResult);
		ExternalInterface.addCallback("c2f_Clans_KickAndBlacklistUser", this, c2f_Clans_KickAndBlacklistUser);
		ExternalInterface.addCallback("c2f_Clans_UnblacklistUser", this, c2f_Clans_UnblacklistUser);
		ExternalInterface.addCallback("c2f_Clans_GetBlacklist", this, c2f_Clans_GetBlacklist);
		
		super.VOnFinishedLoading();
	}
	
	public function OnShow():Void
	{
		super.OnShow();
		mcMail._visible = false;
				
		//ConsoleWindow.Trace("blocked user list length = " + mMovieClipList.length);
		if ( mMovieClipList.length > 0 )
		{
			CleanUpMoveClipList( mMovieClipList );
		}
		
		mMovieClipList = new Array();
		mCurrentPage = 0;		
		GoToPage( 1 );
		f2c_GetIgnoreList();
		mIsWaitingForResponseFromServer = false;		
	}

	public function SetClanName( name:String ):Void		{ tfClanName.text = name; }	
	
	public function Back():Void
	{
		LobbyInterface.Instance.ShowLastScreen();
	}
	
	public function ShowKickAndBlockMail():Void
	{
		mcMail.Clear();
		mcMail.SetButton( 1, "CANCEL", Delegate.create(this, HideMail) );
		mcMail.SetButton( 2, "KICK", Delegate.create(this, f2c_Clans_KickAndBlacklistUser) );
		
		mcMail.SetTitle( "KICK AND BLOCK USER FROM CLAN" );
		mcMail.SetToField( ignoreUsernameEditBox.text );
		mcMail.SetSubjectField( "Kicked From " + tfClanName.text );
		mcMail.SetCheckBox( "BLOCK USER" );
		mcMail.swapDepths( getNextHighestDepth() );
		mcMail._visible = true;				
	}
	
	public function f2c_Clans_KickAndBlacklistUser():Void
	{
		if ( !mIsWaitingForResponseFromServer )
		{
			mIsWaitingForResponseFromServer = true;
			//ExternalInterface.call("f2c_StartIgnore", ignoreUsernameEditBox.text);		
			mUsernameOnHold = ignoreUsernameEditBox.text;
			
			ExternalInterface.call("f2c_Clans_KickAndBlacklistUser", tfClanName.text, mcMail.GetSubjectField(), mcMail.GetMsgField(), "0",
														   mcMail.GetToField(),
														   true,
														   mcMail.IsCheckBoxOn(),
														   "Unknown Reason"   //The 'reason' field is sent to all clan members and is a short message or identifier. It is also stored in the database along with their username, so that if say a moderator later wanted to know why the guy was kicked, you could tell why. As it is defined by the application, this could be a text string, a phrase, a paragraph, or just some identifier.
														   );
		}
	}
		
	public function ShowStopIgnoreMail( blockedUserEntry:MovieClip ):Void
	{
		if ( !mIsWaitingForResponseFromServer )
		{
			mIsWaitingForResponseFromServer = true;
			mUserToUnblock = blockedUserEntry;
			mcMail.Clear();
			mcMail.SetButton( 1, "CANCEL", Delegate.create(this, HideMail) );
			mcMail.SetButton( 2, "UNBLOCK", Delegate.create(this, StopIgnore) );
			
			mcMail.SetTitle( "UNBLOCK USER FROM CLAN" );
			mcMail.SetToField( blockedUserEntry.tfUsername.text );
			mcMail.DisableToField();
			mcMail.SetSubjectField( "Unblocked From " + tfClanName.text );
			mcMail.swapDepths( getNextHighestDepth() );
			mcMail._visible = true;			
		}
	}

	public function StopIgnore():Void
	{
		ConsoleWindow.Trace("StopIgnore: " + mUserToUnblock);
		if ( mUserToUnblock )
		{
			//f2c_StopIgnore( blockedUserEntry.tfUsername.text );
			
			ExternalInterface.call("f2c_Clans_UnblacklistUser", tfClanName.text, mcMail.GetSubjectField(), mcMail.GetMsgField(), "0",
															   mUserToUnblock.tfUsername.text);															   
			RemoveEntryFromList( mUserToUnblock, mMovieClipList );
		}
	}
		
	public function f2c_StopIgnore( _username:String ):Void
	{
		if ( !mIsWaitingForResponseFromServer )
		{
			ExternalInterface.call("f2c_StopIgnore", _username);		
		}
	}

	public function f2c_GetIgnoreList():Void
	{
		//ExternalInterface.call("f2c_GetIgnoreList");
		ExternalInterface.call("f2c_Clans_GetBlacklist", tfClanName.text);
	}

	public function c2f_GetIgnoreListResult():Void
	{
		ConsoleWindow.Trace("c2f_GetIgnoreListResult...");
		for (var i:Number = 0; i < arguments.length; i++) 
		{
			// Array of handles (string)
			ConsoleWindow.Trace(arguments[i]);
		}
		
		//TODO: remove this once c++ sends down proper data
		for ( var n:Number = 0; n < 25; n++ )
		{
			/*var userEntry:MovieClip = attachMovie("BlockListEntry", "blockedUser" + mMovieClipList.length, getNextHighestDepth() );
			userEntry._visible = false;
			userEntry.tfUsername.text = "testUser" + mMovieClipList.length;
			mMovieClipList.push( userEntry );				
			var self:BlockListScreen = this;		
			userEntry.mcDelete.onPress = function() { self.StopIgnore( this._parent ); }*/
			AddBlockListEntry( "testUser" + mMovieClipList.length );
		}		
		
		OnMoveClipListUpdated();		
		ShowPage(1);
	}
		
	public function IgnoreUser( _username:String, _profileImageIndex:Number ):Void
	{
	/*	var userEntry:MovieClip = attachMovie("BlockListEntry", "blockedUser" + mMovieClipList.length, getNextHighestDepth() );
		userEntry._visible = false;
		userEntry.tfUsername.text = _username;
		mMovieClipList.push( userEntry );
		var self:BlockListScreen = this;		
		userEntry.mcDelete.onPress = function() { self.StopIgnore( this._parent ); }*/
		AddBlockListEntry( _username );
		
		TogglePageVisibility(mCurrentPage, true );
		OnMoveClipListUpdated();
	}		
	
	public function HideMail():Void
	{
		mcMail._visible = false;
	}
	
	public function AddBlockListEntry( _username:String ):Void
	{
		var userEntry:MovieClip = attachMovie("BlockListEntry", "blockedUser" + mMovieClipList.length, getNextHighestDepth() );
		userEntry._visible = false;
		userEntry.tfUsername.text = _username;
		mMovieClipList.push( userEntry );	
		ConsoleWindow.Trace("Ignoring user: " + userEntry);
		var self:ClanBlockListScreen = this;		
		userEntry.mcDelete.onPress = function() { self.ShowStopIgnoreMail( this._parent ); }		
	}
		
	private function OnShowMovieClipEntry( mcEntry:MovieClip, index:Number ):Void 
	{
		//ConsoleWindow.Trace("BlockList... OnShowMovieClipEntry: " + mcEntry.mcDelete.onPress);
		super.OnShowMovieClipEntry( mcEntry, index );			
	}
	
	private function OnHideMovieClipEntry( mcEntry:MovieClip ):Void 
	{
		//ConsoleWindow.Trace("BlockList... OnHideMovieClipEntry: " + mcEntry.mcDelete.onPress );
		mcEntry.mcDelete.onPress = null;
	}
	
	public function c2f_Clans_GetBlacklist(resultIdentifier:String):Void
	{
		ConsoleWindow.Trace("c2f_Clans_GetBlacklist(), arguments = " + arguments);
		switch (resultIdentifier)
		{
			case "Clans_GetBlacklist_UNKNOWN_CLAN":
			break;
			case "DATABASE_CONSTRAINT_FAILURE":
			break;
			case "SUCCESS":
				ConsoleWindow.Trace("c2f_Clans_GetBlacklist(), success!");
				var length:Number = arguments.length;
				for ( var i:Number = 1; i < length; i++ )
				{
					AddBlockListEntry( arguments[i] );
				}
		
				OnMoveClipListUpdated();		
				ShowPage(1);
			break;
		}
		
		if ( resultIdentifier != "SUCCESS" )
		{
			LobbyInterface.Instance.CreateMessageBox( resultIdentifier );
		}
	}
	
	public function c2f_Clans_KickAndBlacklistUser(resultIdentifier:String):Void
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
				ConsoleWindow.Trace("c2f_Clans_KickAndBlacklistUser(), success!");
				HideMail();
			break;
		}
		
		mIsWaitingForResponseFromServer = false;
		
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
				ConsoleWindow.Trace("c2f_Clans_UnblacklistUser(), success");
				HideMail();
			break;
		}
		
		mIsWaitingForResponseFromServer = false;
		
		if ( resultIdentifier != "SUCCESS" )
		{
			LobbyInterface.Instance.CreateMessageBox( resultIdentifier );
		}
	}
}