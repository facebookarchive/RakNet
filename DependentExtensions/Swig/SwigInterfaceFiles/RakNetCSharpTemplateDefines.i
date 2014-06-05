//---------------------------------Template Defines-------------------------
//Swig needs to manually define templates you wish to use, this is done here.

%template(Serialize) RakNet::BitStream::Serialize <bool>;
%template(Serialize) RakNet::BitStream::Serialize <unsigned char>;
%template(Serialize) RakNet::BitStream::Serialize <short>;
%template(Serialize) RakNet::BitStream::Serialize <unsigned short>;
%template(Serialize) RakNet::BitStream::Serialize <long>;
%template(Serialize) RakNet::BitStream::Serialize <long long>;
%template(Serialize) RakNet::BitStream::Serialize <float>;
%template(Serialize) RakNet::BitStream::Serialize <RakNet::RakString>;
%template(Serialize) RakNet::BitStream::Serialize <RakNetGUID>;
%template(Serialize) RakNet::BitStream::Serialize <uint24_t>;

%template(SerializeDelta) RakNet::BitStream::SerializeDelta <bool>;
%template(SerializeDelta) RakNet::BitStream::SerializeDelta <unsigned char>;
%template(SerializeDelta) RakNet::BitStream::SerializeDelta <short>;
%template(SerializeDelta) RakNet::BitStream::SerializeDelta <unsigned short>;
%template(SerializeDelta) RakNet::BitStream::SerializeDelta <long>;
%template(SerializeDelta) RakNet::BitStream::SerializeDelta <long long>;
%template(SerializeDelta) RakNet::BitStream::SerializeDelta <float>;
%template(SerializeDelta) RakNet::BitStream::SerializeDelta <RakNet::RakString>;
%template(SerializeDelta) RakNet::BitStream::SerializeDelta <RakNetGUID>;
%template(SerializeDelta) RakNet::BitStream::SerializeDelta <uint24_t>;

%template(SerializeCompressed) RakNet::BitStream::SerializeCompressed <bool>;
%template(SerializeCompressed) RakNet::BitStream::SerializeCompressed <unsigned char>;
%template(SerializeCompressed) RakNet::BitStream::SerializeCompressed <short>;
%template(SerializeCompressed) RakNet::BitStream::SerializeCompressed <unsigned short>;
%template(SerializeCompressed) RakNet::BitStream::SerializeCompressed <long>;
%template(SerializeCompressed) RakNet::BitStream::SerializeCompressed <long long>;
%template(SerializeCompressed) RakNet::BitStream::SerializeCompressed <float>;
%template(SerializeCompressed) RakNet::BitStream::SerializeCompressed <RakNet::RakString>;
%template(SerializeCompressed) RakNet::BitStream::SerializeCompressed <RakNetGUID>;
%template(SerializeCompressed) RakNet::BitStream::SerializeCompressed <uint24_t>;

%template(SerializeCompressedDelta) RakNet::BitStream::SerializeCompressedDelta <bool>;
%template(SerializeCompressedDelta) RakNet::BitStream::SerializeCompressedDelta <unsigned char>;
%template(SerializeCompressedDelta) RakNet::BitStream::SerializeCompressedDelta <short>;
%template(SerializeCompressedDelta) RakNet::BitStream::SerializeCompressedDelta <unsigned short>;
%template(SerializeCompressedDelta) RakNet::BitStream::SerializeCompressedDelta <long>;
%template(SerializeCompressedDelta) RakNet::BitStream::SerializeCompressedDelta <long long>;
%template(SerializeCompressedDelta) RakNet::BitStream::SerializeCompressedDelta <float>;
%template(SerializeCompressedDelta) RakNet::BitStream::SerializeCompressedDelta <RakNet::RakString>;
%template(SerializeCompressedDelta) RakNet::BitStream::SerializeCompressedDelta <RakNetGUID>;
%template(SerializeCompressedDelta) RakNet::BitStream::SerializeCompressedDelta <uint24_t>;

