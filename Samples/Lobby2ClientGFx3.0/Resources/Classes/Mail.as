import flash.external.*;
import gfx.controls.TextInput;
import gfx.controls.Button;
import mx.utils.Delegate;
import gfx.controls.TextArea;
import gfx.controls.CheckBox;

class Mail extends MovieClip
{
	public static var cMode1:Number = 1;
	public static var cMode2:Number = 2;
	//only in mail1 mode
	private var tfTo:TextField;	   //this is the one that says "TO:"  we need access because we change it to "FROM:" for receiving mails 
	private var tiTo:TextInput;    //this is the actual field we display who the mail is address to 
	
	//only in mail2 mode
	private var mcProfileImage:MovieClip;
	private var tfUsername:TextField;
	
	//shared
	private var tfTitle:TextField;
	private var tiSubject:TextInput;
	private var taMessage:TextArea;
	private var btnButton1:Button;
	private var btnButton2:Button;
	private var btnButton3:Button;
	
	private var callbackBtn1:Function;
	private var callbackBtn2:Function;
	private var callbackBtn3:Function;	
	
	private var mCheckBox:CheckBox;
	
	public function Mail()
	{		
		mCheckBox._visible = false;
	}
	
	public function SetMode( mode:Number ):Void
	{
		switch( mode )
		{
			case cMode1:
				gotoAndStop("Mail1");
			break;
			
			case cMode2:
				gotoAndStop("Mail2");			
			break;
			
			default:
			break;
		}
	}
	
	public function onLoad():Void
	{	
		btnButton1.addEventListener("click", this, "OnClickedButton1");
		btnButton2.addEventListener("click", this, "OnClickedButton2");
		btnButton3.addEventListener("click", this, "OnClickedButton3");
		btnButton1.visible = false;
		btnButton2.visible = false;
		btnButton3.visible = false;
	}
	
	public function HideButton( index:Number ):Void
	{
		this["btnButton" + index]._visible = false;
	}
	
	public function Clear():Void
	{
		tfTitle.text = "";
		tiTo.text = "";
		tiTo.disabled = false;
		tiSubject.disabled = false;
		tiSubject.text = "";
		taMessage.text = "";
		taMessage.disabled = false;
		mCheckBox._visible = false;
		btnButton1.visible = false;
		btnButton2.visible = false;
		btnButton3.visible = false;
	}
	
	public function SetTitle( _title:String ):Void
	{
		tfTitle.text = _title;
	}
	
	public function SetButton( _index:Number, _label:String, _callback:Function ):Void
	{
		this["btnButton" + _index].label = _label;
		this["callbackBtn" + _index] = _callback;
		this["btnButton" + _index].visible = true;
	}
	
	public function SetToDisplayText( text:String ):Void
	{
		tfTo.text = text;
	}
	
	public function SetToField( _username:String ):Void
	{
		tiTo.text = _username;
	}
	
	public function DisableToField():Void
	{
		tiTo.disabled = true;
	}
	
	public function SetUsername( _username:String ):Void
	{
		tfUsername.text = _username;
	}
		
	public function GetToField():String
	{
		return tiTo.text;
	}
	
	public function GetSubjectField():String
	{
		return tiSubject.text;
	}
	
	public function SetSubjectField( subject:String ):Void
	{
		tiSubject.text = subject;
	}
	
	public function DisableSubjectField():Void
	{
		tiSubject.disabled = true;
	}
	
	public function GetMsgField():String
	{
		return taMessage.text;
	}
	
	public function SetMsgField( msg:String ):Void
	{
		taMessage.text = msg;
	}
	
	public function DisableMsgField():Void
	{
		taMessage.disabled = true;
	}
	
	public function OnClickedButton1():Void
	{
		callbackBtn1.call();
	}
	
	public function OnClickedButton2():Void
	{
		callbackBtn2.call();
	}
	
	public function OnClickedButton3():Void
	{
		callbackBtn3.call();
	}
	
	public function SetCheckBox( text:String ):Void
	{
		mCheckBox.label = text;
		mCheckBox._visible = true;
	}
	
	public function IsCheckBoxOn():Boolean
	{
		return mCheckBox.selected;
	}
}