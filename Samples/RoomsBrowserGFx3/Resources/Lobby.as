import flash.external.*;

updateButton.addEventListener("click", this, "UpdateRoomsList");
function UpdateRoomsList()
{
	var isNetUpdate:Boolean = netRadioButton.selected;
	
	lobbyRoomsScrollingList.dataProvider=[];
	
	ExternalInterface.call("f2c_UpdateRoomsList", isNetUpdate);
	
	// If the platform requires friends in the scaleform UI, this will update it.
	ExternalInterface.call("f2c_QueryPlatform","c2f_QueryPlatform_Lobby_Callback");
}

netRadioButton.addEventListener("click", this, "UpdateRoomsList");
lanRadioButton.addEventListener("click", this, "UpdateRoomsList");
joinRoomButton.addEventListener("click", this, "JoinRoom");
function JoinRoom()
{
	// 0th element is the unique room id, used as a search flag for when joining a room
	// How do I tell if lobbyRoomsScrollingList has a selection active?
	ExternalInterface.call("f2c_JoinByFilter", 
		Boolean(lobbyRoomsScrollingList.dataProvider[lobbyRoomsScrollingList.selectedIndex][0]), // isFromNetwork
		Number(lobbyRoomsScrollingList.dataProvider[lobbyRoomsScrollingList.selectedIndex][1]), // guid
		String(lobbyRoomsScrollingList.dataProvider[lobbyRoomsScrollingList.selectedIndex][2]) // IP address
	);
}

ExternalInterface.addCallback("c2f_JoinByFilter", this, c2f_JoinByFilter);
function c2f_JoinByFilter( resultCode:String ):Void
{
	if (resultCode=="REC_SUCCESS")
		gotoAndStop("inRoom");
	else
	{
		trace("c2f_JoinByFilter failure. Result= " + resultCode);
		
		/*
		REC_JOIN_BY_FILTER_UNKNOWN_TITLE,
		REC_JOIN_BY_FILTER_NO_ROOMS,
		REC_JOIN_BY_FILTER_CURRENTLY_IN_A_ROOM,
		REC_JOIN_BY_FILTER_CURRENTLY_IN_QUICK_JOIN,
		REC_JOIN_BY_FILTER_CANNOT_JOIN_AS_MODERATOR,
		REC_JOIN_BY_FILTER_ROOM_LOCKED,
		REC_JOIN_BY_FILTER_BANNED,
		REC_JOIN_BY_FILTER_NO_SLOTS,
		*/
	}
}

createRoomButton.addEventListener("click", this, "CreateRoom");
function CreateRoom()
{
	gotoAndStop("CreateRoom");
}

manageFriendsButton.addEventListener("click", this, "ManageFriends");
function ManageFriends()
{
	gotoAndStop("ManageFriends_PC");
}

logoffButton.addEventListener("click", this, "Logoff");
function Logoff()
{
	ExternalInterface.call("f2c_Logoff");
}

chatTextInput.addEventListener("textChange", this, "UpdateSendButton");
chatRecipient.addEventListener("textChange", this, "UpdateSendButton");
function UpdateSendButton()
{
	if (chatTextInput.text.length>0 && chatRecipient.text.length>0)
		sendChatButton.enabled=true;
	else
		sendChatButton.enabled=false;
}

sendChatButton.addEventListener("click", this, "Chat_Func");
function Chat_Func()
{
	if (chatTextInput.text.length>0 && chatRecipient.text.length>0)
	{
		ExternalInterface.call("f2c_Directed_Chat_Func", chatRecipient.text, chatTextInput.text);
	}
}

ExternalInterface.addCallback("c2f_Chat_Callback", this, c2f_Chat_Callback);
function c2f_Chat_Callback(resultCode:String, chatRecipient:String, chatTextInput:String):Void
{
	if (resultCode=="REC_SUCCESS")
	{
		chatTextArea.text+=chatRecipient + " >> " + chatTextInput + "\n";
	}
	else
	{
		trace("c2f_Chat_Callback failure. Result= " + resultCode + " when sent to " + chatRecipient);

			/*
			REC_CHAT_USER_NOT_IN_ROOM,
	REC_CHAT_RECIPIENT_NOT_ONLINE,
	REC_CHAT_RECIPIENT_NOT_IN_ANY_ROOM,
	REC_CHAT_RECIPIENT_NOT_IN_YOUR_ROOM,
	*/

	}
}

ExternalInterface.addCallback("c2f_Chat_Notification", this, c2f_Chat_Notification);
function c2f_Chat_Notification(sender:String, chatRecipient:String, chatTextInput:String, profanityFilteredTextInput:String ):Void
{
	chatTextArea.text+=sender + " << " + chatTextInput + "\n";
}

