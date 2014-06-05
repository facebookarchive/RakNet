//--------------------------------Extends-----------------------------------
//This file extends classes with new C++ code, note that not all things are possible because
//while typically it acts like the code is inserted into the class, it isn't really
//The extends are generated in the wrapper
//An example of a problem is accessing private variables and functions.
//Also instead of just calling the function in the class you use a pointer called self that is a pointer to the object

%extend RakNet::BitStream
{
	inline const char * CSharpStringReader(const char * inString)
	{
		self->Read((char *)inString);
		return inString;
	}

	inline bool CSharpByteReader(unsigned char* inOutByteArray,unsigned int numberOfBytes)
	{
		return self->Read((char *)inOutByteArray,numberOfBytes);
	}

	inline char * CSharpStringReaderCompressedDelta(char * inString)
	{
		self->ReadCompressedDelta(inString);
		return inString;
	}

	inline char * CSharpStringReaderDelta( char * inString)
	{
		self->ReadDelta(inString);
		return inString;
	}

	inline  char * CSharpStringReaderCompressed(char * inString)
	{
		self->ReadCompressed(inString);
		return inString;
	}

	void Write( unsigned char* inputByteArray, const unsigned int numberOfBytes )
	{
		self->Write((const char*)inputByteArray,numberOfBytes);
	}

	BitSize_t CSharpCopyDataHelper(unsigned char* inOutByteArray)
	{
		BitSize_t returnVal;
		returnVal=self->GetNumberOfBitsAllocated();
		memcpy(inOutByteArray, self->GetData(), sizeof(unsigned char) * (size_t) ( BITS_TO_BYTES( returnVal ) ) );
		return returnVal;
	}

	inline char * CSharpPrintBitsHelper(char * inString)
	{
		self->PrintBits(inString);
		return inString;

        }

	inline char * CSharpPrintHexHelper(char * inString)
	{
		self->PrintHex(inString);
		return inString;
        }

	void Serialize(bool writeToBitstream, unsigned char* inputByteArray, const unsigned int numberOfBytes )
	{
		self->Serialize(writeToBitstream,(char*)inputByteArray,numberOfBytes);
	}
	
	bool ReadAlignedBytesSafe(unsigned char *inOutByteArray, int inputLength, const int maxBytesToRead )
	{

		return self->ReadAlignedBytesSafe( (char *)inOutByteArray,inputLength,  maxBytesToRead );
	}

	bool ReadAlignedBytesSafe(unsigned char *inOutByteArray, unsigned int inputLength, const unsigned int maxBytesToRead )
	{
		return self->ReadAlignedBytesSafe((char *)inOutByteArray,inputLength,  maxBytesToRead );
	}

	void WriteAlignedVar8(unsigned char *inByteArray)
	{
		self->WriteAlignedVar8((const char *)inByteArray);
	}
		
	bool ReadAlignedVar8(unsigned char *inOutByteArray)
	{
		return self->ReadAlignedVar8((char *)inOutByteArray);
	}
		
	void WriteAlignedVar16(unsigned char *inByteArray)
	{
		self->WriteAlignedVar16((const char *)inByteArray);
	}
		
	bool ReadAlignedVar16(unsigned char *inOutByteArray)
	{
		return self->ReadAlignedVar16((char *)inOutByteArray);
	}
		
	void WriteAlignedVar32(unsigned char *inByteArray)
	{
		self->WriteAlignedVar32((const char *)inByteArray);
	}		

	bool ReadAlignedVar32(unsigned char *inOutByteArray)
	{
		return self->ReadAlignedVar32((char *)inOutByteArray);
	}

	void WriteAlignedBytesSafe( unsigned char *inByteArray, const unsigned int inputLength, const unsigned int maxBytesToWrite )
	{
		self->WriteAlignedBytesSafe((const char *)inByteArray, inputLength,  maxBytesToWrite );
	}
}

