#ifndef _SQLITE_LOGGER_COMMON_H
#define _SQLITE_LOGGER_COMMON_H

#include <string.h>
#include "NativeTypes.h"

namespace RakNet
{
	class BitStream;

enum SQLLoggerPrimaryDataType
{
	SQLLPDT_POINTER,
	SQLLPDT_INTEGER,
	SQLLPDT_REAL,
	SQLLPDT_TEXT,
	SQLLPDT_BLOB,
	SQLLPDT_IMAGE,
	SQLLPDT_COUNT,
};

extern "C" {
const char *GetSqlDataTypeName(SQLLoggerPrimaryDataType idx);
};

struct BlobDescriptor
{
	BlobDescriptor() {}
	BlobDescriptor(void *_data, int _size) : data(_data), size(_size) {}
	void *data;
	int size;
};
struct RGBImageBlob
{
	enum ImageBlobCompressionMode
	{
		// Fast to compress and read back (1-5 milliseconds to compress depending on image size, server must have NVidia based 3d card)
		DXT,
		// Good storage, slow to compress (about 20 milliseconds for 320x200 on my system).
		JPG
	};
	RGBImageBlob() {data=0; imageWidth=0; imageHeight=0; linePitch=0; input_components=0; compressionMode=DXT; sourceFormatIsBGRA=true;}
	RGBImageBlob(void *_data, uint16_t _imageWidth, uint16_t _imageHeight, uint16_t _linePitch, unsigned char _input_components, bool _sourceFormatIsBGRA=true, ImageBlobCompressionMode mode=DXT) : data(_data), imageWidth(_imageWidth), imageHeight(_imageHeight), linePitch(_linePitch), input_components(_input_components),sourceFormatIsBGRA(_sourceFormatIsBGRA),compressionMode((unsigned char) mode) {}
	// This is just for testing
	void SaveToTGA(const char *filename);
	void *data;
	uint16_t imageWidth;
	uint16_t imageHeight;
	uint16_t linePitch; // bytes per row, may be larger than image width, in which case the excess is discarded
	unsigned char input_components; // 3 for RGB, 4 for RGBA
	unsigned char compressionMode;
	bool sourceFormatIsBGRA; // If true, then G and B will be swapped on the server so that the input is RGBA. This is necessary with Direct3D9
};

#define MAX_SQLLITE_LOGGER_PARAMETERS 12

struct LogParameter
{
	LogParameter() {}

	/*
	template <class T> LogParameter(const T t) : type(SQLLPDT_UNKNOWN), size(sizeof(t)), data(&t) {}
	template <> LogParameter(const void *t) : type(SQLLPDT_POINTER), size(sizeof(t)), data(&t) {}
	template <> LogParameter(const unsigned char t) : type(SQLLPDT_INTEGER), size(sizeof(t)), data(&t) {}
	template <> LogParameter(const char t) : type(SQLLPDT_INTEGER), size(sizeof(t)), data(&t) {}
	template <> LogParameter(const unsigned int t) : type(SQLLPDT_INTEGER), size(sizeof(t)), data(&t) {}
	template <> LogParameter(const int t) : type(SQLLPDT_INTEGER), size(sizeof(t)), data(&t) {}
	template <> LogParameter(const unsigned short t) : type(SQLLPDT_INTEGER), size(sizeof(t)), data(&t) {}
	template <> LogParameter(const short t) : type(SQLLPDT_INTEGER), size(sizeof(t)), data(&t) {}
	template <> LogParameter(const unsigned long long t) : type(SQLLPDT_INTEGER), size(sizeof(t)), data(&t) {}
	template <> LogParameter(const long long t) : type(SQLLPDT_INTEGER), size(sizeof(t)), data(&t) {}
	template <> LogParameter(const float t) : type(SQLLPDT_REAL), size(sizeof(t)), data(&t) {}
	template <> LogParameter(const double t) : type(SQLLPDT_REAL), size(sizeof(t)), data(&t) {}
	template <> LogParameter(const unsigned char *&t) : type(SQLLPDT_TEXT), size((uint32_t) strlen((const char*) t)), data(t) {}
	template <> LogParameter(const char *t) : type(SQLLPDT_TEXT), size((uint32_t) strlen(t)), data(t) {}
	template <> LogParameter(const BlobDescriptor *t) : type(SQLLPDT_BLOB), size(t->size), data(t->data) {}
	template <> LogParameter(const RGBImageBlob *t) : type(SQLLPDT_IMAGE), size(t->size), data(t->data), imageDescriptorFormat(t->imageDescriptorFormat), bytesPerPixel(t->bytesPerPixel), imageWidth(t->imageWidth)  {}
	template <> LogParameter(const BlobDescriptor t) : type(SQLLPDT_BLOB), size(t.size), data(t.data) {}
	template <> LogParameter(const RGBImageBlob t) : type(SQLLPDT_IMAGE), size(t.size), data(t.data), imageDescriptorFormat(t.imageDescriptorFormat), bytesPerPixel(t.bytesPerPixel), imageWidth(t.imageWidth) {}
	*/

