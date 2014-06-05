Project: Autopatcher Server, implemented using MySQL for the database
Description: Provides patch information and asynchronous database queries to AutopatcherClient

Dependencies: MySQL version 5.0 (http://dev.mysql.com/downloads/mysql/5.0.html), installed to C:\Program Files\MySQL\. If your installation directory is different, update it at
C/C++ / General / Additional Include Directories
and under
Build Events / Post Build Events
copy "C:\Program Files\MySQL\MySQL Server 5.0\lib\opt\*.dll" .\Release
to whatever your path is.

Other requirements:
1. Run "CREATE DATABASE myDatabaseName" first in the adminstrator tools. Enter myDatabaseName when prompted "Enter DB schema:" from the C++ sample.
2. In MySQL Administrator, in the advanced networking tab, check max packet size, and change it to 1000M. Then restart the service through service control.

Related projects: AutopatcherClientRestarter, AutopatcherMySQLRepository, AutopatcherServer

For help and support, please visit http://www.jenkinssoftware.com
