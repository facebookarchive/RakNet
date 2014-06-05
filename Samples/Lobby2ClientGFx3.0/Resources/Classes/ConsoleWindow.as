import gfx.controls.Button;

class ConsoleWindow extends MovieClip
{	
	static private var mSingleInstance:ConsoleWindow;
	private var mcButton:Button;
	private var tfWindow:TextField;
	
	public function ConsoleWindow() 
	{
		mSingleInstance = this;
		
		onLoad = OnFinishedLoading;		
	}
	
	public function OnFinishedLoading():Void
	{
		mcButton.addEventListener( "click", this, "ToggleWindow" );	
		tfWindow._visible = false;
	}
	
	public function ToggleWindow():Void
	{
		tfWindow._visible = !tfWindow._visible;
	}
	
	public static function Trace( text:String ):Void
	{
		mSingleInstance.tfWindow.text += "\n" + text;		
		trace(text);
	}
	
	public static function get Instance():ConsoleWindow
	{
		return mSingleInstance;
	}
	
}