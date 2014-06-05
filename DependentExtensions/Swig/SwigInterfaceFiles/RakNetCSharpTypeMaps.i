//------------------------------TypeMaps--------------------------
//Swig typemaps help to either convert a C++ type into a C# type or tell it which types are the same
//Swig typemaps also can modify the postprocessed C++ file as well as the postprocessed C# files
//For example the cscode typemap is used extensively to write C# code into the postprocessed file

//In the below section INOUT is used when the array needs to be passed in and results need copied out
//INPUT is used for just a copy in
//OUTPUT is used just for a copy out
//Any pointers or references will need a typemap to be used in the original api form, otherwise it
//Will show up as SWIGTYPE_p_int

//NOTE: The typmaps are commented for the first class they are specifically made for, that doesn't mean other classes do not depend on the typemaps, once a typemap is defined it applies to all instances

//Allow array of strings
CSHARP_ARRAYS(char *,string)

//INOUT char to byte array
//This is used because RakNet uses char arrays as byte arrays, C# char arrays are unicode
%apply unsigned char INOUT[] {unsigned char* inputByteArray};
%apply unsigned char INOUT[] {unsigned char* inByteArray};
%apply unsigned char INOUT[] {unsigned char* inByteArray2};
%apply unsigned char INOUT[] {unsigned char* inOutByteArray};
%apply unsigned char INOUT[] {unsigned char* _data};
%apply unsigned char INOUT[] {unsigned char* inOutData};

//TeamBalancer
%apply unsigned short INOUT[] {unsigned short *values};

//RakPeer
%apply unsigned char INOUT[] {unsigned char* passwordDataByteArray};

%typemap(ctype)   unsigned char *inByteArrayArray "unsigned char*"
%typemap(cstype)  unsigned char *inByteArrayArray  "byte[,]"
%typemap(imtype, inattributes="[In, MarshalAs(UnmanagedType.LPArray)]") unsigned char *inByteArrayArray  "byte[,]"
%typemap(csin)    unsigned char *inByteArrayArray  "$csinput"

%typemap(in)     unsigned char *inByteArrayArray  "$1 = $input;"
%typemap(freearg) unsigned char *inByteArrayArray  ""
%typemap(argout)  unsigned char *inByteArrayArray  ""

%apply int INOUT[] {int *lengths};

//Other inout - RakPeer
%apply unsigned int INOUT[] {unsigned int outputFrequencyTable[ 256 ]};
%apply unsigned int INOUT[] {unsigned int inputFrequencyTable[ 256 ]};
%apply unsigned short * INOUT {unsigned short *numberOfSystems };

//For the read functions, out vars
%apply long long &OUTPUT {long long &outTemplateVar};
%apply long &OUTPUT {long &outTemplateVar};
%apply short &OUTPUT {short &outTemplateVar};
%apply bool &OUTPUT {bool &outTemplateVar};
%apply float &OUTPUT {float &outTemplateVar};
%apply float &OUTPUT {float &outFloat};
%apply unsigned char &OUTPUT {unsigned char &outTemplateVar};

//RakPeer inout
%apply int *INOUT {int *passwordDataLength};

//RakPeer out,ByteQueue out
%apply unsigned int *OUTPUT {unsigned int *outLength};

//RakNetStatistics
%apply unsigned int OUTPUT[] {unsigned int *inUnsignedIntArray};
%apply unsigned long long OUTPUT[] {unsigned long long  *inUint64Array};
//%apply ulong OUTPUT[] {uint64_t *inUint64Array};
%apply double OUTPUT[] {double *inDoubleArray};

//Inouts for serialize and such
%apply long long &INOUT {long long &inOutTemplateVar};
%apply long &INOUT {long &inOutTemplateVar};
%apply short &INOUT {short &inOutTemplateVar};
%apply unsigned short &INOUT {unsigned short &inOutTemplateVar};
%apply bool &INOUT {bool &inOutTemplateVar};
%apply float &INOUT {float &inOutTemplateVar};
%apply float &INOUT {float &inOutFloat};
%apply unsigned char &INOUT {unsigned char &inOutTemplateVar};

%apply long long &INOUT {long long &inOutCurrentValue};
%apply long &INOUT {long &inOutCurrentValue};
%apply short &INOUT {short &inOutCurrentValue};
%apply unsigned short &INOUT {unsigned short &inOutCurrentValue};
%apply bool &INOUT {bool &inOutCurrentValue};
%apply float &INOUT {float &inOutCurrentValue};
%apply float &INOUT {float &inOutCurrentValue};
%apply unsigned char &INOUT {unsigned char &inOutCurrentValue};

//UDPForwarder
%apply unsigned short *OUTPUT {unsigned short *forwardingPort}
%apply __UDPSOCKET__ *OUTPUT {__UDPSOCKET__ *forwardingSocket}

//CommandParserInterface
%apply char * INPUT[] {char **parameterList}

//DataStructures::Table
%apply int *OUTPUT {int *output};
%apply int *OUTPUT {int *outputLength};
%apply int *INOUT {unsigned *key};
%apply unsigned int INPUT[] {unsigned *columnIndicesSubset}
%apply unsigned int INPUT[] {unsigned *rowIds}
%apply double *OUTPUT {double *output}

//UDPForwarder
%apply unsigned short *OUTPUT {unsigned short *forwardingPort}
%apply unsigned int *OUTPUT {SOCKET *forwardingSocket}

//ConnectionGraph2
%apply unsigned int *INOUT {unsigned int *inOutLength};
%apply DataStructures::DefaultIndexType *OUTPUT {DataStructures::DefaultIndexType *participantListSize};

//AWSSimpleDBInterface

//Define RakString as a simple object situation Only needed once
SIMPLE_OBJECT_OUTPUT_TYPEMAP(RakNet::RakString,RakString)

//Apply to the specific function parameters, need for each parameter
//out prefixed items like outItemCount help prevent mistakes
%apply RakNet::RakString &OUTPUT {RakNet::RakString &itemCount};
%apply RakNet::RakString &OUTPUT {RakNet::RakString &columnCount};
%apply RakNet::RakString &OUTPUT {RakNet::RakString &rowCount};