ExternalInterface.addCallback("c2f_Client_Logoff", this, c2f_Client_Logoff);
function c2f_Client_Logoff(resultCode:String):Void
{
	gotoAndStop("Main");
}

function UpdateFriendsList()
{
	lobbyFriendsScrollingList.dataProvider=[];
	ExternalInterface.call("f2c_UpdateFriendsList");
}

function c2f_QueryPlatform_Lobby_Callback(platform:String):Void
{
	if (platform=="RakNet")
	{
		UpdateFriendsList();
	}
	else
	{
		// This stuff is handled thought internal menus on the XBOX 360 and PS3
		friendsStatusLabel.visible=false;
		lobbyFriendsScrollingList.visible=false;
		lobbyFriendsScrollBar.visible=false;
		manageFriendsButton.visible=false;
		chatTextArea.visible=false;
		sendChatButton.visible=false;
		chatLabel1.visible=false;
		chatLabel2.visible=false;
		chatRecipient.visible=false;
		chatTextInput.visible=false;
		chatScrollBar.visible=false;		

		// Only NET on the XBOX 360 and PS3
		netRadioButton.visible=false;
		lanRadioButton.visible=false;
	}
}
ExternalInterface.addCallback("c2f_AddSingleRoom", this, c2f_AddSingleRoom);
function c2f_AddSingleRoom():Void
{
	var roomIsFromServer = arguments[0];
	var roomId:Number = arguments[1];
	var ipAddrAndPort:String = arguments[2];
	var roomName:String = arguments[3];
	var numRoomMembers:Number = arguments[4];
	var maxRoomMembers:Number = arguments[5];
	var mapName:String = arguments[6];
		
	lobbyRoomsScrollingList.dataProvider.push([roomIsFromServer,roomId,ipAddrAndPort,roomName,numRoomMembers,maxRoomMembers,mapName]);
	lobbyRoomsScrollingList.dataProvider.invalidate();
	
}

ExternalInterface.addCallback("c2f_SearchByFilter_Callback", this, c2f_SearchByFilter_Callback);
function c2f_SearchByFilter_Callback():Void
{
	// push statements below do not work unless I clear the list first. I don't know why though.
	lobbyRoomsScrollingList.dataProvider=[];
	
	var resultCode:String = arguments[0];
	if (resultCode=="REC_SUCCESS")
	{
		var num:Number = arguments[1];
		var startingIdx:Number=2;
		for (var i:Number = 0; i < num; i++)
		{
			var roomIsFromServer = arguments[startingIdx+i*7+0];
			var roomId:Number = arguments[startingIdx+i*7+1];
			var ipAddrAndPort:String = arguments[startingIdx+i*7+2];
			var roomName:String = arguments[startingIdx+i*7+3];
			var numRoomMembers:Number = arguments[startingIdx+i*7+4];
			var maxRoomMembers:Number = arguments[startingIdx+i*7+5];
			var mapName:String = arguments[startingIdx+i*7+6];
			
			lobbyRoomsScrollingList.dataProvider.push([roomIsFromServer,roomId,ipAddrAndPort,roomName,numRoomMembers,maxRoomMembers,mapName]);			
			lobbyRoomsScrollingList.dataProvider.invalidate();
		}
	}
	else
	{
		// Unknown title (Programmer error).
		trace(resultCode);
		
		// REC_SEARCH_BY_FILTER_UNKNOWN_TITLE,
	}	
}

ExternalInterface.addCallback("c2f_Friends_GetInvites", this, c2f_Friends_GetInvites);
function c2f_Friends_GetInvites():Void
{
	var resultCode:String = arguments[0];
	if (resultCode=="L2RC_SUCCESS")
	{
		var num:Number = arguments[1];
		var startingIdx:Number=2;
		
		// Invites received
		for (var i:Number = 0; i < num; i++)
		{
			// params are same as c2f_Friends_GetFriends
			var handle:String = arguments[startingIdx+i*5+0];
			var isOnline:Boolean = arguments[startingIdx+i*5+1];
			var isVisible:Boolean = arguments[startingIdx+i*5+2];
			var loggedInTitle:String = arguments[startingIdx+i*5+3];
			var status:String = arguments[startingIdx+i*5+4];
			
			lobbyFriendsScrollingList.dataProvider.push(["Got invitation", handle,isOnline,isVisible,loggedInTitle,status]);			
		}
		
		// Invites sent
		startingIdx+=num*5;
		num=arguments[startingIdx++];
		for (var i:Number = 0; i < num; i++)
		{
			// params are same as c2f_Friends_GetFriends
			var handle:String = arguments[startingIdx+i*5+0];
			var isOnline:Boolean = arguments[startingIdx+i*5+1];
			var isVisible:Boolean = arguments[startingIdx+i*5+2];
			var loggedInTitle:String = arguments[startingIdx+i*5+3];
			var status:String = arguments[startingIdx+i*5+4];
			
			lobbyFriendsScrollingList.dataProvider.push(["Sent invitation", handle,isOnline,isVisible,loggedInTitle,status]);
		}
		lobbyFriendsScrollingList.dataProvider.invalidate();
	}
	else
	{
		// For RakNet, cannot fail
		trace(resultCode);
	}
}

