import flash.external.*;

ExternalInterface.addCallback("c2f_QueryPlatform_CreateRoom_Callback", this, c2f_QueryPlatform_CreateRoom_Callback);
ExternalInterface.call("f2c_QueryPlatform","c2f_QueryPlatform_CreateRoom_Callback");

function c2f_QueryPlatform_CreateRoom_Callback(platform:String):Void
{
	if (platform=="RakNet")
	{
	}
	else
	{
		lanGameCheckbox.visible=false;
		roomMembersCanInviteCheckbox.visible=false;
	}
}

cancelButton.addEventListener("click", this, "Cancel");
function Cancel()
{
	gotoAndStop("Lobby");
}

okButton.addEventListener("click", this, "CreateRoom");
function CreateRoom()
{
	
	ExternalInterface.call("f2c_CreateRoom", 
		[roomNameTextInput.text,
		mapNameTextInput.text,
		Number(publicSlotsTextInput.text),
		Number(reservedSlotsTextInput.text),
		hiddenFromSearchesCheckbox.selected,
		roomMembersCanInviteCheckbox.selected,
		lanGameCheckbox.selected
		]);
}

// Duplicated
ExternalInterface.addCallback("c2f_CreateRoom", this, c2f_CreateRoom);
function c2f_CreateRoom(resultCode:String, isLanGame:Boolean):Void
{
	if (resultCode=="REC_SUCCESS")
	{
		if (isLanGame)
		{
			gotoAndStop("InGame");
		}
		else
		{
			gotoAndStop("InRoom");
		}
	}
	else
	{
		trace("c2f_CreateRoom failure. Result= " + resultCode);
		
		/*
		REC_CREATE_ROOM_UNKNOWN_TITLE,
	REC_CREATE_ROOM_CURRENTLY_IN_QUICK_JOIN,
	REC_CREATE_ROOM_CURRENTLY_IN_A_ROOM,
	*/
	}
}