//Classless
#ifdef SWIG_ADDITIONAL_AUTOPATCHER
	%apply unsigned int *OUTPUT {unsigned *outSize};//For CreatePatch function
	%apply bool *OUTPUT {bool *outBool};//For CreatePatch function
#endif

//The next few items are to convert members arrays into something nativly accessable in C#
//For example struct a {unsigned char * array1;}; is not processed by Swig, there is no easy way to do it but these
//Typemaps handle it so it is accessable as a byte[] array, this applies to other types of arrays as well.
//----------------byte array out charmap for ByteQueue
%typemap(cstype, out="byte[]") unsigned char* PeekContiguousBytesHelper "byte[]"

%typemap(csout, excode=SWIGEXCODE2) unsigned char* PeekContiguousBytesHelper  
%{
  {
      IntPtr cPtr = RakNetPINVOKE.ByteQueue_PeekContiguousBytesHelper(swigCPtr, out outLength);
      int len = (int)outLength;
      if (len <= 0)
      {
          return null;
      }
      byte[] returnBytes = new byte[len];
      Marshal.Copy(cPtr, returnBytes, 0, len);
      return returnBytes;
  }
%}

//This is more efficient than the loop version but less compatible
/*
Vars:
BOOLNAME- The name of the variable that keeps track of if it is cached, typically varIsCached
CACHENAME- The name of the variable C# cache typically varCache
CTYPE- The type in C of the var
INCSTYPE- C# type
INTERMEDIATETYPE- Type that is worked with in the function 
IN_DATA_CHANGE_FUNCTION- The name of the set function, Typically SetVar
IN_DATA_GET_FUNCTION- The get function for that data in the pinvoke, typically Class_var_get
IN_CLASS- The class we are working with 
IN_LEN_METHOD- Something to get the length of the C/C++ array for marshalling. This is called from C#, not C.
*/
%define STRUCT_CUSTOM_GENERAL_ARRAY_TYPEMAP(BOOLNAME,CACHENAME,CTYPE,INCSTYPE,INTERMEDIATETYPE,IN_DATA_CHANGE_FUNCTION,IN_DATA_GET_FUNCTION,IN_CLASS,IN_LEN_METHOD)
%typemap(cstype, out="INCSTYPE[]") CTYPE "INCSTYPE[]"

%typemap(csvarin, excode=SWIGEXCODE2) CTYPE 
%{
	set 
	{
	    	CACHENAME=value;
		BOOLNAME = true;
		IN_DATA_CHANGE_FUNCTION (value, value.Length);    
	}
%}
%typemap(csvarout, excode=SWIGEXCODE2) CTYPE  
%{
        get
        {
            INCSTYPE[] returnArray;
            if (!BOOLNAME)
            {
                IntPtr cPtr = RakNetPINVOKE.IN_DATA_GET_FUNCTION (swigCPtr);
                int len = (int) IN_LEN_METHOD;
		if (len<=0)
		{
			return null;
		}
                returnArray = new INCSTYPE[len];
                INTERMEDIATETYPE[] marshalArray = new INTERMEDIATETYPE[len];
                Marshal.Copy(cPtr, marshalArray, 0, len);
                marshalArray.CopyTo(returnArray, 0);
                CACHENAME = returnArray;
                BOOLNAME = true;
            }
            else
            {
                returnArray = CACHENAME;
            }
            return returnArray;
        }
 %}
%enddef

//More compatible version of the above macro
//One new variable CONVERSION_MODIFIER that converts the type in the marshalarray to output array
//this can be a function or a typecast if a function leave out the parentheses
%define STRUCT_CUSTOM_GENERAL_ARRAY_TYPEMAP_LOOP_COPY(BOOLNAME,CACHENAME,CTYPE,INCSTYPE,INTERMEDIATETYPE,IN_DATA_CHANGE_FUNCTION,IN_DATA_GET_FUNCTION,IN_CLASS,IN_LEN_METHOD,CONVERSION_MODIFIER)
%typemap(cstype, out="INCSTYPE[]") CTYPE "INCSTYPE[]"

%typemap(csvarin, excode=SWIGEXCODE2) CTYPE 
%{
	set 
	{
	    	CACHENAME=value;
		BOOLNAME = true;
		IN_DATA_CHANGE_FUNCTION (value, value.Length);    
	}
%}
%typemap(csvarout, excode=SWIGEXCODE2) CTYPE  
%{
        get
        {
            INCSTYPE[] returnArray;
            if (!BOOLNAME)
            {
                IntPtr cPtr = RakNetPINVOKE.IN_DATA_GET_FUNCTION (swigCPtr);
                int len = (int) IN_LEN_METHOD;
		if (len<=0)
		{
			return null;
		}
                returnArray = new INCSTYPE[len];
                INTERMEDIATETYPE[] marshalArray = new INTERMEDIATETYPE[len];
                Marshal.Copy(cPtr, marshalArray, 0, len);
                for (int i=0;i<len;i++)
                {
                    returnArray[i]= CONVERSION_MODIFIER ( marshalArray[i] );
                }
                CACHENAME = returnArray;
                BOOLNAME = true;
            }
            else
            {
                returnArray = CACHENAME;
            }
            return returnArray;
        }
 %}
%enddef

STRUCT_CUSTOM_GENERAL_ARRAY_TYPEMAP_LOOP_COPY(bytesInSendBufferIsCached,bytesInSendBufferCache,double bytesInSendBuffer[ NUMBER_OF_PRIORITIES ],double,double,SetBytesInSendBuffer,RakNetStatistics_bytesInSendBuffer_get,RakNet::RakNetStatistics,PacketPriority.NUMBER_OF_PRIORITIES,(double));

STRUCT_CUSTOM_GENERAL_ARRAY_TYPEMAP_LOOP_COPY(messageInSendBufferIsCached,messageInSendBufferCache,unsigned messageInSendBuffer[ NUMBER_OF_PRIORITIES ],uint,int,SetMessageInSendBuffer,RakNetStatistics_messageInSendBuffer_get,RakNet::RakNetStatistics,PacketPriority.NUMBER_OF_PRIORITIES,(uint));

