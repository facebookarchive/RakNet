//----------Ignores----------------
//This file specifies things that should be ignored by Swig
//Typeically this is used for the following reasons: a C# replacement was made for the function, it is not supported by Swig
//,it should not be exposed, or the item is internal and it would take time to convert

//BitStream
%ignore RakNet::BitStream::Write(unsigned char * const inTemplateVar);
%ignore RakNet::BitStream::WriteCompressed(unsigned char * const inTemplateVar);
%ignore RakNet::BitStream::Write(const unsigned char * const inTemplateVar);
%ignore RakNet::BitStream::WriteCompressed(const unsigned char * const inTemplateVar);
%ignore RakNet::BitStream::Write( const char* inputByteArray, const unsigned int numberOfBytes );
%ignore RakNet::BitStream::Read(char *varString);
%ignore RakNet::BitStream::Read(unsigned char *varString);
%ignore RakNet::BitStream::Read( char* output, const unsigned int numberOfBytes );
%ignore RakNet::BitStream::ReadCompressedDelta(char &outTemplateVar);
%ignore RakNet::BitStream::ReadDelta(char &outTemplateVar);
%ignore RakNet::BitStream::ReadCompressed(char &outTemplateVar);
%ignore RakNet::BitStream::ReadCompressedDelta(char* &outTemplateVar);
%ignore RakNet::BitStream::ReadDelta(char* &outTemplateVar);
%ignore RakNet::BitStream::ReadCompressed(char* &outTemplateVar);
%ignore RakNet::BitStream::CopyData(unsigned char** _data ) const;
%ignore RakNet::BitStream::PrintBits(char *out) const;
%ignore RakNet::BitStream::PrintHex(char *out) const;
%ignore RakNet::BitStream::GetData;
%ignore RakNet::BitStream::Serialize(bool writeToBitstream,  char* inputByteArray, const unsigned int numberOfBytes );
%ignore RakNet::BitStream::SerializeDelta(bool writeToBitstream,  char* inputByteArray, const unsigned int numberOfBytes );
%ignore RakNet::BitStream::SerializeCompressed(bool writeToBitstream,  char* inputByteArray, const unsigned int numberOfBytes );
%ignore RakNet::BitStream::SerializeCompressedDelta(bool writeToBitstream, char* inputByteArray, const unsigned int numberOfBytes );
%ignore RakNet::BitStream::ReadAlignedBytesSafe( char *inOutByteArray, int &inputLength, const int maxBytesToRead );
%ignore RakNet::BitStream::ReadAlignedBytesSafe( char *inOutByteArray, unsigned int &inputLength, const unsigned int maxBytesToRead );
%ignore RakNet::BitStream::ReadAlignedBytesSafeAlloc( char **outByteArray, int &inputLength, const int maxBytesToRead );
%ignore RakNet::BitStream::ReadAlignedBytesSafeAlloc( char **outByteArray, unsigned int &inputLength, const unsigned int maxBytesToRead );
%ignore RakNet::BitStream::WriteAlignedVar8(const char *inByteArray);
%ignore RakNet::BitStream::WriteAlignedVar8(const char *inByteArray);
%ignore RakNet::BitStream::ReadAlignedVar8(char *inOutByteArray);		
%ignore RakNet::BitStream::WriteAlignedVar16(const char *inByteArray);		
%ignore RakNet::BitStream::ReadAlignedVar16(char *inOutByteArray);	
%ignore RakNet::BitStream::WriteAlignedVar32(const char *inByteArray);	
%ignore RakNet::BitStream::ReadAlignedVar32(char *inOutByteArray);
%ignore RakNet::BitStream::WriteAlignedBytesSafe( const char *inByteArray, const unsigned int inputLength, const unsigned int maxBytesToWrite );
%ignore RakNet::BitStream::Read( BitStream &bitStream, BitSize_t numberOfBits );
%ignore RakNet::BitStream::Read( BitStream &bitStream );
%ignore RakNet::BitStream::Write( BitStream &bitStream, BitSize_t numberOfBits );
%ignore RakNet::BitStream::Write( BitStream &bitStream );
%ignore RakNet::BitStream::ReadAlignedBytesSafeAlloc( char ** outByteArray, unsigned int &inputLength, const unsigned int maxBytesToRead );
%ignore RakNet::BitStream::ReadAlignedBytesSafeAlloc( char **outByteArray, int &inputLength, const unsigned int maxBytesToRead );

