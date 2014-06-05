using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Diagnostics;
using System.Threading;
using System.Timers;
using System.IO;
using RakNet;

namespace SwigTestApp
{
    class TestMain
    {
        static void Main(string[] args)
        {
            if (!File.Exists("RakNet.dll"))
            {
                Console.WriteLine("The SWIG build of the DLL has not been copied to the executable directory\nCopy from Swig/SwigWindowsCSharpSample/SwigTestApp/bin/X86/Debug/RakNet.dll to\nSwigWindowsCSharpSample/SwigTestApp/bin/Debug/RakNet.dll\nPress enter to quit.");
                Console.Read();
                return;
            }

            try
            {
                RakString dllCallTest = new RakString();
            }
            catch (Exception e)
            {
                Console.WriteLine("DLL issue\nAdd SwigOutput/CplusDLLIncludes/RakNetWrap.cxx to the project\nDLL_Swig/RakNet.sln and rebuild.\nPress enter to quit.");
                Console.Read();
                return;
            }

            Packet testPacket;
            int loopNumber;
            BitStream stringTestSendBitStream = new BitStream();
            BitStream rakStringTestSendBitStream = new BitStream();
            BitStream receiveBitStream = new BitStream();
            String holdingString;
            TimeSpan startTimeSpan;
            RakString rakStringTest = new RakString();

            RakPeerInterface testClient = RakPeer.GetInstance();
            
            testClient.Startup(1, new SocketDescriptor(60000, "127.0.0.1"), 1);

            RakPeerInterface testServer = RakPeer.GetInstance();
            testServer.Startup(1, new SocketDescriptor(60001, "127.0.0.1"), 1);
            testServer.SetMaximumIncomingConnections(1);

            Console.WriteLine("Send and receive loop using BitStream.\nBitStream read done into RakString");

            testClient.Connect("127.0.0.1", 60001, "", 0);

            String sendString = "The test string";
            stringTestSendBitStream.Write((byte)DefaultMessageIDTypes.ID_USER_PACKET_ENUM);
            stringTestSendBitStream.Write(sendString);

            RakString testRakString = new RakString("Test RakString");
            rakStringTestSendBitStream.Write((byte)DefaultMessageIDTypes.ID_USER_PACKET_ENUM);
            rakStringTestSendBitStream.Write(testRakString);

            startTimeSpan = (DateTime.UtcNow - new DateTime(1970, 1, 1));
            loopNumber = 0;

            while (startTimeSpan.TotalSeconds + 5 > (DateTime.UtcNow - new DateTime(1970, 1, 1)).TotalSeconds)
            {
                testPacket = testServer.Receive();
                if (testPacket != null && testPacket.data[0] == (byte)DefaultMessageIDTypes.ID_USER_PACKET_ENUM)
                {
                    receiveBitStream.Reset();
                    receiveBitStream.Write(testPacket.data, testPacket.length);
                    receiveBitStream.IgnoreBytes(1);
                    receiveBitStream.Read(rakStringTest);
                    Console.WriteLine("Loop number: " + loopNumber + "\nData: " + rakStringTest.C_String());
                }
                testServer.DeallocatePacket(testPacket);
                loopNumber++;
                System.Threading.Thread.Sleep(50);
                testClient.Send(rakStringTestSendBitStream, PacketPriority.LOW_PRIORITY, PacketReliability.RELIABLE_ORDERED, (char)0, new AddressOrGUID(new SystemAddress("127.0.0.1", 60001)), false);
            }

            Console.WriteLine("String send and receive loop using BitStream.\nBitStream read done into String");

            SystemAddress[] remoteSystems;
            ushort numberOfSystems=1;
            testServer.GetConnectionList(out remoteSystems, ref numberOfSystems);

            startTimeSpan = (DateTime.UtcNow - new DateTime(1970, 1, 1));
            loopNumber = 0;
            while (startTimeSpan.TotalSeconds + 5 > (DateTime.UtcNow - new DateTime(1970, 1, 1)).TotalSeconds)
            {
                testPacket = testServer.Receive();
                if (testPacket != null && testPacket.data[0] == (byte)DefaultMessageIDTypes.ID_USER_PACKET_ENUM)
                {
                    receiveBitStream.Reset();
                    receiveBitStream.Write(testPacket.data, testPacket.length);
                    receiveBitStream.IgnoreBytes(1);
                    receiveBitStream.Read(out holdingString);
                    Console.WriteLine("Loop number: " + loopNumber + "\nData: " + holdingString);
                }
                testServer.DeallocatePacket(testPacket);
                loopNumber++;
                System.Threading.Thread.Sleep(50);
                SystemAddress sa = RakNet.RakNet.UNASSIGNED_SYSTEM_ADDRESS;
                testClient.Send(stringTestSendBitStream, PacketPriority.LOW_PRIORITY, PacketReliability.RELIABLE_ORDERED, (char)0, new AddressOrGUID(new SystemAddress("127.0.0.1", 60001)), false);
            }
            //If RakString is not freed before program exit it will crash
            rakStringTest.Dispose();
            testRakString.Dispose();

            RakPeer.DestroyInstance(testClient);
            RakPeer.DestroyInstance(testServer);
            Console.WriteLine("Demo complete. Press Enter.");
            Console.Read();
        }
    }

#if AUTOPATCHERMYSQLTESTS

