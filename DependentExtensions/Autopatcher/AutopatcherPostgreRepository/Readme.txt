Project: Autopatcher Server, implemented using PostgreSQL for the database
Description: Provides patch information and asynchronous database queries to AutopatcherClient

Dependencies: PostgreSQL version 8.3 (http://www.postgresql.org/download/), installed to C:\Program Files\PostgreSQL\8.3. If your installation directory is different, update it at
C/C++ / General / Additional Include Directories
and under
Build Events / Post Build Events
copy "C:\Program Files\PostgreSQL\8.1\bin\*.dll" .\Debug
to whatever your path is.

Related projects: AutopatcherClientRestarter, AutopatcherPostgreSQLRepository, AutopatcherServer

For help and support, please visit http://www.jenkinssoftware.com
