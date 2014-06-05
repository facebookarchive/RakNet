/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

#include "Rackspace.h"
#include <stdio.h>
#include "Gets.h"
#include "Rackspace.h"
#include "TCPInterface.h"
#include "Kbhit.h"
#include "RakSleep.h"

struct CommandAndDescription
{
	const char *command;
	const char *description;
};

CommandAndDescription commands[]=
{
	{"Help","This screen"},
	{"Quit","Exit application"},
	{"Authenticate","desc"},
	{"ListServers","desc"},
	{"ListServersWithDetails","desc"},
	{"CreateServer","desc"},
	{"GetServerDetails","desc"},
	{"UpdateServerNameOrPassword","desc"},
	{"DeleteServer","desc"},
	{"ListServerAddresses","desc"},
	{"ShareServerAddress","desc"},
	{"DeleteServerAddress","desc"},
	{"RebootServer","desc"},
	{"RebuildServer","desc"},
	{"ResizeServer","desc"},
	{"ConfirmResizedServer","desc"},
	{"RevertResizedServer","desc"},
	{"ListFlavors","desc"},
	{"GetFlavorDetails","desc"},
	{"ListImages","desc"},
	{"CreateImage","desc"},
	{"GetImageDetails","desc"},
	{"DeleteImage","desc"},
	{"ListSharedIPGroups","desc"},
	{"ListSharedIPGroupsWithDetails","desc"},
	{"CreateSharedIPGroup","desc"},
	{"GetSharedIPGroupDetails","desc"},
	{"DeleteSharedIPGroup","desc"},

};

void PrintCommands(void)
{
	printf("\n");

	unsigned int i;
	for (i=0; i < sizeof(commands) / sizeof(CommandAndDescription); i++)
	{
		printf("%s - %s\n", commands[i].command, commands[i].description);
	}

	printf("\n");
}

bool Authenticate(RakNet::Rackspace *rackspaceApi, RakNet::TCPInterface *tcpInterface)
{

	RakNet::SystemAddress serverAddress;
	char authenticationURL[128];
	char rackspaceCloudUsername[128];
	char apiAccessKey[128];

	// See http://docs.rackspacecloud.com/servers/api/v1.0/cs-devguide-20110112.pdf
	// US-based accounts authenticate through auth.api.rackspacecloud.com
	// UK-based accounts authenticate through lon.auth.api.rackspacecloud.com
	printf("Enter authenticationURL: ");
	Gets(authenticationURL, sizeof(authenticationURL));
	if (authenticationURL[0]==0)
	{
		FILE *fp;
		fp = fopen("authenticationURL.txt", "rb");
		if (fp)
		{
			fgets(authenticationURL,128,fp);
			fclose(fp);
			printf("Using %s\n", authenticationURL);
		}
		else
		{
			printf("Can't open authenticationURL.txt");
			return false;
		}
	}

	printf("Enter rackspaceCloudUsername: ");
	Gets(rackspaceCloudUsername, sizeof(rackspaceCloudUsername));
	if (rackspaceCloudUsername[0]==0)
	{
		FILE *fp;
		fp = fopen("rackspaceCloudUsername.txt", "rb");
		if (fp)
		{
			fgets(rackspaceCloudUsername,128,fp);
			fclose(fp);
			printf("Using %s\n", rackspaceCloudUsername);
		}
		else
		{
			printf("Can't open rackspaceCloudUsername.txt");
			return false;
		}
	}

	// This is found when you log in, under Your Account, expanded to API Access. Then click Show Key under Enable API Access.
	printf("Enter apiAccessKey: ");
	Gets(apiAccessKey, sizeof(apiAccessKey));
	if (apiAccessKey[0]==0)
	{
		FILE *fp;
		fp = fopen("apiAccessKey.txt", "rb");
		if (fp)
		{
			fgets(apiAccessKey,128,fp);
			fclose(fp);
			printf("Using %s\n", apiAccessKey);
		}
		else
		{
			printf("Can't open apiAccessKey.txt");
			return false;
		}
	}

	serverAddress=rackspaceApi->Authenticate(tcpInterface, authenticationURL, rackspaceCloudUsername, apiAccessKey);
	if (serverAddress==RakNet::UNASSIGNED_SYSTEM_ADDRESS)
		printf("Failed to connect to %s\n", authenticationURL);

	return true;
}

