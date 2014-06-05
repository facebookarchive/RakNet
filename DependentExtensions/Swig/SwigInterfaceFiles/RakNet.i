//This is the main file that puts everything together, large sections are put into thier own file
//Smaller sections are placed directly in this file

%module(directors="1") RakNet

#pragma SWIG nowarn=312,302,314,473,516,325

#if defined(SWIG_ADDITIONAL_AUTOPATCHER_MYSQL) || defined(SWIG_ADDITIONAL_AUTOPATCHER_POSTGRESQL)
	#define SWIG_ADDITIONAL_AUTOPATCHER
#endif

%include "RakNetCSharpCPlusIncludes.i"
//----------------Includes for swig functions -----------
%include cpointer.i
%include "arrays_csharp.i"
%include "typemaps.i"
%include "carrays.i"
//----------------------Features--------------------
//The director featur is required for C# function overrides to work on the C++ classes
%feature("director") NatPunchthroughDebugInterface;
%feature("director") FileListTransferCBInterface;
%feature("director") UDPProxyClientResultHandler;
%feature("director") UDPProxyServerResultHandler;
%feature("director") NetworkIDObject;

%nestedworkaround DataStructures::Table::Row;
%nestedworkaround DataStructures::Table::Cell; 
%nestedworkaround DataStructures::Table::FilterQuery;
%nestedworkaround DataStructures::Table::ColumnDescriptor;
%nestedworkaround DataStructures::Table::SortQuery;
%nestedworkaround RakNet::FileListTransferCBInterface::OnFileStruct;
%nestedworkaround RakNet::FileListTransferCBInterface::FileProgressStruct;
%nestedworkaround RakNet::FileListTransferCBInterface::DownloadCompleteStruct;

//----------------Extra Swig PreProcessor------------
%include "RakNetCSharpPreprocessor.i"

//----------Ignores----------------
%include "RakNetCSharpIgnores.i"

//------------------------------TypeMaps--------------------------
%include "RakNetCSharpTypeMaps.i"

//----------------------------Method modifiers---------------------
%include "RakNetCSharpMethodModifiers.i"

//--------------------------Renames--------------------------------
%include "RakNetCSharpRenames.i"

//--------------------------------Extends-----------------------------------
%include "RakNetCSharpExtends.i"

//----------------------------Additional Class/Struct Defines-----------------------
%include "RakNetStructsAndClasses.i"

//---------------------------Extra global functions---------------

//What these two functions are for, is to get around the string in/out problem I return the string
//These helper functions are hidden from the user, the user sees the original in/out api
//This is the C++ code insert
%{
char * StatisticsToStringHelper( RakNet::RakNetStatistics *s,char * buffer,int verbosityLevel )
{
	StatisticsToString( s, buffer, verbosityLevel );
	return buffer;
}

%}

//This code is for Swig parsing
char * StatisticsToStringHelper( RakNet::RakNetStatistics *s,char * buffer,int verbosityLevel );

#ifdef SWIG_ADDITIONAL_AUTOPATCHER
	//Swig parsing
	/*bool CreatePatchHelper(unsigned char *inByteArray, unsigned oldsize, unsigned char *inByteArray2, unsigned int newsize, unsigned char * inOutByteArray, unsigned *outSize);

	//Code include
	%{
		bool CreatePatchHelper(unsigned char *inByteArray, unsigned oldsize, unsigned char *inByteArray2, unsigned int newsize, unsigned char * inOutByteArray, unsigned *outSize)
		{
			char ** passedArrayPointer;
			*passedArrayPointer= new char[99];
			bool returnVal=CreatePatch((char *) inByteArray,oldsize,(char *) inByteArray2,newsize,passedArrayPointer,outSize);
			//memcpy(inOutByteArray, *passedArrayPointer, (size_t)1);
			return true;	
		}
	%}*/
#endif

//------------------------------Header includes for parsing by swig----------------------------
%include "RakNetCSharpSwigIncludes.i"
using namespace RakNet;

//-------------------------Special Extends----------------------------------
//For whatever reason these extends need to be placed after swig parsing.