%template(Write) RakNet::BitStream::Write <const char *>;
%template(Write) RakNet::BitStream::Write <bool>;
%template(Write) RakNet::BitStream::Write <unsigned char>;
%template(Write) RakNet::BitStream::Write <char>;
%template(Write) RakNet::BitStream::Write <short>;
%template(Write) RakNet::BitStream::Write <unsigned short>;
%template(Write) RakNet::BitStream::Write <long>;
%template(Write) RakNet::BitStream::Write <long long>;
%template(Write) RakNet::BitStream::Write <float>;
%template(Write) RakNet::BitStream::Write <RakNet::RakString>;
%template(Write) RakNet::BitStream::Write <RakNetGUID>;
%template(Write) RakNet::BitStream::Write <uint24_t>;

%template(WriteDelta) RakNet::BitStream::WriteDelta <const char *>;
%template(WriteDelta) RakNet::BitStream::WriteDelta <bool>;
%template(WriteDelta) RakNet::BitStream::WriteDelta <unsigned char>;
%template(WriteDelta) RakNet::BitStream::WriteDelta <char>;
%template(WriteDelta) RakNet::BitStream::WriteDelta <short>;
%template(WriteDelta) RakNet::BitStream::WriteDelta <unsigned short>;
%template(WriteDelta) RakNet::BitStream::WriteDelta <long>;
%template(WriteDelta) RakNet::BitStream::WriteDelta <long long>;
%template(WriteDelta) RakNet::BitStream::WriteDelta <float>;
%template(WriteDelta) RakNet::BitStream::WriteDelta <RakNet::RakString>;
%template(WriteDelta) RakNet::BitStream::WriteDelta <RakNetGUID>;
%template(WriteDelta) RakNet::BitStream::WriteDelta <uint24_t>;

%template(WriteCompressed) RakNet::BitStream::WriteCompressed <const char*>;
%template(WriteCompressed) RakNet::BitStream::WriteCompressed <bool>;
%template(WriteCompressed) RakNet::BitStream::WriteCompressed <unsigned char>;
%template(WriteCompressed) RakNet::BitStream::WriteCompressed <char>;
%template(WriteCompressed) RakNet::BitStream::WriteCompressed <short>;
%template(WriteCompressed) RakNet::BitStream::WriteCompressed <unsigned short>;
%template(WriteCompressed) RakNet::BitStream::WriteCompressed <long>;
%template(WriteCompressed) RakNet::BitStream::WriteCompressed <long long>;
%template(WriteCompressed) RakNet::BitStream::WriteCompressed <float>;
%template(WriteCompressed) RakNet::BitStream::WriteCompressed <RakNet::RakString>;
%template(WriteCompressed) RakNet::BitStream::WriteCompressed <RakNetGUID>;
%template(WriteCompressed) RakNet::BitStream::WriteCompressed <uint24_t>;

%template(WriteCompressedDelta) RakNet::BitStream::WriteCompressedDelta <const char *>;
%template(WriteCompressedDelta) RakNet::BitStream::WriteCompressedDelta <bool>;
%template(WriteCompressedDelta) RakNet::BitStream::WriteCompressedDelta <unsigned char>;
%template(WriteCompressedDelta) RakNet::BitStream::WriteCompressedDelta <char>;
%template(WriteCompressedDelta) RakNet::BitStream::WriteCompressedDelta <short>;
%template(WriteCompressedDelta) RakNet::BitStream::WriteCompressedDelta <unsigned short>;
%template(WriteCompressedDelta) RakNet::BitStream::WriteCompressedDelta <long>;
%template(WriteCompressedDelta) RakNet::BitStream::WriteCompressedDelta <long long>;
%template(WriteCompressedDelta) RakNet::BitStream::WriteCompressedDelta <float>;
%template(WriteCompressedDelta) RakNet::BitStream::WriteCompressedDelta <RakNet::RakString>;
%template(WriteCompressedDelta) RakNet::BitStream::WriteCompressedDelta <RakNetGUID>;
%template(WriteCompressedDelta) RakNet::BitStream::WriteCompressedDelta <uint24_t>;