//RakPeer
%define IGNORERAKPEERANDINTERFACE(theMacroInputFunction)
%ignore RakNet::RakPeer::theMacroInputFunction;
%ignore RakNet::RakPeerInterface::theMacroInputFunction;
%enddef

IGNORERAKPEERANDINTERFACE(GetIncomingPassword( char* passwordData, int *passwordDataLength  ))
IGNORERAKPEERANDINTERFACE(GetOfflinePingResponse( char **data, unsigned int *length ))
IGNORERAKPEERANDINTERFACE(RegisterAsRemoteProcedureCall( const char* uniqueID, void ( *functionPointer ) ( RPCParameters *rpcParms ) ))
IGNORERAKPEERANDINTERFACE(RegisterClassMemberRPC( const char* uniqueID, void *functionPointer ))
IGNORERAKPEERANDINTERFACE(UnregisterAsRemoteProcedureCall( const char* uniqueID ))
IGNORERAKPEERANDINTERFACE(RPC( const char* uniqueID, const char *data, BitSize_t bitLength, PacketPriority priority, PacketReliability reliability, char orderingChannel, const AddressOrGUID systemIdentifier, bool broadcast, RakNetTime *includedTimestamp, NetworkID networkID, RakNet::BitStream *replyFromTarget ))
IGNORERAKPEERANDINTERFACE(RPC( const char* uniqueID, const RakNet::BitStream *bitStream, PacketPriority priority, PacketReliability reliability, char orderingChannel, const AddressOrGUID systemIdentifier, bool broadcast, RakNetTime *includedTimestamp, NetworkID networkID, RakNet::BitStream *replyFromTarget ))
IGNORERAKPEERANDINTERFACE(GetRPCString( const char *data, const BitSize_t bitSize, const SystemAddress systemAddress))
IGNORERAKPEERANDINTERFACE(SendOutOfBand(const char *host, unsigned short remotePort, MessageID header, const char *data, BitSize_t dataLength, unsigned connectionSocketIndex=0 ))
IGNORERAKPEERANDINTERFACE(SetUserUpdateThread(void (*_userUpdateThreadPtr)(RakPeerInterface *, void *), void *_userUpdateThreadData))
IGNORERAKPEERANDINTERFACE(SendList)
IGNORERAKPEERANDINTERFACE(ReleaseSockets)
IGNORERAKPEERANDINTERFACE(GetSocket( const SystemAddress target ))
IGNORERAKPEERANDINTERFACE(GetSockets( DataStructures::List<RakNetSmartPtr<RakNetSocket> > &sockets ))
IGNORERAKPEERANDINTERFACE(ConnectWithSocket)
IGNORERAKPEERANDINTERFACE(SetRouterInterface)
IGNORERAKPEERANDINTERFACE(RemoveRouterInterface)
IGNORERAKPEERANDINTERFACE(GetConnectionList( SystemAddress *remoteSystems, unsigned short *numberOfSystems ) const)
IGNORERAKPEERANDINTERFACE(SetIncomingDatagramEventHandler( bool (*_incomingDatagramEventHandler)(RNS2RecvStruct *) ))

