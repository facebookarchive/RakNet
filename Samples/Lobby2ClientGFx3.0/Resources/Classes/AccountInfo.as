import flash.filters.GradientGlowFilter;
import flash.external.*;
import Screens.ProfileScreen;
import Screens.RegisterAccountScreen;

class AccountInfo
{
	private var mFirstName:String;
	private var mMiddleName:String;
	private var mLastName:String;
	private var mHomeAddress1:String;
	private var mHomeAddress2:String;
	private var mHomeCity:String;
	private var mHomeState:String;
	private var mHomeZipCode:String;
	private var mBillingAddress1:String;
	private var mBillingAddress2:String;
	private var mBillingCity:String;
	private var mBillingState:String;
	private var mBillingZipCode:String;
	private var mEmailAddress:String;
	private var mPassword:String;
	private var mPasswordRecoveryQuestion:String;
	private var mPasswordRecoveryAnswer:String;
	//private var caption1:TextInput;
	//private var caption2:TextInput;
	//private var ageInDays:TextInput;
	private var mUserName:String;	
	
	private var mRace:String;
	private var mHomeCountry:String;
	private var mBillingCountry:String;
	private var mIsMale:Boolean;
	private var mAgeInDays:Number;
	private var mBirthYear:Number;
	private var mBirthMonth:Number;
	private var mBirthDate:Number;
	
	private var mAboutMe:String;
	private var mActivities:String;
	private var mInterests:String;
	private var mFavoriteGames:String;
	private var mFavoriteMovies:String;
	private var mFavoriteBooks:String;
	private var mFavoriteQuotes:String;
	
	private static var mInstance:AccountInfo;
	
	public function AccountInfo()
	{
		mInstance = this;
	}
	
	public static function get Instance():AccountInfo
	{
		return mInstance;
	}
	
	public function GetAccountInfoFromServer():Void
	{		
		ConsoleWindow.Trace("AccountInfo.GetAccountInfoFromServer");
		// Do not change the order the parameters are passed in as
		ExternalInterface.call("f2c_GetAccountDetails");
	}
	
	public function ServerSetAccountInfo( resultIdentifier:String, firstNameParam:String, middleNameParam:String, lastNameParam:String, raceParam:String, 
										  sex_maleParam:Boolean, homeAddress1Param:String, homeAddress2Param:String, homeCityParam:String, homeStateParam:String,
										  homeCountryParam:String, homeZipCodeParam:String, billingAddress1Param:String, billingAddress2Param:String, 
										  billingCityParam:String, billingStateParam:String, billingCountryParam:String, billingZipCodeParam:String, 
										  emailAddressParam:String, passwordParam:String, passwordRecoveryQuestionParam:String, passwordRecoveryAnswerParam:String,
										  caption1Param:String, caption2Param:String, ageInDaysParam:Number, aboutMe:String, activities:String , interests:String, 
										  favGames:String , favMovies:String, favBooks:String, favQuotes:String ):Void
	{
		ConsoleWindow.Trace( "AccountInfo.ServerSetAccountInfo()... " + resultIdentifier );
		switch (resultIdentifier)
		{
			case "SUCCESS":
			{
				mFirstName = firstNameParam; 
				mMiddleName = middleNameParam; 
				mLastName = lastNameParam;
				mRace = raceParam; 
				mIsMale = sex_maleParam; 
				mHomeAddress1 = homeAddress1Param;
				mHomeAddress2 = homeAddress2Param; 
				mHomeCity = homeCityParam; 
				mHomeState = homeStateParam;
				mHomeCountry = homeCountryParam; 
				mHomeZipCode = homeZipCodeParam; 
				mBillingAddress1 = billingAddress1Param;
				mBillingAddress2 = billingAddress2Param; 
				mBillingCity = billingCityParam; 
				mBillingState = billingStateParam;
				mBillingCountry = billingCountryParam; 
				mBillingZipCode = billingZipCodeParam; 
				mEmailAddress = emailAddressParam;
				mPassword = passwordParam; 
				mPasswordRecoveryQuestion = passwordRecoveryQuestionParam; 
				mPasswordRecoveryAnswer = passwordRecoveryAnswerParam;
				//caption1.text = caption1Param; 
				//caption2.text = caption2Param; 
				mAgeInDays = ageInDaysParam;
				var object:Object = { };
				ConvertAgeDaysToBirthdate( mAgeInDays, object );
				mBirthYear = object.year;
				mBirthMonth = object.month;
				mBirthDate = object.date;
				
				mUserName = LobbyInterface.Instance.GetUsername();
				
				mAboutMe = aboutMe;
				mActivities = activities;
				mInterests = interests;
				mFavoriteGames = favGames;
				mFavoriteBooks = favBooks;
				mFavoriteMovies = favMovies;
				mFavoriteQuotes = favQuotes;
				
			/*	ConsoleWindow.Trace("Home state = " + mHomeState);
				ConsoleWindow.Trace("Race = " + mRace);
				ConsoleWindow.Trace("mBirthYear = " + mBirthYear);
				ConsoleWindow.Trace("mBirthMonth = " + mBirthMonth);
				ConsoleWindow.Trace("mBirthDate = " + mBirthDate);
				ConsoleWindow.Trace("mAgeInDays = " + mAgeInDays);
				ConsoleWindow.Trace("aboutMe = " + aboutMe);
				ConsoleWindow.Trace("activities = " + activities);
				ConsoleWindow.Trace("interests = " + interests);
				ConsoleWindow.Trace("favGames = " + favGames);
				ConsoleWindow.Trace("favMovies = " + favMovies);
				ConsoleWindow.Trace("favBooks = " + favBooks);
				ConsoleWindow.Trace("favQuotes = " + favQuotes);*/
				
				ProfileScreen.Instance.OnReceivedPlayerInfo();
				break;
			}
			case "DATABASE_CONSTRAINT_FAILURE":
				//ConsoleWindow.Trace( "Can't find the logged in user in the database for some reason" );
				LobbyInterface.Instance.CreateMessageBox(resultIdentifier);
			break;
		}
	}
	