%template(Read) RakNet::BitStream::Read <bool>;
%template(Read) RakNet::BitStream::Read <unsigned char>;
%template(Read) RakNet::BitStream::Read <short>;
%template(Read) RakNet::BitStream::Read <unsigned short>;
%template(Read) RakNet::BitStream::Read <long>;
%template(Read) RakNet::BitStream::Read <long long>;
%template(Read) RakNet::BitStream::Read <float>;
%template(Read) RakNet::BitStream::Read <RakNet::RakString>;
%template(Read) RakNet::BitStream::Read <RakNetGUID>;
%template(Read) RakNet::BitStream::Read <uint24_t>;

%template(ReadDelta) RakNet::BitStream::ReadDelta <bool>;
%template(ReadDelta) RakNet::BitStream::ReadDelta <unsigned char>;
%template(ReadDelta) RakNet::BitStream::ReadDelta <short>;
%template(ReadDelta) RakNet::BitStream::ReadDelta <unsigned short>;
%template(ReadDelta) RakNet::BitStream::ReadDelta <long>;
%template(ReadDelta) RakNet::BitStream::ReadDelta <long long>;
%template(ReadDelta) RakNet::BitStream::ReadDelta <float>;
%template(ReadDelta) RakNet::BitStream::ReadDelta <RakNet::RakString>;
%template(ReadDelta) RakNet::BitStream::ReadDelta <RakNetGUID>;
%template(ReadDelta) RakNet::BitStream::ReadDelta <uint24_t>;

%template(ReadCompressed) RakNet::BitStream::ReadCompressed <bool>;
%template(ReadCompressed) RakNet::BitStream::ReadCompressed <unsigned char>;
%template(ReadCompressed) RakNet::BitStream::ReadCompressed <short>;
%template(ReadCompressed) RakNet::BitStream::ReadCompressed <unsigned short>;
%template(ReadCompressed) RakNet::BitStream::ReadCompressed <long>;
%template(ReadCompressed) RakNet::BitStream::ReadCompressed <long long>;
%template(ReadCompressed) RakNet::BitStream::ReadCompressed <float>;
%template(ReadCompressed) RakNet::BitStream::ReadCompressed <RakNet::RakString>;
%template(ReadCompressed) RakNet::BitStream::ReadCompressed <RakNetGUID>;
%template(ReadCompressed) RakNet::BitStream::ReadCompressed <uint24_t>;

%template(ReadCompressedDelta) RakNet::BitStream::ReadCompressedDelta <bool>;
%template(ReadCompressedDelta) RakNet::BitStream::ReadCompressedDelta <unsigned char>;
%template(ReadCompressedDelta) RakNet::BitStream::ReadCompressedDelta <short>;
%template(ReadCompressedDelta) RakNet::BitStream::ReadCompressedDelta <unsigned short>;
%template(ReadCompressedDelta) RakNet::BitStream::ReadCompressedDelta <long>;
%template(ReadCompressedDelta) RakNet::BitStream::ReadCompressedDelta <long long>;
%template(ReadCompressedDelta) RakNet::BitStream::ReadCompressedDelta <float>;
%template(ReadCompressedDelta) RakNet::BitStream::ReadCompressedDelta <RakNet::RakString>;
%template(ReadCompressedDelta) RakNet::BitStream::ReadCompressedDelta <RakNetGUID>;
%template(ReadCompressedDelta) RakNet::BitStream::ReadCompressedDelta <uint24_t>;

%define ADD_LIST_TYPE(CTYPE,CSTYPE,RENAME_TYPE)
%typemap(cscode) DataStructures::List<CTYPE>
%{
    public CSTYPE this[int index]  
    {  
        get   
        {
            return Get((uint)index); // use indexto retrieve and return another value.    
        }  
        set   
        {
            Replace(value, value, (uint)index, "Not used", 0);// use index and value to set the value somewhere.   
        }  
    } 
%}

