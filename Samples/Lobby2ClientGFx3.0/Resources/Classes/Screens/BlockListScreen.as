import flash.external.*;
import gfx.controls.TextInput;
import gfx.controls.Button;
import gfx.controls.ScrollBar;
import mx.utils.Delegate;

class Screens.BlockListScreen extends ScreenWithPageNavigator
{	
	private var ignoreUsernameEditBox:TextInput;	
	private var startIgnoreButton:Button;
	private var btnBack:Button;		
	private var tfUsername:TextField;
	
	private var mcProfileImage:MovieClip;
	private var mcScrollBar:ScrollBar
	
	private var mIsWaitingForResponseFromServer:Boolean;	//true when user hit ignore or stop ignore to wait for server to verify the action
	private var mUsernameOnHold:String;	
			
	public function BlockListScreen() 
	{
		ConsoleWindow.Trace("Constructing BlockListScreen");				
		
		mScreenId = ScreenID.BLOCK_LIST;
		mScreenTabId = ScreenTab.ID_PROFILE;
		mIsWaitingForResponseFromServer = false;
		
		mEntriesPerPage = 10;
		mFirstEntryX = 440;
		mFirstEntryY = -245;
		mDeltaY = 10;
	}
	
	public function VOnFinishedLoading():Void
	{					
		//Add click event for buttons
		startIgnoreButton.addEventListener("click", this, "f2c_StartIgnore");
		//stopIgnoreButton.addEventListener("click", this, "f2c_StopIgnore");
		btnBack.addEventListener("click", this, "Back");
		mcScrollBar.addEventListener("scroll", this, "OnScroll");
				
		//Add callbacks for C++
		ExternalInterface.addCallback("c2f_StartIgnore", this, c2f_StartIgnore);
		ExternalInterface.addCallback("c2f_StopIgnore", this, c2f_StopIgnore);
		ExternalInterface.addCallback("c2f_GetIgnoreListResult", this, c2f_GetIgnoreListResult);
		
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
	
	public function OnScroll( position:Number ):Void
	{
		ConsoleWindow.Trace("scrolling pos: " + position);
	}
	
	public function Back():Void
	{
		LobbyInterface.Instance.ShowScreen( ScreenID.PROFILE );
	}
	
	public function f2c_StartIgnore():Void
	{
		if ( !mIsWaitingForResponseFromServer )
		{
			mIsWaitingForResponseFromServer = true;
			ExternalInterface.call("f2c_StartIgnore", ignoreUsernameEditBox.text);		
			mUsernameOnHold = ignoreUsernameEditBox.text;
		}
	}

	public function c2f_StartIgnore(resultIdentifier:String):Void
	{
		switch (resultIdentifier)
		{
			case "Client_StartIgnore_UNKNOWN_TARGET_HANDLE":
				//ConsoleWindow.Trace("Client_StartIgnore_UNKNOWN_TARGET_HANDLE");
			break;
			case "Client_StartIgnore_CANNOT_PERFORM_ON_SELF":
				//ConsoleWindow.Trace("Client_StartIgnore_CANNOT_PERFORM_ON_SELF");
			break;
			case "Client_StartIgnore_ALREADY_IGNORED":
				//ConsoleWindow.Trace("Client_StartIgnore_ALREADY_IGNORED");
			break;
			case "SUCCESS":
				ConsoleWindow.Trace("start ignore SUCCESS");
				//IgnoreUser( mUsernameOnHold, 1 );	//TODO: uncomment this
			break;
		}
		
		mIsWaitingForResponseFromServer = false;
		//IgnoreUser( mUsernameOnHold, 1 );		//TODO: comment this out
		if ( resultIdentifier != "SUCCESS" )
		{
			LobbyInterface.Instance.CreateMessageBox( resultIdentifier );
		}
	}

	public function StopIgnore( blockedUserEntry:MovieClip ):Void
	{
		ConsoleWindow.Trace("StopIgnore: " + blockedUserEntry);
		f2c_StopIgnore( blockedUserEntry.tfUsername.text );
		RemoveEntryFromList( blockedUserEntry, mMovieClipList );
	}
		
	public function f2c_StopIgnore( _username:String ):Void
	{
		if ( !mIsWaitingForResponseFromServer )
		{
			ExternalInterface.call("f2c_StopIgnore", _username);
		}
	}

	public function c2f_StopIgnore(resultIdentifier:String):Void
	{
		switch (resultIdentifier)
		{
			case "Client_StopIgnore_UNKNOWN_TARGET_HANDLE":
				//ConsoleWindow.Trace("Client_StopIgnore_UNKNOWN_TARGET_HANDLE");
			break;
			case "Client_StopIgnore_CANNOT_PERFORM_ON_SELF":
				//ConsoleWindow.Trace("Client_StopIgnore_CANNOT_PERFORM_ON_SELF");
			break;
			case "SUCCESS":
				//ConsoleWindow.Trace("stop ignore success");
			break;
		}
		
		mIsWaitingForResponseFromServer = false;
		if ( resultIdentifier != "SUCCESS" )
		{
			LobbyInterface.Instance.CreateMessageBox( resultIdentifier );
		}
	}

	public function f2c_GetIgnoreList():Void
	{
		ExternalInterface.call("f2c_GetIgnoreList");
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
	
	public function AddBlockListEntry( _username:String ):Void
	{
		var userEntry:MovieClip = attachMovie("BlockListEntry", "blockedUser" + mMovieClipList.length, getNextHighestDepth() );
		userEntry._visible = false;
		userEntry.tfUsername.text = _username;
		mMovieClipList.push( userEntry );	
		ConsoleWindow.Trace("Ignoring user: " + userEntry);
		var self:BlockListScreen = this;		
		userEntry.mcDelete.onPress = function() { self.StopIgnore( this._parent ); }		
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
}