	public function GetFirstName():String 	{ return mFirstName; }
	public function GetMiddleName():String 	{ return mMiddleName; }
	public function GetLastName():String 	{ return mLastName; }
	public function GetRace():String 		{ return mRace; }
	public function GetIsMale():Boolean 	{ return mIsMale; }
	public function GetHomeAddress1():String { return mHomeAddress1; }
	public function GetHomeAddress2():String { return mHomeAddress2; }
	public function GetHomeCity():String 	{ return mHomeCity; }
	public function GetHomeState():String 	{ return mHomeState; }
	public function GetHomeCountry():String { return mHomeCountry; }
	public function GetHomeZipCode():String { return mHomeZipCode; }
	public function GetBillingAddress1():String { return mBillingAddress1; }
	public function GetBillingAddress2():String { return mBillingAddress2; }
	public function GetBillingCity():String 	{ return mBillingCity; }
	public function GetBillingState():String 	{ return mBillingState; }
	public function GetBillingCountry():String 	{ return mBillingCountry; }
	public function GetBillingZipCode():String 	{ return mBillingZipCode; }
	public function GetEmailAddress():String 	{ return mEmailAddress; }
	public function GetPassword():String 		{ return mPassword; }
	public function GetPasswordRecoveryQuestion():String 	{ return mPasswordRecoveryQuestion; }
	public function GetPasswordRecoveryAnswer():String 		{ return mPasswordRecoveryAnswer; }
	public function GetAgeInDays():Number	{ return mAgeInDays; }
	public function GetBirthYear():Number	{ return mBirthYear; }
	public function GetBirthMonth():Number	{ return mBirthMonth; }
	public function GetBirthDate():Number	{ return mBirthDate; }
	public function GetUserName():String	{ return mUserName; }
	public function GetAboutMe():String		{ return mAboutMe; }
	public function GetActivities():String	{ return mActivities; }
	public function GetInterests():String	{ return mInterests; }
	public function GetFavoriteGames():String	{ return mFavoriteGames; }
	public function GetFavoriteBooks():String	{ return mFavoriteBooks; }
	public function	GetFavoriteMovies():String { return mFavoriteMovies; }
	public function GetFavoriteQuotes():String { return mFavoriteQuotes; }
	