namespace RakNet
{
STRUCT_CUSTOM_GENERAL_ARRAY_TYPEMAP_LOOP_COPY(runningTotalIsCached,runningTotalCache,unsigned long long runningTotal [ RNS_PER_SECOND_METRICS_COUNT ],ulong,long,SetRunningTotal,RakNetStatistics_runningTotal_get,RakNet::RakNetStatistics,RNSPerSecondMetrics.RNS_PER_SECOND_METRICS_COUNT,(ulong));
STRUCT_CUSTOM_GENERAL_ARRAY_TYPEMAP_LOOP_COPY(valueOverLastSecondIsCached,valueOverLastSecondCache,unsigned long long valueOverLastSecond [ RNS_PER_SECOND_METRICS_COUNT ],ulong,long,SetValueOverLastSecond,RakNetStatistics_valueOverLastSecond_get,RakNet::RakNetStatistics,RNSPerSecondMetrics.RNS_PER_SECOND_METRICS_COUNT,(ulong));
}

%typemap(imtype, out="IntPtr") char *firstDataChunk "IntPtr"
%typemap(imtype, out="IntPtr") char *iriDataChunk "IntPtr"
%typemap(imtype, out="IntPtr") char *fileData "IntPtr"

STRUCT_CUSTOM_GENERAL_ARRAY_TYPEMAP(firstDataChunkIsCached,firstDataChunkCache,char *firstDataChunk,byte,byte,SetFirstDataChunk,FileProgressStruct_firstDataChunk_get,FileProgressStruct,dataChunkLength)
STRUCT_CUSTOM_GENERAL_ARRAY_TYPEMAP(iriDataChunkIsCached,iriDataChunkCache,char *iriDataChunk,byte,byte,SetIriDataChunk,FileProgressStruct_iriDataChunk_get,FileProgressStruct,dataChunkLength)
STRUCT_CUSTOM_GENERAL_ARRAY_TYPEMAP(fileDataIsCached,fileDataCache,char *fileData,byte,byte,SetFileData,OnFileStruct_fileData_get,OnFileStruct,byteLengthOfThisFile)

//GetpntrTypemaps
//Many of these typemaps are required to copy the
//cached or changed data back into C++ when the pointer is passed into a C++ function
//Think of it as a type of cache writeback
%typemap(csbody) RakNet::OnFileStruct
%{
  private HandleRef swigCPtr;
  protected bool swigCMemOwn;

  internal OnFileStruct(IntPtr cPtr, bool cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = new HandleRef(this, cPtr);
  }

  internal static HandleRef getCPtr(OnFileStruct obj) {
     if (obj != null)
     {
        if (obj.fileDataIsCached)
        {
           obj.SetFileData(obj.fileData, obj.fileData.Length); //If an individual index was modified we need to copy the data before passing to C++
        }
	obj.fileDataIsCached=false;
     }
    return (obj == null) ? new HandleRef(null, IntPtr.Zero) : obj.swigCPtr;
  }
%}

%typemap(csbody) RakNet::Packet 
%{
  private HandleRef swigCPtr;
  protected bool swigCMemOwn;

  internal Packet(IntPtr cPtr, bool cMemoryOwn) 
  {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = new HandleRef(this, cPtr);
  }

  internal static HandleRef getCPtr(Packet obj)
  {
     if (obj != null)
     {
        if (obj.dataIsCached)
        {
           obj.SetPacketData(obj.data, obj.data.Length); //If an individual index was modified we need to copy the data before passing to C++
        }
	obj.dataIsCached=false;
     }
     return (obj == null) ? new HandleRef(null, IntPtr.Zero) : obj.swigCPtr;
  }
%}

%typemap(csbody) FileProgressStruct 
%{
  private HandleRef swigCPtr;
  protected bool swigCMemOwn;

  internal FileProgressStruct(IntPtr cPtr, bool cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = new HandleRef(this, cPtr);
  }

  internal static HandleRef getCPtr(FileProgressStruct obj) {

    if (obj != null)
    {
	if (obj.firstDataChunkIsCached)
        {
  	     	obj.SetFirstDataChunk(obj.firstDataChunk, obj.firstDataChunk.Length);
 	}
  	if (obj.iriDataChunkIsCached)
        {
		obj.SetIriDataChunk(obj.iriDataChunk, obj.iriDataChunk.Length);
	}
	obj.firstDataChunkIsCached=false;
	obj.iriDataChunkIsCached=false;
    }

    return (obj == null) ? new HandleRef(null, IntPtr.Zero) : obj.swigCPtr;
  }
%}

%typemap(csbody) RakNet::RakNetStatistics 
%{
  private HandleRef swigCPtr;
  protected bool swigCMemOwn;

  internal RakNetStatistics(IntPtr cPtr, bool cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = new HandleRef(this, cPtr);
  }

  internal static HandleRef getCPtr(RakNetStatistics obj) {
    if (obj != null)
    {
	if (obj.bytesInSendBufferIsCached)
        {
  	     	obj.SetBytesInSendBuffer(obj.bytesInSendBuffer, obj.bytesInSendBuffer.Length);
 	}
  	if (obj.messageInSendBufferIsCached)
        {
		obj.SetMessageInSendBuffer(obj.messageInSendBuffer, obj.messageInSendBuffer.Length);
	}
   	if (obj.runningTotalIsCached)
        {
		obj.SetRunningTotal(obj.runningTotal, obj.runningTotal.Length);
	}
  	if (obj.valueOverLastSecondIsCached)
        {
		obj.SetValueOverLastSecond(obj.valueOverLastSecond, obj.valueOverLastSecond.Length);
	}
	obj.bytesInSendBufferIsCached=false;
	obj.messageInSendBufferIsCached=false;
	obj.runningTotalIsCached=false;
	obj.valueOverLastSecondIsCached=false;
    }
    
    return (obj == null) ? new HandleRef(null, IntPtr.Zero) : obj.swigCPtr;
  }

%}

