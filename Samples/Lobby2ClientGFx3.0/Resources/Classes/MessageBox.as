import gfx.controls.Button;
import mx.utils.Delegate;
import flash.external.*;

class MessageBox extends MovieClip
{
	private var tfMessage:TextField;
	private var btnOK:Button;
	private var mCallback:Function;
	
	public function MessageBox()
	{
		_visible = false;
	}
	
	public function onLoad():Void
	{
		btnOK.addEventListener("click", this, "OnClickOK");		
	}
	
	public function SetMessage( message:String, callback:Function ):Void
	{
		tfMessage.text = message;
		mCallback = callback;
		_visible = true;
	}
	
	public function OnClickOK():Void
	{
		if ( mCallback )
		{
			mCallback.call();
		}
		
		this.removeMovieClip();
	}
}