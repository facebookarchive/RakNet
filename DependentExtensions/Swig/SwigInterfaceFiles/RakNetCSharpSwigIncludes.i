//------------------------------Header includes for parsing by swig----------------------------
//This is included in a different section than the C++ version of these includes so a macro was not made to include
//it in both types
//These files are parsed by Swig
%include "RakNetDefines.h"
%include "Export.h"
%include "NativeFeatureIncludes.h"
//%include "RakNetSmartPtr.h"
%include "MessageIdentifiers.h"
%include "SimpleMutex.h"
%include "NativeTypes.h"
%include "SocketIncludes.h"
%include "RakNetTypes.h"
%include "RakString.h"
%include "RakWString.h"
%include "BitStream.h"
%include "DS_List.h"
%include "DS_ByteQueue.h"
%include "RakNetSocket.h"
%include "RakNetStatistics.h"
%include "NetworkIDObject.h"
%include "NetworkIDManager.h"
%include "RakNetTime.h"	
%include "PacketPriority.h"
//The below three classes have been removed from interface, if PluginInterface2 is fully exposed again
//or another class needs them uncomment them and the related typemaps
//%include "TCPInterface.h"
//%include "PacketizedTCP.h"
//%include "InternalPacket.h"
%include "PluginInterface2.h"
%include "RakPeerInterface.h"
%include "RakPeer.h"
%include "PacketLogger.h"
%include "PacketFileLogger.h"
%include "NatTypeDetectionClient.h"
%include "NatPunchthroughClient.h"
%include "Router2.h"
%include "UDPProxyClient.h"
%include "FullyConnectedMesh2.h"
%include "ReadyEvent.h"
//%include "TeamBalancer.h"
%include "TeamManager.h"
%include "NatPunchthroughServer.h"
%include "UDPForwarder.h"
%include "UDPProxyServer.h"
%include "UDPProxyCoordinator.h"
%include "NatTypeDetectionServer.h"
%include "DS_Table.h"
%include "DS_BPlusTree.h"
%include "FileListTransferCBInterface.h"
%include "IncrementalReadInterface.h"
%include "FileListNodeContext.h"
%include "FileList.h"
%include "TransportInterface.h"
%include "CommandParserInterface.h"
%include "LogCommandParser.h"
%include "MessageFilter.h"
%include "DirectoryDeltaTransfer.h"
%include "FileListTransfer.h"
%include "ThreadsafePacketLogger.h"
%include "PacketConsoleLogger.h"
%include "PacketFileLogger.h"
%include "DS_Multilist.h"
%include "ConnectionGraph2.h"
%include "GetTime.h"
//%include "RakNetTransport2.h"
//%include "RoomsPlugin.h"\

#ifdef SWIG_ADDITIONAL_SQL_LITE
	%include "SQLite3PluginCommon.h"
	%include "SQLite3ClientPlugin.h"
	#ifdef SWIG_ADDITIONAL_SQL_LITE_SERVER
		%include "SQLite3ServerPlugin.h"
	#endif
	#ifdef SWIGWIN
		%include "Logger\SQLiteLoggerCommon.h"
		%include "Logger\ClientOnly\SQLiteClientLoggerPlugin.h"
		#ifdef SWIG_ADDITIONAL_SQL_LITE_SERVER
			%include "Logger\ServerOnly\SQLiteServerLoggerPlugin.h"
		#endif
	#else
		%include "Logger/SQLiteLoggerCommon.h"
		%include "Logger/ClientOnly/SQLiteClientLoggerPlugin.h"
		#ifdef SWIG_ADDITIONAL_SQL_LITE_SERVER
			%include "Logger/ServerOnly/SQLiteServerLoggerPlugin.h"
		#endif
	#endif
#endif

#ifdef SWIG_ADDITIONAL_AUTOPATCHER
		%include "AutopatcherPatchContext.h"
		%include "AutopatcherRepositoryInterface.h"
	#ifdef SWIGWIN
		#ifdef SWIG_ADDITIONAL_AUTOPATCHER_MYSQL 
			%include "MySQLInterface\MySQLInterface.h"
			%include "Autopatcher\AutopatcherMySQLRepository\AutopatcherMySQLRepository.h"
		#endif
		%include "Autopatcher\AutopatcherServer.h"
		%include "Autopatcher\AutopatcherClient.h"
		%include "Autopatcher\CreatePatch.h"
		%include "Autopatcher\MemoryCompressor.h"
		%include "Autopatcher\ApplyPatch.h"
	#else
		#ifdef SWIG_ADDITIONAL_AUTOPATCHER_MYSQL 
			%include "MySQLInterface/MySQLInterface.h"
			%include "Autopatcher/AutopatcherMySQLRepository/AutopatcherMySQLRepository.h"
		#endif
		%include "Autopatcher/AutopatcherServer.h"
		%include "Autopatcher/AutopatcherClient.h"
		%include "Autopatcher/CreatePatch.h"
		%include "Autopatcher/MemoryCompressor.h"
		%include "Autopatcher/ApplyPatch.h"
	#endif
#endif