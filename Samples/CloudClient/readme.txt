Project: Cloud Client

Description:
This project is associated with the CloudServer project and is itself enough to implement a directory server.
It will connect to whichever instance of the CloudServer project one you pass on the command line.
After connection, it will call the functions UploadInstanceToCloud(), GetClientSubscription(), and GetServers()

UploadInstanceToCloud()
This uses CloudClient to upload this instance to the cloud. The key is arbitrary.

GetClientSubscription()
This returns clients uploaded with UploadInstanceToCloud(), and subscribes to the results

GetServers()
This returns the list of running servers, with the connection counts for each. This was uploaded by the servers themselves. The key must match that used by the server (See CloudServerHelper.cpp)

ID_CLOUD_GET_RESPONSE is returned when GetClientSubscription() or GetServers() has results.
If the results are for GetServers(), it will further find which server has the fewest connections and reconnect to that server.
This is client-based load balancing.
For a directory server, you only need the results for the clients. The SystemAddress and RakNetGUID is returned for each client, along with arbitrary user data.

ID_CLOUD_SUBSCRIPTION_NOTIFICATION is returned when our subscription to the client list changes.
This is not necessary for most uses but is here for completeness.

Dependencies: None

Related projects: ComprehensivePCGame demonstrates CloudClient used to upload game servers.

For help and support, please visit http://www.jenkinssoftware.com