%define RAKPEERANDINTERFACEEXTEND()
{
        uint32_t Send( unsigned char *inByteArray, const int length, PacketPriority priority, PacketReliability reliability, char orderingChannel, const AddressOrGUID systemIdentifier, bool broadcast )
	{
		return self->Send((const char*)inByteArray,length,priority,reliability,orderingChannel,systemIdentifier,broadcast);

	}
        void SendLoopback( unsigned char *inByteArray, const int length )
	{
		self->SendLoopback( (const char *)inByteArray, length );

	}
	void SetOfflinePingResponse( unsigned char *inByteArray, const unsigned int length )
	{
		self->SetOfflinePingResponse((const char *) inByteArray,length);

	}
	bool AdvertiseSystem( const char *host, unsigned short remotePort, unsigned  char *inByteArray, int dataLength, unsigned connectionSocketIndex=0 )
	{
		return self->AdvertiseSystem(host,remotePort,(const char *) inByteArray,dataLength,connectionSocketIndex);

	}

	const char *CSharpGetIncomingPasswordHelper( const char* passwordData, int *passwordDataLength  )
	{
		self->GetIncomingPassword((char*)passwordData,passwordDataLength);
		return passwordData;
	}

	void SetIncomingPassword( unsigned char* passwordDataByteArray, int passwordDataLength  )
	{
		self->SetIncomingPassword((char*)passwordDataByteArray,passwordDataLength);
	}

	void GetIncomingPassword( unsigned char* passwordDataByteArray, int *passwordDataLength  )
	{
		self->GetIncomingPassword((char*)passwordDataByteArray,passwordDataLength);
	}

	void CSharpGetOfflinePingResponseHelper( unsigned char *inOutByteArray, unsigned int *outLength )
	{
		char * tmp=(char *)inOutByteArray;
		self->GetOfflinePingResponse(&tmp,outLength);
		memcpy(inOutByteArray,tmp,(size_t)*outLength);
	}

	bool GetConnectionList( DataStructures::List <SystemAddress> * remoteSystems, unsigned short *numberOfSystems ) const
	{
		SystemAddress inSys[256];
		bool returnVal = self->GetConnectionList(inSys,numberOfSystems);
		if(remoteSystems!=NULL)
		{
			for (int i=0;i<*numberOfSystems;i++)
			{
				remoteSystems->Insert(inSys[i],__FILE__,__LINE__);
			}
		}
		return returnVal;
	}

}
%enddef

%extend RakNet::RakPeerInterface
RAKPEERANDINTERFACEEXTEND()


%extend RakNet::RakPeer
RAKPEERANDINTERFACEEXTEND()

%extend RakNet::RakString
{
	void AppendBytes(unsigned char *inByteArray, unsigned int count)
	{
			self->AppendBytes((const char *)inByteArray,count);

	}


}

//Removed from interface, commented rather than removed in case needed later
/*
%extend RakNet::PluginInterface2
{

	void OnDirectSocketReceive(unsigned char *inByteArray, const BitSize_t bitsUsed, SystemAddress remoteSystemAddress)
	{
		self->OnDirectSocketReceive((const char *)inByteArray,   bitsUsed,  remoteSystemAddress);        
	}

	void OnDirectSocketSend(unsigned char *inByteArray, const BitSize_t bitsUsed, SystemAddress remoteSystemAddress) 
	{
		self->OnDirectSocketSend((const char *)inByteArray,   bitsUsed,  remoteSystemAddress);
	}

	void OnPushBackPacket(unsigned char *inByteArray, const BitSize_t bitsUsed, SystemAddress remoteSystemAddress)
	{
		self->OnPushBackPacket((const char *)inByteArray,   bitsUsed,  remoteSystemAddress);
	}

}*/

%define STRUCT_UNSIGNED_CHAR_ARRAY_EXTEND(IN_FUNCTION_NAME,IN_DATA_NAME,LENGTH_MEMBER_VAR)
	void IN_FUNCTION_NAME (unsigned char * inByteArray,int numBytes)
	{
		if (self->IN_DATA_NAME!=NULL)
		{
			rakFree_Ex(self->IN_DATA_NAME, __FILE__, __LINE__);
		}
		//create new with size
		self->IN_DATA_NAME=(unsigned char*) rakMalloc_Ex(numBytes, __FILE__, __LINE__);
		//copy
		memcpy( self->IN_DATA_NAME,inByteArray, numBytes );
		self->LENGTH_MEMBER_VAR=numBytes;
	}