//RakPeer only
//Swig doesn't know how to handle friend functions, so even if it is in the protected section 
//They must be explicitly ignored
//This Specific case is somehow placed in the RakNet namespace rather than RakNet::RakPeer
//Ignore both to be safe
%ignore RakNet::RakPeerInterface::GetStatisticsList;
%ignore RakNet::RakPeer::GetStatisticsList;
%ignore RakNet::RakPeer::ProcessOfflineNetworkPacket;
%ignore RakNet::RakPeer::ProcessNetworkPacket;
%ignore RakNet::ProcessOfflineNetworkPacket;
%ignore RakNet::ProcessNetworkPacket;

//RakString
%ignore AppendBytes(const char *bytes, unsigned int count);//Interface remade
%ignore RakNet::RakString::FPrintf(FILE *fp); //Expects C file pointer
//RakString Internal
%ignore RakNet::RakString::RakString( SharedString *_sharedString );
%ignore RakNet::RakString::sharedString;
%ignore RakNet::RakString::RakString(const unsigned char *format, ...);
%ignore RakNet::RakString::freeList;
%ignore RakNet::RakString::emptyString;

//List
%ignore DataStructures::List::operator[];

//SystemAddress
%ignore RakNet::SystemAddress::ToString(bool writePort, char *dest) const;
%ignore RakNet::SystemAddress::ToString() const;

//RakNetGUID
%ignore RakNet::RakNetGUID::ToString(char *dest) const;

//AddressOrGUID
%ignore RakNet::AddressOrGUID::ToString(bool writePort, char *dest) const;
%ignore RakNet::AddressOrGUID::ToString() const;

//PacketizedTCP
%ignore RakNet::PacketizedTCP::SendList;
%ignore RakNet::TCPInterface::SendList;

//InternalPacket
%ignore RakNet::InternalPacket::refCountedData;
%ignore RakNet::InternalPacketRefCountedData;

//RemoteClient
%ignore RakNet::RemoteClient::SendOrBuffer;

//NetworkIDManager
%ignore RakNet::NetworkIDManager::GET_OBJECT_FROM_ID;
%ignore RakNet::NetworkIDManager::TrackNetworkIDObject;
%ignore RakNet::NetworkIDManager::StopTrackingNetworkIDObject;

//NetworkIDObject
%ignore RakNet::NetworkIDObject::SetParent;
%ignore RakNet::NetworkIDObject::GetParent;

//RakNetSocket
%ignore RakNetSocket::recvEvent;
%ignore RakNetSocket::Fcntl;

//To allow easier future support if needed, rather than not parsing the PluginInterface2 ignore the functions
//Later if decided that is needed remove the ignores and the commented typemaps and includes
%ignore RakNet::PluginInterface2::OnAttach;
%ignore RakNet::PluginInterface2::OnDetach;
%ignore RakNet::PluginInterface2::Update;
%ignore RakNet::PluginInterface2::OnReceive;
%ignore RakNet::PluginInterface2::OnStartup;
%ignore RakNet::PluginInterface2::OnShutdown;
%ignore RakNet::PluginInterface2::OnClosedConnection;
%ignore RakNet::PluginInterface2::OnNewConnection;
%ignore RakNet::PluginInterface2::OnFailedConnectionAttempt;
%ignore RakNet::PluginInterface2::OnDirectSocketSend;
%ignore RakNet::PluginInterface2::OnInternalPacket;
%ignore RakNet::PluginInterface2::OnAck;
%ignore RakNet::PluginInterface2::OnPushBackPacket;
%ignore RakNet::PluginInterface2::OnDirectSocketReceive;
%ignore RakNet::PluginInterface2::OnRakPeerShutdown;
%ignore RakNet::PluginInterface2::OnRakPeerStartup;
%ignore RakNet::PluginInterface2::OnReliabilityLayerPacketError;

//NatPunchthroughClient
	/// \internal For plugin handling
%ignore  RakNet::NatPunchthroughClient::Update(void);
	/// \internal For plugin handling
%ignore RakNet::NatPunchthroughClient::OnReceive(Packet *packet);
	/// \internal For plugin handling
