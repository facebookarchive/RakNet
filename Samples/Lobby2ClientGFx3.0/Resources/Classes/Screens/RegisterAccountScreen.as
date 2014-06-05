import flash.display.BitmapData;
import flash.external.*;
import gfx.controls.TextInput;
import gfx.controls.Button;
import gfx.controls.CheckBox;
import gfx.data.dataProvider;
import gfx.controls.DropdownMenu;

//Personal Infomation
class Screens.RegisterAccountScreen extends Screen
{	
	private var firstName:TextInput;
	private var middleName:TextInput;
	private var lastName:TextInput;
	private var homeAddress1:TextInput;
	private var homeAddress2:TextInput;
	private var homeCity:TextInput;
	private var homeState:DropdownMenu;
	private var homeZipCode:TextInput;
	private var emailAddress:TextInput;
	
	private var birthMonth:DropdownMenu;
	private var birthDay:DropdownMenu;
	private var birthYear:DropdownMenu;
	private var race:DropdownMenu;
	private var homeCountry:DropdownMenu;
	private var sex_male:CheckBox;
	private var sex_female:CheckBox;
	
	public static var Months:Array = [ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 ];
	public static var MonthDays:Array = [ 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 ];  //each element is number of days for that month
			
	/*private var countryList:Array = ["Afghanistan", "Albania", "Algeria", "American Samoa", "Andorra", "Angola", "Anguilla", "Antarctica", "Antigua and Barbuda",
							 "Argentina", "Armenia", "Aruba", "Australia", "Austria", "Azerbaijan", "Azores", "Bahamas", "Bahrain", "Bangladesh", "Barbados", "Belarus",
							 "Belgium", "Belize", "Benin", "Bermuda", "Bhutan", "Bolivia", "Bosnia And Herzegowina", "Bosnia-Herzegovina", "Botswana", "Bouvet Island", "Brazil",
							 "British Indian Ocean Territory", "British Virgin Islands", "Brunei Darussalam", "Bulgaria", "Burkina Faso", "Burundi", "Cambodia", "Cameroon",
							 "Canada", "Cape Verde", "Cayman Islands", "Central African Republic", "Chad", "Chile", "China", "Christmas Island", "Cocos (Keeling) Islands",
							 "Colombia", "Comoros", "Congo", "Congo, The Democratic Republic O", "Cook Islands", "Corsica", "Costa Rica", "Cote d` Ivoire (Ivory Coast)", "Croatia",
							 "Cuba", "Cyprus", "Czech Republic", "Denmark", "Djibouti", "Dominica", "Dominican Republic", "East Timor", "Ecuador", "Egypt", "El Salvador", 
							 "Equatorial Guinea", "Eritrea", "Estonia", "Ethiopia", "Falkland Islands (Malvinas)", "Faroe Islands", "Fiji", "Finland", "France (Includes Monaco)", 
							 "France, Metropolitan", "French Guiana", "French Polynesia", "French Polynesia (Tahiti)", "French Southern Territories", "Gabon", "Gambia", "Georgia", 
							 "Germany", "Ghana", "Gibraltar", "Greece", "Greenland", "Grenada", "Guadeloupe", "Guam", "Guatemala", "Guinea", "Guinea-Bissau", "Guyana", "Haiti", 
							 "Heard And Mc Donald Islands", "Holy See (Vatican City State)", "Honduras", "Hong Kong", "Hungary", "Iceland", "India", "Indonesia", "Iran", "Iraq", 
							 "Ireland", "Ireland (Eire)", "Israel", "Italy", "Jamaica", "Japan", "Jordan", "Kazakhstan", "Kenya", "Kiribati", "Korea, Democratic People''s Repub", 
							 "Kuwait", "Kyrgyzstan", "Laos", "Latvia", "Lebanon", "Lesotho", "Liberia", "Libya", "Liechtenstein", "Lithuania", "Luxembourg", "Macao", "Macedonia", 
							 "Madagascar", "Madeira Islands", "Malawi", "Malaysia", "Maldives", "Mali", "Malta", "Marshall Islands", "Martinique", "Mauritania", "Mauritius", "Mayotte", 
							 "Mexico", "Micronesia, Federated States Of", "Moldova, Republic Of", "Monaco", "Mongolia", "Montserrat", "Morocco", "Mozambique", "Myanmar (Burma)", 
							 "Namibia", "Nauru", "Nepal", "Netherlands", "Netherlands Antilles", "New Caledonia", "New Zealand", "Nicaragua", "Niger", "Nigeria", "Niue", 
							 "Norfolk Island", "Northern Mariana Islands", "Norway", "Oman", "Pakistan", "Palau", "Palestinian Territory, Occupied", "Panama", "Papua New Guinea", 
							 "Paraguay", "Peru", "Philippines", "Pitcairn", "Poland", "Portugal", "Puerto Rico", "Qatar", "Reunion", "Romania", "Russian Federation", "Rwanda", 
							 "Saint Kitts And Nevis", "San Marino", "Sao Tome and Principe", "Saudi Arabia", "Senegal", "Serbia-Montenegro", "Seychelles", "Sierra Leone", "Singapore", 
							 "Slovak Republic", "Slovenia", "Solomon Islands", "Somalia", "South Africa", "South Georgia And The South Sand", "South Korea", "Spain", "Sri Lanka", 
							 "St. Christopher and Nevis", "St. Helena", "St. Lucia", "St. Pierre and Miquelon", "St. Vincent and the Grenadines", "Sudan", "Suriname", 
							 "Svalbard And Jan Mayen Islands", "Swaziland", "Sweden", "Switzerland", "Syrian Arab Republic", "Taiwan", "Tajikistan", "Tanzania", "Thailand", 
							 "Togo", "Tokelau", "Tonga", "Trinidad and Tobago", "Tristan da Cunha", "Tunisia", "Turkey", "Turkmenistan", "Turks and Caicos Islands", "Tuvalu", 
							 "Uganda", "Ukraine", "Unid Arab Emirates", "United Kingdom", "United States", "United States Minor Outlying Isl", "Uruguay", "Uzbekistan", 
							 "Vanuatu", "Vatican City", "Venezuela", "Vietnam", "Virgin Islands (U.S.)", "Wallis and Furuna Islands", "Western Sahara",
							 "Western Samoa", "Yemen", "Yugoslavia", "Zaire", "Zambia", "Zimbabwe", "Great Britain"];*/
	public static var stateList:Array = ["Alabama", "Alaska", "Arizona", "Arkansas", "California", "Colorado", "Connecticut", "Delaware", "Florida", 
									"Georgia", "Hawaii", "Idaho", "Illinois", "Indiana", "Iowa", "Kansas", "Kentucky", "Louisiana", "Maine", "Maryland", 
									"Massachusetts", "Michigan", "Minnesota", "Missouri", "Montana", "Nebraska", "Nevada", "New Hampshire", 
									"New Jersey", "New Mexico", "New York", "North Carolina", "North Dakota", "Ohio", "Oklahoma", "Oregon", 
									"Pennsylvania", "Rhode Island", "South Carolina", "South Dakota", "Tennessee", "Texas", "Utah", "Vermont", 
									"Virginia", "Washington", "West Virginia", "Wisconsin", "Wyoming" ]
							 		
