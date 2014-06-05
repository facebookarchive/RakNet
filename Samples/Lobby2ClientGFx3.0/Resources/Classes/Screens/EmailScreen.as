import flash.external.*;
import gfx.controls.TextInput;
import gfx.controls.Button;
import gfx.controls.CheckBox;
import mx.utils.Delegate;

class Screens.EmailScreen extends ScreenWithPageNavigator
{	
	
	private var emailSendRecipient0EditBox:TextInput;	
	private var emailSendRecipient1EditBox:TextInput;	
	private var emailSendRecipient2EditBox:TextInput;	
	private var emailSendRecipient3EditBox:TextInput;	
	private var emailSendRecipient4EditBox:TextInput;	
	private var emailSendRecipient5EditBox:TextInput;	
	private var emailSendRecipient6EditBox:TextInput;	
	private var emailSendRecipient7EditBox:TextInput;	
	private var emailSubjectEditBox:TextInput;	
	private var emailBodyEditBox:TextInput;	
	private var emailStatusEditBox:TextInput;	
	private var deleteEmailIDEditBox:TextInput;	
	private var updateEmailIDEditBox:TextInput;	
	private var newStatusFlagEditbox:TextInput;	
	
	private var sendEmailButton:Button;
	private var getEmailsButton:Button;
	private var deleteEmailButton:Button;
	private var setEmailStatusButton:Button;
	
	private var updateStatusFlagCheckbox:CheckBox;
	private var updateMarkedReadCheckbox:CheckBox;
	private var isNowMarkedReadCheckbox:CheckBox;
	
	private var btnCompose:Button;
	private var btnDelete:Button;
	private var btnMarkUnread:Button;	
	private var mcMail:Mail;
		
	public function EmailScreen() 
	{
		ConsoleWindow.Trace("Constructing EmailScreen");					
		
		mScreenId = ScreenID.EMAIL;
		mScreenTabId = ScreenTab.ID_EMAIL;
		
		mEntriesPerPage = 10;
		mFirstEntryX = 482;
		mFirstEntryY = -265;
		mDeltaY = 10;
	}
	
	public function VOnFinishedLoading():Void
	{		
		//Add click event for buttons
		//sendEmailButton.addEventListener("click", this, "f2c_SendEmail");
		//deleteEmailButton.addEventListener("click", this, "f2c_DeleteEmail");
		//setEmailStatusButton.addEventListener("click", this, "f2c_UpdateEmail");
		//getEmailsButton.addEventListener("click", this, "f2c_GetEmails");
		btnCompose.addEventListener("click", this, "ComposeMail");
		//btnDelete.addEventListener("click", this, "f2c_DeleteEmail");
		//btnMarkUnread.addEventListener("click", this, "f2c_GetEmails");
		btnDelete.addEventListener("click", this, "DeleteSelectedEmails");
						
		//Add callbacks for C++
		ExternalInterface.addCallback("c2f_SendEmail", this, c2f_SendEmail);
		ExternalInterface.addCallback("c2f_DeleteEmail", this, c2f_DeleteEmail);
		ExternalInterface.addCallback("c2f_UpdateEmail", this, c2f_UpdateEmail);
		ExternalInterface.addCallback("c2f_GetEmails", this, c2f_GetEmails);
		
		mcMail._visible = false;
		
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
		f2c_GetEmails();
		if ( LobbyInterface.Instance.IsInFlashMode() )
		{
			c2f_GetEmails();
		}
	}
		
	function f2c_SendEmail():Void
	{
		if ( mcMail.GetToField().length > 0 )
		{
			ExternalInterface.call("f2c_SendEmail", 
			mcMail.GetToField(),//emailSendRecipient0EditBox.text, emailSendRecipient1EditBox.text, emailSendRecipient2EditBox.text,
			"", "", "", "", "", "", "",//emailSendRecipient3EditBox.text, emailSendRecipient4EditBox.text, emailSendRecipient5EditBox.text,
			//emailSendRecipient6EditBox.text, emailSendRecipient7EditBox.text,
			mcMail.GetSubjectField(), mcMail.GetMsgField(),//emailSubjectEditBox.text, emailBodyEditBox.text,
			""//emailStatusEditBox.text
			);
			
			HideMail();
		}
	}