%ignore RakNet::NatPunchthroughClient::OnNewConnection(SystemAddress systemAddress, RakNetGUID rakNetGUID, bool isIncoming);
	/// \internal For plugin handling
%ignore RakNet::NatPunchthroughClient::OnClosedConnection(SystemAddress systemAddress, RakNetGUID rakNetGUID, PI2_LostConnectionReason lostConnectionReason );
%ignore RakNet::NatPunchthroughClient::OnAttach();
%ignore RakNet::NatPunchthroughClient::OnDetach();
%ignore RakNet::NatPunchthroughClient::OnRakPeerShutdown();

//Uin24_t
%ignore RakNet::uint24_t::operator++(int);
%ignore RakNet::uint24_t::operator--(int);

//ReadyEvent
%ignore RakNet::ReadyEvent::RemoteSystemCompBySystemAddress;
%ignore RakNet::ReadyEvent::RemoteSystem;
%ignore RakNet::ReadyEvent::ReadyEventNodeComp;
%ignore RakNet::ReadyEvent::ReadyEventNode;

//PacketLoggger
%ignore RakNet::PacketLogger::FormatLine;

//FullyConnectedMesh2
%ignore RakNet::FullyConnectedMesh2::GetElapsedRuntime;
%ignore RakNet::FullyConnectedMesh2::OnReceive;

//Structs
%ignore RakNet::PI2_FailedConnectionAttemptReason;
%ignore RakNet::PI2_LostConnectionReason;
//These are internal
%ignore RakNet::PluginInterface2::SetRakPeerInterface;
%ignore RakNet::PluginInterface2::SetPacketizedTCP;

//ByteQueue
%ignore DataStructures::ByteQueue::WriteBytes(const char *in, unsigned length, const char *file, unsigned int line);
%ignore DataStructures::ByteQueue::ReadBytes(char *out, unsigned maxLengthToRead, bool peek);
%ignore DataStructures::ByteQueue::PeekContiguousBytes(unsigned int *outLength) const;

//UDPProxyClient
%ignore RakNet::UDPProxyClient::pingServerGroups;
%ignore RakNet::UDPProxyClient::OnReceive;

//NatPunchThroughServer
%ignore RakNet::NatPunchthroughServer::NatPunchthroughUserComp;

//UDPForwarder
%ignore RakNet::UDPForwarder::threadOperationIncomingQueue;
%ignore RakNet::UDPForwarder::threadOperationOutgoingQueue;
%ignore RakNet::UDPForwarder::forwardList;
%ignore AddForwardingEntry(SrcAndDest srcAndDest, RakNet::TimeMS timeoutOnNoDataMS, unsigned short *port, const char *forceHostAddress);

//MessageFilter
%ignore RakNet::FilterSet;//internal
%ignore RakNet::MessageFilterStrComp;//internal
%ignore RakNet::FilterSetComp;//internal
%ignore RakNet::FilteredSystem;//internal
%ignore RakNet::FilteredSystemComp;//internal
%ignore RakNet::MessageFilter::SetDisallowedMessageCallback(int filterSetID, void *userData, void (*invalidMessageCallback)(RakPeerInterface *peer, AddressOrGUID addressOrGUID, int filterSetID, void *userData, unsigned char messageID)); //Void pointer
%ignore RakNet::MessageFilter::SetTimeoutCallback(int filterSetID, void *userData, void (*invalidMessageCallback)(RakPeerInterface *peer, AddressOrGUID addressOrGUID, int filterSetID, void *userData)); //Void pointer