//Removed from interface, commented rather than removed in case needed later
/*
%typemap(csbody) InternalPacket 
%{
  private HandleRef swigCPtr;

  internal InternalPacket(IntPtr cPtr, bool cMemoryOwn) : base(RakNetPINVOKE.InternalPacketUpcast(cPtr), cMemoryOwn) 
  {
    swigCPtr = new HandleRef(this, cPtr);
  }

  internal static HandleRef getCPtr(InternalPacket obj) 
  {
      if (obj != null)
      {
          if (obj.dataIsCached)
          {
              obj.SetInternalPacketData(obj.data, obj.data.Length); //If an individual index was modified we need to copy the data before passing to C++
          }
      }
      return (obj == null) ? new HandleRef(null, IntPtr.Zero) : obj.swigCPtr;
  }
%}*/

//Extra C# code
//These typemaps add C# code to the postprocessed file, mostly to improve the user interface so it matches the original
//API for things Swig doesn't support

//This adds to the main RakNet class, generally used for globals
#ifdef SWIG_ADDITIONAL_AUTOPATCHER
	%pragma(csharp) modulecode=
	%{ 
	    public static readonly SystemAddress UNASSIGNED_SYSTEM_ADDRESS = new SystemAddress();
	    public static readonly RakNetGUID UNASSIGNED_RAKNET_GUID = new RakNetGUID(ulong.MaxValue);

	    public static void StatisticsToString(RakNetStatistics s, out string buffer, int verbosityLevel) 
	    {
		String tmp = new String('c', 9999);
		buffer=StatisticsToStringHelper(s,tmp,verbosityLevel);
   	    }

	  /*  public static bool CreatePatch(byte[] old, uint oldsize, byte[] _new, uint newsize, out byte[] outStore, out uint outSize)
	    {
		bool returnVal;
		outStore=new byte[999999];
       		return CreatePatchHelper(old,oldsize,_new,newsize,outStore,out outSize);
		Array.Resize<byte>(ref outStore, (int)outSize);
		return returnVal;
    	   }*/
	%}
#else
	%pragma(csharp) modulecode=
	%{ 
	    public static readonly SystemAddress UNASSIGNED_SYSTEM_ADDRESS = new SystemAddress();
	    public static readonly RakNetGUID UNASSIGNED_RAKNET_GUID = new RakNetGUID(ulong.MaxValue);

	    public static void StatisticsToString(RakNetStatistics s, out string buffer, int verbosityLevel) 
 	   {
		String tmp = new String('c', 9999);
		buffer=StatisticsToStringHelper(s,tmp,verbosityLevel);
 	   }
	%}
#endif
//BitStream
%typemap(cscode) RakNet::BitStream 
%{
  //String reading using original api, but converted to c# logic
  public bool Read(out string varString) 
  {
      String tmp = new String('c', (int)GetNumberOfUnreadBits()/8);
      varString = CSharpStringReader(tmp);
      return varString!="";
  }

  //Byte read out
  public bool Read(byte[] inOutByteArray,uint numberOfBytes) 
  {
      return CSharpByteReader(inOutByteArray,numberOfBytes);      
  }

  public bool Read(out char outTemplateVar)
  {
   	byte tmp;
	bool returnVal=Read(out tmp);
	outTemplateVar=(char)tmp;
	return returnVal;
  }

  public bool ReadCompressed(out char outTemplateVar)
  {
   	byte tmp;
	bool returnVal=ReadCompressed(out tmp);
	outTemplateVar=(char)tmp;
	return returnVal;
  }

  public bool ReadCompressedDelta(out char outTemplateVar)
  {
   	byte tmp;
	bool returnVal=ReadCompressedDelta(out tmp);
	outTemplateVar=(char)tmp;
	return returnVal;
  }

  public bool ReadDelta(out char outTemplateVar)
  {
   	byte tmp;
	bool returnVal=ReadDelta(out tmp);;
	outTemplateVar=(char)tmp;
	return returnVal;
  }

  public bool ReadCompressed(out string var)
  {
      String tmp = new String('c', (int)GetNumberOfUnreadBits()/8);
      var = CSharpStringReaderCompressed(tmp);
      return var!="";
  }

  public bool ReadCompressedDelta(out string var)
  {
      String tmp = new String('c', (int)GetNumberOfUnreadBits()/8);
      var = CSharpStringReaderCompressedDelta(tmp);
      return var!="";
  }

  public bool ReadDelta(out string var)
  {
      String tmp = new String('c', (int)GetNumberOfUnreadBits()/8);
      var = CSharpStringReaderDelta(tmp);
      return var!="";
  }

  public uint CopyData(out byte[] outByteArray)
  {
	byte[] tmp= new byte[GetNumberOfBitsAllocated()/8];
	uint byteNum = CSharpCopyDataHelper(tmp);
	outByteArray=tmp;
	return byteNum;
  }

  public byte[] GetData()
  {
	byte[] tmp= new byte[GetNumberOfBitsAllocated()/8];
	CSharpCopyDataHelper(tmp);
	return tmp;
  }

  public void PrintBits(out string var)
  {
      String tmp = new String('c', (int)(GetNumberOfBitsAllocated()+GetNumberOfBitsAllocated()/8));
      var = CSharpPrintBitsHelper(tmp);
  }

  public void PrintHex(out string var)
  {
      String tmp = new String('c', (int)(GetNumberOfBitsAllocated()/4+GetNumberOfBitsAllocated()/8));
      var = CSharpPrintHexHelper(tmp);
  }

  public bool Serialize(bool WriteToBitstream,ref char inOutTemplateVar)
  {
      byte tmp = (byte)inOutTemplateVar;
      bool returnVal = Serialize(WriteToBitstream,ref tmp);
      inOutTemplateVar=(char)tmp;
      return returnVal;
  }
  public bool SerializeDelta(bool WriteToBitstream,ref char inOutTemplateVar)
  {
      byte tmp = (byte)inOutTemplateVar;
      bool returnVal = SerializeDelta(WriteToBitstream,ref tmp);
      inOutTemplateVar=(char)tmp;
      return returnVal;
  }

  public bool SerializeCompressed(bool WriteToBitstream,ref char inOutTemplateVar)
  {
      byte tmp = (byte)inOutTemplateVar;
      bool returnVal = SerializeCompressed(WriteToBitstream,ref tmp);
      inOutTemplateVar=(char)tmp;
      return returnVal;
  }

  public bool SerializeCompressedDelta(bool WriteToBitstream,ref char inOutTemplateVar)
  {
      byte tmp = (byte)inOutTemplateVar;
      bool returnVal = SerializeCompressedDelta(WriteToBitstream,ref tmp);
      inOutTemplateVar=(char)tmp;
      return returnVal;
  }

  public bool ReadAlignedBytesSafeAlloc( out byte[] outByteArray, int inputLength, int maxBytesToRead )
  {
	outByteArray= new byte[inputLength];
	return ReadAlignedBytesSafe(outByteArray,inputLength,maxBytesToRead);
  }

  public bool ReadAlignedBytesSafeAlloc( out byte[] outByteArray, uint inputLength, uint maxBytesToRead )
  {
	outByteArray= new byte[inputLength];
	return ReadAlignedBytesSafe(outByteArray,inputLength,maxBytesToRead);
  }


%}

