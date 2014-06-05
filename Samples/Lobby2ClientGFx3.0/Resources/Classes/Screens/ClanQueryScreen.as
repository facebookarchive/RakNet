import flash.external.*;
import gfx.controls.TextInput;
import gfx.controls.Button;

class Screens.ClanQueryScreen extends Screen
{	
	private var clans_GetMemberProperties_clanHandle_EditBox:TextInput;	
	private var clans_GetMemberProperties_targetHandle_EditBox:TextInput;	
	private var clanHandle_EditBox:TextInput;	
	private var clans_GetMemberProperties_Button:Button;
	private var clans_GetBlacklist_Button:Button;
	private var clans_GetMembers_Button:Button;
	private var clans_GetProperties_Button:Button;
	private var clans_GetList_Button:Button;
	private var clans_DownloadRequestList_Button:Button;
	private var clans_DownloadInvitationList_Button:Button;
	private var clans_Get_Button:Button;
		
	public function ClanQueryScreen() 
	{
		ConsoleWindow.Trace("Constructing ClanQueryScreen");						
		
		mScreenId = ScreenID.CLAN_QUERY;	
		mScreenTabId = ScreenTab.ID_CLANS;;
	}
	
	public function VOnFinishedLoading():Void
	{		
		//Add click event for buttons
	//	clans_GetProperties_Button.addEventListener("click", this, "f2c_Clans_GetProperties");
		clans_GetMemberProperties_Button.addEventListener("click", this, "f2c_Clans_GetMemberProperties");
	//	clans_Get_Button.addEventListener("click", this, "f2c_Clans_Get");
		//clans_DownloadInvitationList_Button.addEventListener("click", this, "f2c_Clans_DownloadInvitationList");
		//clans_DownloadRequestList_Button.addEventListener("click", this, "f2c_Clans_DownloadRequestList");
	//	clans_GetBlacklist_Button.addEventListener("click", this, "f2c_Clans_GetBlacklist");
	//	clans_GetMembers_Button.addEventListener("click", this, "f2c_Clans_GetMembers");
		clans_GetList_Button.addEventListener("click", this, "f2c_Clans_GetList");
				
		//Add callbacks for C++
	//	ExternalInterface.addCallback("c2f_Clans_GetProperties", this, c2f_Clans_GetProperties);
		ExternalInterface.addCallback("c2f_Clans_GetMemberProperties", this, c2f_Clans_GetMemberProperties);
	//	ExternalInterface.addCallback("c2f_Clans_Get", this, c2f_Clans_Get);
		//ExternalInterface.addCallback("c2f_Clans_DownloadInvitationList", this, c2f_Clans_DownloadInvitationList);
		//ExternalInterface.addCallback("c2f_Clans_DownloadRequestList", this, c2f_Clans_DownloadRequestList);
	//	ExternalInterface.addCallback("c2f_Clans_GetBlacklist", this, c2f_Clans_GetBlacklist);
	//	ExternalInterface.addCallback("c2f_Clans_GetMembers", this, c2f_Clans_GetMembers);
	//	ExternalInterface.addCallback("c2f_Clans_GetList", this, c2f_Clans_GetList);
		
		super.VOnFinishedLoading();
	}
	
	public function f2c_Clans_GetProperties():Void
	{
//		ExternalInterface.call("f2c_Clans_GetProperties", clanHandle_EditBox.text);
	}

	public function f2c_Clans_GetMemberProperties():Void
	{
		ExternalInterface.call("f2c_Clans_GetMemberProperties", clans_GetMemberProperties_clanHandle_EditBox.text,
															clans_GetMemberProperties_targetHandle_EditBox.text);
	}

	public function f2c_Clans_Get():Void
	{
//		ExternalInterface.call("f2c_Clans_Get");
	}

	public function f2c_Clans_DownloadInvitationList():Void
	{
		ExternalInterface.call("f2c_Clans_DownloadInvitationList");
	}
	public function f2c_Clans_DownloadRequestList():Void
	{
		ExternalInterface.call("f2c_Clans_DownloadRequestList");
	}

/*	public function f2c_Clans_GetBlacklist():Void
	{
		ExternalInterface.call("f2c_Clans_GetBlacklist", clanHandle_EditBox.text);
	}
*/
	public function f2c_Clans_GetMembers():Void
	{
	//	ExternalInterface.call("f2c_Clans_GetMembers", clanHandle_EditBox.text);
	}

	public function f2c_Clans_GetList():Void
	{
	//	ExternalInterface.call("f2c_Clans_GetList");
	}

/*	public function c2f_Clans_GetProperties(resultIdentifier:String, clanDescription:String):Void
	{
		switch (resultIdentifier)
		{
			case "Clans_GetProperties_UNKNOWN_CLAN":
			break;
			case "DATABASE_CONSTRAINT_FAILURE":
			break;
			case "SUCCESS":
			// clanDescription should be a valid string
			break;
		}
	}*/