//Table
//%ignore DataStructures::Table::GetRows;
%ignore DataStructures::Table::GetListHead;
%ignore DataStructures::Table::SortTable(Table::SortQuery *sortQueries, unsigned numSortQueries, Table::Row** out);//Completely C# replacement using a helper function
%ignore DataStructures::Table::GetCellValueByIndex(unsigned rowIndex, unsigned columnIndex, char *output);
%ignore DataStructures::Table::GetCellValueByIndex(unsigned rowIndex, unsigned columnIndex, char *output, int *outputLength);
%ignore DataStructures::Table::PrintColumnHeaders(char *out, int outLength, char columnDelineator) const;
%ignore DataStructures::Table::PrintRow(char *out, int outLength, char columnDelineator, bool printDelineatorForBinary, Table::Row* inputRow) const;
%ignore DataStructures::Table::QueryTable(unsigned *columnIndicesSubset, unsigned numColumnSubset, FilterQuery *inclusionFilters, unsigned numInclusionFilters, unsigned *rowIds, unsigned numRowIDs, Table *result);
%ignore UpdateCell(unsigned rowId, unsigned columnIndex, int byteLength, char *data);
%ignore UpdateCellByIndex(unsigned rowIndex, unsigned columnIndex, int byteLength, char *data);
%ignore Cell::ptr;
%ignore Cell::Cell(double numericValue, char *charValue, void *ptr, DataStructures::Table::ColumnType type);
%ignore Cell::SetByType(double numericValue, char *charValue, void *ptr, DataStructures::Table::ColumnType type);
%ignore Cell::SetPtr;
%ignore Row::UpdateCell(unsigned columnIndex, int byteLength, const char *data);
%ignore Cell::Get(char *output, int *outputLength);
%ignore Cell::Set(const char *input, int inputLength);
%ignore Cell::Get(char *output);
%ignore Cell::c; //Not really useful externally
%ignore ColumnIndex;// Swig will not ignore the definition I wish to, so I ignore both and make helper functions
 		
//Table Lists
%ignore DataStructures::List <ColumnDescriptor>::GetIndexOf;
%ignore DataStructures::List <Row>::GetIndexOf;
%ignore DataStructures::List <Cell>::GetIndexOf;
%ignore DataStructures::List <FilterQuery>::GetIndexOf;
%ignore DataStructures::List <SortQuery>::GetIndexOf;
 
//BPlusTree
//The next two use C function pointers
%ignore DataStructures::BPlusTree::ForEachData;
%ignore DataStructures::BPlusTree::ForEachLeaf;
%ignore DataStructures::Page::keys;
%ignore DataStructures::Page::children;
%ignore DataStructures::Page::data;
 
//FileList Lists
%ignore DataStructures::List<RakNet::FileListNode>::GetIndexOf;
%ignore RakNet::FileList::GetCallbacks(DataStructures::List<FileListProgress*> &callbacks);

// FileListTransfer
%ignore RakNet::FileListTransfer::GetCallbacks(DataStructures::List<FileListProgress*> &callbacks);
%ignore SendIRIToAddressCB(FileListTransfer::ThreadData threadData, bool *returnOutput, void* perThreadData);

//Bplus
%ignore DataStructures::BPlusTree::ValidateTree;

//File
%ignore RakNet::IncrementalReadInterface::GetFilePart( const char *filename, unsigned int startReadBytes, unsigned int numBytesToRead, void *preallocatedDestination, FileListNodeContext context);

//FileList
%ignore AddFile(const char *filename, const char *fullPathToFile, const char *data, const unsigned dataLength, const unsigned fileLength, FileListNodeContext context, bool isAReference=false, bool takeDataPointer=false);

//CommandParserInterface
//Internal
%ignore RegisteredCommand;
%ignore RegisteredCommandComp( const char* const & key, const RegisteredCommand &data );
%ignore GetRegisteredCommand(const char *command, RegisteredCommand *rc);
%ignore ParseConsoleString(char *str, const char delineator, unsigned char delineatorToggle, unsigned *numParameters, char **parameterList, unsigned parameterListLength);
%ignore SendCommandList(TransportInterface *transport, SystemAddress systemAddress);

//TransportInterface
%ignore RakNet::TransportInterface::Send( SystemAddress systemAddress, const char *data, ... );