	LogParameter(void *t) : type(SQLLPDT_POINTER), size(sizeof(t)), data(t) {}
	LogParameter(unsigned char t) : type(SQLLPDT_INTEGER), size(sizeof(t)), data(t) {}
	LogParameter(char t) : type(SQLLPDT_INTEGER), size(sizeof(t)), data(t) {}
	LogParameter(unsigned int t) : type(SQLLPDT_INTEGER), size(sizeof(t)), data(t) {}
	LogParameter(int t) : type(SQLLPDT_INTEGER), size(sizeof(t)), data(t) {}
	LogParameter(unsigned short t) : type(SQLLPDT_INTEGER), size(sizeof(t)), data(t) {}
	LogParameter(short t) : type(SQLLPDT_INTEGER), size(sizeof(t)), data(t) {}
	LogParameter(unsigned long long t) : type(SQLLPDT_INTEGER), size(sizeof(t)), data(t) {}
	LogParameter(long long t) : type(SQLLPDT_INTEGER), size(sizeof(t)), data(t) {}
	LogParameter(float t) : type(SQLLPDT_REAL), size(sizeof(t)), data(t) {}
	LogParameter(double t) : type(SQLLPDT_REAL), size(sizeof(t)), data(t) {}
	LogParameter(unsigned char *t) : type(SQLLPDT_TEXT), size((uint32_t) strlen((const char*) t)), data(t) {}
	LogParameter(char *t) : type(SQLLPDT_TEXT), size((uint32_t) strlen(t)), data(t) {}
	LogParameter(const char t[]) : type(SQLLPDT_TEXT), size((uint32_t) strlen(t)), data(t) {}
	LogParameter(const unsigned char t[]) : type(SQLLPDT_TEXT), size((uint32_t) strlen((const char*) t)), data(t) {}
	LogParameter(BlobDescriptor *t) : type(SQLLPDT_BLOB), size(t->size), data(t->data) {}
	LogParameter(RGBImageBlob *t) : type(SQLLPDT_IMAGE), size(t->linePitch*t->imageHeight), data(t->data), imageWidth(t->imageWidth), imageHeight(t->imageHeight), linePitch(t->linePitch), input_components(t->input_components), compressionMode(t->compressionMode), sourceFormatIsBGRA(t->sourceFormatIsBGRA)  {}
	LogParameter(BlobDescriptor t) : type(SQLLPDT_BLOB), size(t.size), data(t.data) {}
	LogParameter(RGBImageBlob t) : type(SQLLPDT_IMAGE), size(t.linePitch*t.imageHeight), data(t.data), imageWidth(t.imageWidth), imageHeight(t.imageHeight), linePitch(t.linePitch), input_components(t.input_components), compressionMode(t.compressionMode), sourceFormatIsBGRA(t.sourceFormatIsBGRA) {}

	SQLLoggerPrimaryDataType type;
	uint32_t size;

	union DataUnion
	{
		DataUnion() {}
		DataUnion(void *t) : vptr(t) {}
		DataUnion(unsigned char t) : c(t) {}
		DataUnion(char t) : c(t) {}
		DataUnion(unsigned int t) : i(t) {}
		DataUnion(int t) : i(t) {}
		DataUnion(unsigned short t) : s(t) {}
		DataUnion(short t) : s(t) {}
		DataUnion(unsigned long long t) : ll(t) {}
		DataUnion(long long t) : ll(t) {}
		DataUnion(float t) : f(t) {}
		DataUnion(double t) : d(t) {}
		DataUnion(unsigned char *t) : ucptr(t) {}
		DataUnion(char *t) : cptr(t) {}
		DataUnion(const char t[]) : cptr((char*) t) {}
		DataUnion(const unsigned char t[]) : cptr((char*) t) {}
		DataUnion(BlobDescriptor *t) : vptr(t->data) {}
		DataUnion(RGBImageBlob *t) : vptr(t->data) {}
		DataUnion(const BlobDescriptor &t) : vptr(t.data) {}
		DataUnion(const RGBImageBlob &t) : vptr(t.data) {}

		void *vptr;
		char c;
		int i;
		short s;
		long long ll;
		float f;
		double d;
		char *cptr;
		unsigned char *ucptr;
	};	
	DataUnion data;

	// Only used for SQLLPDT_IMAGE
	uint16_t imageWidth;
	uint16_t imageHeight;
	uint16_t linePitch; // Pitch is bytes per row
	unsigned char input_components; // Bytes per pixel
	unsigned char compressionMode;
	bool sourceFormatIsBGRA;

	void Serialize(RakNet::BitStream *bs) const;
	// Don't forget to deallocate after calling Deserialize
	bool Deserialize(RakNet::BitStream *bs);
	void DoNotFree(void);
	void Free(void);
	static void Free(void *v);
};

}


#endif
