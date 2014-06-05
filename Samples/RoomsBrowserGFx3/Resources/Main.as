import flash.external.*;

startButton.addEventListener("click", this, "Start");
ExternalInterface.addCallback("c2f_QueryPlatform_Main_Callback", this, c2f_QueryPlatform_Main_Callback);
function Start()
{
	ExternalInterface.call("f2c_QueryPlatform","c2f_QueryPlatform_Main_Callback");
}


function c2f_QueryPlatform_Main_Callback(platform:String):Void
{
	if (platform=="RakNet")
	{
		gotoAndStop("connectToServer_RakNet");
		
	}
}

stop();