	public function c2f_Clans_GetMemberProperties(resultIdentifier:String, description:String, rank:Number, isSubleader:Boolean,
										   clanMemberState:String, banReason:String):Void
	{
		switch (resultIdentifier)
		{
			case "Clans_GetMemberProperties_TARGET_NOT_IN_CLAN":
			break;
			case "DATABASE_CONSTRAINT_FAILURE":
			break;
			case "SUCCESS":
			break;
		}
		
		if ( resultIdentifier != "SUCCESS" )
		{
			LobbyInterface.Instance.CreateMessageBox( resultIdentifier );
		}
		
		switch (clanMemberState)
		{
			case "CMD_UNDEFINED":
			break;
			case "CMD_ACTIVE":
			break;
			case "CMD_BANNED":
			break;
			case "CMD_JOIN_INVITED":
			break;
			case "CMD_JOIN_REQUESTED":
			break;
		}
		
	}
/*
	public function c2f_Clans_Get():Void
	{
		var resultIdentifier:String = arguments[0];
		switch (resultIdentifier)
		{
			case "DATABASE_CONSTRAINT_FAILURE":
			break;
			case "SUCCESS":
			break;
		}
		
		var numClansReturned:Number = arguments[1];
		var argumentIndex:Number=2;
		for (var i:Number=0; i < numClansReturned; i++)
		{
			var clanName:String = arguments[argumentIndex++];
			var description:String = arguments[argumentIndex++];
			var clanLeader:String = arguments[argumentIndex++];
			var clanMembersOtherThanLeader:Number = arguments[argumentIndex++];
			for (var j:Number=0; j < clanMembersOtherThanLeader; j++)
			{
				var clanMemberName:String = arguments[argumentIndex++];
			}
		}
	}*/
	/*
	public function c2f_Clans_DownloadInvitationList():Void
	{
		var resultIdentifier:String = arguments[0];
		switch (resultIdentifier)
		{
			case "DATABASE_CONSTRAINT_FAILURE":
			break;
			case "SUCCESS":
			break;
		}
		var numResults:Number = arguments.length-1;
		for (var i:Number=0; i < numResults; i++)


		{
			var clanHandle:String = arguments[1+i];
		}
	}

	public function c2f_Clans_DownloadRequestList():Void
	{
		var resultIdentifier:String = arguments[0];
		switch (resultIdentifier)
		{
			case "DATABASE_CONSTRAINT_FAILURE":
			break;
			case "SUCCESS":
			break;
		}
		var joinRequestsToMyClan:Number = arguments[1];
		var joinRequestsFromMe:Number = arguments[2];
		for (var i:Number=0; i < joinRequestsToMyClan; i++)
		{
			var targetClan:String = arguments[3+i*3+0];
			var dateSent:String = arguments[3+i*3+1];
			var joinRequestSender:String = arguments[2+i*3+2];
		}
		
		for (var i:Number=0; i < joinRequestsFromMe; i++)
		{
			var targetClan:String = arguments[3+i*3+joinRequestsToMyClan*3+0];
			var dateSent:String = arguments[3+i*3+joinRequestsToMyClan*3+1];
			var joinRequestSender:String = arguments[3+i*3+joinRequestsToMyClan*3+2];
		}
	}*/

/*	public function c2f_Clans_GetBlacklist(resultIdentifier:String):Void
	{
		switch (resultIdentifier)
		{
			case "Clans_GetBlacklist_UNKNOWN_CLAN":
			break;
			case "DATABASE_CONSTRAINT_FAILURE":
			break;
			case "SUCCESS":
			break;
		}
		
		if ( resultIdentifier != "SUCCESS" )
		{
			LobbyInterface.Instance.CreateMessageBox( resultIdentifier );
		}
	}*/

/*	public function c2f_Clans_GetMembers():Void
	{
		var resultIdentifier:String = arguments[0];
		switch (resultIdentifier)
		{
			case "Clans_GetMembers_UNKNOWN_CLAN":
			break;
			case "DATABASE_CONSTRAINT_FAILURE":
			break;
			case "SUCCESS":
			break;
		}
		var clanLeader:String = arguments[1];
		var clanMembersOtherThanLeader:Number = arguments.length-2;
		for (var i:Number=0; i < clanMembersOtherThanLeader; i++)
		{
			var clanMember:String = arguments[2+i];
		}
		
	}*/

/*	public function c2f_Clans_GetList():Void
	{
		var resultIdentifier:String = arguments[0];
		switch (resultIdentifier)
		{
			case "DATABASE_CONSTRAINT_FAILURE":
			break;
			case "SUCCESS":
			break;
		}
		var numClanNames = arguments.length-1;
		for (var i:Number=0; i < numClanNames; i++)
		{
			var clanName:String = arguments[1+i];
		}
	}	*/
}