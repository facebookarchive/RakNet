
import flash.external.*;
import gfx.controls.TextInput;
import gfx.controls.Button;
import Screens.ClanGeneralScreen;

class Screens.MyClansScreen extends ScreenWithPageNavigator
{	
	private var btnBack:Button;
	private var mClanNames:Array;
	
	private static var mInstance:MyClansScreen;
		
	public function MyClansScreen() 
	{
		ConsoleWindow.Trace("Constructing MyClansScreen");
		
		mScreenId = ScreenID.CLAN_MY_CLANS;
		mScreenTabId = ScreenTab.ID_CLANS;
		
		mEntriesPerPage = 12;
		mFirstEntryX = 435;
		mFirstEntryY = -380;
		mDeltaY = 10;
		
		mInstance = this;
	}
	
	public static function get Instance():MyClansScreen { return mInstance; }
	
	public function VOnFinishedLoading():Void
	{		
		//Add click event for buttons
		//btnCreateClan.addEventListener("click", this, "SetStateToClanFunctionsGeneral");
		btnBack.addEventListener("click", this, "Back");
				
		//Add callbacks for C++
		ExternalInterface.addCallback("c2f_Clans_Get", this, c2f_Clans_Get);
		
		super.VOnFinishedLoading();
	}
	
	public function OnShow():Void
	{
		super.OnShow();
		
		if ( mMovieClipList.length > 0 )
		{
			CleanUpMoveClipList( mMovieClipList );
		}
		
		mMovieClipList = new Array();
		mCurrentPage = 0;
		GoToPage( 1 );
		ExternalInterface.call("f2c_Clans_Get");
	}
	
	private function AddListEntry( clanToAdd:String, _profileImageIndex:Number ):MovieClip
	{
		var clanEntry:MovieClip = attachMovie("ClanListEntry", "mcClanEntry" + mMovieClipList.length, getNextHighestDepth() );
		clanEntry._visible = false;
		clanEntry.tfClanName.text = clanToAdd;
		mMovieClipList.push( clanEntry );
		ConsoleWindow.Trace("Adding entry: " + clanEntry);
		
		var self:MyClansScreen = this;
		clanEntry.onPress = function() { self.OnSelectClan( this ); }
		
		return clanEntry;
	}
	
	public function OnSelectClan( clanEntry:MovieClip ):Void
	{
		ConsoleWindow.Trace("clanRootScreen.OnSelectClan, clan = " + clanEntry._name);
		ClanGeneralScreen.Instance.SetClanName( clanEntry.tfClanName.text );
		LobbyInterface.Instance.ShowScreen( ScreenID.CLAN_GENERAL );
	}
		
	public function c2f_Clans_Get():Void
	{		
		ConsoleWindow.Trace("c2f_Clans_Get(), resultIdentifier = " + resultIdentifier);
		var resultIdentifier:String = arguments[0];
		switch (resultIdentifier)
		{
			case "DATABASE_CONSTRAINT_FAILURE":
			break;
			case "SUCCESS":
			{
				mClanNames = new Array();
		
				var numClansReturned:Number = arguments[1] + 1;
				ConsoleWindow.Trace("Number of clans = " + numClansReturned);
				var argumentIndex:Number=2;
				for (var i:Number=0; i < numClansReturned; i++)
				{
					ConsoleWindow.Trace("clan = " + clanName + ", description = " + description + ", clanLeader = " + clanLeader );
					var clanName:String = arguments[argumentIndex++];
					var description:String = arguments[argumentIndex++];
					var clanLeader:String = arguments[argumentIndex++];
					if ( clanName )
					{
						mClanNames.push( clanName );
						var clanMembersOtherThanLeader:Number = arguments[argumentIndex++];
						//for (var j:Number=0; j < clanMembersOtherThanLeader; j++)
						{
							//var clanMemberName:String = arguments[argumentIndex++];
							//ConsoleWindow.Trace("other memebers = " + clanMemberName);
						}
												
						AddListEntry(clanName);
					}
				}
										
				OnMoveClipListUpdated();
				ShowPage(1);
			
				break;
			}
		}
		
		if ( resultIdentifier != "SUCCESS" )
		{
			LobbyInterface.Instance.CreateMessageBox( resultIdentifier );
		}
	}
		
	public function Back():Void
	{
		LobbyInterface.Instance.ShowScreen( ScreenID.CLAN_ROOT );
	}
}