        private static int TestAutoPatcherClient()
        {
            //Stick the restarter path here
            String restarterPath = "C:\\Rak4\\Samples\\AutopatcherClientRestarter\\Debug\\AutopatcherClientRestarter.exe";
            TestCB transferCallback = new TestCB();
            Console.Write("A simple client interface for the advanced autopatcher.\n");
            Console.Write("Use DirectoryDeltaTransfer for a simpler version of an autopatcher.\n");
            Console.Write("Difficulty: Intermediate\n\n");

            Console.Write("Client starting...");
            SystemAddress serverAddress = RakNet.RakNet.UNASSIGNED_SYSTEM_ADDRESS;
            AutopatcherClient autopatcherClient = new AutopatcherClient();
            FileListTransfer fileListTransfer = new FileListTransfer();
            autopatcherClient.SetFileListTransferPlugin(fileListTransfer);

            RakPeerInterface rakPeer;
            rakPeer = RakPeerInterface.GetInstance();
            SocketDescriptor socketDescriptor = new SocketDescriptor(0, null);
            rakPeer.Startup(1, socketDescriptor, 1);
            // Plugin will send us downloading progress notifications if a file is split to fit under the MTU 10 or more times
            rakPeer.SetSplitMessageProgressInterval(10);
            rakPeer.AttachPlugin(autopatcherClient);
            rakPeer.AttachPlugin(fileListTransfer);

            Console.Write("started\n");
            String buff;
            Console.Write("Enter server IP: ");
            buff = Console.ReadLine();
            if (buff == "")
                buff = "127.0.0.1";

            rakPeer.Connect(buff, 60000, null, 0);

            Console.Write("Connecting...\n");
            String appDir;
            Console.Write("Enter application directory: ");
            appDir = Console.ReadLine();
            if (appDir == "")
            {
                appDir = "C:/temp2";
            }

            String appName;

            Console.Write("Enter application name: ");
            appName = Console.ReadLine();
            if (appName == "")
                appName = "TestApp";


            bool patchImmediately = false;

            if (patchImmediately == false)
                Console.Write("Hit 'q' to quit, 'p' to patch, 'c' to cancel the patch. 'r' to reconnect. 'd' to disconnect.\n");
            else
                Console.Write("Hit 'q' to quit, 'c' to cancel the patch.\n");

            char ch;
            Packet p;
            while (true)
            {
                p = rakPeer.Receive();
                while (p != null)
                {
                    if (p.data[0] == (byte)DefaultMessageIDTypes.ID_DISCONNECTION_NOTIFICATION)
                        Console.Write("ID_DISCONNECTION_NOTIFICATION\n");
                    else if (p.data[0] == (byte)DefaultMessageIDTypes.ID_CONNECTION_LOST)
                        Console.Write("ID_CONNECTION_LOST\n");
                    else if (p.data[0] == (byte)DefaultMessageIDTypes.ID_CONNECTION_REQUEST_ACCEPTED)
                    {
                        Console.Write("ID_CONNECTION_REQUEST_ACCEPTED\n");
                        serverAddress = p.systemAddress;
                    }
                    else if (p.data[0] == (byte)DefaultMessageIDTypes.ID_CONNECTION_ATTEMPT_FAILED)
                        Console.Write("ID_CONNECTION_ATTEMPT_FAILED\n");
                    else if (p.data[0] == (byte)DefaultMessageIDTypes.ID_AUTOPATCHER_REPOSITORY_FATAL_ERROR)
                    {
                        //String buff;
                        //BitStream temp = new BitStream(p.data, p.length, false);
                        //temp.IgnoreBits(8);
                        //StringCompressor.Instance().DecodeString(buff, 256, temp);
                        //Console.Write("ID_AUTOPATCHER_REPOSITORY_FATAL_ERROR\n");
                        //Console.Write("%s\n", buff);
                    }
                    else if (p.data[0] == (byte)DefaultMessageIDTypes.ID_AUTOPATCHER_FINISHED)
                        Console.Write("ID_AUTOPATCHER_FINISHED\n");
                    else if (p.data[0] == (byte)DefaultMessageIDTypes.ID_AUTOPATCHER_RESTART_APPLICATION)
                        Console.Write("Launch \"AutopatcherClientRestarter.exe autopatcherRestart.txt\"\nQuit this application immediately after to unlock files.\n");

                    rakPeer.DeallocatePacket(p);
                    p = rakPeer.Receive();
                }

                if (Console.KeyAvailable)
                    ch = Console.ReadKey().KeyChar;
                else
                    ch = (char)0;

                if (ch == 'q')
                    break;
                else if (ch == 'r')
                {
                    rakPeer.Connect(buff, 60000, null, 0);
                }
                else if (ch == 'd')
                {
                    rakPeer.CloseConnection(serverAddress, true);
                }
                else if (ch == 'p' || (serverAddress != RakNet.RakNet.UNASSIGNED_SYSTEM_ADDRESS && patchImmediately == true))
                {
                    patchImmediately = false;
                    String lastUpdateDate;
                    String restartFile;
                    restartFile = appDir;
                    restartFile += "/autopatcherRestart.txt";
                    //	Console.Write("Enter last update date (only newer updates retrieved) or nothing to get all updates\n");
                    //	lastUpdateDate = Console.ReadLine();
                    lastUpdateDate = "";

                    if (autopatcherClient.PatchApplication(appName, appDir, lastUpdateDate, serverAddress, transferCallback, restartFile, restarterPath))
                    {
                        Console.Write("Patching process starting.\n");
                    }
                    else
                    {
                        Console.Write("Failed to start patching.\n");
                    }
                }
                else if (ch == 'c')
                {
                    autopatcherClient.Clear();
                    Console.Write("Autopatcher cleared.\n");
                }

                Thread.Sleep(30);
            }

            // Dereference so the destructor doesn't crash
            autopatcherClient.SetFileListTransferPlugin(null);

            rakPeer.Shutdown(500, 0);
            RakPeerInterface.DestroyInstance(rakPeer);

            return 1;

        }

