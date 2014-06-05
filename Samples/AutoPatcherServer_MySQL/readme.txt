Project: Test of the Autopatcher Server, implemented using MySQL for the database

Description: This is a sample implementation of the autopatcher server. It's actually complete enough to use if you wish.

Dependencies: MySQL version 5.1 (http://dev.mysql.com/downloads/mysql/5.0.html). Dlls should exist with the .exe. See the post-build process which copies copy "C:\Program Files\MySQL\MySQL Server 5.0\lib\opt\*.dll" .\Release

Other requirements:
1. Run "CREATE DATABASE myDatabaseName" first in the adminstrator tools, followed by \g to execute. Enter myDatabaseName when prompted "Enter DB schema:" from the C++ sample.
2. Download and install MySQL Administrator, which you have to download separately.
3. Login using localhost for server host, root for username, and the password you entered during installation for the password.
4. In MySQL Administrator, click Startup Variables on the Left. In the advanced networking tab, check max packet size, and change it to 1000M. Then restart the service through service control.

If it doesn't link, this is possibly because you installed the 64 bit download but are compiling 32 bit.

Related projects: AutopatcherClientRestarter, AutopatcherMySQLRepository, AutopatcherServer

For help and support, please visit http://www.jenkinssoftware.com
