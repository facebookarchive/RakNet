import flash.external.*;

returnToTitleButton.addEventListener("click", this, "ReturnToTitle");
function ReturnToTitle()
{
	// Disconnect from the server
	ExternalInterface.call("f2c_Logoff");
}