	private var cancelButton:Button;
	private var nextButton:Button;
	
	public function RegisterAccountScreen() 
	{
		ConsoleWindow.Trace("Constructing RegisterAccountScreen");				
		
		mScreenId = ScreenID.REGISTER_ACCOUNT_PERSONAL;		
		mScreenTabId = ScreenTab.ID_REGISTRATION;
	}
	
	public function VOnFinishedLoading():Void
	{
		birthMonth.dataProvider = Months;
		//homeCountry.dataProvider = countryList;
		homeState.dataProvider = stateList;
		race.dataProvider = ["CAUSCASIAN", "AFRCAN AMERICAN", "NATIVE AMERICAN", "HISPANIC", "ASIAN", "PACIFIC ISLANDS", "UNSPECIFIED"];
		
		var year:Array = new Array();
		for ( var i = 2009; i > 1930; i-- )
		{
			year.push( i );
		}
		
		birthYear.dataProvider = year;		
		
		OnMonthChange();
				
		//Add click event for buttons
		cancelButton.addEventListener("click", this, "Cancel");
		nextButton.addEventListener("click", this, "Next");
		birthMonth.addEventListener("change", this, "OnMonthChange");
		sex_male.addEventListener("select", this, "OnSelectMale");
		sex_female.addEventListener("select", this, "OnSelectFemale");
				
		super.VOnFinishedLoading();
	}
	
	public function OnShow():Void
	{
		if ( LobbyInterface.Instance.IsLoggedIn() )
		{
			gotoAndStop("Update");
			PopulateFromAccountInfo();
		}
		else
		{
			gotoAndStop("New");
		}
		//homeState.dataProvider = ["causcasian", "afrcan american", "native american", "hispanic", "asian", "pacific islands", "unspecified"];//; stateList;		
	}
	
	public function Next():Void
	{
		ConsoleWindow.Trace(" month = " + GetSelectedMonth() + ", day = " + GetSelectedDate() + ", year = " + GetSelectedYear() );
		ConsoleWindow.Trace("Age in days = " + GetAgeInDays());
		if ( LobbyInterface.Instance.IsLoggedIn() )
		{	
			SaveToAccountInfo();
		}
		LobbyInterface.Instance.ShowScreen( ScreenID.REGISTER_ACCOUNT_BILLING );
	}
	
	public function Cancel():Void
	{
		if ( LobbyInterface.Instance.IsLoggedIn() )
		{	
			LobbyInterface.Instance.ShowScreen( ScreenID.PROFILE );
			//user cancel editing profile info, let's reset it by re-getting it from server
			AccountInfo.Instance.GetAccountInfoFromServer();
		}
		else
		{
			LobbyInterface.Instance.ShowScreen( ScreenID.LOGIN );			
		}
	}
	
