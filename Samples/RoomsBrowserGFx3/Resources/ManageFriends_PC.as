import flash.external.*;


sendInviteButton.addEventListener("click", this, "Friends_SendInvite");
function Friends_SendInvite()
{
	ExternalInterface.call("f2c_Friends_SendInvite", usernameTextInput.text);
}

ExternalInterface.addCallback("c2f_Friends_SendInvite", this, c2f_Friends_SendInvite);
function c2f_Friends_SendInvite(resultCode:String, userHandle:String):Void
{
	if (resultCode=="L2RC_SUCCESS")
	{
		trace("c2f_Friends_SendInvite success, " + userHandle);
	}
	else
	{
		// L2RC_Friends_SendInvite_UNKNOWN_TARGET_HANDLE
		// L2RC_Friends_SendInvite_CANNOT_PERFORM_ON_SELF
		// L2RC_DATABASE_CONSTRAINT_FAILURE
		// L2RC_Friends_SendInvite_ALREADY_SENT_INVITE
		// L2RC_Friends_SendInvite_ALREADY_FRIENDS
		
		
		trace(resultCode);
	}
}

removeButton.addEventListener("click", this, "Friends_Remove");
function Friends_Remove()
{
	ExternalInterface.call("f2c_Friends_Remove", usernameTextInput.text);
}

ExternalInterface.addCallback("c2f_Friends_Remove", this, c2f_Friends_Remove);
function c2f_Friends_Remove(resultCode:String, targetHandle:String):Void
{
	if (resultCode=="L2RC_SUCCESS")
	{
		trace("c2f_Friends_Remove success, " + targetHandle);
	}
	else
	{
		// L2RC_Friends_Remove_UNKNOWN_TARGET_HANDLE
		// L2RC_Friends_Remove_CANNOT_PERFORM_ON_SELF
		// L2RC_DATABASE_CONSTRAINT_FAILURE
		// L2RC_Friends_Remove_NOT_FRIENDS

		trace(resultCode);
	}
}

acceptInviteButton.addEventListener("click", this, "Friends_AcceptInvite");
function Friends_AcceptInvite( )
{
	ExternalInterface.call("f2c_Friends_AcceptInvite", usernameTextInput.text);
}

ExternalInterface.addCallback("c2f_Friends_AcceptInvite", this, c2f_Friends_AcceptInvite);
function c2f_Friends_AcceptInvite(resultCode:String, userHandle:String, onlineStatusSetToVisible:Boolean, loggedInTitle:String, status:String):Void
{
	if (resultCode=="L2RC_SUCCESS")
	{
		trace("c2f_Friends_AcceptInvite success, " + userHandle + ", " + onlineStatusSetToVisible + ", " + loggedInTitle + ", " + status);
	}
	else
	{
		// L2RC_Friends_AcceptInvite_UNKNOWN_TARGET_HANDLE
		// L2RC_Friends_AcceptInvite_CANNOT_PERFORM_ON_SELF
		// L2RC_DATABASE_CONSTRAINT_FAILURE
		// L2RC_Friends_AcceptInvite_NO_INVITE

		trace(resultCode);
	}
}

rejectInviteButton.addEventListener("click", this, "Friends_RejectInvite");
function Friends_RejectInvite()
{
	ExternalInterface.call("f2c_Friends_RejectInvite", usernameTextInput.text);
}

ExternalInterface.addCallback("c2f_Friends_RejectInvite", this, c2f_Friends_RejectInvite);
function c2f_Friends_RejectInvite(resultCode:String, targetHandle:String):Void
{
	if (resultCode=="L2RC_SUCCESS")
	{
		trace("c2f_Friends_RejectInvite success, " + targetHandle);
	}
	else
	{
		// L2RC_Friends_RejectInvite_UNKNOWN_TARGET_HANDLE
		// L2RC_Friends_RejectInvite_CANNOT_PERFORM_ON_SELF
		// L2RC_DATABASE_CONSTRAINT_FAILURE
		// L2RC_Friends_RejectInvite_NO_INVITE

		trace(resultCode);
	}
}

backButton.addEventListener("click", this, "BackToLobby");
function BackToLobby()
{
	gotoAndStop("Lobby");
}

stop();
