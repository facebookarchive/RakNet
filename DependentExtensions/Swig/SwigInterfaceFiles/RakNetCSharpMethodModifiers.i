//----------------------------Method modifiers---------------------
//These modify the method types by default it uses just publix x, here you can specify ovverideds private functions
//and such. Many times this is used to hide the helper functions from the user

//BitStream
%csmethodmodifiers RakNet::BitStream::CSharpStringReader "private"
%csmethodmodifiers RakNet::BitStream::CSharpStringReaderCompressedDelta "private"
%csmethodmodifiers RakNet::BitStream::CSharpStringReaderCompressed "private"
%csmethodmodifiers RakNet::BitStream::CSharpStringReaderDelta "private"
%csmethodmodifiers RakNet::BitStream::CSharpByteReader(unsigned char* inOutByteArray,unsigned int numberOfBytes) "private"
%csmethodmodifiers RakNet::BitStream::CSharpCopyDataHelper(unsigned char* inOutByteArray) "private"
%csmethodmodifiers RakNet::BitStream::CSharpPrintHexHelper(char * inString) "private"
%csmethodmodifiers RakNet::BitStream::CSharpPrintBitsHelper(char * inString) "private"

//DataStructures::Table
%csmethodmodifiers DataStructures::Table::GetListHeadHelper "private"
%csmethodmodifiers DataStructures::Table::SortTableHelper "private"
%csmethodmodifiers DataStructures::Table::GetCellValueByIndexHelper "private"
%csmethodmodifiers DataStructures::Table::QueryTableHelper "private"
%csmethodmodifiers DataStructures::Table::ColumnIndexHelper "private"
%csmethodmodifiers Cell::GetHelper "private"
%csmethodmodifiers Cell::ColumnIndexHelper "private"

//Rakpeer
%define RAKPEERANDINTERFACESETPRIVATE(theMacroInputFunction)
%csmethodmodifiers RakNet::RakPeer::theMacroInputFunction "private"
%csmethodmodifiers RakNet::RakPeerInterface::theMacroInputFunction "private"
%enddef

RAKPEERANDINTERFACESETPRIVATE(CSharpGetIncomingPasswordHelper( const char* passwordData, int *passwordDataLength  ))
RAKPEERANDINTERFACESETPRIVATE(CSharpGetOfflinePingResponseHelper( unsigned char *inOutByteArray, unsigned int *outLength ))
RAKPEERANDINTERFACESETPRIVATE(GetBandwidth);

%csmethodmodifiers RakNet::NetworkIDManager::GET_BASE_OBJECT_FROM_ID "protected"

%csmethodmodifiers RakNet::NetworkIDObject::SetNetworkIDManager "protected"

%csmethodmodifiers  DataStructures::ByteQueue::PeekContiguousBytesHelper "private"

%csmethodmodifiers RakNet::RakNetGUID::ToString() const "public override"

%csmethodmodifiers  RakNet::StatisticsToStringHelper "private"

%csmethodmodifiers RakNet::PacketLogger::FormatLineHelper "private"

%csmethodmodifiers DataStructures::List <unsigned short>::GetHelper "private"
%csmethodmodifiers DataStructures::List <unsigned short>::PopHelper "private"

//FileProgressStruct
%csmethodmodifiers FileProgressStruct::SetFirstDataChunk "private"
%csmethodmodifiers FileProgressStruct::SetIriDataChunk "private"
%csmethodmodifiers OnFileStruct::SetFileData "private"

//ConnectionGraph2
%csmethodmodifiers RakNet::ConnectionGraph2::GetConnectionListForRemoteSystemHelper "private"
%csmethodmodifiers RakNet::ConnectionGraph2::GetParticipantListHelper "private"

#ifdef SWIG_ADDITIONAL_AUTOPATCHER
	//AutopatcherServer
	%csmethodmodifiers RakNet::AutopatcherServer::StartThreadsHelper "private"

	%csmethodmodifiers  RakNet::CreatePatchHelper "private"
#endif

#ifdef SWIG_ADDITIONAL_AUTOPATCHER
	%csmethodmodifiers CompressorBase::GetOutputHelper "private";
#endif

//Operators
%csmethodmodifiers operator > "private"
%csmethodmodifiers operator < "private"
%csmethodmodifiers operator != "private"
%csmethodmodifiers operator [] "private"
%csmethodmodifiers operator >= "private"
%csmethodmodifiers operator <= "private"
%csmethodmodifiers operator / "private"
%csmethodmodifiers operator * "private"
%csmethodmodifiers operator -- "private"
%csmethodmodifiers operator ++ "private"
%csmethodmodifiers operator - "private"
%csmethodmodifiers operator + "private"
%csmethodmodifiers operator+(const RakNet::RakString &lhs, const RakNet::RakString &rhs) "public" //The global RakNet operator should be public