using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Diagnostics;
using System.Threading;
using System.Timers;
using System.IO;
using RakNet;

namespace InternalSwigTestApp
{
    class TestMain
    {
        static void Main(string[] args)
        {

           if(!File.Exists("RakNet.dll"))
           {
                Console.WriteLine("Error: The SWIG build of the DLL has not been copied to the executable directory.\nPress enter.");
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

            Console.WriteLine("Press enter to start RakString send and receive loop using BitStream.\nBitStream read done into RakString");
            Console.WriteLine("Loop will run for 15 seconds");
            Console.ReadLine();

            testClient.Connect("127.0.0.1", 60001, "", 0);

            String sendString = "The test string";
            stringTestSendBitStream.Write((byte)DefaultMessageIDTypes.ID_USER_PACKET_ENUM);
            stringTestSendBitStream.Write(sendString);

            RakString testRakString = new RakString("Test RakString");
            rakStringTestSendBitStream.Write((byte)DefaultMessageIDTypes.ID_USER_PACKET_ENUM);
            rakStringTestSendBitStream.Write(testRakString);

            startTimeSpan = (DateTime.UtcNow - new DateTime(1970, 1, 1));
            loopNumber = 0;
                while (startTimeSpan.TotalSeconds + 15 > (DateTime.UtcNow - new DateTime(1970, 1, 1)).TotalSeconds)
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

                Console.WriteLine("Press enter to start String send and receive loop using BitStream.\nBitStream read done into String");
                Console.WriteLine("Loop will run for 15 seconds");
                Console.ReadLine();

                startTimeSpan = (DateTime.UtcNow - new DateTime(1970, 1, 1));
                loopNumber = 0;
                while (startTimeSpan.TotalSeconds + 15 > (DateTime.UtcNow - new DateTime(1970, 1, 1)).TotalSeconds)
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
                    testClient.Send(stringTestSendBitStream, PacketPriority.LOW_PRIORITY, PacketReliability.RELIABLE_ORDERED, (char)0, new AddressOrGUID(new SystemAddress("127.0.0.1", 60001)), false);
                }
            //-----------------------------Above is the same as the public tests, below the internal tests are ran.
            /*Member variables are also tested to assure working typemaps
             * Also variables use functions as well, they are not direct access
             * act like it. Test write and read on these items, especially arrays.
             * Most tests in this section are simple running functions with no verification.
             * 
             * 
            */

            RakString workingRakStringCopy;//This is needed for temp RakString Instances in the main function so dispose can be called

            //RakNetStatistics
            Console.WriteLine("RakNetStatistics");
            string rakNetStatisticsTestString;
            RakNetStatistics testRakNetStatistics = new RakNetStatistics();
            testRakNetStatistics.bytesInSendBuffer[0] = 22.44;
            testRakNetStatistics.BPSLimitByCongestionControl = 9;
            testRakNetStatistics.BPSLimitByOutgoingBandwidthLimit = 9;
            testRakNetStatistics.connectionStartTime = 9;
            testRakNetStatistics.isLimitedByCongestionControl = false;
            testRakNetStatistics.isLimitedByOutgoingBandwidthLimit = true;
            testRakNetStatistics.messageInSendBuffer[0] = 112;
            testRakNetStatistics.messagesInResendBuffer = 9;
            testRakNetStatistics.packetlossLastSecond = 4.3f;
            testRakNetStatistics.packetlossTotal = 9.9f;
            testRakNetStatistics.runningTotal[0] = 121;
            testRakNetStatistics.valueOverLastSecond[0] = 112;

            Console.WriteLine(testRakNetStatistics.bytesInSendBuffer[0]);
            Console.WriteLine(testRakNetStatistics.BPSLimitByCongestionControl);
            Console.WriteLine(testRakNetStatistics.BPSLimitByOutgoingBandwidthLimit);
            Console.WriteLine(testRakNetStatistics.connectionStartTime);
            Console.WriteLine(testRakNetStatistics.isLimitedByCongestionControl);
            Console.WriteLine(testRakNetStatistics.isLimitedByOutgoingBandwidthLimit);
            Console.WriteLine(testRakNetStatistics.messageInSendBuffer[0]);
            Console.WriteLine(testRakNetStatistics.messagesInResendBuffer);
            Console.WriteLine(testRakNetStatistics.packetlossLastSecond);
            Console.WriteLine(testRakNetStatistics.packetlossTotal);
            Console.WriteLine(testRakNetStatistics.runningTotal[0]);
            Console.WriteLine(testRakNetStatistics.valueOverLastSecond[0]);

            RakNet.RakNet.StatisticsToString(testRakNetStatistics, out rakNetStatisticsTestString, 1);
            Console.WriteLine(rakNetStatisticsTestString);

            //SystemAddress
            Console.WriteLine("SystemAddress");
            SystemAddress testSystemAddress = new SystemAddress("127.0.0.1", 60001);

            //RakNetGUID
            RakNetGUID testRakNetGUID = testClient.GetGuidFromSystemAddress(RakNet.RakNet.UNASSIGNED_SYSTEM_ADDRESS);

            Console.WriteLine(testRakNetGUID);

            //AddressOrGUID
            Console.WriteLine("AddressOrGUID");
            AddressOrGUID testAddressOrGUIDAddress = new AddressOrGUID(testSystemAddress);

            Console.WriteLine(testAddressOrGUIDAddress.systemAddress);

            AddressOrGUID testAddressOrGUIDRakNetGUID = new AddressOrGUID(testRakNetGUID);

            Console.WriteLine(testAddressOrGUIDRakNetGUID.rakNetGuid);

            //BitStream
            Console.WriteLine("BitStream");
            BitStream testBitStream1 = new BitStream();
            BitStream testBitStream2 = new BitStream();
            byte[] testBuffer = new byte[999];
            byte[] writeArray = new byte[99];
            Packet bitStreamTestPacket = null;

            testBitStream1.Write((byte)DefaultMessageIDTypes.ID_USER_PACKET_ENUM);
            testBitStream1.ResetWritePointer();
            testBitStream1.Write((byte)DefaultMessageIDTypes.ID_USER_PACKET_ENUM);

            writeArray[0] = (byte)127;
            testBitStream1.Write("xy");
            testBitStream1.WriteCompressed("xy");
            testBitStream1.WriteFloat16(23.5f, 0, 100);
            testBitStream1.WriteBits(writeArray, 7);
            testBitStream1.WriteAlignedBytesSafe(writeArray, 1, 1);
            testBitStream1.WriteAlignedBytes(writeArray, 1);
            testBitStream1.Write0();
            testBitStream1.Write1();
            int intForSerialization = 99;
            testBitStream1.Serialize(true, ref intForSerialization);

            testClient.Send(testBitStream1, PacketPriority.HIGH_PRIORITY, PacketReliability.RELIABLE_ORDERED, (char)0, new AddressOrGUID(RakNet.RakNet.UNASSIGNED_SYSTEM_ADDRESS), true);

            while (bitStreamTestPacket == null || bitStreamTestPacket.data[0] != (byte)DefaultMessageIDTypes.ID_USER_PACKET_ENUM)
            {
                if (bitStreamTestPacket != null)
                {
                    testServer.DeallocatePacket(bitStreamTestPacket);
                }
                bitStreamTestPacket = testServer.Receive();
                testClient.Send(testBitStream1, PacketPriority.HIGH_PRIORITY, PacketReliability.RELIABLE_ORDERED, (char)0, new AddressOrGUID(RakNet.RakNet.UNASSIGNED_SYSTEM_ADDRESS), true);
            }

            testBitStream2.Reset();
            testBitStream2 = new BitStream(bitStreamTestPacket.data, bitStreamTestPacket.length, true);

            testBitStream2.IgnoreBytes(1);
            testBitStream2.ResetReadPointer();
            testBitStream2.IgnoreBits(8);
            testBitStream2.ResetReadPointer();
            testBitStream2.ReadBits(testBuffer, 8);
            Console.WriteLine(testBuffer[0]);
            Console.WriteLine(DefaultMessageIDTypes.ID_USER_PACKET_ENUM);
            testBitStream2.ResetReadPointer();
            testBitStream2.ReadAlignedVar8(testBuffer);
            Console.WriteLine(testBuffer[0]);
            testBitStream2.ResetReadPointer();
            testBitStream2.ReadAlignedVar16(testBuffer);
            Console.WriteLine(testBuffer[1]);
            testBitStream2.ResetReadPointer();
            testBitStream2.ReadAlignedVar32(testBuffer);
            Console.WriteLine(testBuffer[1]);
            testBitStream2.ResetReadPointer();
            testBitStream2.IgnoreBits(8);

            Console.WriteLine("testBitStream1 hex and bits");
            testBitStream1.PrintHex();
            Console.WriteLine("\n");
            testBitStream1.PrintBits();
            Console.WriteLine("\ntestBitStream1 hex and bits");

            Console.WriteLine("testBitStream2 hex and bits end");
            testBitStream2.PrintHex();
            Console.WriteLine("\n");
            testBitStream2.PrintBits();
            Console.WriteLine("\ntestBitStream2 hex and bits end");

            String testString;
            Console.WriteLine("testBitStream2 hex and bits string output");
            testBitStream2.PrintHex(out testString);
            Console.WriteLine(testString + "\n");
            testBitStream2.PrintBits(out testString);
            Console.WriteLine(testString);
            Console.WriteLine("testBitStream2 hex and bits string output end");

            testBitStream2.Read(out testString);
            Console.WriteLine(testString);

            Console.WriteLine(testBitStream2.ReadCompressed(out testString));
            Console.WriteLine(testString);
            testBitStream2.WriteCompressedDelta("xy", "yz");
            testBitStream2.WriteCompressedDelta("xy", "xy");
            Console.WriteLine(testBitStream2.ReadCompressedDelta(out testString));
            Console.WriteLine(testString);
            Console.WriteLine(testBitStream2.ReadCompressedDelta(out testString));
            Console.WriteLine(testString);
            testBitStream2.WriteDelta("xy", "yz");
            testBitStream2.WriteDelta("xy", "xy");
            Console.WriteLine(testBitStream2.ReadDelta(out testString));
            Console.WriteLine(testString);
            Console.WriteLine(testBitStream2.ReadDelta(out testString));
            Console.WriteLine(testString);

            float testFloat;
            testBitStream2.ReadFloat16(out testFloat, 0, 100);
            Console.WriteLine(testFloat);
            testBitStream2.ReadBits(testBuffer, 7);
            Console.WriteLine(testBuffer[0]);
            testBitStream2.ReadAlignedBytesSafe(testBuffer, 1, 1);
            Console.WriteLine(testBuffer[0]);
            testBitStream2.ReadAlignedBytes(testBuffer, 1);
            Console.WriteLine(testBuffer[0]);
            Console.WriteLine(testBitStream2.ReadBit());
            Console.WriteLine(testBitStream2.ReadBit());
            int intForSerialization2 = 0;
            testBitStream2.Serialize(false, ref intForSerialization2);
            Console.WriteLine(intForSerialization2);

            testBitStream2.PadWithZeroToByteLength(99);
            testBitStream2.PrintBits();
            testBitStream2.SetData(testBuffer);
            Console.WriteLine("setbuff");
            testBitStream2.PrintBits();

            Console.WriteLine(testBitStream2.GetNumberOfBitsAllocated());

            //ByteQueue
            Console.WriteLine("ByteQueue");
            ByteQueue testByteQueue = new ByteQueue();
            testByteQueue.WriteBytes(testBuffer, (uint)testBuffer.Length, "", 0);
            testByteQueue.DecrementReadOffset(1);
            Console.WriteLine(testByteQueue.GetBytesWritten());
            testByteQueue.IncrementReadOffset(1);
            uint testUint;
            byte[] returnArray = testByteQueue.PeekContiguousBytes(out testUint);
            testByteQueue.Print();
            testByteQueue.ReadBytes(returnArray, (uint)returnArray.Length, true);
            testByteQueue.Clear("", 0);

            testBitStream2.AddBitsAndReallocate(99);
            testBitStream2.AlignReadToByteBoundary();
            testBitStream2.AlignWriteToByteBoundary();
            testBitStream2.EndianSwapBytes(5, 5);
            returnArray = testBitStream2.GetData();
            Console.WriteLine(testBitStream2.GetNumberOfBitsAllocated());
            Console.WriteLine(testBitStream2.GetNumberOfBitsUsed());
            Console.WriteLine(testBitStream2.GetNumberOfBytesUsed());
            Console.WriteLine(testBitStream2.GetNumberOfUnreadBits());
            Console.WriteLine(testBitStream2.GetReadOffset());
            Console.WriteLine(testBitStream2.GetWriteOffset());
            testBitStream2.SetData(returnArray);
            testBitStream2.SetNumberOfBitsAllocated(testBitStream2.GetNumberOfBitsAllocated());
            testBitStream2.SetReadOffset(3);
            testBitStream2.SetWriteOffset(3);
            testBitStream2.WriteBits(returnArray, (uint)returnArray.Length);

            //Cell, Table, Row
            Console.WriteLine("Cell, Table, Row");
            Cell testCell = new Cell();
            Byte[] tableTestByteArray = new byte[1];
            tableTestByteArray[0] = (byte)'t';
            int outInt;
            double outDouble;

            testCell.Set("test1232");
            testCell.Get(out testString);
            Console.WriteLine(testString);
            Console.WriteLine((int)testCell.EstimateColumnType());
            testCell.CopyData(testCell);
            testCell.Set(12.5d);
            testCell.Get(out outDouble);
            Console.WriteLine(outDouble);
            testCell.Set((uint)1);
            testCell.Set((int)1);
            testCell.Get(out outInt);
            Console.WriteLine(outInt);
            testCell.Set(tableTestByteArray, tableTestByteArray.Length);
            testCell.Get(tableTestByteArray, out outInt);
            Console.WriteLine(testCell.i);
            Console.WriteLine(testCell.isEmpty);

            Table testTable = new Table();
            testTable.Clear();
            testTable.AddColumn("testNumeric", Table.ColumnType.NUMERIC);
            testTable.AddColumn("testString", Table.ColumnType.STRING);
            testTable.AddColumn("testBinary", Table.ColumnType.BINARY);
            testTable.AddRow(0, new RakNetListCell());
            testTable.AddRow(1, new RakNetListCell());
            testTable.AddRow(2);
            testTable.AddRow(3);
            testTable.AddRow(4);
            testTable.AddRow(5);

            Console.WriteLine(testTable.ColumnIndex("testNumeric"));
            Console.WriteLine(testTable.ColumnName(0));
            Console.WriteLine(testTable.GetAvailableRowId());
            testTable.UpdateCell(0, 0, 9);
            testTable.UpdateCell(0, 1, "testCell");
            testTable.UpdateCell(0, 2, 1, tableTestByteArray);
            testTable.GetCellValueByIndex(0, 0, out outInt);
            Console.WriteLine(outInt);
            string outString;
            testTable.GetCellValueByIndex(0, 1, out outString);
            Console.WriteLine(outString);

            int outLen;
            testTable.GetCellValueByIndex(0, 2, out tableTestByteArray, out outLen);
            Console.WriteLine(tableTestByteArray[0]);
            Console.WriteLine(outLen);
            Console.WriteLine(testTable.GetColumnCount());
            RakNetListColumnDescriptor descriptorList = testTable.GetColumns();
            Console.WriteLine(testTable.GetColumnType(1) == Table.ColumnType.STRING);

            RakNetPageRow testPageRow = testTable.GetListHead();
            Console.WriteLine(testPageRow.isLeaf);
            Console.WriteLine(testPageRow.size);
            Row testRow = testTable.GetRowByID(0);
            Console.WriteLine(testRow.cells[0]);

            Console.WriteLine(testTable.GetRowCount());
            RakNetBPlusTreeRow bplus = testTable.GetRows();

            byte[] byteTest = new byte[999];
            testTable.PrintColumnHeaders(byteTest, 999, ';');

            testTable.PrintRow(byteTest, 999, ';', true, testRow);

            uint[] columnIds = new uint[1];
            columnIds[0] = 0;
            uint[] rowIds = new uint[1];
            rowIds[0] = 0;

            Table outTable = new Table();
            testTable.QueryTable(columnIds, 1, null, 0, rowIds, 1, outTable);
            Row[] testRows;
            testTable.SortTable(null, 0, out testRows);

            testTable.UpdateCellByIndex(0, 1, "5");
            testTable.PrintColumnHeaders(testBuffer, testBuffer.Length, ';');
            Console.WriteLine(System.Text.Encoding.GetEncoding(1251).GetString(testBuffer).Trim('\0'));

            testTable.PrintRow(testBuffer, testBuffer.Length, ';', true, testRow);
            Console.WriteLine(System.Text.Encoding.GetEncoding(1251).GetString(testBuffer).Trim('\0'));

            testTable.RemoveColumn(2);
            testTable.RemoveRow(2);
            testTable.RemoveRows(outTable);

            //ColumnDescriptor
            Console.WriteLine("ColumnDescriptor");
            ColumnDescriptor testColumnDescriptor = descriptorList.Pop();
            Console.WriteLine(testColumnDescriptor.columnName);
            Console.WriteLine(testColumnDescriptor.columnType);

            //ConnectionGraph2
            Console.WriteLine("ConnectionGraph2");
            ConnectionGraph2 testConnectionGraph2 = new ConnectionGraph2();

            testClient.AttachPlugin(testConnectionGraph2);
            testConnectionGraph2.ConnectionExists(testClient.GetGuidFromSystemAddress(RakNet.RakNet.UNASSIGNED_SYSTEM_ADDRESS), testServer.GetGuidFromSystemAddress(RakNet.RakNet.UNASSIGNED_SYSTEM_ADDRESS));
            SystemAddress[] outSystemAddressList = new SystemAddress[1];
            RakNetGUID[] outGuidList = new RakNetGUID[1];
            uint inOutUint = 1;
            testConnectionGraph2.GetConnectionListForRemoteSystem(testServer.GetGuidFromSystemAddress(RakNet.RakNet.UNASSIGNED_SYSTEM_ADDRESS), outSystemAddressList, outGuidList, ref inOutUint);

            //DirectoryDeltaTransfer
            Console.WriteLine("DirectoryDeltaTransfer");
            DirectoryDeltaTransfer testDirectoryDeltaTransfer = new DirectoryDeltaTransfer();
            testClient.AttachPlugin(testDirectoryDeltaTransfer);

            testDirectoryDeltaTransfer.AddUploadsFromSubdirectory("./");
            testDirectoryDeltaTransfer.ClearUploads();

            FileListTransferCBInterface testCallBack = new FileListTransferCB();
            FileListProgress testFileListProgress = new FileListProgress();

            Console.WriteLine(testDirectoryDeltaTransfer.GetNumberOfFilesForUpload());
            testDirectoryDeltaTransfer.SetApplicationDirectory("./");
           
            IncrementalReadInterface testIncrementalReadInterface = null;
            testDirectoryDeltaTransfer.SetDownloadRequestIncrementalReadInterface(testIncrementalReadInterface, 24);
            FileListTransfer testFileListTransfer = new FileListTransfer();
            testDirectoryDeltaTransfer.SetFileListTransferPlugin(testFileListTransfer);
            testDirectoryDeltaTransfer.SetUploadSendParameters(PacketPriority.HIGH_PRIORITY, (char)0);

            testDirectoryDeltaTransfer.DownloadFromSubdirectory("./", "./", false, testClient.GetSystemAddressFromIndex(0), testCallBack, PacketPriority.HIGH_PRIORITY, (char)0, testFileListProgress);

            //FileListNode
            Console.WriteLine("FileListNode");
            FileListNode testFileListNode = new FileListNode();

            //FileListNodeContext
            Console.WriteLine("FileListNodeContext");
            FileListNodeContext testFileListNodeContext = new FileListNodeContext();
            testFileListNodeContext.op = 0;
            testFileListNodeContext.fileId = 0;
            Console.WriteLine(testFileListNodeContext.op);
            Console.WriteLine(testFileListNodeContext.fileId);

            //FileList
            Console.WriteLine("FileList");
            FileList testFileList = FileList.GetInstance();

            testFileList.AddFile("./", "RakNet.dll", testFileListNodeContext);
            testFileList.AddFilesFromDirectory("./", "./", true, true, false, testFileListNodeContext);
            testFileList.FlagFilesAsReferences();
            FileList outFileList = FileList.GetInstance();
            testFileList.GetDeltaToCurrent(testFileList, outFileList, "", "./");
            Console.WriteLine(testFileList.fileList);
            testFileList.Clear();
            testFileList.DeleteFiles("./notexisting");
            testFileList.ListMissingOrChangedFiles("./", outFileList, false, false);
            testFileList.PopulateDataFromDisk("./", false, false, false);
            testFileList.WriteDataToDisk("./notexisting/");
            testFileList.Serialize(testBitStream2);
            testFileList.SetCallback(testFileListProgress);

            //FileListProgress
            Console.WriteLine("FileListProgress");
            testFileListProgress.OnAddFilesFromDirectoryStarted(testFileList, "./");
            testFileListProgress.OnDirectory(testFileList, "./", 9);
            testFileListProgress.OnFile(testFileList, "./", "nonexist", 22);
            testFileListProgress.OnFilePush("./Nonexist", 99, 99, 99, true, RakNet.RakNet.UNASSIGNED_SYSTEM_ADDRESS);

            //FileListTransferCBInterface
            Console.WriteLine("FileListTransferCBInterface");
            FileListTransferCBInterface testFileListTransferCBInterface = new FileListTransferCBInterface();

            //FileListTransfer
            Console.WriteLine("FileListTransfer");
            FileListTransfer testFileListTransferServer = new FileListTransfer();

            testClient.AttachPlugin(testFileListTransfer);
            testServer.AttachPlugin(testFileListTransferServer);
            testFileListTransfer.GetCallback();
            testFileListTransfer.GetPendingFilesToAddress(RakNet.RakNet.UNASSIGNED_SYSTEM_ADDRESS);
            testFileListTransfer.IsHandlerActive(99);
            testFileListTransfer.SetCallback(testFileListProgress);
            testFileListTransfer.SetupReceive(testFileListTransferCBInterface, false, RakNet.RakNet.UNASSIGNED_SYSTEM_ADDRESS);
            testFileListTransfer.Send(testFileList, testClient, RakNet.RakNet.UNASSIGNED_SYSTEM_ADDRESS, 99, PacketPriority.LOW_PRIORITY, (char)0, testIncrementalReadInterface);
            testFileListTransfer.CancelReceive(0);
            testFileListTransfer.RemoveReceiver(RakNet.RakNet.UNASSIGNED_SYSTEM_ADDRESS);

            //FileProgressStruct
            Console.WriteLine("FileProgressStruct");
            FileProgressStruct testFileProgressStruct = new FileProgressStruct();

            testFileProgressStruct.allocateIrIDataChunkAutomatically = true;
            testFileProgressStruct.dataChunkLength = 99;
            testFileProgressStruct.firstDataChunk = testBuffer;
            testFileProgressStruct.iriDataChunk = testBuffer;
            testFileProgressStruct.iriWriteOffset = 99;
            testFileProgressStruct.onFileStruct = new OnFileStruct();
            testFileProgressStruct.partCount = 99;
            testFileProgressStruct.partTotal = 99;

            Console.WriteLine(testFileProgressStruct.allocateIrIDataChunkAutomatically);
            Console.WriteLine(testFileProgressStruct.dataChunkLength);
            Console.WriteLine(testFileProgressStruct.firstDataChunk[0]);
            Console.WriteLine(testFileProgressStruct.iriDataChunk[0]);
            Console.WriteLine(testFileProgressStruct.iriWriteOffset);
            Console.WriteLine(testFileProgressStruct.onFileStruct);
            Console.WriteLine(testFileProgressStruct.partCount);
            Console.WriteLine(testFileProgressStruct.partTotal);

            //FilterQuery
            Console.WriteLine("FilterQuery");
            FilterQuery testFilterQuery = new FilterQuery();
            testFilterQuery.cellValue = testCell;
            testFilterQuery.columnIndex = 0;
            testFilterQuery.columnName = "TestName";
            testFilterQuery.operation = Table.FilterQueryType.QF_EQUAL;

            Console.WriteLine(testFilterQuery.cellValue);
            Console.WriteLine(testFilterQuery.columnIndex);
            Console.WriteLine(testFilterQuery.columnName);
            Console.WriteLine(testFilterQuery.operation);

            //FLP_Printf
            Console.WriteLine("FLP_Printf");
            FLP_Printf testFLP_Printf = new FLP_Printf();

            testFLP_Printf.OnAddFilesFromDirectoryStarted(testFileList, "./");
            testFLP_Printf.OnDirectory(testFileList, "./", 9);
            testFLP_Printf.OnFile(testFileList, "./", "nonexist", 22);
            testFLP_Printf.OnFilePush("./Nonexist", 99, 99, 99, true, RakNet.RakNet.UNASSIGNED_SYSTEM_ADDRESS);

            //FullyConnectedMesh2
            Console.WriteLine("FullyConnectedMesh2");
            FullyConnectedMesh2 testFullyConnectedMesh2 = FullyConnectedMesh2.GetInstance();
            testClient.AttachPlugin(testFullyConnectedMesh2);
            testFullyConnectedMesh2.AddParticipant(testServer.GetGuidFromSystemAddress(RakNet.RakNet.UNASSIGNED_SYSTEM_ADDRESS));
            Console.WriteLine(testFullyConnectedMesh2.GetConnectedHost());
            Console.WriteLine(testFullyConnectedMesh2.GetConnectedHostAddr());
            Console.WriteLine(testFullyConnectedMesh2.GetHostSystem());
            Console.WriteLine(testFullyConnectedMesh2.GetParticipantCount());
            uint outUint;
            testFullyConnectedMesh2.GetParticipantCount(out outUint);
            Console.WriteLine(outUint);
            Console.WriteLine(testFullyConnectedMesh2.IsConnectedHost());
            Console.WriteLine(testFullyConnectedMesh2.IsHostSystem());
            testFullyConnectedMesh2.ResetHostCalculation();
            testFullyConnectedMesh2.SetAutoparticipateConnections(true);
            workingRakStringCopy = "none";
            testFullyConnectedMesh2.SetConnectOnNewRemoteConnection(false, workingRakStringCopy);
            workingRakStringCopy.Dispose();

            //MessageFilter
            Console.WriteLine("MessageFilter");
            MessageFilter testMessageFilter = new MessageFilter();
            testClient.AttachPlugin(testMessageFilter);

            testMessageFilter.SetSystemFilterSet(testClient.GetSystemAddressFromIndex(0), 99);
            testMessageFilter.SetAllowRPC(false, "test", 99);
            testMessageFilter.SetAllowMessageID(true, 99, 99, 99);
            testMessageFilter.SetAutoAddNewConnectionsToFilter(99);
            testMessageFilter.SetFilterMaxTime(99, false, 0, 98);
            testMessageFilter.GetSystemFilterSet(testClient.GetSystemAddressFromIndex(0));
            testMessageFilter.GetSystemCount(99);
            testMessageFilter.GetSystemByIndex(99, 0);
            testMessageFilter.GetFilterSetIDByIndex(0);
            testMessageFilter.GetFilterSetCount();
            testMessageFilter.DeleteFilterSet(99);

            //NatPunchthroughClient
            Console.WriteLine("NatPunchthroughClient");
            NatPunchthroughClient testNatPunchthroughClient = new NatPunchthroughClient();
            testClient.AttachPlugin(testNatPunchthroughClient);
            testNatPunchthroughClient.GetPunchthroughConfiguration();
            testNatPunchthroughClient.GetUPNPExternalPort();
            workingRakStringCopy = testNatPunchthroughClient.GetUPNPInternalAddress();
            workingRakStringCopy.Dispose();
            testNatPunchthroughClient.GetUPNPInternalPort();
            testNatPunchthroughClient.OpenNAT(testServer.GetGuidFromSystemAddress(RakNet.RakNet.UNASSIGNED_SYSTEM_ADDRESS), testClient.GetSystemAddressFromIndex(0));
            NatPunchthroughDebugInterface_PacketLogger testNatPunchthroughDebugInterface_PacketLogger = new NatPunchthroughDebugInterface_PacketLogger();
            testNatPunchthroughClient.SetDebugInterface(testNatPunchthroughDebugInterface_PacketLogger);
            
            //NatPunchthroughDebugInterface_PacketLogger
            Console.WriteLine("NatPunchthroughDebugInterface_PacketLogger");
            testNatPunchthroughDebugInterface_PacketLogger.OnClientMessage("test");
            Console.WriteLine(testNatPunchthroughDebugInterface_PacketLogger.pl);

            //NatPunchthroughDebugInterface_Printf
            Console.WriteLine("NatPunchthroughDebugInterface_Printf");
            NatPunchthroughDebugInterface_Printf testNatPunchthroughDebugInterface_Printf = new NatPunchthroughDebugInterface_Printf();
            testNatPunchthroughClient.SetDebugInterface(testNatPunchthroughDebugInterface_Printf);
            testNatPunchthroughDebugInterface_Printf.OnClientMessage("test");
 
            //NatTypeDetectionClient
            Console.WriteLine("NatTypeDetectionClient");
            NatTypeDetectionClient testNatTypeDetectionClient = new NatTypeDetectionClient();
            testClient.AttachPlugin(testNatTypeDetectionClient);
            testNatTypeDetectionClient.DetectNATType(testClient.GetSystemAddressFromIndex(0));

            //NatTypeDetectionServer
            Console.WriteLine("NatTypeDetectionServer");
            NatTypeDetectionServer testNatTypeDetectionServer = new NatTypeDetectionServer();
            testServer.AttachPlugin(testNatTypeDetectionServer);
            testNatTypeDetectionServer.Startup("127.0.0.1","127.0.0.1","127.0.0.1");
            testNatTypeDetectionServer.Shutdown();

            //NetworkIDObject
            Console.WriteLine("NetworkIDObject");
            NetworkIDManager testNetworkIDManager = new NetworkIDManager();
            NetworkIDObject testNetworkIDObject = new TestNetworkIDObjectSubClass();
            testNetworkIDObject.SetNetworkIDManager(testNetworkIDManager);
            testNetworkIDObject.GetNetworkIDManager();
            testNetworkIDObject.GetNetworkID();

            //OnFileStruct
            Console.WriteLine("OnFileStruct");
            OnFileStruct testOnFileStruct = new OnFileStruct();
            testOnFileStruct.byteLengthOfThisFile = 99;
            testOnFileStruct.byteLengthOfThisSet = 99;
            testOnFileStruct.bytesDownloadedForThisFile = 99;
            testOnFileStruct.bytesDownloadedForThisSet = 99;
            testOnFileStruct.context = testFileListNodeContext;
            testOnFileStruct.fileIndex = 99;
            testOnFileStruct.fileName = "99";
            testOnFileStruct.numberOfFilesInThisSet = 99;
            testOnFileStruct.setID = 99;
            testOnFileStruct.fileData = testBuffer;
            Console.WriteLine(testOnFileStruct.fileData[0]);

            //OutOfBandIdentifiers
            Console.WriteLine("OutOfBandIdentifiers");
            OutOfBandIdentifiers testOutOfBandIdentifiers = new OutOfBandIdentifiers();

            //Packet
            Console.WriteLine("Packet");

            //PacketConsoleLogger
            Console.WriteLine("PacketConsoleLogger");
            PacketConsoleLogger testPacketConsoleLogger = new PacketConsoleLogger();

            testClient.AttachPlugin(testPacketConsoleLogger);

            LogCommandParser testLogCommandParser = new LogCommandParser();

            testPacketConsoleLogger.SetLogCommandParser(testLogCommandParser);

            //LogCommandParser
            Console.WriteLine("LogCommandParser");

            //PacketFileLogger
            Console.WriteLine("PacketFileLogger");
            PacketFileLogger testPacketFileLogger = new PacketFileLogger();
            string bigString = new string('c', 9999);

            testPacketFileLogger.StartLog("TestLog");
            testClient.AttachPlugin(testPacketFileLogger);
            testPacketFileLogger.FormatLine(ref bigString, "./", "none", 9, 9, 99, 99, 99, testClient.GetSystemAddressFromIndex(0), testClient.GetSystemAddressFromIndex(0), 99, 99, 99, 99);
            testPacketFileLogger.LogHeader();
            testPacketFileLogger.SetLogDirectMessages(false);
            testPacketFileLogger.SetPrefix("lv");
            testPacketFileLogger.SetPrintAcks(false);
            testPacketFileLogger.SetPrintID(false);
            testPacketFileLogger.SetSuffix("tr");
            testPacketFileLogger.WriteLog("gr");
            testPacketFileLogger.WriteMiscellaneous("lf", "mr");

            testClient.DetachPlugin(testPacketFileLogger);
            testPacketFileLogger.Dispose();

            foreach (string file in Directory.GetFiles("./", "TestLog*.csv"))
            {
                File.Delete(file);
            }
             
            //PluginInterface2
            Console.WriteLine("PluginInterface2");
            PluginInterface2 testPluginInterface2 = new PluginInterface2();

            //PluginReceiveResult
            Console.WriteLine("PluginReceiveResult");
            PluginReceiveResult testPluginReceiveResult = new PluginReceiveResult();

            //PunchthroughConfiguration
            Console.WriteLine("PunchthroughConfiguration");
            PunchthroughConfiguration testPunchthroughConfiguration = new PunchthroughConfiguration();

            //Raknet
            RakNet.RakNet.NonNumericHostString("12");

            //RakNetBPlusTreeRow
            Console.WriteLine("RakNetBPlusTreeRow");
            RakNetBPlusTreeRow testRakNetBPlusTreeRow = new RakNetBPlusTreeRow();

            //RakNetGUID
            Console.WriteLine("RakNetGUID");
            Console.WriteLine(testRakNetGUID.g);
            Console.WriteLine(testRakNetGUID.systemIndex);
            Console.WriteLine(testRakNetGUID == RakNet.RakNet.UNASSIGNED_RAKNET_GUID);
            Console.WriteLine(testRakNetGUID > RakNet.RakNet.UNASSIGNED_RAKNET_GUID);
            Console.WriteLine(testRakNetGUID < RakNet.RakNet.UNASSIGNED_RAKNET_GUID);

            //RakNetList, only one is needed
            //RakNetListSystemAddress
            Console.WriteLine("RakNetListSystemAddress");
            RakNetListSystemAddress testRakNetListSystemAddress = new RakNetListSystemAddress();
            testRakNetListSystemAddress.Insert(RakNet.RakNet.UNASSIGNED_SYSTEM_ADDRESS,"",0);
            testRakNetListSystemAddress.Preallocate(99, "", 0);
            testRakNetListSystemAddress.Push(RakNet.RakNet.UNASSIGNED_SYSTEM_ADDRESS,"",0);
            testRakNetListSystemAddress.RemoveAtIndex(0);
            testRakNetListSystemAddress.RemoveAtIndexFast(0);
            testRakNetListSystemAddress.Insert(RakNet.RakNet.UNASSIGNED_SYSTEM_ADDRESS, "", 0);
            testRakNetListSystemAddress.Push(RakNet.RakNet.UNASSIGNED_SYSTEM_ADDRESS, "", 0);
            testRakNetListSystemAddress.RemoveFromEnd();
            testRakNetListSystemAddress.Replace(RakNet.RakNet.UNASSIGNED_SYSTEM_ADDRESS);
            testRakNetListSystemAddress.Size();
            testRakNetListSystemAddress.Pop();
            testRakNetListSystemAddress.Compress("", 0);
            testRakNetListSystemAddress.CopyData(testRakNetListSystemAddress);
            testRakNetListSystemAddress.Clear(false, "", 0);
            testRakNetListSystemAddress.Insert(RakNet.RakNet.UNASSIGNED_SYSTEM_ADDRESS, "", 0);
            testRakNetListSystemAddress.Push(RakNet.RakNet.UNASSIGNED_SYSTEM_ADDRESS, "", 0);
            testRakNetListSystemAddress.Get(0);
            testRakNetListSystemAddress.GetIndexOf(RakNet.RakNet.UNASSIGNED_SYSTEM_ADDRESS);

            //RakNetPageRow
            Console.WriteLine("RakNetPageRow");
            RakNetPageRow testRakNetPageRow = new RakNetPageRow();
            testRakNetPageRow.isLeaf = false;
            testRakNetPageRow.next = testRakNetPageRow;
            testRakNetPageRow.previous = testRakNetPageRow;
            testRakNetPageRow.size = 9;

            Console.WriteLine(testRakNetPageRow.isLeaf);
            Console.WriteLine(testRakNetPageRow.next);
            Console.WriteLine(testRakNetPageRow.previous);
            Console.WriteLine(testRakNetPageRow.size);

            //RakNetSocket
            Console.WriteLine("RakNetSocket");
            RakNetSocket testRakNetSocket = new RakNetSocket();
            testRakNetSocket.boundAddress = RakNet.RakNet.UNASSIGNED_SYSTEM_ADDRESS;
            testRakNetSocket.remotePortRakNetWasStartedOn_PS3 = 0;
            testRakNetSocket.s = 99;
            testRakNetSocket.userConnectionSocketIndex = 0;

            Console.WriteLine(testRakNetSocket.boundAddress);
            Console.WriteLine(testRakNetSocket.remotePortRakNetWasStartedOn_PS3);
            Console.WriteLine(testRakNetSocket.s);
            Console.WriteLine(testRakNetSocket.userConnectionSocketIndex);

            //ReadyEvent
            Console.WriteLine("ReadyEvent");
            ReadyEvent testReadyEvent = new ReadyEvent();
            testClient.AttachPlugin(testReadyEvent);
            testReadyEvent.AddToWaitList(99, RakNet.RakNet.UNASSIGNED_SYSTEM_ADDRESS);
            testReadyEvent.GetEventAtIndex(0);
            testReadyEvent.GetEventListSize();
            testReadyEvent.GetFromWaitListAtIndex(99, 0);
            testReadyEvent.GetReadyStatus(99, RakNet.RakNet.UNASSIGNED_SYSTEM_ADDRESS);
            testReadyEvent.GetRemoteWaitListSize(99);
            testReadyEvent.HasEvent(99);
            testReadyEvent.IsEventCompleted(99);
            testReadyEvent.IsEventCompletionProcessing(99);
            testReadyEvent.IsEventSet(99);
            testReadyEvent.IsInWaitList(99, RakNet.RakNet.UNASSIGNED_SYSTEM_ADDRESS);
            testReadyEvent.SetEvent(99, false);
            testReadyEvent.SetSendChannel(9);
            testReadyEvent.RemoveFromWaitList(99, RakNet.RakNet.UNASSIGNED_SYSTEM_ADDRESS);
            testReadyEvent.ForceCompletion(99);
            testReadyEvent.DeleteEvent(99);

            //RemoteSystemIndex
            Console.WriteLine("RemoteSystemIndex");
            RemoteSystemIndex testRemoteSystemIndex = new RemoteSystemIndex();
            testRemoteSystemIndex.index = 5;
            testRemoteSystemIndex.next = testRemoteSystemIndex;

            Console.WriteLine(testRemoteSystemIndex.index);
            Console.WriteLine(testRemoteSystemIndex.next);

            //Router2
            Console.WriteLine("Router2");
            Router2 testRouter2 = new Router2();
            testClient.AttachPlugin(testRouter2);
            testRouter2.EstablishRouting(testClient.GetGUIDFromIndex(0));
            testRouter2.GetConnectionRequestIndex(testClient.GetGUIDFromIndex(0));
            testRouter2.SetMaximumForwardingRequests(99);
         

            //SimpleMutex
            Console.WriteLine("SimpleMutex");
            SimpleMutex testSimpleMutex = new SimpleMutex();
            testSimpleMutex.Lock();
            testSimpleMutex.Unlock();

            //SocketDescriptor
            Console.WriteLine("SocketDescriptor");
            SocketDescriptor testSocketDescriptor = new SocketDescriptor();
            testSocketDescriptor.hostAddress = "127.0.0.1";
            testSocketDescriptor.port = 60002;
            testSocketDescriptor.remotePortRakNetWasStartedOn_PS3 = 0;

            Console.WriteLine(testSocketDescriptor.hostAddress);
            Console.WriteLine(testSocketDescriptor.port);
            Console.WriteLine(testSocketDescriptor.remotePortRakNetWasStartedOn_PS3);

            //TeamBalancer
            Console.WriteLine("TeamBalancer");
            TeamBalancer testTeamBalancer = new TeamBalancer();
            testClient.AttachPlugin(testTeamBalancer);
            testTeamBalancer.SetAllowHostMigration(false);
            testTeamBalancer.SetDefaultAssignmentAlgorithm(TeamBalancer.DefaultAssigmentAlgorithm.FILL_IN_ORDER);
            testTeamBalancer.SetHostGuid(testClient.GetGUIDFromIndex(0));
            testTeamBalancer.SetLockTeams(false);
            RakNetListUnsignedShort ushortList = new RakNetListUnsignedShort();
            ushortList.Push(5, "", 0);
            testTeamBalancer.SetTeamSizeLimits(ushortList);
            testTeamBalancer.SetForceEvenTeams(true);
            testTeamBalancer.CancelRequestSpecificTeam();
            testTeamBalancer.GetMyTeam();
            testTeamBalancer.RequestAnyTeam(); 
            testTeamBalancer.RequestSpecificTeam(0);
            
            //ThreadsafePacketLogger
            Console.WriteLine("ThreadsafePacketLogger");
            ThreadsafePacketLogger testThreadsafePacketLogger = new ThreadsafePacketLogger();
            testClient.AttachPlugin(testThreadsafePacketLogger);
            testThreadsafePacketLogger.FormatLine(ref bigString,"./","none",9,9,99,99,99,testClient.GetSystemAddressFromIndex(0),testClient.GetSystemAddressFromIndex(0),99,99,99,99);
            testThreadsafePacketLogger.LogHeader();
            testThreadsafePacketLogger.SetLogDirectMessages(false);
            testThreadsafePacketLogger.SetPrefix("lv");
            testThreadsafePacketLogger.SetPrintAcks(false);
            testThreadsafePacketLogger.SetPrintID(false);
            testThreadsafePacketLogger.SetSuffix("tr");
            testThreadsafePacketLogger.WriteLog("gr");
            testThreadsafePacketLogger.WriteMiscellaneous("lf","mr");

            //UDPForwarder
            Console.WriteLine("UDPForwarder");
            UDPForwarder testUDPForwarder = new UDPForwarder();
            testUDPForwarder.Startup();
            testUDPForwarder.GetMaxForwardEntries();
            testUDPForwarder.GetUsedForwardEntries();
            testUDPForwarder.SetMaxForwardEntries(99);
            ushort outUshort;
            testUDPForwarder.StartForwarding(testClient.GetSystemAddressFromIndex(0), testServer.GetSystemAddressFromIndex(0), 99, "127.0.0.1", out outUshort, out outUint);
            testUDPForwarder.StopForwarding(testClient.GetSystemAddressFromIndex(0),testServer.GetSystemAddressFromIndex(0));
            Thread.Sleep(2000);
            testUDPForwarder.StartForwardingThreaded(testClient.GetSystemAddressFromIndex(0), testServer.GetSystemAddressFromIndex(0), 99, "127.0.0.1", out outUshort, out outUint);
            testUDPForwarder.StopForwardingThreaded(testClient.GetSystemAddressFromIndex(0), testServer.GetSystemAddressFromIndex(0));
            Thread.Sleep(1000);
            //testUDPForwarder.threadOperationIncomingMutex=new SimpleMutex();
           // testUDPForwarder.threadOperationOutgoingMutex=new SimpleMutex();
            testUDPForwarder.threadRunning=false;
            testUDPForwarder.isRunning=false;
            testUDPForwarder.maxForwardEntries = 99;

            Console.WriteLine(testUDPForwarder.threadOperationIncomingMutex);
            Console.WriteLine(testUDPForwarder.threadOperationOutgoingMutex);
            Console.WriteLine(testUDPForwarder.threadRunning);
            Console.WriteLine(testUDPForwarder.isRunning);
            Console.WriteLine(testUDPForwarder.maxForwardEntries);

            testUDPForwarder.Shutdown();

            //UDPProxyClient
            Console.WriteLine("UDPProxyClient");
            UDPProxyClient testUDPProxyClient = new UDPProxyClient();
            UDPProxyClientResultHandler testUDPProxyClientResultHandler= new UDPProxyClientResultHandlerCB();

            testUDPProxyClient.SetResultHandler(testUDPProxyClientResultHandler);
            testClient.AttachPlugin(testUDPProxyClient);
            testUDPProxyClient.RequestForwarding(testClient.GetSystemAddressFromIndex(0), testServer.GetSystemAddressFromIndex(0),testClient.GetGUIDFromIndex(0),1);
 
            //UDPProxyCoordinator
            Console.WriteLine("UDPProxyCoordinator");
            UDPProxyCoordinator testUDPProxyCoordinator = new UDPProxyCoordinator();
            testClient.AttachPlugin(testUDPProxyCoordinator);
            testUDPProxyCoordinator.SetRemoteLoginPassword(rakStringTest);
           
            //UDPProxyServer
            Console.WriteLine("UDPProxyServer");
            UDPProxyServer testUDPProxyServer = new UDPProxyServer();
            UDPProxyServerResultHandler testUDPProxyServerResultHandler= new UDPProxyServerResultHandlerCB();
            testServer.AttachPlugin(testUDPProxyServer);
            testUDPProxyServer.SetResultHandler(testUDPProxyServerResultHandler);
            testUDPProxyServer.LoginToCoordinator(rakStringTest,testServer.GetSystemAddressFromIndex(0));
            testUDPProxyServer.udpForwarder = testUDPProxyServer.udpForwarder;
            Console.WriteLine(testUDPProxyServer.udpForwarder);

            //RakString
            Console.WriteLine("RakString");
            testRakString.AppendBytes(testBuffer, (uint)testBuffer.Length);
            Console.WriteLine(testRakString.C_String());
            Console.WriteLine(testRakString.C_StringUnsafe());
            testRakString.Clear();
            Console.WriteLine(testRakString.ContainsNonprintableExceptSpaces());
            workingRakStringCopy = testRakString.Assign("MayJuneJuly", 0, 3);
            workingRakStringCopy.Dispose();
            Console.WriteLine(testRakString);
            testRakString.CopyData(testRakString);
            testRakString.Serialize(testBitStream2);
            testRakString.Deserialize(testBitStream2);
            testRakString.SerializeCompressed(testBitStream2);
            testRakString.DeserializeCompressed(testBitStream2);
            workingRakStringCopy = testRakString.Assign("MayJuneJuly", 0, 3);
            workingRakStringCopy.Dispose();

            testRakString.Erase(0, 1);

            Console.WriteLine(testRakString.Find("y", 0));
            Console.WriteLine(testRakString.GetLength());
            Console.WriteLine(testRakString.IPAddressMatch("127.0.0.1"));
            Console.WriteLine(testRakString.IsEmailAddress());
            Console.WriteLine(testRakString.IsEmpty());
            Console.WriteLine(testRakString.MakeFilePath());
            testRakString.Printf();
            testRakString.RemoveCharacter('a');
            testRakString.Replace(0, 1, 'c');
            testRakString.Set("cat");
            testRakString.SetChar(0, 'r');
            testRakString.SQLEscape();
            Console.WriteLine(testRakString.StrCmp(testRakString));
            Console.WriteLine(testRakString.StrICmp(testRakString));
            workingRakStringCopy = testRakString.SubStr(0, 2);
            Console.WriteLine(workingRakStringCopy);
            workingRakStringCopy.Dispose();
            Console.WriteLine(testRakString);
            testRakString.TerminateAtFirstCharacter('y');
            testRakString.TerminateAtLastCharacter('y');
            Console.WriteLine(testRakString.ToLower());
            Console.WriteLine(testRakString.ToUpper());
            testRakString.Truncate(2);
            testRakString.URLDecode();
            testRakString.URLEncode();

            //PluginInterface2
            testUDPProxyServer.GetRakPeerInterface();

            //RakPeerInterface
            testPacket = testClient.AllocatePacket(5);
            testClient.AddToBanList("127.0.0.1", 10);
            testClient.AddToSecurityExceptionList("127.0.0.1");
            testBuffer = testPacket.data;
            testClient.AdvertiseSystem("127.0.0.1",60002,testBuffer,testBuffer.Length);
            testClient.AllowConnectionResponseIPMigration(false);
            testClient.CancelConnectionAttempt(testClient.GetSystemAddressFromIndex(0));
            testClient.ChangeSystemAddress(testClient.GetGUIDFromIndex(0),testClient.GetSystemAddressFromIndex(0));
            testClient.ClearBanList();
            uint[] testFrequencyTable = new uint[256];
            testClient.DisableSecurity();
            testClient.GetAveragePing(new AddressOrGUID(RakNet.RakNet.UNASSIGNED_SYSTEM_ADDRESS));
            SystemAddress[] remoteSystems;
            ushort numberOfSystems=4;
            testClient.GetConnectionList(out remoteSystems,ref numberOfSystems);
            testClient.GetExternalID(RakNet.RakNet.UNASSIGNED_SYSTEM_ADDRESS);
            testClient.GetGUIDFromIndex(0);
            testClient.GetGuidFromSystemAddress(RakNet.RakNet.UNASSIGNED_SYSTEM_ADDRESS);
            int inOutInt=testBuffer.Length;
            testClient.GetIncomingPassword(testBuffer,ref inOutInt);
            testClient.GetIndexFromSystemAddress(testClient.GetSystemAddressFromIndex(0));
            testClient.GetInternalID();
            testClient.GetLastPing(new AddressOrGUID(RakNet.RakNet.UNASSIGNED_SYSTEM_ADDRESS));
            testClient.GetLocalIP(0);
            testClient.GetLowestPing(new AddressOrGUID(RakNet.RakNet.UNASSIGNED_SYSTEM_ADDRESS));
            testClient.GetMaximumIncomingConnections();
            testClient.GetMaximumNumberOfPeers();
            testClient.GetMTUSize(RakNet.RakNet.UNASSIGNED_SYSTEM_ADDRESS);
            testClient.GetNextSendReceipt();
            testClient.GetNumberOfAddresses();
            testClient.SetOfflinePingResponse(testBuffer, (uint)testBuffer.Length);
            testClient.GetOfflinePingResponse(testBuffer,out outUint);
            testClient.GetReceiveBufferSize();
            testClient.GetSplitMessageProgressInterval();
            testClient.GetStatistics(RakNet.RakNet.UNASSIGNED_SYSTEM_ADDRESS);
            testClient.GetSystemAddressFromGuid(testClient.GetGUIDFromIndex(0));
            testClient.GetSystemAddressFromIndex(0);
            RakNetListSystemAddress systemAddressList= new RakNetListSystemAddress();
            RakNetListRakNetGUID guidList= new RakNetListRakNetGUID();
            testClient.GetSystemList(systemAddressList,guidList);
            testClient.GetTimeoutTime(RakNet.RakNet.UNASSIGNED_SYSTEM_ADDRESS);
            testClient.IncrementNextSendReceipt();
            testClient.InitializeSecurity("dsf","sdaf");
            testClient.IsActive();
            testClient.IsBanned("127.0.0.1");
            testClient.GetConnectionState(new AddressOrGUID(testClient.GetSystemAddressFromIndex(0)));
            testClient.IsInSecurityExceptionList("127.0.0.1");
            testClient.IsLocalIP("127.0.0.1");
            testClient.NumberOfConnections();
            testClient.Ping(testClient.GetSystemAddressFromIndex(0));
            testClient.PushBackPacket(testPacket,true);
            testClient.RemoveFromBanList("127.0.0.1");
            testClient.RemoveFromSecurityExceptionList("127.0.0.1");
            testClient.SendLoopback(testBuffer,testBuffer.Length);
            testClient.SendTTL("127.0.0.1",60002,5);
            testClient.SetIncomingPassword(testBuffer,testBuffer.Length);
            testClient.SetLimitIPConnectionFrequency(true);
            testClient.SetMaximumIncomingConnections(99);
            testClient.SetOccasionalPing(true);
            testClient.SetPerConnectionOutgoingBandwidthLimit(10);
            testClient.SetSplitMessageProgressInterval(99);
            testClient.SetTimeoutTime(99,RakNet.RakNet.UNASSIGNED_SYSTEM_ADDRESS);
            testClient.SetUnreliableTimeout(99);
            testClient.WriteOutOfBandHeader(testBitStream1);
            testClient.CloseConnection(testClient.GetSystemAddressFromIndex(0),false);
            testClient.Shutdown(20);

            RakPeer.DestroyInstance(testClient);
            RakPeer.DestroyInstance(testServer);

            //These need to be freed if used in the main function.
            testUDPForwarder.Dispose();
            testUDPProxyCoordinator.Dispose();
            FileList.DestroyInstance(testFileList);
            FileList.DestroyInstance(outFileList);
            FullyConnectedMesh2.DestroyInstance(testFullyConnectedMesh2);
            //If RakString is not freed before program exit it will crash
            rakStringTest.Dispose();
            testRakString.Dispose();
            workingRakStringCopy.Dispose();
            RakString.FreeMemory();

            Console.WriteLine("Demo complete. Press enter.");
            Console.Read();
        }
    }
}