	function c2f_SendEmail(resultIdentifier:String):Void
	{
		switch (resultIdentifier)
		{
			case "SUCCESS":
				ConsoleWindow.Trace("c2f_SendEmail: " + resultIdentifier);
			break;
		}
		
		if ( resultIdentifier != "SUCCESS" )
		{
			LobbyInterface.Instance.CreateMessageBox( resultIdentifier );
		}
	}

	function f2c_DeleteEmail( emailID:Number ):Void
	{
		ConsoleWindow.Trace("f2c_DeleteEmail(), emailID = " + emailID);
		if ( emailID != null )
		{
			ExternalInterface.call("f2c_DeleteEmail", String(emailID));
		}
	}

	function c2f_DeleteEmail(resultIdentifier:String):Void
	{
		switch (resultIdentifier)
		{
			case "DATABASE_CONSTRAINT_FAILURE":
			break;
			case "Emails_Delete_UNKNOWN_EMAIL_ID":
			break;
			case "Emails_Delete_ALREADY_DELETED":
			break;
			case "SUCCESS":
				ConsoleWindow.Trace("c2f_DeleteEmail()... resultIdentifier = " + resultIdentifier);
			break;
		}
		
		if ( resultIdentifier != "SUCCESS" )
		{
			LobbyInterface.Instance.CreateMessageBox( resultIdentifier );
		}
	}

	function f2c_UpdateEmail():Void
	{
		ExternalInterface.call("f2c_UpdateEmail", updateEmailIDEditBox.text,
		  updateStatusFlagCheckbox.selected, updateMarkedReadCheckbox.selected, 
		  newStatusFlagEditbox.text, isNowMarkedReadCheckbox.selected
		  );
	}

	function c2f_UpdateEmail(resultIdentifier:String):Void
	{
		switch (resultIdentifier)
		{
			case "DATABASE_CONSTRAINT_FAILURE":
			break;
			case "Emails_SetStatus_UNKNOWN_EMAIL_ID":
			break;
			case "Emails_SetStatus_WAS_DELETED":
			break;
			case "SUCCESS":
			break;
		}
		
		if ( resultIdentifier != "SUCCESS" )
		{
			LobbyInterface.Instance.CreateMessageBox( resultIdentifier );
		}
	}

	function f2c_GetEmails():Void
	{
		ExternalInterface.call("f2c_GetEmails");
	}
	
	public function DeleteEmail( emailEntry:MovieClip ):Void
	{
		
	}
	
	public function DeleteSelectedEmails():Void
	{
		var startIndex:Number = mCurrentPage * mEntriesPerPage - ( mEntriesPerPage );
		var endIndex:Number = startIndex + mEntriesPerPage;
		var listForRemoval:Array = new Array();;
		
		for ( var i:Number = 0; i < mEntriesPerPage; i++ )
		{				
			var mcEntry:MovieClip = mMovieClipList[startIndex+i];
			if ( !mcEntry )
			{
				break;
			}
			
			if ( mcEntry.cbSelected.selected )
			{
				//RemoveEntryFromList(mcEntry, null, true);
				listForRemoval.push(mcEntry);
			}
		}
		
		var length:Number = listForRemoval.length;
		for ( var n:Number = 0; n < length; n++ )
		{
			ConsoleWindow.Trace("Removing: " + mcEntry);
			f2c_DeleteEmail( listForRemoval[n].emailID );
			RemoveEntryFromList( listForRemoval[n], null, true );
		}
	}