%define RAKPEERANDINTERFACECSCODE(MACRO_FUNCTIONTYPE_MOD)
%{
	public MACRO_FUNCTIONTYPE_MOD void GetIncomingPassword(ref string passwordData, ref int passwordDataLength  )
	{
		passwordData=CSharpGetIncomingPasswordHelper(passwordData,ref passwordDataLength);
	}

	public MACRO_FUNCTIONTYPE_MOD void GetOfflinePingResponse( byte[] inOutByteArray, out uint length )
	{
		CSharpGetOfflinePingResponseHelper(inOutByteArray,out length);
	}

	public MACRO_FUNCTIONTYPE_MOD bool GetConnectionList(out SystemAddress[] remoteSystems, ref ushort numberOfSystems)
	{
		RakNetListSystemAddress passVal= new RakNetListSystemAddress();
		bool returnVal = GetConnectionList(passVal,ref numberOfSystems);
		SystemAddress[] outVal = new SystemAddress[numberOfSystems];
		for (int i=0; i<numberOfSystems;i++)
		{
			outVal[i]=passVal[i];
		} 
		remoteSystems=outVal;
		return returnVal;
	}

%}
%enddef

//Table
 %typemap(cscode) DataStructures::Table 
 %{

	public RakNetPageRow GetListHead() {
		return GetListHeadHelper();
	}

  	public void SortTable(SortQuery [] sortQueries, uint numSortQueries, out Row[] arg2) 
 	{
		RakNetListSortQuery sortQueriesList =null;
 		if (sortQueries!=null)
 		{
 			sortQueriesList = new RakNetListSortQuery();
 			int listLen = sortQueries.Length;
 			for (int i = 0; i < listLen; i++)
 			{
      	 		    sortQueriesList.Insert(sortQueries[i], "", 1);
 			}
 		}
 
 		int rowCount= (int)GetRowCount();
 		Row[] rowArray= new Row[rowCount];
 		RakNetListTableRow passVal = new RakNetListTableRow();
 		SortTableHelper(sortQueriesList,numSortQueries,passVal);
 		for (int i=0; i<rowCount;i++)
 		{
 		rowArray[i]=passVal[i];
	 	} 
 		arg2=rowArray;
  	}

 	public void GetCellValueByIndex(uint rowIndex, uint columnIndex, out byte[] outByteArray, out int outputLength) 
         {
 		int returnInt=0;
 		Row row = GetRowByIndex(rowIndex,ref returnInt);
 		int arrayLen=0;
 		if (row!=null)
 		{
 			arrayLen=(int)row.cells[(int)columnIndex].i;
 		}
 		byte[] inOutByteArray = new byte[arrayLen];
 		GetCellValueByIndexHelper(rowIndex,columnIndex,inOutByteArray,out outputLength);
 		outByteArray=inOutByteArray;
 	}

 	public void GetCellValueByIndex(uint rowIndex, uint columnIndex,out string output) 
 	{
 		int returnInt=0;
 		Row row = GetRowByIndex(rowIndex,ref returnInt);
 		int arrayLen=0;
 		if (row!=null)
 		{
 			arrayLen=(int)row.cells[(int)columnIndex].i;
 		}
    		String tmp = new String('c', arrayLen);
 		output=GetCellValueByIndexHelper( rowIndex, columnIndex, tmp);
 	}

       public  void QueryTable(uint[] columnIndicesSubset, uint numColumnSubset, FilterQuery[] inclusionFilters, uint numInclusionFilters, uint[] rowIds, uint numRowIDs, Table result)
 	{
 		RakNetListFilterQuery inclusionFiltersList =null;
 		if (inclusionFiltersList!=null)
 		{
 			inclusionFiltersList = new RakNetListFilterQuery();
 			int listLen = inclusionFilters.Length;
 			for (int i = 0; i < listLen; i++)
 			{
      	 		    inclusionFiltersList.Insert(inclusionFilters[i], "", 1);
 			}
 		}
 		QueryTableHelper(columnIndicesSubset, numColumnSubset, inclusionFiltersList, numInclusionFilters, rowIds, numRowIDs, result);
 	}

 	public uint ColumnIndex(string columnName) {
 	return ColumnIndexHelper(columnName);
 	}
 %}

//Table Cell
 %typemap(cscode) Cell
 %{
 	public void Get(out string output)
 	{
 		string temp=new String('c', (int) this.i);
 		output=GetHelper(temp);
 	}
 %}

//RakPeer
%typemap(cscode) RakNet::RakPeer 
RAKPEERANDINTERFACECSCODE(override)

//RakPeerInterface
%typemap(cscode) RakNet::RakPeerInterface
RAKPEERANDINTERFACECSCODE(virtual)


