------------------ TECHNICAL OVERVIEW ------------------

Project: TeamManager

Description: Demonstrates the TeamManager in a typical in-game lobby setting. Uses the TeamBalancer, ReplicaManager3, and FullyConnectedMesh2 plugins

Dependencies: None

Related projects: None

For help and support, please visit http://vwww.jenkinssoftware.com

------------------ NETWORKING FLOW ------------------

TeamManager demonstrates an in-game lobby where users can switch between 3 teams. It uses the following RakNet plugins:

1. TeamBalancer: Handles networking and logic involved with joining and leaving teams. Requires that the user have corresponding TM_Team and TM_TeamMember objects in the game code.
2. ReplicaManager3: Handles networking and logic involved with creating replicated instances of objects. Used to replicate the User class to other systems, and to transmit serialization of the TM_Team and TM_TeamMember objects. ReplicaManager3 also depends on networkIDManager.
3. FullyConnectedMesh2: Handles determining the host of a peer to peer session. Queried by ReplicaManager3 callbacks to determine which systems serialize objects to new connections.

Before using TeamBalancer and ReplicaManager3, I wait until I know who the host is of the session. I do not know this until one other systems connects, at which point I get ID_FCM2_NEW_HOST. This is the purpose behaind the two calls to SetAutoManageConnections() with false. When I do know the host, I call RegisterFullyConnectedMesh2Participants() to register all prior connections with ReplicaManager3 and TeamBalancer. When I get ID_NEW_INCOMING_CONNECTION and ID_CONNECTION_REQUEST_ACCEPTED while I already know the host, I also register those connections with PushConnection() and AddParticipant()

ReplicaManager3 handles object replication to new participants. SerializeConstructionExisting() is called on all teams. The User object is replicated, using SerializeConstruction() to send initial state data. The Team and User classes contain corresponding instances of TM_Team and TM_TeamMember, so SerializeConstruction() is called on those instances. I reference the Team objects before the User objects with ReplicaManager3 first to ensure that the Team objects are serialized first. TeamBalancer requires this since TM_TeamMember::DeserializeConstruction() needs to be able to look up teams, so those teams must have been previously registered with TeamBalancer and deserialized.

The setup has one team "REFEREE_TEAM" joinable only through direct request. The other teams are subject to autobalancing.

Examples of intended operation

1. If there are two players on team one and no players on team two, when autobalancing is turned on the second player will be forced from team one to team two.
2. If there are two players on team one and one player on team two, when autobalancing is turned on the second player will be forced off team one and set to no team.
3. If there are two players on team 1, and the team limit for team 1 is reduced to 1, the second player to join team 1 is kicked off.
4. If there is one player on team 1, one player on team 2, and auto balancing is turned on, normally neither player can switch teams without the other player first leaving. However, if both players use RequestTeamSwitch() to switch to each other's teams, they exchange teams.
5. If team 1 is full or unjoinable due to unbalanced teams, anyone that requests to join team 1 has that team added to their requested list. If someone leaves team 1, or the team size is increased, or team balancing is turned off, requesting players join the vacated slots in order of request.