	public function OnMonthChange(event:Object):Void 
	{
		RepopulateBirthDayList( birthMonth.selectedIndex + 1 );
	}	
	
	public function RepopulateBirthDayList( month:Number ):Void
	{
		var length = MonthDays[month - 1];
		var days:Array = new Array();
		for ( var i = 1; i <= length; i++ )
		{
			days.push( string(i) );
		}
		birthDay.dataProvider = days;
	}
	
	public function OnSelectMale():Void
	{
		sex_female.selected = !sex_male.selected;
	}
	
	public function OnSelectFemale():Void
	{
		sex_male.selected = !sex_female.selected;
	}
	
	public function GetStateList():Array
	{
		return stateList;
	}
	
	public function GetStreetAddress():String
	{
		return homeAddress1.text;
	}
	
	public function GetStreetAddress2():String
	{
		return homeAddress2.text;
	}
	
	public function GetCity():String
	{
		return homeCity.text;
	}
	
	public function GetState():Number
	{
		return homeState.selectedIndex;
	}
	
	public function GetZipCode():String
	{
		return homeZipCode.text;
	}
	
	public function GetFirstName():String
	{
		return firstName.text;
	}
	
	public function GetMiddleInitial():String
	{
		return middleName.text;
	}
	
	public function GetLastName():String
	{
		return lastName.text;
	}
	
	public function GetSelectedRace():String
	{
		return race.dataProvider[race.selectedIndex];
	}
	
	public function GetIsMale():Boolean
	{
		return sex_male.selected;
	}
	
	public function GetSelectedState():String
	{
		return homeState.dataProvider[homeState.selectedIndex];
	}
	
	public function GetEmail():String
	{
		return emailAddress.text;
	}
	
	public function GetSelectedMonth():Number
	{
		return birthMonth.dataProvider[birthMonth.selectedIndex];		
	}
	
	public function GetSelectedDate():Number
	{
		return birthDay.dataProvider[birthDay.selectedIndex];		
	}
	
	public function GetSelectedYear():Number
	{
		return birthYear.dataProvider[birthYear.selectedIndex];		
	}
	
	public function GetAgeInDays():Number
	{
		return AccountInfo.ConvertBirthdateToDays( GetSelectedMonth(), GetSelectedDate(), GetSelectedYear() );
	}
	
	private function PopulateFromAccountInfo():Void
	{
		firstName.text = AccountInfo.Instance.GetFirstName();
		middleName.text = AccountInfo.Instance.GetMiddleName();
		lastName.text = AccountInfo.Instance.GetLastName();
		//birthMonth.selectedIndex = AccountInfo.Instance.get
		emailAddress.text = AccountInfo.Instance.GetEmailAddress();
		sex_male.selected = AccountInfo.Instance.GetIsMale();
		sex_female.selected = !sex_male.selected;
		homeAddress1.text = AccountInfo.Instance.GetHomeAddress1();
		homeAddress2.text = AccountInfo.Instance.GetHomeAddress2();
		homeCity.text = AccountInfo.Instance.GetHomeCity();
		homeState.selectedIndex = GetIndexFromElement( AccountInfo.Instance.GetHomeState(), stateList );
		homeZipCode.text = AccountInfo.Instance.GetHomeZipCode();
		race.selectedIndex = GetIndexFromElement( AccountInfo.Instance.GetRace(), race.dataProvider );
		birthYear.selectedIndex = GetIndexFromElement( String(AccountInfo.Instance.GetBirthYear()), birthYear.dataProvider );
		birthMonth.selectedIndex = GetIndexFromElement( AccountInfo.Instance.GetBirthMonth(), birthMonth.dataProvider );
		birthDay.selectedIndex = GetIndexFromElement( AccountInfo.Instance.GetBirthDate(), birthDay.dataProvider );
		//AccountInfo.ConvertAgeDaysToBirthdate( 45 );
	}
	
	//temporary save, data will get wiped if user exits edit account
	private function SaveToAccountInfo():Void
	{
		AccountInfo.Instance.SetFirstName( firstName.text );
		AccountInfo.Instance.SetMiddleName( middleName.text );
		AccountInfo.Instance.SetLastName( lastName.text );
		AccountInfo.Instance.SetEmailAddress( emailAddress.text );
		AccountInfo.Instance.SetIsMale( sex_male.selected );
		AccountInfo.Instance.SetHomeAddress1( homeAddress1.text );
		AccountInfo.Instance.SetHomeAddress2( homeAddress2.text );
		AccountInfo.Instance.SetHomeCity( homeCity.text );
		AccountInfo.Instance.SetHomeState( GetSelectedState() );
		AccountInfo.Instance.SetHomeZipCode( homeZipCode.text );
		AccountInfo.Instance.SetRace( GetSelectedRace() );
		AccountInfo.Instance.SetAgeInDays( AccountInfo.ConvertBirthdateToDays( GetSelectedMonth(), GetSelectedDate(), GetSelectedYear() ) );
	}
}