%typemap(cscode) RakNet::PacketLogger 
%{
  public virtual void FormatLine(ref string preInitializedStringBigEnoughToFitResult, string dir, string type, uint packet, uint frame, byte messageIdentifier, uint bitLen, ulong time, SystemAddress local, SystemAddress remote, uint splitPacketId, uint splitPacketIndex, uint splitPacketCount, uint orderingIndex) {
	preInitializedStringBigEnoughToFitResult=FormatLineHelper( preInitializedStringBigEnoughToFitResult, dir, type,  packet, frame,  messageIdentifier, bitLen,  time, local,  remote, splitPacketId,  splitPacketIndex, splitPacketCount,  orderingIndex);
  }

  public virtual void FormatLine(ref string preInitializedStringBigEnoughToFitResult, string dir, string type, uint packet, uint frame, string idToPrint, uint bitLen, ulong time, SystemAddress local, SystemAddress remote, uint splitPacketId, uint splitPacketIndex, uint splitPacketCount, uint orderingIndex) {
  	preInitializedStringBigEnoughToFitResult=FormatLineHelper( preInitializedStringBigEnoughToFitResult,  dir, type,  packet, frame,  idToPrint, bitLen,  time, local,  remote, splitPacketId,  splitPacketIndex, splitPacketCount,  orderingIndex);
  }
%}

%typemap(csimports) RakNet::SystemAddress 
%{
using System;
using System.Runtime.InteropServices;
#pragma warning disable 0660
%}

%typemap(cscode) RakNet::SystemAddress 
%{

	public override int GetHashCode()
	{    
		// return (int)((this.port+this.binaryAddress)% int.MaxValue);
		return (int) ToInteger(this);
	}
	public static bool operator ==(SystemAddress a, SystemAddress b)
	{
 	   	// If both are null, or both are same instance, return true.
 		if (System.Object.ReferenceEquals(a, b))
 		{
 	       		return true;
 	   	}

  		// If one is null, but not both, return false.
   	 	if (((object)a == null) || ((object)b == null))
    		{
       		 	return false;
    		}

		    return a.Equals(b);//Equals should be overloaded as well
	}

	public static bool operator !=(SystemAddress a, SystemAddress b)
	{
   		 return a.OpNotEqual(b);
	}

	public static bool operator < (SystemAddress a, SystemAddress b)
	{
    		return a.OpLess(b);
	}

	public static bool operator >(SystemAddress a, SystemAddress b)
	{
		return a.OpGreater(b);
	}

	public static bool operator <=(SystemAddress a, SystemAddress b)
	{
		return (a.OpLess(b) || a==b);
	}

	public static bool operator >=(SystemAddress a, SystemAddress b)
	{
		return (a.OpGreater(b) || a==b);
	}

	public override string ToString()
	{
		return ToString(true);
	}

	public void ToString(bool writePort,out string dest)
	{
		dest=ToString(writePort);
	}
%}

%typemap(csimports) RakNet::RakNetGUID
%{
using System;
using System.Runtime.InteropServices;
#pragma warning disable 0660
%}


%typemap(cscode) RakNet::RakNetGUID 
%{

	public override int GetHashCode()
	{
		// return (int)(this.g%int.MaxValue);
		 return (int) ToUint32(this);
	}

	public static bool operator ==(RakNetGUID a, RakNetGUID b)
	{
 	   	// If both are null, or both are same instance, return true.
 		if (System.Object.ReferenceEquals(a, b))
 		{
 	       		return true;
 	   	}

  		// If one is null, but not both, return false.
   	 	if (((object)a == null) || ((object)b == null))
    		{
       		 	return false;
    		}

		    return a.Equals(b);//Equals should be overloaded as well
	}

	public static bool operator !=(RakNetGUID a, RakNetGUID b)
	{
   		 return a.OpNotEqual(b);
	}

	public static bool operator < (RakNetGUID a, RakNetGUID b)
	{
    		return a.OpLess(b);
	}

	public static bool operator >(RakNetGUID a, RakNetGUID b)
	{
		return a.OpGreater(b);
	}

	public static bool operator <=(RakNetGUID a, RakNetGUID b)
	{
		return (a.OpLess(b) || a==b);
	}

	public static bool operator >=(RakNetGUID a, RakNetGUID b)
	{
		return (a.OpGreater(b) || a==b);
	}

 	public void ToString(out string dest) 
	{
		dest = ToString();
	}
%}

%typemap(csimports) RakNet::RakString  
%{
using System;
using System.Runtime.InteropServices;
#pragma warning disable 0660
%}

%typemap(cscode) RakNet::RakString 
%{

	public override int GetHashCode()
	{
		return this.C_String().GetHashCode();
	}

	public static bool operator ==(RakString a, RakString b)
	{
 	   	// If both are null, or both are same instance, return true.
 		if (System.Object.ReferenceEquals(a, b))
 		{
 	       		return true;
 	   	}

  		// If one is null, but not both, return false.
   	 	if (((object)a == null) || ((object)b == null))
    		{
       		 	return false;
    		}

		    return a.Equals(b);//Equals should be overloaded as well
	}

	public static bool operator ==(RakString a, string b)
	{
 	   	// If both are null, or both are same instance, return true.
 		if (System.Object.ReferenceEquals(a, b))
 		{
 	       		return true;
 	   	}

  		// If one is null, but not both, return false.
   	 	if (((object)a == null) || ((object)b == null))
    		{
       		 	return false;
    		}

		    return a.Equals(b);//Equals should be overloaded as well
	}

	public static bool operator ==(RakString a, char b)
	{
 	   	// If both are null, or both are same instance, return true.
 		if (System.Object.ReferenceEquals(a, b))
 		{
 	       		return true;
 	   	}

  		// If one is null, but not both, return false.
   	 	if (((object)a == null) || ((object)b == null))
    		{
       		 	return false;
    		}

		    return a.Equals(b);//Equals should be overloaded as well
	}

	public static bool operator !=(RakString a, char b)
	{
   		 return !(a==b);
	}

	public static bool operator !=(RakString a, RakString b)
	{
   		 return a.OpNotEqual(b);
	}

	public static bool operator !=(RakString a, string b)
	{
   		 return a.OpNotEqual(b);
	}

	public static bool operator < (RakString a, RakString b)
	{
    		return a.OpLess(b);
	}

	public static bool operator >(RakString a, RakString b)
	{
		return a.OpGreater(b);
	}

	public static bool operator <=(RakString a, RakString b)
	{
		return a.OpLessEquals(b);
	}

	public static bool operator >=(RakString a, RakString b)
	{
		return a.OpGreaterEquals(b);
	}

	public char this[int index]  
 	{  
		get   
		{
			 return (char)OpArray((uint)index); // use indexto retrieve and return another value.    
		}  
		set   
		{
        		Replace((uint)index,1,(byte)value);// use index and value to set the value somewhere.   
		}  
	}  

	public static RakString operator +(RakString a, RakString b)
	{
		return RakNet.OpPlus(a,b);
	}

	public static implicit operator RakString(String s)
	{
		return new RakString(s);
	} 

	public static implicit operator RakString(char c)
	{
		return new RakString(c);
	} 

	public static implicit operator RakString(byte c)
	{
		return new RakString(c);
	} 
	
	public override string ToString()
	{
		return C_String();
	}

	public void SetChar(uint index, char inChar)
	{
		SetChar(index,(byte)inChar);
	}

	public void Replace(uint index, uint count, char inChar)
	{
		Replace(index,count,(byte)inChar);
	}
%}

