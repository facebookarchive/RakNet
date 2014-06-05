#include "SQLiteServerLoggerPlugin.h"
#include "RakPeerInterface.h"
#include "PacketizedTCP.h"
#include "MessageIdentifiers.h"
#include "SQLiteLoggerCommon.h"
#include "jpeglib.h"
#include "jpeg_memory_dest.h"
#include "FileOperations.h"
#include "GetTime.h"
#include <time.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/timeb.h>
#include "DXTCompressor.h"

// http://web.utk.edu/~jplyon/sqlite/SQLite_optimization_FAQ.html

// See jmorecfg.h
/*
* Ordering of RGB data in scanlines passed to or from the application.
* If your application wants to deal with data in the order B,G,R, just
* change these macros.  You can also deal with formats such as R,G,B,X
* (one extra byte per pixel) by changing RGB_PIXELSIZE.  Note that changing
* the offsets will also change the order in which colormap data is organized.
* RESTRICTIONS:
* 1. The sample applications cjpeg,djpeg do NOT support modified RGB formats.
* 2. These macros only affect RGB<=>YCbCr color conversion, so they are not
*    useful if you are using JPEG color spaces other than YCbCr or grayscale.
* 3. The color quantizer modules will not behave desirably if RGB_PIXELSIZE
*    is not 3 (they don't understand about dummy color components!).  So you
*    can't use color quantization if you change that value.
*/

//#define RGB_RED		0	/* Offset of Red in an RGB scanline element */
//#define RGB_GREEN	1	/* Offset of Green */
//#define RGB_BLUE	2	/* Offset of Blue */
//#define RGB_PIXELSIZE	3	/* JSAMPLEs per RGB scanline element */

using namespace RakNet;

// JPEG ENCODING ERRORS
struct my_error_mgr {
	struct jpeg_error_mgr pub;
};
METHODDEF(void) my_error_exit (j_common_ptr cinfo) {}

#define FILE_COLUMN "SourceFile"
#define LINE_COLUMN "SourceLine"
#define TICK_COUNT_COLUMN "AutoTickCount"
#define AUTO_IP_COLUMN "AutoIPAddress"
#define TIMESTAMP_NUMERIC_COLUMN "TimestampNumeric"
#define TIMESTAMP_TEXT_COLUMN "TimestampText"
#define FUNCTION_CALL_FRIENDLY_TEXT "FunctionCallFriendlyText"
#define FUNCTION_CALL_TABLE "'functionCalls'"
#define FUNCTION_CALL_PARAMETERS_TABLE "'functionCallParameters'"

// Store packets in the CPUThreadInput until at most this much time has elapsed. This is so
// batch processing can occur on multiple image sources at once
static const RakNet::TimeMS MAX_TIME_TO_BUFFER_PACKETS=1000;

// WTF am I getting this?
// 2>SQLiteServerLoggerPlugin.obj : error LNK2019: unresolved external symbol _GetSqlDataTypeName referenced in function "struct RakNet::SQLite3ServerPlugin::SQLExecThreadOutput __cdecl ExecSQLLoggingThread(struct RakNet::SQLite3ServerPlugin::SQLExecThreadInput,bool *,void *)" (?ExecSQLLoggingThread@@YA?AUExecThreadOutput@SQLite3ServerPlugin@RakNet@@UExecThreadInput@23@PA_NPAX@Z)
// 2>C:\RakNet\Debug\SQLiteServerLogger.exe : fatal error LNK1120: 1 unresolved externals
static const char *sqlDataTypeNames[SQLLPDT_COUNT] = 
{
	"INTEGER",
	"INTEGER",
	"NUMERIC",
	"TEXT",
	"BLOB",
	"BLOB",
};
const char *GetSqlDataTypeName2(SQLLoggerPrimaryDataType idx) {return sqlDataTypeNames[(int)idx];}

