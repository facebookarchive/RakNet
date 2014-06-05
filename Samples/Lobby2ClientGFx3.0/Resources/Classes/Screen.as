/*
 * Screen - base screen class.  Many screen classes will derive from this
 * Note: V before the function names stand for virtual, these functions are intended to be overwritten by children if needed
 * 
 */
import mx.transitions.easing.None;
import mx.transitions.Tween;

class Screen extends MovieClip
{	
	private var mScreenId:Number;
	private var mScreenTabId:Number;
	private var mIsLoaded:Boolean;
	
	public function Screen()
	{
		_alpha = 0;
		_x = -1000;
		ConsoleWindow.Trace("Constructing Screen");
		mScreenId = -1;
		mIsLoaded = false;
		
		onLoad = VOnFinishedLoading;
	}
	
	public function VInitialize():Boolean
	{
		return true;
	}
	
	//Child class should still call this even if it needs to define its own VOnFinishedLoading func
	public function VOnFinishedLoading():Void
	{
		mIsLoaded = true;
		
		LobbyInterface.Instance.RegisterScreen( mScreenId, this );
	}
		
	public function IsLoaded():Boolean
	{
		return mIsLoaded;
	}
	
	public function Show( bDoTween:Boolean, tweenFrom:Number, tweenTo:Number ):Void
	{
		ConsoleWindow.Trace("Showing " + this);
		if ( bDoTween )
		{
			new Tween( this, "_x", None.easeNone, tweenFrom, 0, .5, true );
			new Tween( this, "_alpha", None.easeNone, 0, 100, .5, true );
		}
		else
		{
			_x = 0;
			_alpha = 100;			
		}
		
		OnShow();
	}
	
	public function OnShow():Void
	{		
	}
	
	public function Hide( bDoTween:Boolean, tweenTo:Number ):Void
	{
		ConsoleWindow.Trace("Hiding " + this);
		if ( bDoTween )
		{
			new Tween( this, "_x", None.easeNone, this._x, tweenTo, .5, true );
			new Tween( this, "_alpha", None.easeNone, 100, 0, .5, true );
		}
		else
		{
			this._x = -1000;// tweenTo;
			this._alpha = 0;
		}
		OnHide();
	}
	
	public function OnHide():Void
	{
	}
	
	public function GetScreenTabId():Number
	{
		return mScreenTabId;
	}
	
	public function GetScreenId():Number
	{
		return mScreenId;
	}
	
	public function CleanUpMoveClipList( list:Array ):Void
	{
		var length:Number = list.length;
		for ( var i:Number = 0; i < length; i++ )
		{
			var mc:MovieClip = list[i];
			//ConsoleWindow.Trace("Removing mc: " + mc);
			mc.removeMovieClip();
		}
	}
	
	public static function GetIndexFromElement( element, array ):Number
	{
		var length:Number = array.length;
		for ( var i:Number = 0; i < length; i++ )
		{
			if ( element == array[i] )
			{
				return i;
			}
		}		
		
		ConsoleWindow.Trace("Can't find element: " + element);
		return 0;
	}
}