%enddef

%extend RakNet::Packet
{
	Packet()
	{
		Packet * newPacket = new Packet();
		newPacket->data=NULL;
		return newPacket;
	}
	STRUCT_CUSTOM_UNSIGNED_CHAR_ARRAY_TYPEMAP(dataIsCached,dataCache,unsigned char * data,SetPacketData,Packet_data_get,Packet,length)
	STRUCT_UNSIGNED_CHAR_ARRAY_EXTEND(SetPacketData,data,length)
}

//Removed from interface, commented rather than removed in case needed later
/*
%extend InternalPacket
{
	STRUCT_CUSTOM_UNSIGNED_CHAR_ARRAY_TYPEMAP(dataIsCached,dataCache,unsigned char * data,SetInternalPacketData,InternalPacket_data_get,InternalPacket,dataBitLength/8)
	STRUCT_UNSIGNED_CHAR_ARRAY_EXTEND(SetInternalPacketData,data)
}*/

%define STRUCT_UNSIGNED_INT_ARRAY_EXTEND_SPECIAL_RAKNETSTATISTICS(IN_FUNCTION_NAME,IN_DATA_NAME)
	void IN_FUNCTION_NAME (unsigned int * inUnsignedIntArray,int numInts)
	{
	for (int i=0;i<numInts;i++)
	{
		if (i>=NUMBER_OF_PRIORITIES)
		{break;}
		self->IN_DATA_NAME[i]=inUnsignedIntArray[i];
	}
	}	
%enddef

%define STRUCT_UNSIGNED_INT64_ARRAY_EXTEND_SPECIAL_RAKNETSTATISTICS(IN_FUNCTION_NAME,IN_DATA_NAME)
	void IN_FUNCTION_NAME (unsigned long long int  * inUint64Array,int numUint64)
	{
	for (int i=0;i<numUint64;i++)
	{
		if (i>=RNS_PER_SECOND_METRICS_COUNT)
		{break;}
		self->IN_DATA_NAME[i]=inUint64Array[i];
	}
	}
%enddef

%define STRUCT_DOUBLE_ARRAY_EXTEND_SPECIAL_RAKNETSTATISTICS(IN_FUNCTION_NAME,IN_DATA_NAME)
	void IN_FUNCTION_NAME (double * inDoubleArray,int numDoubles)
	{
	for (int i=0;i<numDoubles;i++)
	{
		if (i>=NUMBER_OF_PRIORITIES)
		{break;}
		self->IN_DATA_NAME[i]=inDoubleArray[i];
	}
	}	
%enddef

%extend RakNet::RakNetStatistics
{
STRUCT_DOUBLE_ARRAY_EXTEND_SPECIAL_RAKNETSTATISTICS(SetBytesInSendBuffer,bytesInSendBuffer);
STRUCT_UNSIGNED_INT_ARRAY_EXTEND_SPECIAL_RAKNETSTATISTICS(SetMessageInSendBuffer,messageInSendBuffer);
STRUCT_UNSIGNED_INT64_ARRAY_EXTEND_SPECIAL_RAKNETSTATISTICS(SetRunningTotal,runningTotal);
STRUCT_UNSIGNED_INT64_ARRAY_EXTEND_SPECIAL_RAKNETSTATISTICS(SetValueOverLastSecond,valueOverLastSecond);
}