void CompressAsJpeg(char **cptrInOut, uint32_t *sizeInOut, uint16_t imageWidth, uint16_t imageHeight, int16_t linePitch, unsigned char input_components)
{
	RakNet::TimeUS t1=RakNet::GetTimeUS();

	// Compress to jpg
	// http://www.google.com/codesearch/p?hl=en#I-_InJ6STRE/gthumb-1.108/libgthumb/pixbuf-utils.c&q=jpeg_create_compress
	// http://ftp.gnome.org	/ pub/	GNOME	/	sources	/gthumb	/1.108/	gthumb-1.108.tar.gz/ 

	struct jpeg_compress_struct cinfo;
	struct my_error_mgr jerr;
	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = my_error_exit;
	jpeg_create_compress(&cinfo);
	cinfo.smoothing_factor = 0;
	cinfo.optimize_coding = false;
	cinfo.image_width      = imageWidth;
	cinfo.image_height     = imageHeight;
	cinfo.input_components = input_components;
	cinfo.in_color_space = JCS_RGB;


	jpeg_set_defaults (&cinfo);
	cinfo.dct_method=JDCT_FLOAT;
	// Not sure why they made RGB_PIXELSIZE a global define. I added this so it only uses my value in this one place, to not break other things
	cinfo.hack_use_input_components_as_RGB_PIXELSIZE=1;
	jpeg_set_quality (&cinfo, 75, TRUE);
	int jpegSizeAfterCompression = 0; //size of jpeg after compression
	char * storage = (char*) rakMalloc_Ex(*sizeInOut+50000,_FILE_AND_LINE_);
	jpeg_memory_dest(&cinfo,(JOCTET*)storage,*sizeInOut+50000,&jpegSizeAfterCompression);

	JSAMPROW row_pointer[1];
	jpeg_start_compress (&cinfo, TRUE);
	while( cinfo.next_scanline < cinfo.image_height )
	{
		row_pointer[0] = (JSAMPROW) &((*cptrInOut)[cinfo.next_scanline * linePitch ]);
		jpeg_write_scanlines(&cinfo, row_pointer, 1);

	}

	/* finish off */
	jpeg_finish_compress (&cinfo);
	jpeg_destroy_compress(&cinfo);

	rakFree_Ex(*cptrInOut,_FILE_AND_LINE_);
	*cptrInOut = (char*) rakRealloc_Ex(storage, jpegSizeAfterCompression,_FILE_AND_LINE_);
	*sizeInOut=jpegSizeAfterCompression;

	RakNet::TimeUS t2=RakNet::GetTimeUS();
	RakNet::TimeUS diff=t2-t1;
}
static bool needsDxtInit=true;
static bool dxtCompressionSupported=false;
void* InitDxt()
{
	if (needsDxtInit==true)
	{
		dxtCompressionSupported=DXTCompressor::Initialize();
		if (dxtCompressionSupported==false)
		{
			printf("Warning, DXT compressor failed to start.\nImages will be compressed with jpg instead.\n");
		}
	}
	needsDxtInit=false;
	return 0;
}
void DeinitDxt(void*)
{
	if (dxtCompressionSupported)
		DXTCompressor::Shutdown();
	needsDxtInit=true;
	dxtCompressionSupported=false;
}
SQLiteServerLoggerPlugin::CPUThreadOutput* ExecCPULoggingThread(SQLiteServerLoggerPlugin::CPUThreadInput* cpuThreadInput, bool *returnOutput, void* perThreadData)
{
	int i;
	*returnOutput=true;
	SQLiteServerLoggerPlugin::CPUThreadOutput *cpuThreadOutput = RakNet::OP_NEW<SQLiteServerLoggerPlugin::CPUThreadOutput>(_FILE_AND_LINE_);
	cpuThreadOutput->arraySize=cpuThreadInput->arraySize;
	//cpuThreadOutput->cpuOutputNodeArray=RakNet::OP_NEW_ARRAY<SQLiteServerLoggerPlugin::CPUThreadOutputNode*>(cpuThreadInput->arraySize,_FILE_AND_LINE_);
	//printf("1. arraySize=%i, ",cpuThreadInput->arraySize);
	for (i=0; i<cpuThreadInput->arraySize; i++)
	{
		cpuThreadOutput->cpuOutputNodeArray[i]=RakNet::OP_NEW<SQLiteServerLoggerPlugin::CPUThreadOutputNode>(_FILE_AND_LINE_);
		SQLiteServerLoggerPlugin::CPUThreadOutputNode *outputNode = cpuThreadOutput->cpuOutputNodeArray[i];
		Packet *packet = cpuThreadInput->cpuInputArray[i].packet;
		RakNet::RakString dbIdentifier = cpuThreadInput->cpuInputArray[i].dbIdentifier;
		// outputNode->whenMessageArrived = cpuThreadInput->cpuInputArray[i].whenMessageArrived;
		outputNode->packet=packet;
		
		packet->systemAddress.ToString(true,outputNode->ipAddressString);
		RakNet::BitStream bitStream(packet->data, packet->length, false);
		bitStream.IgnoreBytes(1);
		bitStream.Read(outputNode->dbIdentifier);
		bitStream.Read(outputNode->tableName);
		outputNode->tableName.SQLEscape();
		bitStream.Read(outputNode->line);
		bitStream.Read(outputNode->file);
		bitStream.Read(outputNode->tickCount);
		bitStream.Read(outputNode->clientSendingTime);
		bitStream.Read(outputNode->isFunctionCall);
		bitStream.Read(outputNode->parameterCount);
		if (outputNode->isFunctionCall==false)
		{
			RakNet::RakString columnName;
		//	printf("2. parameterCount=%i, ",outputNode->parameterCount);
			for (int i=0; i < outputNode->parameterCount; i++)
			{
				bitStream.Read(columnName);
				columnName.SQLEscape();
				columnName.RemoveCharacter(' ');
				outputNode->insertingColumnNames.Push(columnName, _FILE_AND_LINE_ );
			}
		}

		int parameterCountIndex=0;
	//	printf("3. parameterCount=%i, ",outputNode->parameterCount);
		while (parameterCountIndex < outputNode->parameterCount)
		{
			outputNode->parameterList[parameterCountIndex].Deserialize(&bitStream);

			if (outputNode->parameterList[parameterCountIndex].size>0)
			{
				parameterCountIndex++;
			}
			else
			{
				// Skip empty parameters
				if (outputNode->isFunctionCall==false)
					outputNode->insertingColumnNames.RemoveAtIndex(parameterCountIndex);
				outputNode->parameterCount--;
			}
		}

//		sqlite3_stmt *statement;
//		char *errorMsg;
	//	printf("4. parameterCount=%i, ",outputNode->parameterCount);
		for (parameterCountIndex=0; parameterCountIndex < outputNode->parameterCount; parameterCountIndex++)
		{
			if (outputNode->parameterList[parameterCountIndex].type==SQLLPDT_IMAGE)
			{

				bool dxtCompressSuccess=false;
				if (dxtCompressionSupported)
				{
					char *outputData;
					int bufferSize = DXTCompressor::GetBufferSize(DXT1,
						outputNode->parameterList[parameterCountIndex].imageWidth,
						outputNode->parameterList[parameterCountIndex].imageHeight);
					int ddsHeaderSize = DXTCompressor::GetDDSHeaderSize();
					outputData = (char*) rakMalloc_Ex(bufferSize + ddsHeaderSize, _FILE_AND_LINE_ );
					dxtCompressSuccess = DXTCompressor::CompressImageData(
					DXT1,
					outputNode->parameterList[parameterCountIndex].data.cptr,
					outputNode->parameterList[parameterCountIndex].imageWidth,
					outputNode->parameterList[parameterCountIndex].imageHeight,
					outputData+ddsHeaderSize, false, outputNode->parameterList[parameterCountIndex].sourceFormatIsBGRA );

					if (dxtCompressSuccess)
					{
						rakFree_Ex(outputNode->parameterList[parameterCountIndex].data.cptr,_FILE_AND_LINE_);
						DXTCompressor::WriteDDSHeader(DXT1,
							outputNode->parameterList[parameterCountIndex].imageWidth,
							outputNode->parameterList[parameterCountIndex].imageHeight,
							bufferSize,
							outputData);
						outputNode->parameterList[parameterCountIndex].data.cptr=outputData;
						outputNode->parameterList[parameterCountIndex].size=bufferSize + ddsHeaderSize;

// 						static bool testWriteToDisk=true;
// 						if (testWriteToDisk)
// 						{
// 							printf("Wrote test.dds\n");
// 							FILE *fp = fopen("test.dds", "wb");
// 							fwrite(outputData,1,outputNode->parameterList[parameterCountIndex].size,fp);
// 							fclose(fp);
// 							testWriteToDisk=false;
// 						}
					}
					else
					{
						rakFree_Ex(outputData,_FILE_AND_LINE_);
					}
				}

				if (dxtCompressSuccess==false)
				{
					if (outputNode->parameterList[parameterCountIndex].sourceFormatIsBGRA)
					{
						// Endian swap each color component. Input should be RGBA
						//					int pixelIndex;
						//					int rowIndex;
						int imageHeight = outputNode->parameterList[parameterCountIndex].imageHeight;
						//					int imageWidth = outputNode->parameterList[parameterCountIndex].imageWidth;
						int bytesPerPixel = outputNode->parameterList[parameterCountIndex].input_components;
						int linePitch = outputNode->parameterList[parameterCountIndex].linePitch;
						char *dataPtr = outputNode->parameterList[parameterCountIndex].data.cptr;
						int totalBytes=linePitch*imageHeight;
						char *endPtr = dataPtr+totalBytes;
						unsigned char temp1;
						if (bytesPerPixel==3)
						{
							while (dataPtr!=endPtr)
							{
								temp1=dataPtr[2];
								dataPtr[2]=dataPtr[0];
								dataPtr[0]=temp1;
								dataPtr+=3;
							}
						}
						else
						{
							RakAssert(bytesPerPixel==4);
							while (dataPtr!=endPtr)
							{
								temp1=dataPtr[2];
								dataPtr[2]=dataPtr[0];
								dataPtr[0]=temp1;
								dataPtr+=4;
							}
						}
					}

					CompressAsJpeg(
						&outputNode->parameterList[parameterCountIndex].data.cptr,
						&outputNode->parameterList[parameterCountIndex].size,
						outputNode->parameterList[parameterCountIndex].imageWidth,
						outputNode->parameterList[parameterCountIndex].imageHeight,
						outputNode->parameterList[parameterCountIndex].linePitch,
						outputNode->parameterList[parameterCountIndex].input_components
						);
// 
// 						static bool testWriteToDisk=true;
// 						if (testWriteToDisk)
// 						{
// 							printf("Wrote test.jpg\n");
// 							FILE *fp = fopen("test.jpg", "wb");
// 							fwrite(outputNode->parameterList[parameterCountIndex].data.cptr,1,outputNode->parameterList[parameterCountIndex].size,fp);
// 							fclose(fp);
// 							testWriteToDisk=false;
// 						}

				}
			}
		}	
	}

//	printf("5. out1, ");
	RakNet::OP_DELETE(cpuThreadInput,_FILE_AND_LINE_);
//	printf("6. out2\n");
	return cpuThreadOutput;
}
struct SQLPreparedStatements
{
	SQLPreparedStatements() 
	{
		selectNameFromMaster=0;
		insertIntoFunctionCalls=0;
		insertIntoFunctionCallParameters=0;
	}
	sqlite3_stmt *selectNameFromMaster;
	sqlite3_stmt *insertIntoFunctionCalls;
	sqlite3_stmt *insertIntoFunctionCallParameters;
};
void* SQLLoggerThreadAllocPreparedStatements()
{
	SQLPreparedStatements *s = RakNet::OP_NEW<SQLPreparedStatements>(_FILE_AND_LINE_);
	return s;
}
void SQLLoggerThreadDeallocPreparedStatements(void* statementStruct)
{
	SQLPreparedStatements *s = (SQLPreparedStatements *) statementStruct;
	if (s->selectNameFromMaster) sqlite3_finalize(s->selectNameFromMaster);
	if (s->insertIntoFunctionCalls) sqlite3_finalize(s->insertIntoFunctionCalls);
	if (s->insertIntoFunctionCallParameters) sqlite3_finalize(s->insertIntoFunctionCallParameters);
	RakNet::OP_DELETE(s,_FILE_AND_LINE_);
}
void DeleteBlobOrText(void* v)
{
	LogParameter::Free(v);
}
SQLiteServerLoggerPlugin::SQLThreadOutput ExecSQLLoggingThread(SQLiteServerLoggerPlugin::SQLThreadInput sqlThreadInput, bool *returnOutput, void* perThreadData)
{
	SQLiteServerLoggerPlugin::CPUThreadOutputNode *cpuOutputNode = sqlThreadInput.cpuOutputNode;
	SQLPreparedStatements *preparedStatements = (SQLPreparedStatements*) perThreadData;

	*returnOutput=true;
	SQLiteServerLoggerPlugin::SQLThreadOutput sqlThreadOutput;
	sqlThreadOutput.cpuOutputNode=cpuOutputNode;
	sqlite3 *dbHandle = sqlThreadInput.dbHandle;
//	sqlite3_stmt *statement;
	char *errorMsg;
	
	sqlite3_exec(dbHandle,"BEGIN TRANSACTION", 0, 0, 0);
	
	int rc;
	if (cpuOutputNode->isFunctionCall)
	{
		if (preparedStatements->selectNameFromMaster==0)
		{
			// Create function tables if they are not there already
			if (sqlite3_prepare_v2(
				dbHandle, 
				"SELECT name FROM sqlite_master WHERE type='table' AND name="FUNCTION_CALL_TABLE" ",
				-1,
				&preparedStatements->selectNameFromMaster,
				0
				)!=SQLITE_OK)
			{
				RakAssert("Failed PRAGMA table_info for function tables in SQLiteServerLoggerPlugin.cpp" && 0);
				for (int i=0; i < cpuOutputNode->parameterCount; i++)
					cpuOutputNode->parameterList[i].Free();
				sqlite3_exec(dbHandle,"END TRANSACTION", 0, 0, 0);
				return sqlThreadOutput;
			}
		}
		 rc = sqlite3_step(preparedStatements->selectNameFromMaster);
		sqlite3_reset(preparedStatements->selectNameFromMaster);

		if (rc!=SQLITE_ROW)
		{
			// Create table if it isn't there already
			rc = sqlite3_exec(dbHandle,"CREATE TABLE "FUNCTION_CALL_TABLE" (functionId_pk INTEGER PRIMARY KEY, "FUNCTION_CALL_FRIENDLY_TEXT" TEXT, functionName TEXT, "FILE_COLUMN" TEXT, "LINE_COLUMN" INTEGER, "TICK_COUNT_COLUMN" INTEGER, "AUTO_IP_COLUMN" TEXT, "TIMESTAMP_TEXT_COLUMN" TIMESTAMP DATE DEFAULT (datetime('now','localtime')), "TIMESTAMP_NUMERIC_COLUMN" NUMERIC )", 0, 0, &errorMsg);
			RakAssert(rc==SQLITE_OK);
			sqlite3_free(errorMsg);
			// See sqlDataTypeNames for *val
			rc = sqlite3_exec(dbHandle,"CREATE TABLE "FUNCTION_CALL_PARAMETERS_TABLE" (fcpId_pk INTEGER PRIMARY KEY, functionId_fk integer NOT NULL, value TEXT, FOREIGN KEY (functionId_fk) REFERENCES "FUNCTION_CALL_TABLE" (functionId_pk))", 0, 0, &errorMsg);
			RakAssert(rc==SQLITE_OK);
			sqlite3_free(errorMsg);
		}
		else
		{
			// Table already there
		}

		// Insert into function calls
		int parameterCountIndex;
		RakNet::RakString functionCallFriendlyText("%s(", cpuOutputNode->tableName.C_String());
		for (parameterCountIndex=0; parameterCountIndex < cpuOutputNode->parameterCount; parameterCountIndex++)
		{
			if (parameterCountIndex!=0)
				functionCallFriendlyText+=", ";
			switch (cpuOutputNode->parameterList[parameterCountIndex].type)
			{
			case SQLLPDT_POINTER:
				if (cpuOutputNode->parameterList[parameterCountIndex].size==4)
					functionCallFriendlyText+=RakNet::RakString("%p", cpuOutputNode->parameterList[parameterCountIndex].data.i);
				else
					functionCallFriendlyText+=RakNet::RakString("%p", cpuOutputNode->parameterList[parameterCountIndex].data.ll);
				break;
			case SQLLPDT_INTEGER:
				switch (cpuOutputNode->parameterList[parameterCountIndex].size)
				{
				case 1:
					functionCallFriendlyText+=RakNet::RakString("%i", cpuOutputNode->parameterList[parameterCountIndex].data.c);
					break;
				case 2:
					functionCallFriendlyText+=RakNet::RakString("%i", cpuOutputNode->parameterList[parameterCountIndex].data.s);
					break;
				case 4:
					functionCallFriendlyText+=RakNet::RakString("%i", cpuOutputNode->parameterList[parameterCountIndex].data.i);
					break;
				case 8:
					functionCallFriendlyText+=RakNet::RakString("%i", cpuOutputNode->parameterList[parameterCountIndex].data.ll);
					break;
				}
				break;
			case SQLLPDT_REAL:
				if (cpuOutputNode->parameterList[parameterCountIndex].size==sizeof(float))
					functionCallFriendlyText+=RakNet::RakString("%f", cpuOutputNode->parameterList[parameterCountIndex].data.f);
				else
					functionCallFriendlyText+=RakNet::RakString("%d", cpuOutputNode->parameterList[parameterCountIndex].data.d);
				break;
			case SQLLPDT_TEXT:
				functionCallFriendlyText+='"';
				if (cpuOutputNode->parameterList[parameterCountIndex].size>0)
					functionCallFriendlyText.AppendBytes(cpuOutputNode->parameterList[parameterCountIndex].data.cptr, cpuOutputNode->parameterList[parameterCountIndex].size);
				functionCallFriendlyText+='"';
				break;
			case SQLLPDT_IMAGE:
				functionCallFriendlyText+=RakNet::RakString("<%i byte image>", cpuOutputNode->parameterList[parameterCountIndex].size, cpuOutputNode->parameterList[parameterCountIndex].data.cptr);
				break;
			case SQLLPDT_BLOB:
				functionCallFriendlyText+=RakNet::RakString("<%i byte binary>", cpuOutputNode->parameterList[parameterCountIndex].size, cpuOutputNode->parameterList[parameterCountIndex].data.cptr);
				break;
			}
		}

		functionCallFriendlyText+=");";
		
		if (preparedStatements->insertIntoFunctionCalls==0)
		{
			rc = sqlite3_prepare_v2(dbHandle, "INSERT INTO "FUNCTION_CALL_TABLE" ("FUNCTION_CALL_FRIENDLY_TEXT", "FILE_COLUMN", "LINE_COLUMN", "TICK_COUNT_COLUMN", "AUTO_IP_COLUMN", "TIMESTAMP_NUMERIC_COLUMN" ,functionName) VALUES (?,?,?,?,?,?,?)", -1, &preparedStatements->insertIntoFunctionCalls, 0);
			if (rc!=SQLITE_DONE && rc!=SQLITE_OK)
			{
				RakAssert("Failed INSERT INTO "FUNCTION_CALL_PARAMETERS_TABLE" in SQLiteServerLoggerPlugin.cpp" && 0);
			}
		}
		sqlite3_bind_text(preparedStatements->insertIntoFunctionCalls, 1, functionCallFriendlyText.C_String(), -1, SQLITE_TRANSIENT);
		sqlite3_bind_text(preparedStatements->insertIntoFunctionCalls, 2, cpuOutputNode->file.C_String(), -1, SQLITE_TRANSIENT);
		sqlite3_bind_int(preparedStatements->insertIntoFunctionCalls, 3, cpuOutputNode->line);
		sqlite3_bind_int(preparedStatements->insertIntoFunctionCalls, 4, cpuOutputNode->tickCount);
		sqlite3_bind_text(preparedStatements->insertIntoFunctionCalls, 5, cpuOutputNode->ipAddressString, -1, SQLITE_TRANSIENT);
		sqlite3_bind_int(preparedStatements->insertIntoFunctionCalls, 6, (uint32_t) (cpuOutputNode->clientSendingTime));
		sqlite3_bind_text(preparedStatements->insertIntoFunctionCalls, 7, cpuOutputNode->tableName.C_String(), -1, SQLITE_TRANSIENT);
		rc = sqlite3_step(preparedStatements->insertIntoFunctionCalls);
		sqlite3_reset(preparedStatements->insertIntoFunctionCalls);
		if (rc!=SQLITE_DONE && rc!=SQLITE_OK)
		{
			RakAssert("Failed binding parameters to functionCalls in SQLiteServerLoggerPlugin.cpp" && 0);
		}
//		sqlite3_finalize(statement);

//		RakNet::RakString insertIntoFunctionCallsQuery("INSERT INTO 'functionCalls' ("FUNCTION_CALL_FRIENDLY_TEXT", "FILE_COLUMN", "LINE_COLUMN", "TICK_COUNT_COLUMN",functionName) VALUES ('%s','%s',%i,%i,'%s') ", functionCallFriendlyText.C_String(), file.C_String(), line, tickCount,tableName.C_String());
//		rc = sqlite3_exec(dbHandle,insertIntoFunctionCallsQuery.C_String(), 0, 0, &errorMsg);
//		RakAssert(rc==SQLITE_OK);
//		sqlite3_free(errorMsg);
		// Read last row id
		// Requires that this thread has its own connection
		sqlite3_int64 lastRowId = sqlite3_last_insert_rowid(dbHandle);

		if (preparedStatements->insertIntoFunctionCallParameters==0)
		{
			rc = sqlite3_prepare_v2(dbHandle, "INSERT INTO functionCallParameters (functionId_fk, value) VALUES (?,?);", -1, &preparedStatements->insertIntoFunctionCallParameters, 0);
			RakAssert(rc==SQLITE_OK);
			sqlite3_bind_int64(preparedStatements->insertIntoFunctionCallParameters, 1, lastRowId);
		}

		// Insert into parameters table
		for (parameterCountIndex=0; parameterCountIndex < cpuOutputNode->parameterCount; parameterCountIndex++)
		{
			switch (cpuOutputNode->parameterList[parameterCountIndex].type)
			{
			case SQLLPDT_POINTER:
			case SQLLPDT_INTEGER:
				switch (cpuOutputNode->parameterList[parameterCountIndex].size)
				{
				case 1:
					sqlite3_bind_int(preparedStatements->insertIntoFunctionCallParameters, 2, cpuOutputNode->parameterList[parameterCountIndex].data.c);
					break;
				case 2:
					sqlite3_bind_int(preparedStatements->insertIntoFunctionCallParameters, 2, cpuOutputNode->parameterList[parameterCountIndex].data.s);
					break;
				case 4:
					sqlite3_bind_int(preparedStatements->insertIntoFunctionCallParameters, 2, cpuOutputNode->parameterList[parameterCountIndex].data.i);
					break;
				case 8:
					sqlite3_bind_int64(preparedStatements->insertIntoFunctionCallParameters, 2, cpuOutputNode->parameterList[parameterCountIndex].data.ll);
					break;
				}
				break;
			case SQLLPDT_REAL:
				if (cpuOutputNode->parameterList[parameterCountIndex].size==sizeof(float))
					sqlite3_bind_double(preparedStatements->insertIntoFunctionCallParameters, 2, cpuOutputNode->parameterList[parameterCountIndex].data.f);
				else
					sqlite3_bind_double(preparedStatements->insertIntoFunctionCallParameters, 2, cpuOutputNode->parameterList[parameterCountIndex].data.d);
				break;
			case SQLLPDT_TEXT:
				sqlite3_bind_text(preparedStatements->insertIntoFunctionCallParameters, 2, cpuOutputNode->parameterList[parameterCountIndex].data.cptr, cpuOutputNode->parameterList[parameterCountIndex].size, 0);
				break;
			case SQLLPDT_IMAGE:
			case SQLLPDT_BLOB:
				sqlite3_bind_blob(preparedStatements->insertIntoFunctionCallParameters, 2, cpuOutputNode->parameterList[parameterCountIndex].data.vptr, cpuOutputNode->parameterList[parameterCountIndex].size, DeleteBlobOrText);
				cpuOutputNode->parameterList[parameterCountIndex].DoNotFree();
				break;
			default:
				RakAssert("Hit invalid default in case label in SQLiteServerLoggerPlugin.cpp" && 0);
			}
			rc = sqlite3_step(preparedStatements->insertIntoFunctionCallParameters);
			sqlite3_reset(preparedStatements->insertIntoFunctionCallParameters);
			if (rc!=SQLITE_DONE && rc!=SQLITE_OK)
			{
				RakAssert("Failed sqlite3_step to bind functionCall parameters in SQLiteServerLoggerPlugin.cpp" && 0);
			}
		}

	//	if (statement)
	//		sqlite3_finalize(statement);
	}
	else
	{

		sqlite3_stmt *pragmaTableInfo;
		RakNet::RakString pragmaQuery("PRAGMA table_info(%s)",cpuOutputNode->tableName.C_String());
		if (sqlite3_prepare_v2(
			dbHandle, 
			pragmaQuery.C_String(),
			-1,
			&pragmaTableInfo,
			0
			)!=SQLITE_OK)
		{
			RakAssert("Failed PRAGMA table_info for tableName in SQLiteServerLoggerPlugin.cpp" && 0);
			for (int i=0; i < cpuOutputNode->parameterCount; i++)
				cpuOutputNode->parameterList[i].Free();
			sqlite3_exec(dbHandle,"END TRANSACTION", 0, 0, 0);
			return sqlThreadOutput;
		}

		int rc = sqlite3_step(pragmaTableInfo);
		DataStructures::List<RakNet::RakString> existingColumnNames;
		DataStructures::List<RakNet::RakString> existingColumnTypes;
		char *errorMsg;
		while (rc==SQLITE_ROW)
		{
			/*
			int nameColumn;
			for (int j=0; j < sqlite3_column_count(statement); j++)
			{
				if (strcmp(sqlite3_column_name(statement,j),"name")==0)
				{
					nameColumn=j;
					break;
				}
			}
			int typeColumn;
			for (int j=0; j < sqlite3_column_count(statement); j++)
			{
				if (strcmp(sqlite3_column_name(statement,j),"type")==0)
				{
					typeColumn=j;
					break;
				}
			}
			*/
			const int nameColumn=1;
			const int typeColumn=2;
			RakAssert(strcmp(sqlite3_column_name(pragmaTableInfo,nameColumn),"name")==0);
			RakAssert(strcmp(sqlite3_column_name(pragmaTableInfo,typeColumn),"type")==0);
			RakNet::RakString columnName = sqlite3_column_text(pragmaTableInfo,nameColumn);
			RakNet::RakString columnType = sqlite3_column_text(pragmaTableInfo,typeColumn);
			existingColumnNames.Push(columnName, _FILE_AND_LINE_ );
			existingColumnTypes.Push(columnType, _FILE_AND_LINE_ );

			rc = sqlite3_step(pragmaTableInfo);
		}
		sqlite3_reset(pragmaTableInfo);
		sqlite3_finalize(pragmaTableInfo);
		if (rc==SQLITE_ERROR)
		{
			RakAssert("Failed sqlite3_step in SQLiteServerLoggerPlugin.cpp" && 0);
			for (int i=0; i < cpuOutputNode->parameterCount; i++)
				cpuOutputNode->parameterList[i].Free();
			sqlite3_exec(dbHandle,"END TRANSACTION", 0, 0, 0);
			return sqlThreadOutput;
		}

		int existingColumnNamesIndex,insertingColumnNamesIndex;
		if (existingColumnNames.Size()==0)
		{
			RakNet::RakString createQuery("CREATE TABLE %s (rowId_pk INTEGER PRIMARY KEY, "FILE_COLUMN" TEXT, "LINE_COLUMN" INTEGER, "TICK_COUNT_COLUMN" INTEGER, "AUTO_IP_COLUMN" TEXT, "TIMESTAMP_TEXT_COLUMN" TIMESTAMP DATE DEFAULT (datetime('now','localtime')), "TIMESTAMP_NUMERIC_COLUMN" NUMERIC",cpuOutputNode->tableName.C_String());

			for (int i=0; i < cpuOutputNode->parameterCount; i++)
			{
				createQuery+=", ";
				createQuery+=cpuOutputNode->insertingColumnNames[i];
				createQuery+=" ";
				createQuery+=GetSqlDataTypeName2(cpuOutputNode->parameterList[i].type);
			}
			createQuery+=" )";

			sqlite3_exec(dbHandle,
				createQuery.C_String(),
				0, 0, &errorMsg);
			RakAssert(errorMsg==0);
			sqlite3_free(errorMsg);
		}
		else
		{
			// Compare what is there (columnNames,columnTypes) to what we are adding. Add what is missing
			bool alreadyExists;
			for (insertingColumnNamesIndex=0; insertingColumnNamesIndex<(int) cpuOutputNode->insertingColumnNames.Size(); insertingColumnNamesIndex++)
			{
				alreadyExists=false;
				for (existingColumnNamesIndex=0; existingColumnNamesIndex<(int) existingColumnNames.Size(); existingColumnNamesIndex++)
				{
					if (existingColumnNames[existingColumnNamesIndex]==cpuOutputNode->insertingColumnNames[insertingColumnNamesIndex])
					{
						// Type mismatch? If so, abort
						if (existingColumnTypes[existingColumnNamesIndex]!=GetSqlDataTypeName2(cpuOutputNode->parameterList[insertingColumnNamesIndex].type))
						{
							printf("Error: Column type mismatch. TableName=%s. ColumnName%s. Existing=%s. New=%s\n",
								cpuOutputNode->tableName.C_String(),
								existingColumnNames[existingColumnNamesIndex].C_String(),
								existingColumnTypes[existingColumnNamesIndex].C_String(),
								GetSqlDataTypeName2(cpuOutputNode->parameterList[insertingColumnNamesIndex].type)
								);
							for (int i=0; i < cpuOutputNode->parameterCount; i++)
								cpuOutputNode->parameterList[i].Free();
							sqlite3_exec(dbHandle,"END TRANSACTION", 0, 0, 0);
							return sqlThreadOutput;
						}

						alreadyExists=true;
						break;
					}
				}

				if (alreadyExists==false)
				{
					sqlite3_exec(dbHandle,
						RakNet::RakString("ALTER TABLE %s ADD %s %s",
						cpuOutputNode->tableName.C_String(),
						cpuOutputNode->insertingColumnNames[insertingColumnNamesIndex].C_String(),
						GetSqlDataTypeName2(cpuOutputNode->parameterList[insertingColumnNamesIndex].type)
						).C_String(),
						0, 0, &errorMsg);
					RakAssert(errorMsg==0);
					sqlite3_free(errorMsg);
				}
			}
		}



		// Insert new row
		RakNet::RakString insertQuery("INSERT INTO %s (", cpuOutputNode->tableName.C_String());
		int parameterCountIndex;
		for (parameterCountIndex=0; parameterCountIndex<cpuOutputNode->parameterCount; parameterCountIndex++)
		{
			if (parameterCountIndex!=0)
				insertQuery+=", ";
			insertQuery+=cpuOutputNode->insertingColumnNames[parameterCountIndex].C_String();
		}
		// Add file and line to the end
		insertQuery+=", "FILE_COLUMN", "LINE_COLUMN", "TICK_COUNT_COLUMN", "AUTO_IP_COLUMN", "TIMESTAMP_NUMERIC_COLUMN" ) VALUES (";

		for (parameterCountIndex=0; parameterCountIndex<cpuOutputNode->parameterCount+5; parameterCountIndex++)
		{
			if (parameterCountIndex!=0)
				insertQuery+=", ?";
			else
				insertQuery+="?";
		}
		insertQuery+=")";

		sqlite3_stmt *customStatement;
		if (sqlite3_prepare_v2(
			dbHandle, 
			insertQuery.C_String(),
			-1,
			&customStatement,
			0
			)!=SQLITE_OK)
		{
			RakAssert("Failed second sqlite3_prepare_v2 in SQLiteServerLoggerPlugin.cpp" && 0);
			for (int i=0; i < cpuOutputNode->parameterCount; i++)
				cpuOutputNode->parameterList[i].Free();
			sqlite3_exec(dbHandle,"END TRANSACTION", 0, 0, 0);
			return sqlThreadOutput;
		}

		for (parameterCountIndex=0; parameterCountIndex<cpuOutputNode->parameterCount; parameterCountIndex++)
		{
			switch (cpuOutputNode->parameterList[parameterCountIndex].type)
			{
				case SQLLPDT_POINTER:
				case SQLLPDT_INTEGER:
					switch (cpuOutputNode->parameterList[parameterCountIndex].size)
					{
					case 1:
						sqlite3_bind_int(customStatement, parameterCountIndex+1, cpuOutputNode->parameterList[parameterCountIndex].data.c);
						break;
					case 2:
						sqlite3_bind_int(customStatement, parameterCountIndex+1, cpuOutputNode->parameterList[parameterCountIndex].data.s);
						break;
					case 4:
						sqlite3_bind_int(customStatement, parameterCountIndex+1, cpuOutputNode->parameterList[parameterCountIndex].data.i);
						break;
					case 8:
						sqlite3_bind_int64(customStatement, parameterCountIndex+1, cpuOutputNode->parameterList[parameterCountIndex].data.ll);
						break;
					}
					break;
				case SQLLPDT_REAL:
					if (cpuOutputNode->parameterList[parameterCountIndex].size==sizeof(float))
						sqlite3_bind_double(customStatement, parameterCountIndex+1, cpuOutputNode->parameterList[parameterCountIndex].data.f);
					else
						sqlite3_bind_double(customStatement, parameterCountIndex+1, cpuOutputNode->parameterList[parameterCountIndex].data.d);
					break;
				case SQLLPDT_TEXT:
					sqlite3_bind_text(customStatement, parameterCountIndex+1, cpuOutputNode->parameterList[parameterCountIndex].data.cptr, cpuOutputNode->parameterList[parameterCountIndex].size, DeleteBlobOrText);
					cpuOutputNode->parameterList[parameterCountIndex].DoNotFree();
					break;
				case SQLLPDT_IMAGE:
				case SQLLPDT_BLOB:
					sqlite3_bind_blob(customStatement, parameterCountIndex+1, cpuOutputNode->parameterList[parameterCountIndex].data.vptr, cpuOutputNode->parameterList[parameterCountIndex].size, DeleteBlobOrText);
					cpuOutputNode->parameterList[parameterCountIndex].DoNotFree();
					break;
			}
		}

		// Add file and line to the end
		sqlite3_bind_text(customStatement, parameterCountIndex+1, cpuOutputNode->file.C_String(), (int) cpuOutputNode->file.GetLength(), SQLITE_TRANSIENT);
		sqlite3_bind_int(customStatement, parameterCountIndex+2, cpuOutputNode->line);
		sqlite3_bind_int(customStatement, parameterCountIndex+3, cpuOutputNode->tickCount);
		sqlite3_bind_text(customStatement, parameterCountIndex+4, cpuOutputNode->ipAddressString, -1, SQLITE_TRANSIENT);
		sqlite3_bind_int(customStatement, parameterCountIndex+5, (uint32_t) (cpuOutputNode->clientSendingTime));

		rc = sqlite3_step(customStatement);
		if (rc!=SQLITE_DONE && rc!=SQLITE_OK)
		{
			RakAssert("Failed sqlite3_step to bind blobs in SQLiteServerLoggerPlugin.cpp" && 0);
			for (int i=0; i < cpuOutputNode->parameterCount; i++)
				cpuOutputNode->parameterList[i].Free();
			sqlite3_exec(dbHandle,"END TRANSACTION", 0, 0, 0);
			return sqlThreadOutput;
		}
		sqlite3_finalize(customStatement);
	}
	sqlite3_exec(dbHandle,"END TRANSACTION", 0, 0, 0);

	for (int i=0; i < cpuOutputNode->parameterCount; i++)
		cpuOutputNode->parameterList[i].Free();

	return sqlThreadOutput;
}

