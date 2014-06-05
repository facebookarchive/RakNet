Project: Scalable autopatcher server service

Description: Expanded version of AutopatcherServer_PostgreSQL. It will self-scale to load, using Rackspace Cloud to add additional servers when all servers are full. Load balancing is accomplished with the help of CloudServer / CloudClient. DynDNS is used to point to the host of the system.

Sever setup:
1. Download RakNet
2. Define OPEN_SSL_CLIENT_SUPPORT 1 in RakNetDefines.h
3. Build AutopatcherServer-SelfScaling
4. Create a new user with administrator
5. Set Windows to auto-login with this user http://pcsupport.about.com/od/windows7/ht/auto-logon-windows-7.htm
6. Add to the startup folder (http://support.microsoft.com/kb/2806079) of that user ( C:\Users\Rakkar\AppData\Roaming\Microsoft\Windows\Start Menu\Programs\Startup ) C:\RakNet\Samples\AutopatcherServer_SelfScaling\Release\AutopatcherServer_SelfScaling.bat
7. In AutopatcherServer_SelfScaling.bat put
cd C:\RakNet\Samples\AutopatcherServer_SelfScaling
CALL C:\RakNet\Samples\AutopatcherServer_SelfScaling\Release\AutopatcherServer_SelfScaling.exe serverToServerPassword 60000 8096 8000 version1.mygamepatcher.com mygamepatcher.com https://identity.api.rackspacecloud.com/v2.0 rackspaceUsername rackspaceAPIAccessKey databasePassword 3 6 0
8. Install PostgreSQL to C:\Program Files (x86)\PostgreSQL\9.2 from http://www.postgresql.org/download/Dlls should exist with the .exe. See the post-build process which copies "C:\Program Files\PostgreSQL\8.2\bin\*.dll" .\Release
9. If you want to use xdelta3 to generate patches, this is assumed to be located at c:\xdelta3-3.0.6-win32.exe
10. Image the server
11. Delete the server
12. Recreate the server from the image
13. Create additional servers

Related projects: AutopatcherClientRestarter, AutopatcherPostgreSQLRepository, AutopatcherServer, RackspaceConsole

For help and support, please visit http://www.jenkinssoftware.com
