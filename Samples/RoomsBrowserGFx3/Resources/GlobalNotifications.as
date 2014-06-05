import flash.external.*;

ExternalInterface.addCallback("c2f_NotifyServerConnectionLost", this, c2f_NotifyServerConnectionLost);
function c2f_NotifyServerConnectionLost(reason:String):Void
{
	trace("Server connection lost. Reason=" + reason);
	gotoAndStop("Main");
}