SQLiteServerLoggerPlugin::SQLiteServerLoggerPlugin()
{
	sessionManagementMode=CREATE_EACH_NAMED_DB_HANDLE;
	createDirectoryForFile=true;
	cpuThreadInput=0;
	dxtCompressionEnabled=false;
}

SQLiteServerLoggerPlugin::~SQLiteServerLoggerPlugin()
{
	StopCPUSQLThreads();
	RakNet::OP_DELETE(cpuThreadInput,_FILE_AND_LINE_);
	CloseAllSessions();
}
void SQLiteServerLoggerPlugin::Update(void)
{
	SQLite3ServerPlugin::Update();

	bool hadOutput=false;

	int arrayIndex;
//	unsigned int i;

	PushCpuThreadInputIfNecessary();

	while (cpuLoggerThreadPool.HasOutputFast() && cpuLoggerThreadPool.HasOutput())
	{
		CPUThreadOutput* cpuThreadOutput=cpuLoggerThreadPool.GetOutput();
		for (arrayIndex=0; arrayIndex < cpuThreadOutput->arraySize; arrayIndex++)
		{
			SQLThreadInput sqlThreadInput;
			CPUThreadOutputNode *outputNode = cpuThreadOutput->cpuOutputNodeArray[arrayIndex];
			sqlThreadInput.cpuOutputNode=outputNode;
			// bool alreadyHasLoggedInSession=false;
			int sessionIndex;
			for (sessionIndex=0; sessionIndex < loggedInSessions.Size(); sessionIndex++)
			{
				if (loggedInSessions[sessionIndex].systemAddress==outputNode->packet->systemAddress &&
					loggedInSessions[sessionIndex].sessionName==outputNode->dbIdentifier)
				{
					break;
				}
			}

			unsigned int idx;
			if (sessionManagementMode==USE_ANY_DB_HANDLE)
			{
				if (dbHandles.GetSize()>0)
					idx=0;
				else
					idx=-1;
			}
			else
			{
				idx = dbHandles.GetIndexOf(outputNode->dbIdentifier);
				if (sessionIndex==loggedInSessions.Size() && (createDirectoryForFile==true || idx==-1) && sessionManagementMode==CREATE_EACH_NAMED_DB_HANDLE)
				{
					// Create it, and set idx to the new one
					idx = CreateDBHandle(outputNode->dbIdentifier);
				}
				else if (idx==-1)
				{
					if (sessionManagementMode==CREATE_EACH_NAMED_DB_HANDLE || sessionManagementMode==CREATE_SHARED_NAMED_DB_HANDLE)
					{
						// Create it, and set idx to the new one
						idx = CreateDBHandle(outputNode->dbIdentifier);
					}
					if (sessionManagementMode==USE_NAMED_DB_HANDLE)
					{
						RakAssert("Can't find named DB handle\n" && 0);
					}
				}
				else
				{
					// Use idx
				}
			}

			if (idx==-1)
			{
				DeallocPacketUnified(outputNode->packet);
				RakNet::OP_DELETE(outputNode,_FILE_AND_LINE_);
			}
			else
			{
				if (sessionIndex==loggedInSessions.Size())
				{
					SessionNameAndSystemAddress sassy;
					sassy.sessionName=outputNode->dbIdentifier;
					sassy.systemAddress=outputNode->packet->systemAddress;
					sassy.referencedPointer=dbHandles[idx].dbHandle;
					RakNet::TimeMS curTime = RakNet::GetTimeMS();
					RakNet::TimeMS dbAge = curTime - dbHandles[idx].whenCreated;
//					RakNet::TimeMS timeDelta = outputNode->clientSendingTime - curTime;
					sassy.timestampDelta=dbAge - outputNode->clientSendingTime ;
					// sassy.dbAgeWhenCreated=dbHandles[idx].whenCreated;					
					loggedInSessions.Push(sassy, _FILE_AND_LINE_ );
					sessionIndex=loggedInSessions.Size()-1;
				}

				DeallocPacketUnified(outputNode->packet);
				sqlThreadInput.dbHandle=dbHandles[idx].dbHandle;
				outputNode->clientSendingTime+=loggedInSessions[sessionIndex].timestampDelta;
				sqlLoggerThreadPool.AddInput(ExecSQLLoggingThread, sqlThreadInput);
			}
		}

//		RakNet::OP_DELETE_ARRAY(cpuThreadOutput->cpuOutputNodeArray);
		RakNet::OP_DELETE(cpuThreadOutput,_FILE_AND_LINE_);
	}

	while (sqlLoggerThreadPool.HasOutputFast() && sqlLoggerThreadPool.HasOutput())
	{
		hadOutput=true;
		RakNet::OP_DELETE(sqlLoggerThreadPool.GetOutput().cpuOutputNode,_FILE_AND_LINE_);
	}

	if (hadOutput)
		CloseUnreferencedSessions();
}
PluginReceiveResult SQLiteServerLoggerPlugin::OnReceive(Packet *packet)
{
	PluginReceiveResult prr = SQLite3ServerPlugin::OnReceive(packet);
	if (prr!=RR_CONTINUE_PROCESSING)
		return prr;

	switch (packet->data[0])
	{
	case ID_SQLLITE_LOGGER:
		{
			RakNet::BitStream bitStream(packet->data, packet->length, false);
			bitStream.IgnoreBytes(1);
			RakNet::RakString dbIdentifier;
			bitStream.Read(dbIdentifier);

			if (sessionManagementMode==CREATE_EACH_NAMED_DB_HANDLE)
			{
				unsigned char senderAddr[32];
				packet->systemAddress.ToString(true,(char*) senderAddr);
				dbIdentifier+=':';
				dbIdentifier+=senderAddr;
			}

			CPUThreadInput *ti = LockCpuThreadInput();
			ti->cpuInputArray[ti->arraySize].packet=packet;
		//	ti->cpuInputArray[ti->arraySize].whenMessageArrived=RakNet::GetTimeMS();
			ti->cpuInputArray[ti->arraySize].dbIdentifier=dbIdentifier;
			UnlockCpuThreadInput();

			
			/*
			unsigned int i;
			bool alreadyHasLoggedInSession=false;
			for (i=0; i < loggedInSessions.Size(); i++)
			{
				if (loggedInSessions[i].systemAddress==packet->systemAddress &&
					loggedInSessions[i].sessionName==dbIdentifier)
				{
					alreadyHasLoggedInSession=true;
					break;
				}
			}

			unsigned int idx;
			if (sessionManagementMode==USE_ANY_DB_HANDLE)
			{
				if (dbHandles.GetSize()>0)
					idx=0;
				else
					idx=-1;
			}
			else
			{
				idx = dbHandles.GetIndexOf(dbIdentifier);
				if (alreadyHasLoggedInSession==false && (createDirectoryForFile==true || idx==-1) && sessionManagementMode==CREATE_EACH_NAMED_DB_HANDLE)
				{
					// Create it, and set idx to the new one
					idx = CreateDBHandle(dbIdentifier);
				}
				else if (idx==-1)
				{
					if (sessionManagementMode==CREATE_EACH_NAMED_DB_HANDLE || sessionManagementMode==CREATE_SHARED_NAMED_DB_HANDLE)
					{
						// Create it, and set idx to the new one
						idx = CreateDBHandle(dbIdentifier);
					}
					if (sessionManagementMode==USE_NAMED_DB_HANDLE)
					{
						RakAssert("Can't find named DB handle\n" && 0);
					}
				}
				else
				{
					// Use idx
				}
			}

			if (idx==-1)
			{
				return RR_STOP_PROCESSING_AND_DEALLOCATE;
			}

			if (alreadyHasLoggedInSession==false)
			{
				SessionNameAndSystemAddress sassy;
				sassy.sessionName=dbIdentifier;
				sassy.systemAddress=packet->systemAddress;
				sassy.referencedPointer=dbHandles[idx].dbHandle;
				loggedInSessions.Push(sassy);
			}	

			SQLExecThreadInput input;			
			input.dbHandle=dbHandles[idx].dbHandle;
			input.packet=packet;
			input.whenMessageArrived=RakNet::GetTimeMS()-dbHandles[idx].whenCreated;
			__sqlThreadPool.AddInput(ExecSQLLoggingThread, input);
//			printf("Pending Queries: %i\n", __sqlThreadPool.InputSize());
*/
			return RR_STOP_PROCESSING;
		}
	}
	return RR_CONTINUE_PROCESSING;
}
void SQLiteServerLoggerPlugin::OnClosedConnection(const SystemAddress &systemAddress, RakNetGUID rakNetGUID, PI2_LostConnectionReason lostConnectionReason )
{
	RakNet::RakString removedSession;
	unsigned int i=0;
	while (i < loggedInSessions.Size())
	{
		if (loggedInSessions[i].systemAddress==systemAddress)
		{
			removedSession=loggedInSessions[i].sessionName;
			loggedInSessions.RemoveAtIndexFast(i);

			/*
			unsigned int j;
			bool removedSessionReferenced=false;
			for (j=0; j < loggedInSessions.Size(); j++)
			{
				if (loggedInSessions[j].sessionName==removedSession)
				{
					removedSessionReferenced=true;
					break;
				}
			}
			if (removedSessionReferenced==false)
			{
//				if (__sqlThreadPool.InputSize()>0 || __sqlThreadPool.IsWorking())
//					return;

//				RemoveDBHandle(removedSession, sessionManagementMode==CREATE_EACH_NAMED_DB_HANDLE||sessionManagementMode==CREATE_ONE_NAMED_DB_HANDLE);
			}
			*/
		}
		else
			i++;
	}

	CloseUnreferencedSessions();
}
void SQLiteServerLoggerPlugin::CloseUnreferencedSessions(void)
{
	DataStructures::List<sqlite3 *> sessionNames;
	unsigned int j;
	for (j=0; j < loggedInSessions.Size(); j++)
	{
		if (sessionNames.GetIndexOf(loggedInSessions[j].referencedPointer)==-1)
			sessionNames.Push(loggedInSessions[j].referencedPointer, _FILE_AND_LINE_ );		
	}

	DataStructures::List<sqlite3*> unreferencedHandles;
	bool isReferenced;
	for (unsigned int i=0; i < dbHandles.GetSize(); i++)
	{
		if (dbHandles[i].dbAutoCreated)
		{
			j=0;
			isReferenced=false;
			for (j=0; j < sessionNames.Size(); j++)
			{
				if (sessionNames[j]==dbHandles[i].dbHandle)
				{
					isReferenced=true;
					break;
				}
			}

			if (isReferenced==false)
			{
				unreferencedHandles.Push(dbHandles[i].dbHandle,_FILE_AND_LINE_);
			}
		}
	}

	if (unreferencedHandles.Size())
	{
		sqlLoggerThreadPool.LockInput();
		if (sqlLoggerThreadPool.HasInputFast()==false)
		{
			RakSleep(100);
			while (sqlLoggerThreadPool.NumThreadsWorking()>0)
				RakSleep(30);
			for (unsigned int k=0; k < unreferencedHandles.Size(); k++)
			{
				RemoveDBHandle(unreferencedHandles[k], true);
			}
		}
		sqlLoggerThreadPool.UnlockInput();

		if (dbHandles.GetSize()==0)
			StopCPUSQLThreads();
	}
}
void SQLiteServerLoggerPlugin::CloseAllSessions(void)
{
	loggedInSessions.Clear(false, _FILE_AND_LINE_);
	CloseUnreferencedSessions();
}
unsigned int SQLiteServerLoggerPlugin::CreateDBHandle(RakNet::RakString dbIdentifier)
{
	if (sessionManagementMode!=CREATE_EACH_NAMED_DB_HANDLE && sessionManagementMode!=CREATE_SHARED_NAMED_DB_HANDLE)
		return dbHandles.GetIndexOf(dbIdentifier);

	RakNet::RakString filePath = newDatabaseFilePath;
	if (createDirectoryForFile)
	{
		filePath+=dbIdentifier;
		filePath.TerminateAtLastCharacter('.');
		filePath.MakeFilePath();

		time_t     now;
		struct tm  *ts;
		char       buf[80];

		/* Get the current time */
		now = time(NULL);

		/* Format and print the time, "ddd yyyy-mm-dd hh:mm:ss zzz" */
		ts = localtime(&now);
		strftime(buf, sizeof(buf), "__%a_%Y-%m-%d__%H;%M", ts);

		filePath+=buf;
		filePath+=RakNet::RakString("__%i", RakNet::GetTimeMS());

		filePath.MakeFilePath();
	}


	// With no file data, just creates the directory structure
	WriteFileWithDirectories(filePath.C_String(), 0, 0);

	RakNet::RakString fileSafeDbIdentifier = dbIdentifier;
	fileSafeDbIdentifier.TerminateAtLastCharacter(':');
	RakNet::RakString fileNameWithPath=filePath+fileSafeDbIdentifier;
	
	// SQL Open this file, and register it
	sqlite3 *database;
	if (sqlite3_open_v2(fileNameWithPath.C_String(), &database, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, 0)!=SQLITE_OK)
	{
		RakAssert("sqlite3_open_v2 failed in SQLiteServerLoggerPlugin.cpp" && 0);
		return -1;
	}
	if (AddDBHandle(dbIdentifier, database, true))
	{

		char *errorMsg;
		int rc = sqlite3_exec(database,"PRAGMA synchronous=OFF", 0, 0, &errorMsg);
		RakAssert(rc==SQLITE_OK);
		sqlite3_free(errorMsg);
		rc = sqlite3_exec(database,"PRAGMA count_changes=OFF", 0, 0, &errorMsg);
		RakAssert(rc==SQLITE_OK);
		sqlite3_free(errorMsg);

		printf("Created %s\n", fileNameWithPath.C_String());
		return dbHandles.GetIndexOf(dbIdentifier);
	}
	else
	{
		RakAssert("Failed to call AddDbHandle" && 0);
		return -1;
	}
	return -1;
}
void SQLiteServerLoggerPlugin::SetSessionManagementMode(SessionManagementMode _sessionManagementMode, bool _createDirectoryForFile, const char *_newDatabaseFilePath)
{
	sessionManagementMode=_sessionManagementMode;
	createDirectoryForFile=_createDirectoryForFile;
	newDatabaseFilePath=_newDatabaseFilePath;
	newDatabaseFilePath.MakeFilePath();
}
void SQLiteServerLoggerPlugin::OnShutdown(void)
{
	CloseAllSessions();
}
void SQLiteServerLoggerPlugin::StopCPUSQLThreads(void)
{
	unsigned int i;
	int j,k;
	cpuLoggerThreadPool.StopThreads();
	ClearCpuThreadInput();
	for (i=0; i < cpuLoggerThreadPool.InputSize(); i++)
	{
		CPUThreadInput *cpuThreadInput = cpuLoggerThreadPool.GetInputAtIndex(i);
		for (j=0; j < cpuThreadInput->arraySize; j++)
		{
			DeallocPacketUnified(cpuThreadInput->cpuInputArray[j].packet);
		}
		RakNet::OP_DELETE(cpuThreadInput,_FILE_AND_LINE_);
	}
	cpuLoggerThreadPool.ClearInput();
	for (i=0; i < cpuLoggerThreadPool.OutputSize(); i++)
	{
		CPUThreadOutput *cpuThreadOutput = cpuLoggerThreadPool.GetOutputAtIndex(i);
		for (j=0; j < cpuThreadOutput->arraySize; j++)
		{
			CPUThreadOutputNode *cpuThreadOutputNode = cpuThreadOutput->cpuOutputNodeArray[j];
			DeallocPacketUnified(cpuThreadOutputNode->packet);
			for (k=0; k < cpuThreadOutputNode->parameterCount; k++)
				cpuThreadOutputNode->parameterList[k].Free();
			RakNet::OP_DELETE(cpuThreadOutputNode,_FILE_AND_LINE_);
		}
//		RakNet::OP_DELETE_ARRAY(cpuThreadOutput->cpuOutputNodeArray,_FILE_AND_LINE_);
		RakNet::OP_DELETE(cpuThreadOutput,_FILE_AND_LINE_);
	}
	cpuLoggerThreadPool.ClearOutput();

	sqlLoggerThreadPool.StopThreads();
	for (i=0; i < sqlLoggerThreadPool.InputSize(); i++)
		RakNet::OP_DELETE(sqlLoggerThreadPool.GetInputAtIndex(i).cpuOutputNode,_FILE_AND_LINE_);
	sqlLoggerThreadPool.ClearInput();
	for (i=0; i < sqlLoggerThreadPool.OutputSize(); i++)
		RakNet::OP_DELETE(sqlLoggerThreadPool.GetOutputAtIndex(i).cpuOutputNode,_FILE_AND_LINE_);
	sqlLoggerThreadPool.ClearOutput();
}
void SQLiteServerLoggerPlugin::GetProcessingStatus(ProcessingStatus *processingStatus)
{
	if (cpuThreadInput)
		processingStatus->packetsBuffered=cpuThreadInput->arraySize;
	else
		processingStatus->packetsBuffered=0;
	processingStatus->cpuPendingProcessing=cpuLoggerThreadPool.InputSize();
	processingStatus->cpuProcessedAwaitingDeallocation=cpuLoggerThreadPool.OutputSize();
	processingStatus->cpuNumThreadsWorking=cpuLoggerThreadPool.NumThreadsWorking();
	processingStatus->sqlPendingProcessing=sqlLoggerThreadPool.InputSize();
	processingStatus->sqlProcessedAwaitingDeallocation=sqlLoggerThreadPool.OutputSize();
	processingStatus->sqlNumThreadsWorking=sqlLoggerThreadPool.NumThreadsWorking();
}