%template(RENAME_TYPE) DataStructures::List <CTYPE>;
%enddef

ADD_LIST_TYPE(RakNet::RakNetGUID,RakNetGUID,RakNetListRakNetGUID)
ADD_LIST_TYPE(RakNet::SystemAddress,SystemAddress,RakNetListSystemAddress)
ADD_LIST_TYPE(RakNet::RakString,RakString,RakNetListRakString)
ADD_LIST_TYPE(Cell,Cell,RakNetListCell)
ADD_LIST_TYPE(ColumnDescriptor,ColumnDescriptor,RakNetListColumnDescriptor)
ADD_LIST_TYPE(Row,Row,RakNetListTableRow);
ADD_LIST_TYPE(RakNet::FileListNode,FileListNode,RakNetListFileListNode);
ADD_LIST_TYPE(FilterQuery,FilterQuery,RakNetListFilterQuery);
ADD_LIST_TYPE(SortQuery,SortQuery,RakNetListSortQuery);

// 1/1/2011 Commented out below line: Doesn't build into RakNet_wrap.cxx properly
// %template(RakNetSmartPtrRakNetSocket) RakNetSmartPtr<RakNetSocket>;

//Can't use the macro because it won't include the space then nested templates won't work
/*
%typemap(cscode) DataStructures::List<RakNetSmartPtr<RakNetSocket> >
%{
    public RakNetSmartPtrRakNetSocket this[int index]  
    {  
        get   
        {
            return Get((uint)index); // use indexto retrieve and return another value.    
        }  
        set   
        {
            Replace(value, value, (uint)index, "Not used", 0);// use index and value to set the value somewhere.   
        }  
    } 
%}

%template(RakNetListRakNetSmartPtrRakNetSocket) DataStructures::List <RakNetSmartPtr<RakNetSocket> >;
*/

%define ADD_POINTER_LIST_TYPE(CTYPE,CSTYPE,RENAME_TYPE)
%ignore DataStructures::List<CTYPE>::Get;
%ignore DataStructures::List<CTYPE>::Pop;

%typemap(cscode) DataStructures::List<CTYPE>
%{
    public CSTYPE this[int index]  
    {  
        get   
        {
            return Get((uint)index); // use indexto retrieve and return another value.    
        }  
        set   
        {
            Replace(value, value, (uint)index, "Not used", 0);// use index and value to set the value somewhere.   
        }  
    }


    public CSTYPE Get(uint position) 
    {   
	return GetHelper(position);
    }

    public CSTYPE Pop()
    {
	return PopHelper();
    }

%}

%extend DataStructures::List<CTYPE>
{
	CTYPE GetHelper ( const unsigned int position ) const
	{
		return self->Get(position);
        }


	CTYPE PopHelper () 
	{
		return self->Pop();
        }
}

%template(RENAME_TYPE) DataStructures::List <CTYPE>;
%enddef

ADD_POINTER_LIST_TYPE(Cell *,Cell,RakNetListCellPointer)
#ifdef SWIG_ADDITIONAL_AUTOPATCHER
	ADD_POINTER_LIST_TYPE(AutopatcherRepositoryInterface *,AutopatcherRepositoryInterface,RakNetListAutopatcherRepositoryInterfacePointer)
#endif

%define ADD_PRIMITIVE_LIST_TYPE(CTYPE,CSTYPE,RENAME_TYPE,SWIG_TYPE,POINTER_NAME)
%pointer_class(CTYPE, POINTER_NAME)

%csmethodmodifiers DataStructures::List <CTYPE>::Get "private"
%csmethodmodifiers DataStructures::List <CTYPE>::Pop "private"

%rename(GetHelper) DataStructures::List <CTYPE>::Get;
%rename(PopHelper) DataStructures::List <CTYPE>::Pop;