	public function SetFirstName( firstName:String ):Void 		{ mFirstName = firstName; }
	public function SetMiddleName( middleName:String ):Void 	{ mMiddleName = middleName; }
	public function	SetLastName( lastName:String ):Void 		{ mLastName = lastName; }
	public function SetRace( race:String ):Void 				{ mRace = race; }
	public function SetIsMale( isMale:Boolean ):Void	 		{ mIsMale = isMale; }
	public function SetHomeAddress1( homeAddress1:String):Void 	{ mHomeAddress1 = homeAddress1; }
	public function SetHomeAddress2( homeAddress2:String):Void 	{ mHomeAddress2 = homeAddress2; }
	public function SetHomeCity( homeCity:String):Void 			{ mHomeCity = homeCity; }
	public function SetHomeState( homeState:String):Void 		{ mHomeState = homeState; }
	public function SetHomeCountry( homeCountry:String):Void 	{ mHomeCountry = homeCountry; }
	public function SetHomeZipCode( homeZipeCode:String):Void 	{ mHomeZipCode = homeZipeCode; }
	public function SetBillingAddress1( billingAddress1:String):Void 	{ mBillingAddress1 = billingAddress1; }
	public function SetBillingAddress2( billingAddress2:String):Void 	{ mBillingAddress2 = billingAddress2; }
	public function SetBillingCity( billingCity:String):Void 			{ mBillingCity = billingCity; }
	public function SetBillingState( billingState:String):Void 			{ mBillingState = billingState; }
	public function SetBillingCountry( billingCountry:String):Void 		{ mBillingCountry = billingCountry; }
	public function SetBillingZipCode( billingZipCode:String):Void 		{ mBillingZipCode = billingZipCode; }
	public function SetEmailAddress( billingEmail:String):Void 			{ mEmailAddress = billingEmail; }
	public function SetPassword( password:String):Void 					{ mPassword = password; }
	public function SetPasswordRecoveryQuestion( passwordQuestion:String):Void 	{ mPasswordRecoveryQuestion = passwordQuestion; }
	public function SetPasswordRecoveryAnswer( passwordAnswer:String):Void		{ mPasswordRecoveryAnswer = passwordAnswer; }
	public function SetAgeInDays( ageInDays:Number ):Void	{ mAgeInDays = ageInDays; }
	public function SetBirthYear( birthYear:Number ):Void	{ mBirthYear = birthYear; }
	public function SetBirthMonth( birthMonth:Number ):Void	{ mBirthMonth = birthMonth; }
	public function SetBirthDate( birthDate:Number ):Void	{ mBirthDate = birthDate; }
	public function SetUserName( username:String ):Void		{ mUserName = username; }
	public function SetAboutMe( aboutMe:String ):Void		{ mAboutMe = aboutMe; }
	public function SetActivities( activities:String ):Void	{ mActivities = activities; }
	public function SetInterests( interests:String ):Void	{ mInterests = interests; }
	public function SetFavoriteGames( games:String ):Void	{ mFavoriteGames = games; }
	public function SetFavoriteBooks( books:String ):Void	{ mFavoriteBooks = books; }
	public function	SetFavoriteMovies( movies:String ):Void { mFavoriteMovies = movies; }
	public function SetFavoriteQuotes( quotes:String ):Void { mFavoriteQuotes = quotes; }
	
	public static function ConvertBirthdateToDays( month:Number, date:Number, year:Number ):Number
	{
		var ageInDays:Number = 0;
		month -= 1; //jan = 0, feb = 1....
		
		var curDate:Date = new Date();
		var curMonth:Number = curDate.getMonth();
		ageInDays = (curDate.getFullYear() - year) * 365;
		if( month > curMonth )
		{
			ageInDays -= 365;
		}
		for ( var i:Number = 0; i < 12; i++ )
		{
			if ( month != curMonth )
			{
				ageInDays += RegisterAccountScreen.MonthDays[month];
				month = ( month + 1 ) % 12;
			}
			else
			{				
				ageInDays += curDate.getDate() - date;
				break;
			}
		}
				
		return ageInDays;
	}
	
	public static function ConvertAgeDaysToBirthdate( ageInDays:Number, object:Object ):Void
	{
		//ConsoleWindow.Trace("ConsoleWindow.Trace(), ageInDays = " + ageInDays);
		var curDate:Date = new Date();
		var yearFromNow:Number = Math.floor(ageInDays / 365);
		ageInDays = ageInDays % 365;
		var month:Number = curDate.getMonth();
		var date:Number = curDate.getDate();
		for ( var i:Number = 0; i < 12; i++ )
		{
			trace("Age in days  = " + ageInDays);
			if( ageInDays >= date )
			{
				month--;
				if( month < 0 )
				{
					month = 11;
					yearFromNow++;
				}
				
				ageInDays -= RegisterAccountScreen.MonthDays[month];
			}
			else
			{
				date = date - ageInDays;
				break;
			}
		}
		
		object.year = curDate.getFullYear() - yearFromNow;
		object.month = month + 1;
		object.date = date;
		
		//ConsoleWindow.Trace("year = " + year + ", month = " + month + ", date = " + date);
	}
}