class DisplayHTMLPage : public RakNet::RackspaceEventCallback_Default
{
	virtual void ExecuteDefault(const char *callbackName, RakNet::RackspaceEventType eventType, const char *htmlAdditionalInfo)
	{
		/*
		RakNet::RakString fileName("%s.html", callbackName);
		FILE *fp = fopen(fileName.C_String(), "wb");
		if (fp)
		{
			fwrite(htmlAdditionalInfo,1,strlen(htmlAdditionalInfo),fp);
			fclose(fp);
			TCHAR szDirectory[MAX_PATH] = "";
			GetCurrentDirectory(sizeof(szDirectory) - 1, szDirectory);
			ShellExecute(NULL, "open", fileName.C_String(), NULL, szDirectory, SW_SHOWNORMAL);
		}
		*/
		printf("%s completed with result %s\n", callbackName, RakNet::Rackspace::EventTypeToString(eventType));
		printf(htmlAdditionalInfo);
		printf("\n");
	}
	virtual void OnConnectionAttemptFailure( RakNet::RackspaceOperationType operationType, const char *url) {printf("OnConnectionAttemptFailure\noperationType=%i\n%s\n", operationType,url);}
};

int main()
{
#if OPEN_SSL_CLIENT_SUPPORT!=1
	printf("RakNet must be built with OPEN_SSL_CLIENT_SUPPORT");
	return 1;
#endif

	RakNet::Rackspace rackspaceApi;
	RakNet::TCPInterface tcpInterface;

	DisplayHTMLPage callback;
	rackspaceApi.AddEventCallback(&callback);

	tcpInterface.Start(0, 0, 1);

	if (!Authenticate(&rackspaceApi, &tcpInterface))
	{
		return 1;
	}
	//rackspaceApi.ListImages();

	PrintCommands();

	RakNet::SystemAddress systemAddress;
	RakNet::Packet *packet;
	while (1)
	{
		for (packet=tcpInterface.Receive(); packet; tcpInterface.DeallocatePacket(packet), packet=tcpInterface.Receive())
		{
			rackspaceApi.OnReceive(packet);
		}

		RakNet::SystemAddress lostConnectionAddress = tcpInterface.HasLostConnection();
		if (lostConnectionAddress!=RakNet::UNASSIGNED_SYSTEM_ADDRESS)
			rackspaceApi.OnClosedConnection(lostConnectionAddress);

		if (kbhit())
		{
			printf("Command: ");

			char command[128];
			Gets(command, sizeof(command));

			if (stricmp(command,"Help")==0)
			{
				PrintCommands();
			}
			else if (stricmp(command,"Quit")==0)
			{
				break;
			}
			else if (stricmp(command,"Authenticate")==0)
			{
				Authenticate(&rackspaceApi, &tcpInterface);
			}
			else if (stricmp(command,"ListServers")==0)
			{
				rackspaceApi.ListServers();
			}
			else if (stricmp(command,"ListServersWithDetails")==0)
			{
				rackspaceApi.ListServersWithDetails();
			}
			else if (stricmp(command,"CreateServer")==0)
			{
				RakNet::RakString name;
				printf("Enter server name: ");
				Gets(command, sizeof(command));
				name=command;
				RakNet::RakString imageId;
				printf("Enter imageId: ");
				Gets(command, sizeof(command));
				imageId=command;
				RakNet::RakString flavorId;
				printf("Enter flavorId: ");
				Gets(command, sizeof(command));
				flavorId=command;

				rackspaceApi.CreateServer(name, imageId, flavorId);
			}
			else if (stricmp(command,"GetServerDetails")==0)
			{
				printf("Enter server id: ");
				Gets(command, sizeof(command));
				rackspaceApi.GetServerDetails(command);
			}
			else if (stricmp(command,"UpdateServerNameOrPassword")==0)
			{
				RakNet::RakString serverId;
				printf("Enter server id: ");
				Gets(command, sizeof(command));
				serverId=command;
				RakNet::RakString newName;
				printf("Enter newName: ");
				Gets(command, sizeof(command));
				newName=command;
				RakNet::RakString newPassword;
				printf("Enter newPassword: ");
				Gets(command, sizeof(command));
				newPassword=command;

				rackspaceApi.UpdateServerNameOrPassword(serverId,newName,newPassword);
			}
			else if (stricmp(command,"DeleteServer")==0)
			{
				printf("Enter server id: ");
				Gets(command, sizeof(command));
				rackspaceApi.DeleteServer(command);
			}
			else if (stricmp(command,"ListServerAddresses")==0)
			{
				printf("Enter server id: ");
				Gets(command, sizeof(command));
				rackspaceApi.ListServerAddresses(command);
			}
			else if (stricmp(command,"ShareServerAddress")==0)
			{
				RakNet::RakString serverId;
				printf("Enter server id: ");
				Gets(command, sizeof(command));
				serverId=command;
				RakNet::RakString serverAddress;
				printf("Enter server serverAddress: ");
				Gets(command, sizeof(command));
				serverAddress=command;
				rackspaceApi.ShareServerAddress(serverId, serverAddress);
			}
			else if (stricmp(command,"DeleteServerAddress")==0)
			{
				RakNet::RakString serverId;
				printf("Enter server id: ");
				Gets(command, sizeof(command));
				serverId=command;
				RakNet::RakString serverAddress;
				printf("Enter server serverAddress: ");
				Gets(command, sizeof(command));
				serverAddress=command;
				rackspaceApi.DeleteServerAddress(serverId, serverAddress);
			}
			else if (stricmp(command,"RebootServer")==0)
			{
				RakNet::RakString serverId;
				printf("Enter server id: ");
				Gets(command, sizeof(command));
				serverId=command;
				RakNet::RakString rebootType;
				printf("Enter rebootType: ");
				Gets(command, sizeof(command));
				rebootType=command;
				rackspaceApi.RebootServer(serverId,rebootType);
			}
			else if (stricmp(command,"RebuildServer")==0)
			{
				RakNet::RakString serverId;
				printf("Enter server id: ");
				Gets(command, sizeof(command));
				serverId=command;
				RakNet::RakString imageId;
				printf("Enter imageId: ");
				Gets(command, sizeof(command));
				imageId=command;
				rackspaceApi.RebuildServer(serverId,imageId);
			}
			else if (stricmp(command,"ResizeServer")==0)
			{
				RakNet::RakString serverId;
				printf("Enter server id: ");
				Gets(command, sizeof(command));
				serverId=command;
				RakNet::RakString flavorId;
				printf("Enter flavorId: ");
				Gets(command, sizeof(command));
				flavorId=command;
				rackspaceApi.ResizeServer(serverId,flavorId);
			}
			else if (stricmp(command,"ConfirmResizedServer")==0)
			{
				printf("Enter server id: ");
				Gets(command, sizeof(command));
				rackspaceApi.ConfirmResizedServer(command);
			}
			else if (stricmp(command,"RevertResizedServer")==0)
			{
				printf("Enter server id: ");
				Gets(command, sizeof(command));
				rackspaceApi.RevertResizedServer(command);
			}
			else if (stricmp(command,"ListFlavors")==0)
			{
				rackspaceApi.ListFlavors();
			}
			else if (stricmp(command,"GetFlavorDetails")==0)
			{
				printf("Enter flavor id: ");
				Gets(command, sizeof(command));
				rackspaceApi.GetFlavorDetails(command);
			}
			else if (stricmp(command,"ListImages")==0)
			{
				rackspaceApi.ListImages();
			}
			else if (stricmp(command,"CreateImage")==0)
			{
				RakNet::RakString serverId;
				printf("Enter server id: ");
				Gets(command, sizeof(command));
				serverId=command;
				RakNet::RakString imageName;
				printf("Enter imageName: ");
				Gets(command, sizeof(command));
				imageName=command;
				rackspaceApi.CreateImage(serverId,imageName);
			}
			else if (stricmp(command,"GetImageDetails")==0)
			{
				printf("Enter image id: ");
				Gets(command, sizeof(command));
				rackspaceApi.GetImageDetails(command);
			}
			else if (stricmp(command,"DeleteImage")==0)
			{
				printf("Enter image id: ");
				Gets(command, sizeof(command));
				rackspaceApi.DeleteImage(command);
			}
			else if (stricmp(command,"ListSharedIPGroups")==0)
			{
				rackspaceApi.ListSharedIPGroups();
			}
			else if (stricmp(command,"ListSharedIPGroupsWithDetails")==0)
			{
				rackspaceApi.ListSharedIPGroupsWithDetails();
			}
			else if (stricmp(command,"CreateSharedIPGroup")==0)
			{
				rackspaceApi.CreateSharedIPGroup("testSharedIPGroup","");
			}
			else if (stricmp(command,"GetSharedIPGroupDetails")==0)
			{
				printf("Enter group id: ");
				Gets(command, sizeof(command));
				rackspaceApi.GetSharedIPGroupDetails(command);
			}
			else if (stricmp(command,"DeleteSharedIPGroup")==0)
			{
				printf("Enter group id: ");
				Gets(command, sizeof(command));
				rackspaceApi.DeleteSharedIPGroup(command);
			}
			else
			{
				printf("Unknown command. Type 'help' for help.\n");
			}
		}

		RakSleep(30);
	}

	return 0;
}
