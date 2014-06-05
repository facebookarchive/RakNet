import flash.external.*;
import gfx.controls.TextInput;
import gfx.controls.Button;
import Screens.ClanGeneralScreen;

class Screens.ClanRootScreen extends ScreenWithPageNavigator
{	
	private var btnCreateClan:Button;
	private var btnPending:Button;
	private var btnMyClans:Button;
	private var btnSearch:Button;
	private var tiSearch:TextInput;
	
	private var mClanNames:Array;
	
	private static var mInstance:ClanRootScreen;
		
	public function ClanRootScreen() 
	{
		ConsoleWindow.Trace("Constructing ClanRootScreen");						
		
		mScreenId = ScreenID.CLAN_ROOT;	
		mScreenTabId = ScreenTab.ID_CLANS;
		
		mEntriesPerPage = 10;
		mFirstEntryX = 435;
		mFirstEntryY = -280;
		mDeltaY = 10;
		
		mInstance = this;
	}
	
	public static function get Instance():ClanRootScreen { return mInstance; }
	
	public function VOnFinishedLoading():Void
	{		
		//Add click event for buttons
		//btnCreateClan.addEventListener("click", this, "SetStateToClanFunctionsGeneral");
		btnCreateClan.addEventListener("click", this, "CreateClan");
		btnPending.addEventListener("click", this, "SetStateToClanFunctionsInvolvingEmail");
		btnMyClans.addEventListener("click", this, "GoToMyClans");
		btnSearch.addEventListener("click", this, "Search");
				
		//Add callbacks for C++
		ExternalInterface.addCallback("c2f_Clans_GetList", this, c2f_Clans_GetList);
		
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
		ExternalInterface.call("f2c_Clans_GetList");	
	}
	
	public function CreateClan():Void
	{
		LobbyInterface.Instance.ShowScreen( ScreenID.CLAN_CREATE );
	}
	
	public function GoToMyClans():Void
	{
		LobbyInterface.Instance.ShowScreen( ScreenID.CLAN_MY_CLANS );
	}
	
	public function SetStateToClanFunctionsGeneral():Void
	{
		//_root.gotoAndStop("ClanFunctionsGeneral");
		LobbyInterface.Instance.ShowScreen( ScreenID.CLAN_GENERAL );
	}

	public function SetStateToClanFunctionsInvolvingEmail():Void
	{
		//_root.gotoAndStop("ClanFunctionsInvolvingEmail");
		//LobbyInterface.Instance.ShowScreen( ScreenID.CLAN_EMAIL );
		LobbyInterface.Instance.ShowScreen( ScreenID.CLAN_INVITED_REQUESTED );
	}

	public function SetStateToClanFunctionsQuery():Void
	{
		//_root.gotoAndStop("ClanFunctionsQuery");
		LobbyInterface.Instance.ShowScreen( ScreenID.CLAN_QUERY );
	}
	
	public function Search():Void
	{		
		var targetClan:String = tiSearch.text.toLowerCase();
		if ( targetClan.length > 0 )
		{
			if ( mMovieClipList.length > 0 )
			{
				CleanUpMoveClipList( mMovieClipList );
			}
			
			mMovieClipList = new Array();
			mCurrentPage = 0;
			GoToPage( 1 );
			
			ConsoleWindow.Trace("Search()... " + targetClan);
			var length:Number = mClanNames.length;
			for ( var i:Number = 0; i < length; i++ )
			{
				var clan:String = mClanNames[i].toLowerCase();
				//ConsoleWindow.Trace("... looking at " + clan);
				if ( clan.length >= targetClan.length )
				{
					var length2:Number = clan.length;
					var lettersFound:Number = 0;
					for ( var m:Number = 0; m < length2; m++ )
					{					
						//ConsoleWindow.Trace("... letter " + clan.charAt(m));
						//ConsoleWindow.Trace("... comparing " + targetClan.charAt(lettersFound) + " and " + clan.charAt(m));
						if ( targetClan.charAt(lettersFound) == clan.charAt(m) )
						{
							lettersFound++;						
							//ConsoleWindow.Trace("... found " + lettersFound);
							if ( lettersFound == targetClan.length )
							{
								//ConsoleWindow.Trace("Adding entry! " + clan);
								AddListEntry( clan, 1 );
								m = length2;
							}
						}
						else
						{
							if ( lettersFound > 0 )
							{
								m--;
							}
							lettersFound = 0;
						}
					}
				}
			}
			
			OnMoveClipListUpdated();
			ShowPage(1);
		}
	}
	
	public function c2f_Clans_GetList():Void
	{
		var resultIdentifier:String = arguments[0];
		ConsoleWindow.Trace("c2f_Clans_GetList, resultIdentifier = " + resultIdentifier);
		switch (resultIdentifier)
		{
			case "DATABASE_CONSTRAINT_FAILURE":
			break;
			case "SUCCESS":
			{				
				mClanNames = new Array();
				var numClanNames = arguments.length-1;
				for (var i:Number=0; i < numClanNames; i++)
				{
					var clanName:String = arguments[1+i];
					ConsoleWindow.Trace("clanName = " + clanName);
					AddListEntry(clanName);
					mClanNames.push( clanName ); 
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
	
	private function AddListEntry( clanToAdd:String, _profileImageIndex:Number ):MovieClip
	{
		var clanEntry:MovieClip = attachMovie("ClanListEntry", "mcClanEntry" + mMovieClipList.length, getNextHighestDepth() );
		clanEntry._visible = false;
		clanEntry.tfClanName.text = clanToAdd;
		mMovieClipList.push( clanEntry );
		ConsoleWindow.Trace("Adding entry: " + clanEntry);
		
		var self:ClanRootScreen = this;
		clanEntry.onPress = function() { self.OnSelectClan( this ); }
		
		return clanEntry;
	}
	
	public function OnSelectClan( clanEntry:MovieClip ):Void
	{
		ConsoleWindow.Trace("clanRootScreen.OnSelectClan, clan = " + clanEntry._name);
		ClanGeneralScreen.Instance.SetClanName( clanEntry.tfClanName.text );
		LobbyInterface.Instance.ShowScreen( ScreenID.CLAN_GENERAL );
	}		
}