//Router2
%ignore OnReceive(Packet *packet);

//MultiList
%ignore DataStructures::Multilist::ForEach;

//ConnectionGraph2
%ignore RakNet::ConnectionGraph2::SystemAddressAndGuid;
%ignore RakNet::ConnectionGraph2::SystemAddressAndGuidComp;
%ignore RakNet::ConnectionGraph2::RemoteSystem;
%ignore RakNet::ConnectionGraph2::RemoteSystemComp;
%ignore RakNet::ConnectionGraph2::GetConnectionList;
%ignore RakNet::ConnectionGraph2::GetParticipantList;

#ifdef SWIG_ADDITIONAL_SQL_LITE
//LogParameter
%ignore RakNet::LogParameter::LogParameter(void *t);
%ignore RakNet::LogParameter::LogParameter(void *t);
%ignore RakNet::LogParameter::LogParameter(const unsigned char t[]);
%ignore RakNet::LogParameter::LogParameter(const char t[]);
%ignore RakNet::LogParameter::LogParameter(BlobDescriptor t);
%ignore RakNet::LogParameter::LogParameter(RGBImageBlob t);

//SQLite3Row List and SQLLite3Table, not pointers so these functions not needed and will error
%ignore DataStructures::Multilist<ML_STACK, RakNet::RakString,RakNet::RakString,DefaultIndexType>::GetPtr;
%ignore DataStructures::Multilist<ML_STACK, RakNet::RakString,RakNet::RakString,DefaultIndexType>::ClearPointers;
%ignore DataStructures::Multilist<ML_STACK, RakNet::RakString,RakNet::RakString,DefaultIndexType>::ClearPointer;

//common
%ignore LogParameter;
%ignore BlobDescriptor;
%ignore RGBImageBlob;

//SQLiteClientLogger
%ignore RakNet::SQLiteClientLoggerPlugin::CheckQuery;
%ignore RakNet::SQLiteClientLoggerPlugin::ParameterListHelper;
%ignore RakNet::SQLiteClientLoggerPlugin::SqlLog;
%ignore RakNet::SQLiteClientLoggerPlugin::__sqlLogInternal;
%ignore RakNet::SQLiteClientLoggerPlugin::logger;
#endif

//Global
%ignore REGISTER_STATIC_RPC;
%ignore CLASS_MEMBER_ID;
%ignore REGISTER_CLASS_MEMBER_RPC;
%ignore UNREGISTER_STATIC_RPC;
%ignore UNREGISTER_CLASS_MEMBER_RPC;

//Operators
//These need te be handled manually or not at all
%ignore operator const char*;
%ignore operator uint32_t;
%ignore operator &; //Not overloadable in C#
%ignore operator <<;//Doesn't work the same in C#, only usable with int
%ignore operator >>;//Doesn't work the same in C#, only usable with int

//X= is automatically handled in C# if you overload = and X, you can't specify an overload
%ignore operator +=;
%ignore operator -=;
%ignore operator /=;

//RakString
%ignore RakNet::RakString::operator = (char *);
%ignore RakNet::RakString::operator == (char *) const;
%ignore RakNet::RakString::operator != (char *) const;

//Structs
%ignore RPCParameters;

//Global
%ignore StatisticsToString; //Custom C# wrapper written for it

#ifdef SWIG_ADDITIONAL_AUTOPATCHER
	%ignore CreatePatch; //Custom C# wrapper written for it
	%ignore RakNet::AutopatcherServer::StartThreads;
	%ignore RakNet::AutopatcherClient::OnThreadCompletion;
	%ignore RakNet::MemoryCompressor::Compress(char *input, const unsigned inputLength, bool finish);
	%ignore RakNet::MemoryDecompressor::MemoryDecompress(unsigned char *inputByteArray, const unsigned inputLength, bool ignoreStreamEnd);
	%ignore CompressorBase::GetOutput;
#endif