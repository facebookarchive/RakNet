Project: SteamLobby

Description: Demonstrates how to integrate the Steam lobby and NAT traversal sockets with RakNet via a plugin

Dependencies:

1. Assumes the Steam SDK is located at C:\Steamworks . For example, with version 115, unzip sdk to C:\Steamworks . If not, modify the post-build step paths and linker paths accordingly.
2. You can get the Steamworks API from https://partner.steamgames.com/ , which requires a signup and legal agreement.
3. You need your own steam_appid.txt file. This sample just copies the one that comes with steam.
4. Must recompile RakNet with MAXIMUM_MTU_SIZE set to 1200 in the preprocessor settings

Related projects: Lobby2

For help and support, please visit http://www.jenkinssoftware.com