//For the template to work the DataStructures namespace needs to be parsed by swig
%extend DataStructures::Table
 {
	DataStructures::Page<unsigned, Row*, _TABLE_BPLUS_TREE_ORDER> * GetListHeadHelper()
	{//Bypass of strange bug reprted by user
		return self->GetListHead();
	}

	void SortTableHelper(DataStructures::List< SortQuery > *sortQueries, unsigned numSortQueries, DataStructures::List< Row > *out)
 	{
 		SortQuery * passedArray=NULL;
 		if (sortQueries!=NULL)
 		{
 		passedArray=&((*sortQueries)[0]); /*The memory should be contigous since this is a vector class copy. It was last time I checked the implementation. So this will efficiently pass the array without needing to copy it*/
 		}
 
 		int numRows = self->GetRowCount();
		Row **tempIn = NULL;
 		tempIn = new Row*[numRows];
 		self->SortTable(passedArray,numSortQueries, tempIn);
 
 		for (int i=0;i<numRows;i++)
 		{
 			out->Insert(*(tempIn[i]),__FILE__,__LINE__);
 		}
 		delete [] tempIn;
 		
 	}
 	
 	void GetCellValueByIndexHelper(unsigned rowIndex, unsigned columnIndex, unsigned char *inOutByteArray, int *outputLength)
 	{
 		self->GetCellValueByIndex(rowIndex, columnIndex,  (char *)inOutByteArray, outputLength);
 	}
 
 	char * GetCellValueByIndexHelper(unsigned rowIndex, unsigned columnIndex, char *output)
 	{
 		self->GetCellValueByIndex(rowIndex,columnIndex,output);
 		return output;
 	}
 
	/// \brief Prints out the names of all the columns.
 	/// \param[out] inOutByteArray A pointer to an array of bytes which will hold the output.
 	/// \param[in] outLength The size of the \a out array
 	/// \param[in] columnDelineator What character to print to delineate columns
 	void PrintColumnHeaders(unsigned char *inOutByteArray, int byteArrayLength, char columnDelineator) const
 	{
 		 self->PrintColumnHeaders((char *) inOutByteArray,byteArrayLength, columnDelineator);
 	}
 
 	/// \brief Writes a text representation of the row to \a out.
 	/// \param[out] inOutByteArray A pointer to an array of bytes which will hold the output.
 	/// \param[in] outLength The size of the \a out array
 	/// \param[in] columnDelineator What character to print to delineate columns
 	/// \param[in] printDelineatorForBinary Binary output is not printed.  True to still print the delineator.
 	/// \param[in] inputRow The row to print
 	void PrintRow(unsigned char *inOutByteArray, int byteArrayLength, char columnDelineator, bool printDelineatorForBinary, DataStructures::Table::Row* inputRow) const
 	{
 		self->PrintRow((char *)inOutByteArray,byteArrayLength,columnDelineator,printDelineatorForBinary,inputRow);
 	}
 
 	void QueryTableHelper(unsigned *columnIndicesSubset, unsigned numColumnSubset, DataStructures::List <FilterQuery> * inclusionFilters, unsigned numInclusionFilters, unsigned *rowIds, unsigned numRowIDs, Table *result)
 	{
 		FilterQuery * passedArray=NULL;
		if (inclusionFilters!=NULL)
 		{
 		passedArray=&((*inclusionFilters)[0]); /*The memory should be contigous since this is a vector class copy. It was last time I checked the implementation. So this will efficiently pass the array without needing to copy it*/
 		}
 		self->QueryTable(columnIndicesSubset, numColumnSubset, passedArray,  numInclusionFilters, rowIds, numRowIDs, result);
 		
 	}
 
 	bool UpdateCell(unsigned rowId, unsigned columnIndex, int byteLength, unsigned char *inByteArray)
 	{
 		return self->UpdateCell( rowId,  columnIndex,  byteLength,  (char *) inByteArray);
 	}
 
 	bool UpdateCellByIndex(unsigned rowIndex, unsigned columnIndex, int byteLength, unsigned char *inByteArray)
 	{
 		return self->UpdateCellByIndex( rowIndex,  columnIndex,  byteLength, (char *) inByteArray);
 	}
 	
 	//This is needed because Swig Will not ignore the definistion I tell it to, so I need to ignore all ColumnIndexes
 	unsigned ColumnIndexHelper(const char *columnName) const
 	{
 		return self->ColumnIndex(columnName);
 	}
 
 }

//---------------------------------Template Defines-------------------------
%include "RakNetCSharpTemplateDefines.i"
