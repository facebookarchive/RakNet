
class ScreenID
{	
	public static var INVALID_ID:Number				= -1;
	
	public static var BLOCK_LIST:Number 			= 0;
	public static var CLAN_ROOT:Number 				= 1;
	public static var CLAN_EMAIL:Number 			= 2;
	public static var CLAN_GENERAL:Number 			= 3;
	public static var CLAN_QUERY:Number 			= 4;
	public static var CONNECTING_TO_SERVER:Number 	= 5;
	public static var CONNECTION:Number 			= 6;
	public static var EMAIL:Number	 				= 7;
	public static var ENTER_CD_KEY:Number			= 8;
	public static var FRIENDS:Number				= 9;
	public static var POSSIBLE_FRIENDS:Number		= 10;
	public static var LOGGING_IN:Number				= 11;
	public static var LOGIN:Number					= 12;
	public static var LOGGED_IN:Number				= 13;
	public static var RECOVER_PASSWORD:Number			= 14;
	public static var REGISTER_ACCOUNT_PERSONAL:Number	= 15;
	public static var REGISTER_ACCOUNT_BILLING:Number	= 16;
	public static var REGISTER_ACCOUNT_LOGIN:Number		= 17;
	public static var UPDATE_ACCOUNT:Number				= 18;
	public static var PROFILE:Number					= 19;
	public static var CHANGE_PHOTO:Number				= 20;
	public static var CLAN_CREATE:Number				= 21;
	public static var CLAN_INVITED_REQUESTED:Number		= 22;
	public static var CLAN_MY_CLANS:Number				= 23;
	public static var CLAN_BLOCK_LIST:Number			= 24;
	
	public static var TOTAL_SCREENS:Number 	= 25;
	
	//Used as screen instance names
	/*public static function GetScreenName( id:Number ):String
	{
		switch( id )
		{
			case BLOCK_LIST:
				return "mcBlockListScreen";
				
			case CLAN_ROOT:
				return "mcClanRootScreen";
				
			case CLAN_EMAIL:
				return "mcClanEmailScreen";
				
			case CLAN_GENERAL:
				return "mcClanGeneralScreen";
			
			case CLAN_QUERY:
				return "mcClanQueryScreen";
			
			case CONNECTING_TO_SERVER:
				return "mcConnectingToServerScreen";
				
			case CONNECTION:
				return "mcConnectionScreen";
				
			case EMAIL:
				return "mcEmailScreen";
			
			case ENTER_CD_KEY:
				return "mcEnterCDKeyScreen";
				
			case FRIENDS:
				return "mcFriendScreen";
			
			case LOGGING_IN:
				return "mcLoggingInScreen";
				
			case LOGIN:
				return "mcLoginScreen";
				
			case LOGGED_IN:
				return "mcLoggedInScreen";
				
			case RECOVER_PASSWORD:
				return "mcRecoverPasswordScreen";
				
			case REGISTER_ACCOUNT_PERSONAL:
				return "mcRegisterAccountScreen";
				
			case REGISTER_ACCOUNT_BILLING:
				return "mcRegisterAccountScreen2";
				
			case REGISTER_ACCOUNT_LOGIN:
				return "mcRegisterAccountScreen3";
				
			case UPDATE_ACCOUNT:
				return "mcUpdateAccountScreen";
				
			case PROFILE:
				return "mcProfileScreen";
				
			default:
				ConsoleWindow("ERROR!! INVALID SCREEN ID: " + id);
				return null;
		}
	}*/
}	