        private static void TestAutoPatcherServer()
        {
            TimeSpan startTimeSpan;
            // Avoids the Error: Got a packet bigger than 'max_allowed_packet' bytes
            Console.Write("Important: Requires that you first set the DB schema and the max packet size on the server.\n");
            Console.Write("See DependentExtensions/AutopatcherMySQLRepository/readme.txt\n");

            Console.Write("Server starting... ");
            AutopatcherServer autopatcherServer = new AutopatcherServer();
            FLP_Printf progressIndicator = new FLP_Printf();
            FileListTransfer fileListTransfer = new FileListTransfer();
            // So only one thread runs per connection, we create an array of connection objects, and tell the autopatcher server to use one thread per item
            const int sqlConnectionObjectCount = 4;
            AutopatcherMySQLRepository[] connectionObject = new AutopatcherMySQLRepository[sqlConnectionObjectCount];
            AutopatcherRepositoryInterface[] connectionObjectAddresses = new AutopatcherRepositoryInterface[sqlConnectionObjectCount];
            for (int i = 0; i < sqlConnectionObjectCount; i++)
            {
                connectionObject[i] = new AutopatcherMySQLRepository();
                connectionObjectAddresses[i] = connectionObject[i];
            }
            fileListTransfer.SetCallback(progressIndicator);
            autopatcherServer.SetFileListTransferPlugin(fileListTransfer);

            RakPeerInterface rakPeer;
            rakPeer = RakPeerInterface.GetInstance();
            SocketDescriptor socketDescriptor = new SocketDescriptor((ushort)LISTEN_PORT, null);
            rakPeer.Startup(8, socketDescriptor, 1);
            rakPeer.SetMaximumIncomingConnections(MAX_INCOMING_CONNECTIONS);
            rakPeer.AttachPlugin(autopatcherServer);
            rakPeer.AttachPlugin(fileListTransfer);

            Console.Write("started.\n");

            Console.Write("Enter database password:\n");
            String password;
            String username = "root";
            password = Console.ReadLine();
            if (password == "")
                password = "aaaa";
            string db;
            Console.Write("Enter DB schema: ");
            // To create the schema, go to the command line client and type create schema autopatcher;
            // You also have to add 
            // max_allowed_packet=128M
            // Where 128 is the maximum size file in megabytes you'll ever add
            // to MySQL\MySQL Server 5.1\my.ini in the [mysqld] section
            // Be sure to restart the service after doing so
            db = Console.ReadLine(); ;
            if (db == "")
                db = "autopatcher";
            for (int conIdx = 0; conIdx < sqlConnectionObjectCount; conIdx++)
            {
                if (!connectionObject[conIdx].Connect("localhost", username, password, db, 0, null, 0))
                {
                    Console.Write("Database connection failed.\n");
                    return;
                }
            }
            Console.Write("Database connection suceeded.\n");

            Console.Write("Starting threads\n");
            autopatcherServer.StartThreads(sqlConnectionObjectCount, connectionObjectAddresses);
            Console.Write("System ready for connections\n");

            Console.Write("(D)rop database\n(C)reate database.\n(A)dd application\n(U)pdate revision.\n(R)emove application\n(Q)uit\n");

            char ch;
            Packet p;

            while (true)
            {
                p = rakPeer.Receive();
                while (p != null)
                {
                    if (p.data[0] == (byte)DefaultMessageIDTypes.ID_NEW_INCOMING_CONNECTION)
                        Console.Write("ID_NEW_INCOMING_CONNECTION\n");
                     else if (p.data[0] == (byte)DefaultMessageIDTypes.ID_DISCONNECTION_NOTIFICATION)
                    	Console.Write("ID_DISCONNECTION_NOTIFICATION\n");
                     else if (p.data[0] == (byte)DefaultMessageIDTypes.ID_CONNECTION_LOST)
                    	Console.Write("ID_CONNECTION_LOST\n");

                    rakPeer.DeallocatePacket(p);
                    p = rakPeer.Receive();
                }
                if (Console.KeyAvailable)
                {
                    ch = Console.ReadKey().KeyChar;
                    if (ch == 'q')
                        break;
                    else if (ch == 'c')
                    {
                        if (connectionObject[0].CreateAutopatcherTables() == false)
                            Console.Write("Error: %s\n", connectionObject[0].GetLastError());
                        else
                            Console.Write("Created\n");
                    }
                    else if (ch == 'd')
                    {
                        if (connectionObject[0].DestroyAutopatcherTables() == false)
                            Console.Write("Error: %s\n", connectionObject[0].GetLastError());
                        else
                            Console.Write("Destroyed\n");
                    }
                    else if (ch == 'a')
                    {
                        Console.Write("Enter application name to add: ");
                        string appName;
                        appName = Console.ReadLine(); ;
                        if (appName == "")
                            appName = "TestApp";

                        if (connectionObject[0].AddApplication(appName, username) == false)
                            Console.Write("Error: %s\n", connectionObject[0].GetLastError());
                        else
                            Console.Write("Done\n");
                    }
                    else if (ch == 'r')
                    {
                        Console.Write("Enter application name to remove: ");
                        string appName;
                        appName = Console.ReadLine(); ;
                        if (appName == "")
                            appName = "TestApp";

                        if (connectionObject[0].RemoveApplication(appName) == false)
                            Console.Write("Error: %s\n", connectionObject[0].GetLastError());
                        else
                            Console.Write("Done\n");
                    }
                    else if (ch == 'u')
                    {
                        Console.Write("Enter application name: ");
                        string appName;
                        appName = Console.ReadLine(); ;
                        if (appName == "")
                            appName = "TestApp";

                        Console.Write("Enter application directory: ");
                        string appDir;
                        appDir = Console.ReadLine(); ;
                        if (appDir == "")
                            appDir = "C:/temp";

                        if (connectionObject[0].UpdateApplicationFiles(appName, appDir, username, progressIndicator) == false)
                        {
                            Console.Write("Error: %s\n", connectionObject[0].GetLastError());
                        }
                        else
                        {
                            Console.Write("Update success.\n");
                        }
                    }
                }
                Thread.Sleep(30);
            }
            RakPeerInterface.DestroyInstance(rakPeer);
        }
#endif

}