	function c2f_GetEmails():Void
	{
		ConsoleWindow.Trace("c2f_GetEmails: " );
		var numEmails:Number = arguments.length/9;
		for (var i:Number=0; i < numEmails; i++)
		{
			var sender:String = arguments[i*9+0];
			var recipient:String = arguments[i*9+1];
			var subject:String = arguments[i*9+2];
			var body:String = arguments[i*9+3];
			var status:Number = arguments[i*9+4];
			var wasSendByMe:Boolean = arguments[i*9+5];
			var wasReadByMe:Boolean = arguments[i*9+6];
			var emailID:Number = arguments[i*9+7];
			var creationDate:String = arguments[i * 9 + 8];
			
			ConsoleWindow.Trace("sender: " + sender + ", recipient: " + recipient + ", subject: " + subject + ", body: " + body + ", status: " + status + ", wasSendByMe: " + wasSendByMe 
								+ ", wasReadByMe: " + wasReadByMe + ", emailID: " + emailID + ", creationDate: " + creationDate);
								
			AddListEntry( sender, recipient, subject, body, creationDate, "", emailID );
		}
						
		//TODO: remove this once c++ sends down proper data
		for ( var n:Number = 0; n < 15; n++ )
		{
			AddListEntry( "from user" + mMovieClipList.length, "??", "Hi!", "lkajs;ldf", "mm/day/year" );
		}		
		
		OnMoveClipListUpdated();		
		ShowPage(1);
	}
	
	private function AddListEntry( usernameFrom:String, recipient:String, subject:String, body:String, date:String, time:String, emailID:Number ):MovieClip
	{
		var userEntry:MovieClip = attachMovie("EmailListEntry", "emailEntry" + mMovieClipList.length, getNextHighestDepth() );
		userEntry._visible = false;
		userEntry.tfUsername.text = usernameFrom;
		userEntry.tfSubject.text = subject;
		userEntry.tfDate.text = date;
		userEntry.emailID = emailID;
		userEntry.bodyText = body;
		mMovieClipList.push( userEntry );
		
		var self:EmailScreen = this;
		userEntry.mcMail.onPress = function() { self.ReadMail( this._parent.tfUsername.text, this._parent.tfSubject.text, this._parent.bodyText, this._parent ); }
		return userEntry;
	}	
	
	private function OnShowMovieClipEntry( mcEntry:MovieClip, index:Number ):Void 
	{
		super.OnShowMovieClipEntry( mcEntry, index );
		mcEntry.cbSelected.selected = false;
	}
	
	public function ComposeMail():Void
	{
		mcMail.Clear();
		mcMail.SetMode( Mail.cMode1 );
		mcMail.SetButton( 1, "CANCEL", Delegate.create(this, HideMail) );
		mcMail.SetButton( 2, "SEND", Delegate.create(this, f2c_SendEmail) );
		mcMail.HideButton( 3 );
		
		mcMail.SetTitle( "COMPOSE MESSAGE" );
		mcMail.SetToDisplayText( "TO:" );
		mcMail.SetToField( );
		mcMail.swapDepths( getNextHighestDepth() );
		mcMail._visible = true;
	}
	
	public function ReadMail( username:String, subject:String, body:String, mailEntry:MovieClip ):Void
	{
		mcMail.Clear();
		mcMail.SetMode( Mail.cMode2 );
		mcMail.SetButton( 1, "DELETE", Delegate.create(this, DeleteMail) );
		mcMail.SetButton( 2, "CLOSE", Delegate.create(this, HideMail) );
		mcMail.SetButton( 3, "REPLY", Delegate.create(this, ComposeMail) );
		mcMail["mailEntry"] = mailEntry;
		
		mcMail.SetTitle("RECEIVED MESSAGE:" );
		mcMail.SetUsername( username );
		mcMail.SetSubjectField( subject );
		mcMail.SetMsgField( body );
		mcMail.swapDepths( getNextHighestDepth() );
		mcMail._visible = true;
	}
	
	public function HideMail():Void
	{
		mcMail._visible = false;
	}
	
	public function DeleteMail( ):Void
	{
		mcMail._visible = false;	
		var mailEntry:MovieClip = mcMail["mailEntry"];
		if ( mailEntry )
		{
			f2c_DeleteEmail( mailEntry.emailID );
			RemoveEntryFromList( mailEntry );	
		}
		else
		{
			ConsoleWindow.Trace("DeleteMail, entry = " + mailEntry);
		}
	}
}