%typemap(cscode) RakNet::AddressOrGUID
%{
	public static implicit operator AddressOrGUID(SystemAddress systemAddress)
	{
		return new AddressOrGUID(systemAddress);
	} 

	public static implicit operator AddressOrGUID(RakNetGUID guid)
	{
		return new AddressOrGUID(guid);
	} 


%}

%typemap (cscode) DataStructures::ByteQueue
%{
	public byte[] PeekContiguousBytes(out uint outLength) 
	{
		return PeekContiguousBytesHelper(out outLength);
	}
%}

/*
%typemap(cscode) RakNetSmartPtr<RakNetSocket>
%{
	public static bool operator ==(RakNetSmartPtrRakNetSocket a, RakNetSmartPtrRakNetSocket b)
	{
 	   	// If both are null, or both are same instance, return true.
 		if (System.Object.ReferenceEquals(a, b))
 		{
 	       		return true;
 	   	}

  		// If one is null, but not both, return false.
   	 	if (((object)a == null) || ((object)b == null))
    		{
       		 	return false;
    		}

		    return a.Equals(b);//Equals should be overloaded as well
	}

	public static bool operator !=(RakNetSmartPtrRakNetSocket a, RakNetSmartPtrRakNetSocket b)
	{
   		 return a.OpNotEqual(b);
	}

	public static bool operator < (RakNetSmartPtrRakNetSocket a, RakNetSmartPtrRakNetSocket b)
	{
    		return a.OpLess(b);
	}

	public static bool operator >(RakNetSmartPtrRakNetSocket a, RakNetSmartPtrRakNetSocket b)
	{
		return a.OpGreater(b);
	}
%}
*/

%typemap(csimports) RakNet::uint24_t
%{
using System;
using System.Runtime.InteropServices;
#pragma warning disable 0660
%}

%typemap(cscode) RakNet::uint24_t
%{

	public override int GetHashCode()
	{    
		return (int)this.val;
	}

	public static implicit operator uint24_t(uint inUint)
	{
		return new uint24_t(inUint);
	} 


	public static bool operator ==(uint24_t a, uint24_t b)
	{
 	   	// If both are null, or both are same instance, return true.
 		if (System.Object.ReferenceEquals(a, b))
 		{
 	       		return true;
 	   	}

  		// If one is null, but not both, return false.
   	 	if (((object)a == null) || ((object)b == null))
    		{
       		 	return false;
    		}

		    return a.Equals(b);//Equals should be overloaded as well
	}

	public static bool operator !=(uint24_t a, uint24_t b)
	{
   		 return a.OpNotEqual(b);
	}

	public static bool operator < (uint24_t a, uint24_t b)
	{
    		return a.OpLess(b);
	}

	public static bool operator >(uint24_t a, uint24_t b)
	{
		return a.OpGreater(b);
	}

	public static uint24_t operator +(uint24_t a, uint24_t b)
	{
		return a.OpPlus(b);
	}

	public static uint24_t operator ++(uint24_t a)
	{
		return a.OpPlusPlus();
	}

	public static uint24_t operator --(uint24_t a)
	{
		return a.OpMinusMinus();
	}

	public static uint24_t operator *(uint24_t a, uint24_t b)
	{
		return a.OpMultiply(b);
	}

	public static uint24_t operator /(uint24_t a, uint24_t b)
	{
		return a.OpDivide(b);
	}

	public static uint24_t operator -(uint24_t a, uint24_t b)
	{
		return a.OpMinus(b);
	}
//------------

	public static bool operator ==(uint24_t a, uint b)
	{
 	   	// If both are null, or both are same instance, return true.
 		if (System.Object.ReferenceEquals(a, b))
 		{
 	       		return true;
 	   	}

  		// If one is null, but not both, return false.
   	 	if (((object)a == null) || ((object)b == null))
    		{
       		 	return false;
    		}

		    return a.Equals(b);//Equals should be overloaded as well
	}

	public static bool operator !=(uint24_t a, uint b)
	{
   		 return a.OpNotEqual(b);
	}

	public static bool operator < (uint24_t a, uint b)
	{
    		return a.OpLess(b);
	}

	public static bool operator >(uint24_t a, uint b)
	{
		return a.OpGreater(b);
	}

	public static uint24_t operator +(uint24_t a, uint b)
	{
		return a.OpPlus(b);
	}

	public static uint24_t operator *(uint24_t a, uint b)
	{
		return a.OpMultiply(b);
	}

	public static uint24_t operator /(uint24_t a, uint b)
	{
		return a.OpDivide(b);
	}

	public static uint24_t operator -(uint24_t a, uint b)
	{
		return a.OpMinus(b);
	}

	public override string ToString()
	{
		return val.ToString();
	}

%}

%define STRUCT_UNSIGNED_CHAR_ARRAY_ONLY_CSCODE(BOOLNAME,CACHENAME)
%{
    private bool BOOLNAME = false;
    private byte[] CACHENAME;
%}
%enddef

%typemap(cscode) RakNet::FileListNode
%{
    private bool dataIsCached = false;
    private byte[] dataCache;
%}

%typemap(cscode) FileProgressStruct
%{
    private bool firstDataChunkIsCached = false;
    private byte[] firstDataChunkCache;
    private bool iriDataChunkIsCached = false;
    private byte[] iriDataChunkCache;
%}