%extend FileProgressStruct
{
	FileProgressStruct()
	{
		FileProgressStruct * returnVal= new FileProgressStruct();
		returnVal->firstDataChunk=NULL;
		returnVal->iriDataChunk=NULL;
		return returnVal;
	}
	void SetFirstDataChunk (unsigned char  * inByteArray,int numBytes)
	{
		if (self->firstDataChunk!=NULL)
		{
			rakFree_Ex(self->firstDataChunk, __FILE__, __LINE__);
		}
		//create new with size
		self->firstDataChunk=(char*) rakMalloc_Ex(numBytes, __FILE__, __LINE__);
		//copy
		memcpy( self->firstDataChunk,inByteArray, numBytes );
	}

	void SetIriDataChunk(unsigned char  * inByteArray,int numBytes)
	{
		if (self->iriDataChunk!=NULL)
		{
			rakFree_Ex(self->iriDataChunk, __FILE__, __LINE__);
		}	
		//create new with size
		self->iriDataChunk=(char*) rakMalloc_Ex(numBytes, __FILE__, __LINE__);
		//copy
		memcpy( self->iriDataChunk,inByteArray, numBytes );
	}
}

%extend OnFileStruct
{
	OnFileStruct()
	{
		OnFileStruct * returnVal= new OnFileStruct();
		returnVal->fileData=NULL;
		return returnVal;
	}

	void SetFileData(unsigned char  * inByteArray,int numBytes)
	{
		if(self->fileData!=NULL)
		{
			rakFree_Ex(self->fileData, __FILE__, __LINE__);
		}
		//create new with size
		self->fileData=(char*) rakMalloc_Ex(numBytes, __FILE__, __LINE__);
		//copy
		memcpy( self->fileData,inByteArray, numBytes );
	}

}

%extend RakNet::IncrementalReadInterface
{
	unsigned int GetFilePart( const char *filename, unsigned int startReadBytes, unsigned int numBytesToRead, unsigned char  *inOutByteArray, FileListNodeContext context)
	{
		return self->GetFilePart( filename,  startReadBytes,  numBytesToRead, (void *)inOutByteArray,  context);
	}

}



%extend DataStructures::ByteQueue
{
	void WriteBytes(unsigned char *inByteArray, unsigned length, const char *file, unsigned int line)
	{
		self->WriteBytes((const char *)inByteArray,  length, file,  line);
	}
		
	bool ReadBytes(unsigned char *inOutByteArray, unsigned maxLengthToRead, bool peek)
	{		
		return self->ReadBytes((char *)inOutByteArray,  maxLengthToRead,  peek);
	}

	unsigned char* PeekContiguousBytesHelper(unsigned int *outLength) const
	{
		unsigned char* returnVal=(unsigned char* ) self->PeekContiguousBytes(outLength);
		return returnVal;	
	}
}

%extend RakNet::PacketLogger
{
        const char * FormatLineHelper(char* into, const char* dir, const char* type, unsigned int packet, unsigned int frame, unsigned char messageIdentifier, const BitSize_t bitLen, unsigned long long time, const SystemAddress& local, const SystemAddress& remote,unsigned int splitPacketId, unsigned int splitPacketIndex, unsigned int splitPacketCount, unsigned int orderingIndex)
	{
		self->FormatLine(into,dir,type,packet, frame,messageIdentifier, bitLen,time, local,  remote,splitPacketId, splitPacketIndex, splitPacketCount,  orderingIndex);
		return into;
	}

        const char * FormatLineHelper(char* into, const char* dir, const char* type, unsigned int packet, unsigned int frame, const char* idToPrint, const BitSize_t bitLen, unsigned long long time, const SystemAddress& local, const SystemAddress& remote,unsigned int splitPacketId, unsigned int splitPacketIndex, unsigned int splitPacketCount, unsigned int orderingIndex)
	{
		self->FormatLine(into,dir,type,packet,frame, idToPrint, bitLen, time, local,  remote, splitPacketId,  splitPacketIndex, splitPacketCount,  orderingIndex);
		return into;
	}
}
 
 %extend Cell
 {
 	void Set(unsigned char *inByteArray, int inputLength)
 	{
 		self->Set((const char *)inByteArray,inputLength);
 	}
 
 	void Get(unsigned char *inOutByteArray, int *outputLength)
 	{
 		self->Get((char *)inOutByteArray,outputLength);
 	}
 
 	char *GetHelper(char *output)
 	{
 		self->Get(output);
 		return output;
 	}
 }
 
 %extend Row
 {
 	void UpdateCell(unsigned columnIndex, int byteLength, unsigned char *inByteArray)
 	{
 		self->UpdateCell(columnIndex, byteLength, (const char *)inByteArray);
 	}
 }

