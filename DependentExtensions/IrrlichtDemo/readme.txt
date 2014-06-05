------------------ NONTECHNICAL NOTES ------------------

This is a game demo based on modified Irrlicht demo code to add multiplayer capabilities with RakNet. WADS to move, click to shoot, escape to quit. Get the source from RakNet at DependentExtensions\IrrlichtDemo.

For help and support with the demo, email rakkar@jenkinssoftware.com

For information about RakNet, go to http://www.jenkinssoftware.com

For information about Irrlicht, go to http://irrlicht.sourceforge.net/ .
The sample assumes Irrlicht is unzipped to C:\irrlicht-1.7.2\include

Sound uses IrrKlang, which the author was nice enough to let us distribute. IrrKlang is located at http://www.ambiera.com/irrklang/

------------------ TECHNICAL OVERVIEW ------------------

Project: IrrlichtDemo

Description: Demonstrates Irrlicht modified with RakNet for peer to peer multiplayer, written in two days. See RakNetStuff.cpp for most of the networking code

Dependencies: For windows Irrlicht version 1.7.2 is assumed to be installed at C:\irrlicht-1.7.2 . If you don't have it installed here, change the project paths. IrrKlang included by default with permission of the author. The website for Irrlicht is http://irrlicht.sourceforge.net/
For linux version 1.7.2 or higher may be used, irrKlang headers and .so file is also required for the linux version.

Related projects: NatPunchthroughServer, DependentExtensions\miniupnpc-1.5

For help and support, please visit http://www.jenkinssoftware.com

------------------ NETWORKING FLOW ------------------

Once the user press Start Demo, InstantiateRakNetClasses is called. It allocates all RakNet classes including the dependent plugins. It will also try to connect to the NATCompleteServer sample hosted  hosted by Jenkins Software.

Once we are connected to the NATPunchthroughServer (See ID_CONNECTION_REQUEST_ACCEPTED), UPNP will run to open the router if possible. It will try to open the external port connected to the NATPunchthroughServer. It will map that to the internal port used by RakNet. If it succeeds, NATPunchthrough should automatically succeed for this system. Next, the cloudServer will be queried for active connections. If any connections are returned, NATPunchthroughClient::OpenNATGroup() to open the router for those systems. On success, those systems are connected to. If there are no existing games, or on failure, a new game is started.

Incoming packets are checked in UpdateRakNet(). If the NAT punchthrough fails, it will use the proxy server instead. CDemoderives from UDPProxyClientResultHandler, which will get the results of the proxy connection attempt through it callback interfaces.

When another user connections to us (either ID_NEW_INCOMING_CONNECTION or ID_CONNECTION_REQUEST_ACCEPTED), we create a new connection object and call ReplicaManager3::PushConnection(). This tells the automatic object replication system that this connection is ready to participate in the game.

On pushing a new connection to ReplicaManager3, all existing Replica3 objects are sent to that server. In this case, it is our own player, PlayerReplica, which was created in InstantiateRakNetClasses.

PlayerReplica derives from BaseIrrlichtReplica, which derives from Replica3. BaseIrrlichtReplica implements all the interfaces necessary for peer to peer multiplayer, particularly returning QueryConstruction_PeerToPeer, QueryRemoteConstruction_PeerToPeer, and QuerySerialization_PeerToPeer. It also has a member variable position, which is used by all derived classes. This variable is automatically synchronized in SerializeConstruction and Serialize.

PlayerReplica additionally serializes playerName, isMoving, isDead, and rotationAroundYAxis. playerName never changes so is sent only in SerializeConstruction. isMoving and isDead are serialized per-tick, and are used to control what animation is played on remote systems. rotationAroundYAxis is the camera rotation, which rotates the player on the remote system.

Both position and rotationAroundYAxis are interpolated on the remote system, using positionDeltaPerMS and rotationDeltaPerMS. When we deserialize either of these values, that amount is added per-tick based on the amount of time elapsed, until the real position is reached. This happens in Update(), which is called from the CDemo.

When the player presses the shoot button, CDemo::shoot() is called. If the player is not dead, CDemo::shootFromOrigin is called, which acts the same as in the original demo. It creates a moving ball, and plays a particle effect in the amount of time it would take the ball to hit the nearest terrain object. In the same function, a new instance of BallReplica is created and referenced. ReplicaManager3 will automatically transmit this new object to connected systems (also systems that connect later).

BallReplica is initialized with the same parameters as the animated particle created in shootFromOrigin. Its position is a different variable, but the math works the same so the replicated object is always in the same spot as the particle you see.

BallReplica::PostDeserializeConstruction is called on remote systems when a new ball is created. It calls shootFromOrigin to create the particle visible effect. It also causes the remote player with the same creatingSystemGUID to play the attack animation. creatingSystemGUID is a value automatically set by ReplicaManager3, and identifies which system originally instantiated this object.

Note that the position variable in BallReplica works differently than with PlayerReplica. In PlayerReplica, it is updated from the remote system because it can change at random. In BallReplica, it represents only the origin of when the ball was created, and doesn't otherwise change. This can be done because the path the ball takes is deterministic, and saves bandwidth and programming.

In BallReplica::Update, if this is our own ball, we check if the ball has existed long enough that it should hit a wall. If so, we destroy it, and send out this destruction packet to the other systems.

In BallReplica::Update, if this is a ball created by a remote system, we check if the ball has hit our own player. The function GetSyndeyBoundingBox() is needed because our own player has no model, it is only a camera. Were the game to use other models, we would need to calculate the bounding box for whatever player model we are using.

If we die, PlayerReplica::deathTimeout is set, and is sent to remote systems in PlayerReplica::Serialize as a single boolean read into the isDead member variable.

That's it :)

------------------ OTHER NOTES ------------------

1. I picked peer to peer because then I do not have to run a game server and it makes the sample simpler. It also lets me test and demonstrate NAT Punchthrough class. Were I to run a game server, it would have to use Irrlicht's NULL renderer, load the level, and perform all the same game functionality as the clients.
2. Peer to peer is very insecure from a cheating standpoint. For example, each peer determines for themselves if the ball hits them. This design is only suitable for games that cannot be hacked, such as console games, or if using anti-cheat measures (such as Punkbuster).
3. Because the ball effect in Irrlicht and the BallReplica class for the actual gameplay are disjoint, were a player to disconnect and his ball deleted, the visible effect would still be there. This is a minor bug. It could be fixed by adding a reference to the particle effect, and removing the particle when the ball is destroyed.
4. Since all peers connect to the same CloudServer, there is essentially only one game world
5. Yes, this was really written in two days :) Irrlicht and RakNet are awesome together.

Have fun!

Kevin Jenkins
rakkar@jenkinssoftware.com
