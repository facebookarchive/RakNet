import gfx.controls.TextArea;
import gfx.controls.Button;
import Screens.RegisterAccountScreen3;
import flash.external.*;

class Screens.ProfileScreen extends Screen
{
	private var mcEditProfile:Button;
	private var mcSaveProfile:Button;
	private var mcChange:Button;
	private var mcEditBlockList:Button;
	private var mcEditInfo:Button;
	private var tfUsername:TextField;	
	private var mcProfileImage:MovieClip;
	private var taAboutMe:TextArea;
	private var taActivities:TextArea;
	private var taInterests:TextArea;
	private var taFavoriteGames:TextArea;
	private var taFavoriteMovies:TextArea;
	private var taFavoriteBooks:TextArea;
	private var taFavoriteQuotes:TextArea;
	
	private var mEditMode:Boolean;
	
	private static var mInstance:ProfileScreen;
			
	public function ProfileScreen()
	{
		ConsoleWindow.Trace("Constructing ProfileScreen");
		
		mScreenId = ScreenID.PROFILE;
		mScreenTabId = ScreenTab.ID_PROFILE;
		
		mInstance = this;
	}
	
	public static function get Instance():ProfileScreen { return mInstance; }
	
	public function VOnFinishedLoading():Void
	{	
		//Add click event for buttons
		mcEditBlockList.addEventListener("click", this, "OnClickEditBlockList");
		mcSaveProfile.addEventListener("click", this, "OnClickSaveProfile");
		mcEditProfile.addEventListener("click", this, "OnClickEditProfile");
		mcChange.addEventListener("click", this, "OnClickedChange");
		mcEditInfo.addEventListener("click", this, "OnClickEditInfo");
		
		//Add callbacks for C++
		super.VOnFinishedLoading();
	}
	
	public function OnShow():Void
	{
		if ( !mcProfileImage.mcImageContainer.mcImage )
		{
			var imageIndex:Number = LobbyInterface.Instance.GetProfileImageIndex();
			mcProfileImage.attachMovie( "ProfileImage" + imageIndex, "mcImage", mcProfileImage.getNextHighestDepth() );			
		}
		tfUsername.text = LobbyInterface.Instance.GetUsername();
		
		EnableTextInput( false );
	}
	
	public function EnableTextInput( state:Boolean ):Void
	{
		mcEditProfile._visible = !state;
		mcSaveProfile._visible = state;
		mEditMode = state;
		taAboutMe.disabled = !state;
		taActivities.disabled = !state;
		taInterests.disabled = !state;
		taFavoriteBooks.disabled = !state;
		taFavoriteGames.disabled = !state;
		taFavoriteMovies.disabled = !state;
		taFavoriteQuotes.disabled = !state;
	}
	
	public function OnReceivedPlayerInfo():Void
	{		
		taAboutMe.text = AccountInfo.Instance.GetAboutMe();
		taActivities.text = AccountInfo.Instance.GetActivities();
		taInterests.text = AccountInfo.Instance.GetInterests();
		taFavoriteGames.text = AccountInfo.Instance.GetFavoriteGames();
		taFavoriteMovies.text = AccountInfo.Instance.GetFavoriteBooks();
		taFavoriteBooks.text = AccountInfo.Instance.GetFavoriteMovies();
		taFavoriteQuotes.text = AccountInfo.Instance.GetFavoriteQuotes();	
	}
	
	public function GetAboutMe():String 	{ return taAboutMe.text; }
	public function GetActivities():String 	{ return taActivities.text; }
	public function GetInterests():String 	{ return taInterests.text; }
	public function GetFavoriteGames():String { return taFavoriteGames.text; }
	public function GetFavoriteBooks():String { return taFavoriteMovies.text; }
	public function GetFavoriteMovies():String { return taFavoriteBooks.text; }
	public function GetFavoriteQuotes():String { return taFavoriteQuotes.text; }
	
	public function OnClickSaveProfile():Void
	{
		EnableTextInput(false);
		f2c_UpdateAccount();
	}
	
	public function OnClickEditProfile():Void
	{
		EnableTextInput( true );
	}
	
	public function OnClickedChange():Void
	{
		LobbyInterface.Instance.ShowScreen( ScreenID.CHANGE_PHOTO );		
	}
	
	public function OnClickEditBlockList():Void
	{
		LobbyInterface.Instance.ShowScreen( ScreenID.BLOCK_LIST );
	}
	
	public function OnClickEditInfo():Void
	{
		LobbyInterface.Instance.ShowScreen( ScreenID.REGISTER_ACCOUNT_PERSONAL );
	}	
	
	public function f2c_UpdateAccount():Void
	{									
		// Do not change the order the parameters are passed in as
		ExternalInterface.call("f2c_UpdateAccount",
		[
		 AccountInfo.Instance.GetFirstName(), AccountInfo.Instance.GetMiddleName(), AccountInfo.Instance.GetLastName(),
		 AccountInfo.Instance.GetRace(), AccountInfo.Instance.GetIsMale(), AccountInfo.Instance.GetHomeAddress1(),
		 AccountInfo.Instance.GetHomeAddress2(), AccountInfo.Instance.GetHomeCity(), AccountInfo.Instance.GetHomeState(),
		 AccountInfo.Instance.GetHomeCountry(),
		 AccountInfo.Instance.GetHomeZipCode(),
		 AccountInfo.Instance.GetBillingAddress1(),
		 AccountInfo.Instance.GetBillingAddress2(), AccountInfo.Instance.GetBillingCity(), AccountInfo.Instance.GetBillingState(),
		 AccountInfo.Instance.GetBillingCountry(),
		 AccountInfo.Instance.GetBillingZipCode(), AccountInfo.Instance.GetEmailAddress(),
		 AccountInfo.Instance.GetPassword(), AccountInfo.Instance.GetPasswordRecoveryQuestion(), AccountInfo.Instance.GetPasswordRecoveryAnswer(),
		 "", "", 
		 String( AccountInfo.Instance.GetAgeInDays() ),
		 GetAboutMe(),
		 GetActivities(),
		 GetInterests(),
		 GetFavoriteGames(),
		 GetFavoriteMovies(),
		 GetFavoriteBooks(),
		 GetFavoriteQuotes()
		 ]);					 
	}
}