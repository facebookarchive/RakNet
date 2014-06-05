//----------------------------Additional Class/Struct Defines-----------------------
//These is here because it is a nested class and swig doesn't handle nested/structs/classes, this presents the interface for this struct. If the struct changes in the code so must this

//These are DataStructures::Table

/// Stores the name and type of the column
/// \internal
struct  ColumnDescriptor
{
 	ColumnDescriptor();
 	~ColumnDescriptor();
 	ColumnDescriptor(const char cn[_TABLE_MAX_COLUMN_NAME_LENGTH],DataStructures::Table::ColumnType ct);
 
 	char columnName[_TABLE_MAX_COLUMN_NAME_LENGTH];
 	DataStructures::Table::ColumnType columnType;
};
 
struct  Cell
{
 	Cell();
 	~Cell();
 	Cell(double numericValue, char *charValue, void *ptr, DataStructures::Table::ColumnType type);
 	void SetByType(double numericValue, char *charValue, void *ptr, DataStructures::Table::ColumnType type);
 	void Clear(void);
 			
 	/// Numeric
 	void Set(int input);
 	void Set(unsigned int input);
 	void Set(double input);
 
 	/// String
 	void Set(const char *input);
 	
 	/// Binary
 	void Set(const char *input, int inputLength);
 
 	/// Pointer
 	void SetPtr(void* p);
 
 	/// Numeric
 	void Get(int *output);
 	void Get(double *output);
 
 	/// String
 	void Get(char *output);
 
 	/// Binary
 	void Get(char *output, int *outputLength);
 
 	RakNet::RakString ToString(DataStructures::Table::ColumnType columnType);
 
 	// assignment operator and copy constructor
 	Cell& operator = ( const Cell& input );
 	Cell( const Cell & input);
 
 	DataStructures::Table::ColumnType EstimateColumnType(void) const;
 
 	bool isEmpty;
 	double i;
 	char *c;
 	void *ptr;
};
 
 /// Stores the list of cells for this row, and a special flag used for internal sorting
struct  Row
{
 	// list of cells
 	DataStructures::List<Cell*> cells;
 
 	/// Numeric
 	void UpdateCell(unsigned columnIndex, double value);
 
 	/// String
 	void UpdateCell(unsigned columnIndex, const char *str);
 
 	/// Binary
 	void UpdateCell(unsigned columnIndex, int byteLength, const char *data);
};
 		
// Compare the cell value for a row at columnName to the cellValue using operation.
struct  FilterQuery
{
 	FilterQuery();
 	~FilterQuery();
 	FilterQuery(unsigned column, Cell *cell, DataStructures::Table::FilterQueryType op);
 
 	// If columnName is specified, columnIndex will be looked up using it.
 	char columnName[_TABLE_MAX_COLUMN_NAME_LENGTH];
 	unsigned columnIndex;
 	Cell *cellValue;
 	DataStructures::Table::FilterQueryType operation;
};
 
 		
// Sort on increasing or decreasing order for a particular column
struct  SortQuery
{
 	/// The index of the table column we are sorting on
 	unsigned columnIndex;
 
 	/// See SortQueryType
 	DataStructures::Table::SortQueryType operation;
};


//FileListTransferCBInterFace
struct OnFileStruct
{
	/// \brief The index into the set of files, from 0 to numberOfFilesInThisSet
	unsigned fileIndex;

	/// \brief The name of the file
	char fileName[512];

	/// \brief The data pointed to by the file
	char *fileData;

	/// \brief The actual length of this file.
	BitSize_t byteLengthOfThisFile;

	/// \brief How many bytes of this file has been downloaded
	BitSize_t bytesDownloadedForThisFile;

	/// \brief Files are transmitted in sets, where more than one set of files can be transmitted at the same time.
	/// \details This is the identifier for the set, which is returned by FileListTransfer::SetupReceive
	unsigned short setID;

	/// \brief The number of files that are in this set.
	unsigned numberOfFilesInThisSet;

	/// \brief The total length of the transmitted files for this set, after being uncompressed
	unsigned byteLengthOfThisSet;

	/// \brief The total length, in bytes, downloaded for this set.
	unsigned bytesDownloadedForThisSet;

	/// \brief User data passed to one of the functions in the FileList class.
	/// \details However, on error, this is instead changed to one of the enumerations in the PatchContext structure.
	FileListNodeContext context;

	/// \brief Who sent this file
	SystemAddress senderSystemAddress;

	/// \brief Who sent this file. Not valid when using TCP, only RakPeer (UDP)
	RakNetGUID senderGuid;
};

struct FileProgressStruct
{
	/// \param[out] onFileStruct General information about this file, such as the filename and the first \a partLength bytes. You do NOT need to save this data yourself. The complete file will arrive normally.
	OnFileStruct *onFileStruct;
	/// \param[out] partCount The zero based index into partTotal. The percentage complete done of this file is 100 * (partCount+1)/partTotal
	unsigned int partCount;
	/// \param[out] partTotal The total number of parts this file was split into. Each part will be roughly the MTU size, minus the UDP header and RakNet headers
	unsigned int partTotal;
	/// \param[out] dataChunkLength How many bytes long firstDataChunk and iriDataChunk are
	unsigned int dataChunkLength;
	/// \param[out] firstDataChunk The first \a partLength of the final file. If you store identifying information about the file in the first \a partLength bytes, you can read them while the download is taking place. If this hasn't arrived yet, firstDataChunk will be 0
	char *firstDataChunk;
	/// \param[out] iriDataChunk If the remote system is sending this file using IncrementalReadInterface, then this is the chunk we just downloaded. It will not exist in memory after this callback. You should either store this to disk, or in memory. If it is 0, then the file is smaller than one chunk, and will be held in memory automatically
	char *iriDataChunk;
	/// \param[out] iriWriteOffset Offset in bytes from the start of the file for the data pointed to by iriDataChunk
	unsigned int iriWriteOffset;
	/// \param[out] Who sent this file
	SystemAddress senderSystemAddress;
	/// \param[out] Who sent this file. Not valid when using TCP, only RakPeer (UDP)
	RakNetGUID senderGuid;
	/// \param[in] allocateIrIDataChunkAutomatically If true, then RakNet will hold iriDataChunk for you and return it in OnFile. Defaults to true
	bool allocateIrIDataChunkAutomatically;
};

struct DownloadCompleteStruct
{
	/// \brief Files are transmitted in sets, where more than one set of files can be transmitted at the same time.
	/// \details This is the identifier for the set, which is returned by FileListTransfer::SetupReceive
	unsigned short setID;

	/// \brief The number of files that are in this set.
	unsigned numberOfFilesInThisSet;

	/// \brief The total length of the transmitted files for this set, after being uncompressed
	unsigned byteLengthOfThisSet;

	/// \brief Who sent this file
	SystemAddress senderSystemAddress;

	/// \brief Who sent this file. Not valid when using TCP, only RakPeer (UDP)
	RakNetGUID senderGuid;
};