%typemap(cscode) OnFileStruct
STRUCT_UNSIGNED_CHAR_ARRAY_ONLY_CSCODE(fileDataIsCached,fileDataCache)

//Packet->data related typemaps
%typemap(cscode) RakNet::Packet
STRUCT_UNSIGNED_CHAR_ARRAY_ONLY_CSCODE(dataIsCached,dataCache)

%typemap(cscode) RakNet::NetworkIDObject
%{
  NetworkIDManager oldManager;
  public virtual void SetNetworkIDManager(NetworkIDManager manager) 
  {
      if (oldManager != null)
      {
          oldManager.pointerDictionary.Remove(GetIntPtr());
      }
      if (manager != null)
      {
          manager.pointerDictionary.Add(GetIntPtr(), this);
          oldManager = manager;
      }
      SetNetworkIDManagerOrig(manager);
  }

  public IntPtr GetIntPtr()
  {
      return swigCPtr.Handle;
  }
%}

%typemap(csimports) RakNet::NetworkIDManager
%{
using System;
using System.Runtime.InteropServices;
using System.Collections.Generic;
%}

%typemap(cscode) RakNet::NetworkIDManager
%{
    public Dictionary<IntPtr, NetworkIDObject> pointerDictionary = new Dictionary<IntPtr, NetworkIDObject>();

     public NetworkIDObject GET_BASE_OBJECT_FROM_ID(ulong x)
     {
	return pointerDictionary[GET_BASE_OBJECT_FROM_IDORIG(x).GetIntPtr()];
     }

%}

%typemap(cscode) RakNet::ConnectionGraph2
%{
    public bool GetConnectionListForRemoteSystem(RakNetGUID remoteSystemGuid, SystemAddress[] saOut, RakNetGUID[] guidOut, ref uint inOutLength)
    {
        uint minLength = inOutLength;
        if (guidOut.Length < minLength)
        { minLength = (uint)guidOut.Length; }

        if (saOut.Length < minLength)
        { minLength = (uint)saOut.Length; }

        RakNetListRakNetGUID passListGUID = new RakNetListRakNetGUID();
        RakNetListSystemAddress passListSystemAddress = new RakNetListSystemAddress();

        bool returnVal = GetConnectionListForRemoteSystemHelper(remoteSystemGuid, passListSystemAddress, passListGUID, ref inOutLength);

        if (inOutLength< minLength)
        { minLength = (uint)inOutLength;}

        for (int i = 0; i < minLength; i++)
        {
            guidOut[i] = passListGUID[i];
            saOut[i] = passListSystemAddress[i];
        }
        return returnVal;
    }
    
    public void GetParticipantList(RakNetGUID[] participantList)
    {
			RakNetListRakNetGUID passListGUID = new RakNetListRakNetGUID();
			GetParticipantListHelper(passListGUID);
			for (int i = 0; i < participantList.Length && i < passListGUID.Size(); i++)
			{
			  participantList[i] = passListGUID[i];
			}
    }

%}


//Removed from interface, commented rather than removed in case needed later
/*
%typemap(cscode) InternalPacket
STRUCT_UNSIGNED_CHAR_ARRAY_ONLY_CSCODE(dataIsCached,dataCache)*/

%typemap(cscode) RakNet::RakNetStatistics
%{

	private bool bytesInSendBufferIsCached  = false;
	private bool messageInSendBufferIsCached  = false;
	private bool runningTotalIsCached  = false;
	private bool valueOverLastSecondIsCached  = false;
	private double[] bytesInSendBufferCache;
	private uint[] messageInSendBufferCache;
	private ulong[] runningTotalCache;
	private ulong[] valueOverLastSecondCache;
%}

%define STRUCT_CUSTOM_UNSIGNED_CHAR_ARRAY_TYPEMAP(BOOLNAME,CACHENAME,CTYPE,IN_DATA_CHANGE_FUNCTION,IN_DATA_GET_FUNCTION,IN_CLASS,IN_LEN_METHOD)
%typemap(cstype, out="byte[]") CTYPE "byte[]"

%typemap(csvarin, excode=SWIGEXCODE2) CTYPE %{
	set 
	{
	    	CACHENAME=value;
		BOOLNAME = true;
		IN_DATA_CHANGE_FUNCTION (value, value.Length);
	    

	}
%}
%typemap(csvarout, excode=SWIGEXCODE2) CTYPE  %{
        get
        {
            byte[] returnBytes;
            if (!BOOLNAME)
            {
                IntPtr cPtr = RakNetPINVOKE.IN_DATA_GET_FUNCTION (swigCPtr);
                int len = (int)((IN_CLASS)swigCPtr.Wrapper).IN_LEN_METHOD;
		if (len<=0)
		{
			return null;
		}
                returnBytes = new byte[len];
                Marshal.Copy(cPtr, returnBytes, 0, len);
                CACHENAME = returnBytes;
                BOOLNAME = true;
            }
            else
            {
                returnBytes = CACHENAME;
            }
            return returnBytes;
        }
 %}
%enddef

#ifdef SWIG_ADDITIONAL_AUTOPATCHER
	%typemap (cscode) RakNet::AutopatcherServer
	%{
		public void StartThreads(int numThreads, AutopatcherRepositoryInterface [] sqlConnectionPtrArray) 
		{
			RakNetListAutopatcherRepositoryInterfacePointer inSqlConnect =null;
 			if (sqlConnectionPtrArray!=null)
 			{
 				inSqlConnect = new RakNetListAutopatcherRepositoryInterfacePointer();
 				int listLen = sqlConnectionPtrArray.Length;
 				for (int i = 0; i < listLen; i++)
 				{
      		 		    inSqlConnect.Insert(sqlConnectionPtrArray[i], "", 1);
 				}
 			}

			StartThreadsHelper(numThreads,inSqlConnect);
		}
	%}

	%typemap (cscode) CompressorBase
	%{
		public byte[] GetOutput()
		{
			byte[] returnBytes= new byte[GetTotalOutputSize()];
			GetOutputHelper(returnBytes);
			return returnBytes;
		}
	%}

#endif