%define STRUCT_CHAR_TO_BYTE_ARRAY_TYPEMAP_INSIDE_EXTEND(BOOLNAME,CACHENAME,IN_DATA_CHANGE_FUNCTION,IN_DATA_GET_FUNCTION,IN_CLASS,IN_LEN_ATTRIBUTE,IN_DATA_NAME)
	%typemap(imtype, out="IntPtr") char * IN_DATA_NAME "IntPtr"

	STRUCT_CUSTOM_GENERAL_ARRAY_TYPEMAP(BOOLNAME,CACHENAME,char * IN_DATA_NAME,byte,byte,IN_DATA_CHANGE_FUNCTION,IN_DATA_GET_FUNCTION,IN_CLASS,IN_LEN_ATTRIBUTE)

	void IN_DATA_CHANGE_FUNCTION (unsigned char * inByteArray,int numBytes)
	{
		if (self->IN_DATA_NAME!=NULL)
		{
			rakFree_Ex(self->IN_DATA_NAME, __FILE__, __LINE__);
		}	
	
		//create new with size
		self->IN_DATA_NAME=(char *) rakMalloc_Ex(numBytes, __FILE__, __LINE__);
		//copy
		memcpy( self->IN_DATA_NAME,inByteArray, numBytes );
		self->IN_LEN_ATTRIBUTE=numBytes;
	}
%enddef

%extend RakNet::FileListNode
{
	FileListNode()
	{
		FileListNode *returnVal = new FileListNode();
		returnVal->data=NULL;
		return returnVal;
	}

	STRUCT_CHAR_TO_BYTE_ARRAY_TYPEMAP_INSIDE_EXTEND(dataIsCached,dataCache,SetData,FileListNode_data_get,RakNet::FileListNode,dataLengthBytes,data)
}

%extend RakNet::TransportInterface
{
	void Send( SystemAddress systemAddress, unsigned char * inByteArray)
	{
		self->Send( systemAddress, (const char *)inByteArray);
	}
}

%extend RakNet::FileList
{
	void AddFile(const char *filename, const char *fullPathToFile, unsigned char *inByteArray, const unsigned dataLength, const unsigned fileLength, FileListNodeContext context, bool isAReference=false)
	{
		self->AddFile(filename,fullPathToFile, (const char *)inByteArray, dataLength,  fileLength,  context,isAReference);
	}

}

%extend RakNet::ConnectionGraph2
{

	bool GetConnectionListForRemoteSystemHelper(RakNetGUID remoteSystemGuid, DataStructures::List<SystemAddress> * saOut, DataStructures::List<RakNetGUID> * guidOut, unsigned int *inOutLength)
	{
		SystemAddress * tempInSystemAddress;
		RakNetGUID * tempInRakNetGUID;
		tempInSystemAddress = new SystemAddress[*inOutLength];
		tempInRakNetGUID = new RakNetGUID[*inOutLength];

		bool returnVal = self->GetConnectionListForRemoteSystem(remoteSystemGuid,tempInSystemAddress,tempInRakNetGUID,inOutLength);

		for (int i=0;i<*inOutLength;i++)
		{
			saOut->Insert(tempInSystemAddress[i],__FILE__,__LINE__);
			guidOut->Insert(tempInRakNetGUID[i],__FILE__,__LINE__);
		}
		delete [] tempInSystemAddress;
		delete [] tempInRakNetGUID;

		return returnVal;
	}
	
	void GetParticipantListHelper( DataStructures::List<RakNetGUID> * guidOut)
	{
		DataStructures::OrderedList<RakNetGUID, RakNetGUID> participantList;
		self->GetParticipantList(participantList);
		for (unsigned int i=0; i < participantList.Size(); i++)
			guidOut->Insert(participantList[i],__FILE__,__LINE__);
	}

}