ExternalInterface.addCallback("c2f_Friends_GetFriends", this, c2f_Friends_GetFriends);
function c2f_Friends_GetFriends():Void
{	
	var resultCode:String = arguments[0];
	if (resultCode=="L2RC_SUCCESS")
	{
		var num:Number = arguments[1];
		var startingIdx:Number=2;
		
		for (var i:Number = 0; i < num; i++)
		{
			// params are same as c2f_Friends_GetInvites
			var handle:String = arguments[startingIdx+i*5+0];
			var isOnline:Boolean = arguments[startingIdx+i*5+1];
			var isVisible:Boolean = arguments[startingIdx+i*5+2];
			var loggedInTitle:String = arguments[startingIdx+i*5+3];
			var status:String = arguments[startingIdx+i*5+4];
			
			lobbyFriendsScrollingList.dataProvider.push(["Is a friend", handle,isOnline,isVisible,loggedInTitle,status]);			
			lobbyFriendsScrollingList.dataProvider.invalidate();
		}
	}
	else
	{
		// For RakNet, cannot fail
		trace(resultCode);
	}
}

ExternalInterface.addCallback("c2f_Notification_Friends_PresenceUpdate", this, c2f_Notification_Friends_PresenceUpdate);
function c2f_Notification_Friends_PresenceUpdate(otherHandle:String, onlineStatusSetToVisible:Boolean, loggedInTitle:String, status:String):Void
{
	trace("c2f_Notification_Friends_PresenceUpdate, " + otherHandle + ", " + onlineStatusSetToVisible + ", " + loggedInTitle + ", " + status);
	
	// Update the friends list if the platform requires it
	ExternalInterface.call("f2c_QueryPlatform","c2f_QueryPlatform_Lobby_Callback");
}

// LAN connection attempt failed
ExternalInterface.addCallback("c2f_NotifyFailedConnectionAttempt", this, c2f_NotifyFailedConnectionAttempt);
function c2f_NotifyFailedConnectionAttempt(resultCode:String, systemAddress:String):Void
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

ExternalInterface.addCallback("c2f_NotifyNewConnection", this, c2f_NotifyNewConnection);
function c2f_NotifyNewConnection(systemAddress:String, rakNetGuid:String, isIncoming:Boolean):Void
{
	trace("c2f_NotifyNewConnection, " + systemAddress + ", " + rakNetGuid + ", " + isIncoming);
	// Connecting at this stage must be through the LAN
	gotoAndStop("InGame");
}


ExternalInterface.addCallback("c2f_Notification_Friends_StatusChange", this, c2f_Notification_Friends_StatusChange);
function c2f_Notification_Friends_StatusChange(operation:String, otherHandle:String, onlineStatusSetToVisible:Boolean, loggedInTitle:String, status:String):Void
{
	// operation can be:
	// FRIEND_LOGGED_IN,
	// FRIEND_LOGGED_OFF,
	// FRIEND_ACCOUNT_WAS_DELETED,
	// YOU_WERE_REMOVED_AS_A_FRIEND,
	// GOT_INVITATION_TO_BE_FRIENDS,
	// THEY_ACCEPTED_OUR_INVITATION_TO_BE_FRIENDS,
	// THEY_REJECTED_OUR_INVITATION_TO_BE_FRIENDS,
		
	trace("c2f_Notification_Friends_StatusChange, " + operation + ", " + otherHandle + ", " + onlineStatusSetToVisible + ", " + loggedInTitle + ", " + status);
	
	// If the platform requires friends in the scaleform UI, this will update it.
	ExternalInterface.call("f2c_QueryPlatform","c2f_QueryPlatform_Lobby_Callback");
}

ExternalInterface.addCallback("c2f_RoomInvitationSent_Callback", this, c2f_RoomInvitationSent_Callback)
function c2f_RoomInvitationSent_Callback( invitorName:String, inviteToSpectatorSlot:Boolean ):Void
{
	trace("You got a room invitation from " + invitorName);
}
ExternalInterface.addCallback("c2f_RoomInvitationWithdrawn_Callback", this, c2f_RoomInvitationWithdrawn_Callback)
function c2f_RoomInvitationWithdrawn_Callback( invitorName:String, invitorAddress:String ):Void
{
	trace("Your room invitation was withdrawn from " + invitorName);
}


sendChatButton.enabled=false;
netRadioButton.selected=true;
UpdateRoomsList();


stop();
