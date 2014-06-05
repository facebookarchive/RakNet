#include "SQLiteLoggerCommon.h"
#include "BitStream.h"

using namespace RakNet;

static const char *sqlDataTypeNames[SQLLPDT_COUNT] = 
{
	"INTEGER",
	"INTEGER",
	"NUMERIC",
	"TEXT",
	"BLOB",
	"BLOB",
};

extern "C" const char *GetSqlDataTypeName(SQLLoggerPrimaryDataType idx) {return sqlDataTypeNames[(int)idx];}

void LogParameter::Serialize(RakNet::BitStream *bs) const
{
	unsigned char c = type;
	bs->Write(c);
	bs->Write(size);
	switch (type)
	{
		case SQLLPDT_POINTER:
		case SQLLPDT_BLOB:
		case SQLLPDT_TEXT:
			bs->WriteAlignedBytes(data.ucptr, size);
			break;
		case SQLLPDT_IMAGE:
			bs->WriteAlignedBytes(data.ucptr, size);
			bs->Write(imageWidth);
			bs->Write(imageHeight);
			bs->Write(linePitch);
			bs->Write(input_components);
			bs->Write(compressionMode);
			break;
		case SQLLPDT_REAL:
		case SQLLPDT_INTEGER:
			{
				bs->WriteAlignedBytes((const unsigned char*) &data, size);
				bs->EndianSwapBytes(bs->GetNumberOfBytesUsed()-size,size);
			}
			break;
	}
}
bool LogParameter::Deserialize(RakNet::BitStream *bs)
{
	bool b;
	unsigned char c;
	bs->Read(c);
	type=(SQLLoggerPrimaryDataType)c;
	b=bs->Read(size);
	if (size==0)
	{
		data.vptr=0;
		return b;
	}
	switch (type)
	{
	case SQLLPDT_POINTER:
	case SQLLPDT_BLOB:
	case SQLLPDT_TEXT:
		data.vptr=rakMalloc_Ex(size,_FILE_AND_LINE_);
		b=bs->ReadAlignedBytes(data.ucptr, size);
		break;
	case SQLLPDT_IMAGE:
		data.vptr=rakMalloc_Ex(size,_FILE_AND_LINE_);
		bs->ReadAlignedBytes(data.ucptr, size);
		bs->Read(imageWidth);
		bs->Read(imageHeight);
		bs->Read(linePitch);
		bs->Read(input_components);
		b=bs->Read(compressionMode);
		break;
	case SQLLPDT_REAL:
	case SQLLPDT_INTEGER:
		{
			b=bs->ReadAlignedBytes((unsigned char*) &data, size);
			if (bs->DoEndianSwap())
				bs->ReverseBytesInPlace((unsigned char *)&data, size);
		}
		break;
	}

	return b;
}
void LogParameter::DoNotFree(void)
{
	type=SQLLPDT_COUNT;
}
void LogParameter::Free(void)
{
	if (type==SQLLPDT_BLOB || type==SQLLPDT_TEXT || type==SQLLPDT_IMAGE || type==SQLLPDT_POINTER)
		Free(data.vptr);
}
void LogParameter::Free(void *v)
{
	rakFree_Ex(v,_FILE_AND_LINE_);
}
#pragma pack(push)
#pragma pack(1)
// 18 bytes
struct TGAHEADER {
	char  idlength;
	char  colourmaptype;
	char  datatypecode;
	short int colourmaporigin;
	short int colourmaplength;
	char  colourmapdepth;
	short int x_origin;
	short int y_origin;
	short width;
	short height;
	char  bitsperpixel;
	char  imagedescriptor;
};
#pragma pack(pop)
void RGBImageBlob::SaveToTGA(const char *filename)
{
// 	DirectX Color format in memory is BGRA, and written as such to disk.
// 	Written to disk is the correct side up (point of triangle facing up, as it should). However, TGA displays this incorrectly (upside down)
// 	TGA color format, on disk, is BGRA.
// 	DXT compressor input format is ARGB.

	// http://local.wasp.uwa.edu.au/~pbourke/dataformats/tga/
	FILE *fptr = fopen(filename, "wb");
	TGAHEADER h;
	memset(&h,0,sizeof(h));
	h.datatypecode=2;
	h.width=imageWidth;
	if (BitStream::IsBigEndian()==true)
		BitStream::ReverseBytesInPlace((unsigned char*) &h.width,sizeof(h.width));
	h.height=imageHeight;
	if (BitStream::IsBigEndian()==true)
		BitStream::ReverseBytesInPlace((unsigned char*) &h.height,sizeof(h.height));
	h.bitsperpixel=input_components*8;

	// TGAs have a flag indicating if they are upside down or right side up
	// Be sure to set right side up.
	// http://www.gamedev.net/community/forums/topic.asp?topic_id=42001
	h.imagedescriptor=(1<<5);

	fwrite(&h,1,sizeof(h),fptr);

/*
	putc(0,fptr);
	putc(0,fptr);
	putc(2,fptr);                         
	putc(0,fptr); putc(0,fptr);
	putc(0,fptr); putc(0,fptr);
	putc(0,fptr);
	putc(0,fptr); putc(0,fptr);           
	putc(0,fptr); putc(0,fptr);          
	putc((imageWidth & 0x00FF),fptr);
	putc((imageWidth & 0xFF00) / 256,fptr);
	putc((imageHeight & 0x00FF),fptr);
	putc((imageHeight & 0xFF00) / 256,fptr);
	putc(input_components*8,fptr);             
	putc(0,fptr);
*/


	for (int row=0; row < imageHeight; row++)
	{
		fwrite((char*) data+row*linePitch, input_components, imageWidth, fptr);
	}
	fclose(fptr);
}
