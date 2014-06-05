/**
* October 20, 2009
* @author Dezhu Chen
*/

class ScreenTab extends MovieClip
{
	// Public Properties that are set from the FLA file	
	[Inspectable(type="Number")]
	public var tabID:Number;	
	
	[Inspectable(type="Boolean")]
	public var isHiddenByDefault:Boolean;
	
	private var mcLabel:MovieClip;
	private var mStartingPosX:Number;
	public static var cExtendedPosX:Number = -100;
	
	//DO NOT Change these, they're being referenced in the FLA
	public static var ID_INVALID:Number			= 0;
	public static var ID_LOGIN:Number			= 0;
	public static var ID_PROFILE:Number			= 1;
	public static var ID_FRIENDS:Number			= 2;
	public static var ID_EMAIL:Number			= 3;
	public static var ID_CLANS:Number			= 4;
	public static var ID_CONNECTION:Number		= 5;
	public static var ID_CD_KEY:Number			= 6;
	public static var ID_REGISTRATION:Number	= 7;
		
	public function ScreenTab() 
	{
		_visible = !isHiddenByDefault;		
		mStartingPosX = _x;
		
		mcLabel.tfTabName.text = GetTabName( tabID );
		
		var timerformat:TextFormat = new TextFormat();
		timerformat.letterSpacing = 4;	
		mcLabel.tfTabName.setTextFormat(timerformat);
		//ConsoleWindow.Trace("mcLabel.tfTabName = " + mcLabel.tfTabName);
		//ConsoleWindow.Trace("letter spacing = " + mcLabel.tfTabName.getTextFormat().letterSpacing);
		
		
		ScreenTabManager.Instance.AddScreenTab( this );		
	}	
	
	public function GetStartingPos():Number
	{
		return mStartingPosX;
	}
	
	public function GetTabName( _tabID:Number ):String
	{
		switch( _tabID )
		{
			case ID_LOGIN:
				return "LOGIN";
				
			case ID_PROFILE:
				return "PROFILE";
				
			case ID_FRIENDS:
				return "FRIENDS";
				
			case ID_EMAIL:
				return "EMAIL";
				
			case ID_CLANS:
				return "CLAN";
				
			case ID_CONNECTION:
				return "CONNECTION";			
				
			case ID_CD_KEY:
				return "CD KEY";			
				
			case ID_REGISTRATION:
				return "REGISTRATION";		
		}
	}
}