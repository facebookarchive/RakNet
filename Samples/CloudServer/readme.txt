Project: Cloud Server

Description:
The CloudServer plugin allows for queries on remote systems but does not provide a way to discover those systems.

This sample uses helper code, found in CloudServerSample.cpp to accomplish discovery and migration

1. When a server is activated, it checks a domain name passed to the command line for RakNet connectivity
1.A If the domain name connects to our own IP, we act as host
1.B If the domain name does not connect to another system, we point it to our own IP and act as host
1.C If the domain name connects to another system, we treat that system as host.
    The TwoWayAuthentication plugin is used to validate that the system is host by checking a pre-designated password.
    The CloudServer plugin is queried using a local CloudClient instance for other servers, on key CloudServerList,1
    The list of results is a list of other servers, including both internal and external IP.
    For each returned server, we try to first connect to the internal IP in case that server is co-located.
    If that fails, we connect to the external IP.
2. The local CloudClient instance will then upload to the cloud our own internal and external IP on CloudServerList,1.
3. The local CloudClient instance will also upload our extternal IP only to CloudServerList,0.
4. CloudServerQueryFilter restricts writes to CloudServerList,0 and CloudServerList,1, and restricts reads to CloudServerList,1 to the local RakPeer instance.
5. FullyConnectedMesh2::AddParticipant() is used to determine the host of the server fully connected mesh.
   When the host changes to the local system, The DynDNS class is used to update the DNS to point to the new host

Load balancing:

This project adds new servers to a mesh of servers, but does not restrict or load balance client connections.
To implement cloud server load balancing, connect a client to the domain name and use CloudClient::Get() with the key CloudConnCount,0.
This will return the number of connections to each server. Then reconnect to the server with the lowest load.
For more advanced load balancing, have the CloudClient instance on the server upload your own custom key

Adding server-based plugins to the cloud:

The following server-based plugins do not perform interactions between clients, so can be added into this sample as-is
1. AutopatcherServer (provided all servers contain the same database)
2. DeltaDirectoryTransfer
3. FileListTransfer
4. Lobby2 (database operations only, not login or presence)
5. NATTypeDetection

The following server-based plugins require that interacting clients be on the same server. I recommend connecting to all servers in advance,
and sending the request to the server that is connected to the client you are interacting with
1. NATPunchthroughServer
2. TeamManager (entire team must be on the same server)
3. RoomsPlugin (all users that interact with each other must be on the same server)

The following server-based plugins are already distributed
1. UDPProxyCoordinator supports multiple UDPProxyServers, although only one coordinator is supported

Dependencies: DynDNS

Related projects: None

For help and support, please visit http://www.jenkinssoftware.com
