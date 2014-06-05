import flash.external.*;

connectToServerButton.addEventListener("click", this, "connectToServer");
ExternalInterface.addCallback("c2f_connectToServer_callback", this, c2f_connectToServer_callback);
function connectToServer()
{
	ExternalInterface.call("f2c_ConnectToServer", ipAddressTextInput.text, portTextInput.text, "c2f_connectToServer_callback");
}

function c2f_connectToServer_callback(callSucceeded : Boolean)
{
	if (callSucceeded==false)
		gotoAndStop("Main");	
}

ExternalInterface.addCallback("c2f_NotifyConnectionAttemptToServerSuccess", this, c2f_NotifyConnectionAttemptToServerSuccess);
function c2f_NotifyConnectionAttemptToServerSuccess():Void
{
	gotoAndStop("Accounts_RakNet");
}

ExternalInterface.addCallback("c2f_NotifyConnectionAttemptToServerFailure", this, c2f_NotifyConnectionAttemptToServerFailure);
function c2f_NotifyConnectionAttemptToServerFailure(resultCode:String, systemAddress:String):Void
{
	// Result codes are:
	// CONNECTION_ATTEMPT_FAILED
	// ALREADY_CONNECTED
	// NO_FREE_INCOMING_CONNECTIONS
	// RSA_PUBLIC_KEY_MISMATCH
	// CONNECTION_BANNED
    // INVALID_PASSWORD
	trace(resultCode);
	gotoAndStop("Main");
}

stop();