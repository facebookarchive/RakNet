//This is not parsed by swig but inserted into the generated C++ wrapper, these includes
//are needed so the wrapper includes the needed .h filese
//This also includes the typemaps used.
 %{
 /* Includes the header in the wrapper code */
//Defines
#ifdef SWIGWIN
#define _MSC_VER 10000
#define WIN32
#define _WIN32
#define _DEBUG
#define _RAKNET_DLL
#endif SWIGWIN
//TypeDefs
typedef int int32_t;
typedef unsigned int uint32_t;
typedef uint32_t DefaultIndexType;
#ifdef SWIGWIN
typedef unsigned int SOCKET;
#endif
//Includes
#include "RakNetSmartPtr.h"
#include "RakNetDefines.h"
#include "MessageIdentifiers.h"
#include "Export.h"
#include "SimpleMutex.h"
#include "RakString.h"
#include "RakWString.h"
#include "BitStream.h"
#include "DS_List.h"	
#include "DS_ByteQueue.h"
#include "RakAssert.h"
#include "NativeTypes.h"
#include "SocketIncludes.h"
#include "RakNetTime.h"
#include "Export.h"
#include "RakMemoryOverride.h"
#include "RakNetTypes.h"
#include "RakNetSocket.h"
#include "RakNetStatistics.h"
#include "NetworkIDObject.h"
#include "NetworkIDManager.h"
#include "RakNetTime.h"	
//The below three classes have been removed from interface, if PluginInterface2 is fully exposed again
//or another class needs them uncomment them and the related typemaps
//#include "TCPInterface.h"
//#include "PacketizedTCP.h"
//#include "InternalPacket.h"
#include "PluginInterface2.h"
#include "RakPeerInterface.h"
#include "RakPeer.h"
#include "PacketPriority.h"
#include "PacketLogger.h"
#include "PacketFileLogger.h"
#include "NatTypeDetectionClient.h"
#include "NatPunchthroughClient.h"
#include "Router2.h"
#include "UDPProxyClient.h"
#include "FullyConnectedMesh2.h"
#include "ReadyEvent.h"
//#include "TeamBalancer.h"
#include "TeamManager.h"
#include "NatPunchthroughServer.h"
#include "UDPForwarder.h"
#include "UDPProxyServer.h"
#include "UDPProxyCoordinator.h"
#include "NatTypeDetectionServer.h"
#include "DS_BPlusTree.h"
#include "DS_Table.h"
#include "FileListTransferCBInterface.h"//
#include "IncrementalReadInterface.h"//
#include "FileListNodeContext.h"//
#include "FileList.h"//
#include "TransportInterface.h"//
#include "CommandParserInterface.h"//
#include "LogCommandParser.h"//
#include "MessageFilter.h"//
#include "DirectoryDeltaTransfer.h"//
#include "FileListTransfer.h"//
#include "ThreadsafePacketLogger.h"//
#include "PacketConsoleLogger.h"//
#include "PacketFileLogger.h"//
#include "DS_Multilist.h"
#include "ConnectionGraph2.h"
#include "GetTime.h"
//#include "RakNetTransport2.h"
//#include "RoomsPlugin.h"
//Macros
//Swig C++ code only TypeDefs
//Most of these are nested structs/classes that swig needs to understand as global
//They will reference the nested struct/class while appearing global
typedef RakNet::RakString::SharedString SharedString;
typedef DataStructures::Table::Row Row;
typedef DataStructures::Table::Cell Cell; 
typedef DataStructures::Table::FilterQuery FilterQuery;
typedef DataStructures::Table::ColumnDescriptor ColumnDescriptor;
typedef DataStructures::Table::SortQuery SortQuery;
typedef RakNet::FileListTransferCBInterface::OnFileStruct OnFileStruct;
typedef RakNet::FileListTransferCBInterface::FileProgressStruct FileProgressStruct;
typedef RakNet::FileListTransferCBInterface::DownloadCompleteStruct DownloadCompleteStruct;

 %}

#ifdef SWIG_ADDITIONAL_SQL_LITE
	%{
	#include "SQLite3PluginCommon.h"
	#include "SQLite3ClientPlugin.h"
	#include "SQLiteLoggerCommon.h"
	#include "SQLiteClientLoggerPlugin.h"
	#ifdef SWIG_ADDITIONAL_SQL_LITE_SERVER
		#include "SQLite3ServerPlugin.h"
		#include "SQLiteServerLoggerPlugin.h"
	#endif
	typedef RakNet::LogParameter::DataUnion DataUnion;
	typedef RakNet::SQLiteClientLoggerPlugin::ParameterListHelper ParameterListHelper;
	%}
#endif

#ifdef SWIG_ADDITIONAL_AUTOPATCHER
	%{
	#include "AutopatcherRepositoryInterface.h"
	#include "AutopatcherServer.h"
	#include "AutopatcherClient.h"
	#include "AutopatcherMySQLRepository.h"
	#include "CreatePatch.h"
	#include "MemoryCompressor.h"
	#include "ApplyPatch.h"
	#include "AutopatcherPatchContext.h"
	%}
#endif

%{
using namespace RakNet;
%}