%typemap(cscode) DataStructures::List <CTYPE>
%{
  public CSTYPE Get(uint position) {
    SWIG_TYPE ret = GetHelper(position);
    return POINTER_NAME.frompointer(ret).value();
  }

  public CSTYPE Pop() {
    SWIG_TYPE ret = PopHelper();
    return POINTER_NAME.frompointer(ret).value();
  }
    public CSTYPE this[int index]  
    {  
        get   
        {
            return Get((uint)index); // use indexto retrieve and return another value.    
        }  
        set   
        {
            Replace(value, value, (uint)index, "Not used", 0);// use index and value to set the value somewhere.   
        }  
    } 
%}

%template(RENAME_TYPE) DataStructures::List <CTYPE>;
%enddef

ADD_PRIMITIVE_LIST_TYPE(unsigned short,ushort,RakNetListUnsignedShort,SWIGTYPE_p_unsigned_short,UnsignedShortPointer)
ADD_PRIMITIVE_LIST_TYPE(unsigned,uint,RakNetListUnsignedInt,SWIGTYPE_p_unsigned_int,UnsignedIntPointer)

%template(RakNetPageRow) DataStructures::Page<unsigned, DataStructures::Table::Row*, _TABLE_BPLUS_TREE_ORDER>;

//needed here for scoping issues
%ignore DataStructures::BPlusTree<unsigned, DataStructures::Table::Row*, _TABLE_BPLUS_TREE_ORDER>::Delete;
%ignore DataStructures::BPlusTree<unsigned, DataStructures::Table::Row*, _TABLE_BPLUS_TREE_ORDER>::Get;

%csmethodmodifiers DataStructures::BPlusTree<unsigned, DataStructures::Table::Row*, _TABLE_BPLUS_TREE_ORDER>::DeleteHelper "private";
%csmethodmodifiers DataStructures::BPlusTree<unsigned, DataStructures::Table::Row*, _TABLE_BPLUS_TREE_ORDER>::GetHelper "private";

%template(RakNetBPlusTreeRow) DataStructures::BPlusTree<unsigned, DataStructures::Table::Row*, _TABLE_BPLUS_TREE_ORDER>;

%typemap(cscode) DataStructures::BPlusTree<unsigned, DataStructures::Table::Row*, _TABLE_BPLUS_TREE_ORDER>
%{
	public bool Get(uint key, ref Row arg1) 
	{
		bool outBool;

		arg1=GetHelper(key,arg1,out outBool);

  		return outBool;
  	}

  	public bool Delete(uint key) 
	{
    		return DeleteHelper(key);
  	}

	public bool Delete(uint key, ref Row arg1) 
	{
		bool outBool;
		
		arg1=DeleteHelper(key,arg1,out outBool);

  		return outBool;
 	}
%}

%define ADD_STANDARD_MULTILIST_TYPE(CTYPE,CSTYPE,RENAME_TYPE)
%typemap(cscode) DataStructures::Multilist <ML_STACK,CTYPE,CTYPE,DefaultIndexType>
%{
    public CSTYPE this[int index]  
    {  
        get   
        {
            return OpArray((uint)index); // use indexto retrieve and return another value.    
        }  
        set   
        {
            RemoveAtIndex((uint)index);
            InsertAtIndex(value,(uint)index);
	}  
    } 
%}

%template(RENAME_TYPE) DataStructures::Multilist <ML_STACK,CTYPE,CTYPE,DefaultIndexType>;
%enddef

#ifdef SWIG_ADDITIONAL_SQL_LITE
ADD_STANDARD_MULTILIST_TYPE(SQLite3Row*,SQLite3Row,RakNetMultiListML_StackSQLite3RowP)
ADD_STANDARD_MULTILIST_TYPE(RakNet::RakString,RakString,RakNetMultiListML_StackRakString)
ADD_STANDARD_MULTILIST_TYPE(RakNet::SQLite3PluginResultInterface *,SQLite3PluginResultInterface,RakNetMultiListML_StackSQLite3PluginResultInterfaceP)
#endif