%extend DataStructures::BPlusTree<unsigned, DataStructures::Table::Row*, _TABLE_BPLUS_TREE_ORDER>
{
	DataStructures::Table::Row* DeleteHelper(const unsigned key,  DataStructures::Table::Row* out,bool &outBool)
	{
		outBool=self->Delete(key,out);

		return out;
	}


	bool BPlusTree<KeyType, DataType, order>::DeleteHelper(const unsigned key)
	{
		this->DeleteHelper(key);
	}

	DataStructures::Table::Row* BPlusTree<KeyType, DataType, order>::GetHelper(const unsigned key, DataStructures::Table::Row* out,bool &outBool) const
	{
		outBool=self->Get(key,out);

		return out;
	}
}

#ifdef SWIG_ADDITIONAL_AUTOPATCHER

	%extend RakNet::AutopatcherServer
	{
	       void StartThreadsHelper(int numThreads, DataStructures::List<AutopatcherRepositoryInterface *> *sqlConnectionPtrArray)
		{
 			AutopatcherRepositoryInterface ** passedArray=NULL;
 			if (sqlConnectionPtrArray!=NULL)
 			{
 				passedArray=&((*sqlConnectionPtrArray)[0]); /*The memory should be contigous since this is a vector class copy. It was last time I checked the implementation. So this will efficiently pass the array without needing to copy it*/
 			}

			self->StartThreads(numThreads,passedArray);
		}
	}

	%extend RakNet::AutopatcherMySQLRepository
	{

		/// Calls mysql_real_connect with the implicit mySqlConnection 
		bool Connect (const char *host,const char *user,const char *passwd,const char *db,unsigned int port,const char *unix_socket,unsigned long clientflag)
		{
			return self->Connect (host,user,passwd,db,port,unix_socket,clientflag);
		}	
	
		/// Disconnect from the database
		void Disconnect(void)
		{
			return self->Disconnect();
		}
	
		/// Returns if we are connected to the database
		bool IsConnected(void) const
		{
			return self->IsConnected();
		}
	
		/// If any of the above functions fail, the error string is stored internally.  Call this to get it.
		const char *GetLastError(void) const
		{
			return self->GetLastError();
		}
	
		/// Returns the result of SELECT LOCALTIMESTAMP
		char *GetLocalTimestamp(void)
		{
			return self->GetLocalTimestamp();
		}

	}

	%extend MemoryCompressor
	{
	        /// Compress a block of data.  Pass true to finish if this is the last block in the series.  If you don't know if it's the last block, you can call it again with 0 for inputLength
	        /// \note Data passed to input isn't necessarily immediately compressed to output.  You can force a write by passing true to finish.
	        /// Multiple calls concatenate the written data.
	        /// \param[in] input A pointer to a block of data
	        /// \param[in] inputLength The length of input
	        /// \param[in] finish Write the last of the data.
	        bool Compress(unsigned char *inputByteArray, const unsigned inputLength, bool finish)
		{
			return self->Compress((char *) inputByteArray,inputLength,finish);
		}
	}

	%extend MemoryDecompressor
	{
	        /// Read \a inputLength bytes of compressed data from \a input
	        /// Writes the decompressed output to GetOutput().  Note that unlike the class MemoryCompressor, output data is updated immediately and not internally buffered
	        /// \param[in] input A pointer to a block of data
	        /// \param[in] inputLength The length of input
	        /// \param[in] ignoreStreamEnd Normally when Compress is called with finish==true stream end markers are placed.  These are honored, such that the read will end early if a stream marker is hit.  Pass true to ignore this and just output all the data.
	        bool Decompress(unsigned char *inputByteArray, const unsigned inputLength, bool ignoreStreamEnd)
		{
			return self->Decompress((char *) inputByteArray,inputLength,ignoreStreamEnd);
		}
	}		

	%extend CompressorBase
	{
		void GetOutputHelper(unsigned char * inOutByteArray)
		{
			char * returnByteArray;
			returnByteArray=self->GetOutput();

			memcpy(inOutByteArray, returnByteArray, self->GetTotalOutputSize() );
		}
	}
#endif