SQLiteServerLoggerPlugin::CPUThreadInput *SQLiteServerLoggerPlugin::LockCpuThreadInput(void)
{
	if (cpuThreadInput==0)
	{
		cpuThreadInput=RakNet::OP_NEW<CPUThreadInput>(_FILE_AND_LINE_);
		cpuThreadInput->arraySize=0;
		whenCpuThreadInputAllocated=RakNet::GetTimeMS();
	}
	return cpuThreadInput;
}
void SQLiteServerLoggerPlugin::ClearCpuThreadInput(void)
{
	if (cpuThreadInput!=0)
	{
		for (int i=0; i < cpuThreadInput->arraySize; i++)
			DeallocPacketUnified(cpuThreadInput->cpuInputArray[i].packet);
		RakNet::OP_DELETE(cpuThreadInput,_FILE_AND_LINE_);
		cpuThreadInput=0;
	}
}
void SQLiteServerLoggerPlugin::UnlockCpuThreadInput(void)
{
	cpuThreadInput->arraySize++;
	if (cpuThreadInput->arraySize==MAX_PACKETS_PER_CPU_INPUT_THREAD)
		PushCpuThreadInput();
	else
		PushCpuThreadInputIfNecessary();
}

void SQLiteServerLoggerPlugin::CancelLockCpuThreadInput(void)
{
	if (cpuThreadInput->arraySize==0)
	{
		RakNet::OP_DELETE(cpuThreadInput,_FILE_AND_LINE_);
		cpuThreadInput=0;
	}
}
void SQLiteServerLoggerPlugin::PushCpuThreadInput(void)
{
	// cpu threads can probably be as many as I want
	if (cpuLoggerThreadPool.WasStarted()==false)
	{
		if (dxtCompressionEnabled)
			cpuLoggerThreadPool.StartThreads(1,0,InitDxt, DeinitDxt);
		else
			cpuLoggerThreadPool.StartThreads(1,0,0, 0);
	}
	// sql logger threads should probably be limited to 1 since I'm doing transaction locks and calling sqlite3_last_insert_rowid
	if (sqlLoggerThreadPool.WasStarted()==false)
		sqlLoggerThreadPool.StartThreads(1,0, SQLLoggerThreadAllocPreparedStatements, SQLLoggerThreadDeallocPreparedStatements);

	cpuLoggerThreadPool.AddInput(ExecCPULoggingThread, cpuThreadInput);
	cpuThreadInput=0;
}
void SQLiteServerLoggerPlugin::PushCpuThreadInputIfNecessary(void)
{
	RakNet::TimeMS curTime = RakNet::GetTimeMS();
	if (cpuThreadInput && curTime-whenCpuThreadInputAllocated>MAX_TIME_TO_BUFFER_PACKETS)
		PushCpuThreadInput();
}
void SQLiteServerLoggerPlugin::OnAttach(void)
{
}
void SQLiteServerLoggerPlugin::OnDetach(void)
{
}
void SQLiteServerLoggerPlugin::SetEnableDXTCompression(bool enable)
{
	dxtCompressionEnabled=enable;
}
