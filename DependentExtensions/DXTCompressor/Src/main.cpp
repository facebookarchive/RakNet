#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "DXTCompressor.h"

/* ------------------------------------------------------------------------------------------------------------------------------------ */

bool LoadTGAFromFile( const char* pFilename, void **image, int* width, int* height )
{
	typedef struct
	{
		char identsize;
		char colourmaptype;
		char imagetype;
		unsigned short colourmapstart;
		unsigned short colourmaplength;
		char colourmapbits;
		unsigned short xstart;
		unsigned short ystart;
		unsigned short width;
		unsigned short height;
		char bits;
		char descriptor;

	} TGA_HEADER;

	// Open the file
	FILE* pic;
	if((pic=fopen( pFilename, "rb"))==NULL ) 
	{
		return false;
	}

	// Zero out the header
	TGA_HEADER TGAheader;
	memset(&TGAheader,0,sizeof(TGA_HEADER));

	// Read the header
	fread(&TGAheader.identsize,sizeof(char),1,pic);
	fread(&TGAheader.colourmaptype,sizeof(char),1,pic);
	fread(&TGAheader.imagetype,sizeof(char),1,pic);
	fread(&TGAheader.colourmapstart,sizeof(unsigned short),1,pic);
	fread(&TGAheader.colourmaplength,sizeof(unsigned short),1,pic);
	fread(&TGAheader.colourmapbits,sizeof(char),1,pic);
	fread(&TGAheader.xstart,sizeof(unsigned short),1,pic);
	fread(&TGAheader.ystart,sizeof(unsigned short),1,pic);
	fread(&TGAheader.width,sizeof(unsigned short),1,pic);
	fread(&TGAheader.height,sizeof(unsigned short),1,pic);
	fread(&TGAheader.bits,sizeof(char),1,pic);
	fread(&TGAheader.descriptor,sizeof(char),1,pic);

	*width = TGAheader.width;
	*height = TGAheader.height;
	int DataSize = TGAheader.width*TGAheader.height*4;

	// Read the pixels
	*image = new char[DataSize];
	if ((TGAheader.descriptor>>5) & 1)
	{
		// Right side up
		fread(*image, sizeof(char),DataSize, pic);
	}
	else
	{
		//Upside down
		for (int row=TGAheader.height-1; row >=0; row--)
		{
			fread(((char*) (*image))+row*TGAheader.width*TGAheader.bits/8, TGAheader.bits/8, TGAheader.width, pic);
		}

	}
	// Close the file
	fclose(pic);

	// TGA is stored on disk BGRA
	// Endian swap bits so that the image is actually in RGBA format
	if( TGAheader.bits == 32 )
	{
		unsigned char* pRunner = (unsigned char*)*image;
		for( int i = 0; i < DataSize; i+=4 )
		{
			char color[4] =
			{
				pRunner[ 0 ],
				pRunner[ 1 ],
				pRunner[ 2 ],
				pRunner[ 3 ],
			};

			pRunner[ 0 ] = color[ 2 ];
			pRunner[ 1 ] = color[ 1 ];
			pRunner[ 2 ] = color[ 0 ];
			pRunner[ 3 ] = color[ 3 ];

			pRunner += 4;
		}
	}

	return true;
}

/* ------------------------------------------------------------------------------------------------------------------------------------ */

int main( int argc, const char* argv[] )
{
	// Initialize the compressor
	DXTCompressor::Initialize();

	// Load sample .tga
	void* pSourceData;
	int w, h;
	//bool bFileLoaded = LoadTGAFromFile( "1600x1200.tga", &pSourceData, &w, &h );
	bool bFileLoaded = LoadTGAFromFile( "320x200.tga", &pSourceData, &w, &h );

	if( bFileLoaded )
	{

		// Test performance
// 		const int numIterations = 100;
// 		for( int i = 0; i < numIterations; i++ )
// 		{
// 			// Compress the data
// 			void*	pOutputData;
// 			int		outputLength;
// 			bool bCompressSuccess = DXTCompressor::CompressRGBAImageData( DXT1, pSourceData, w, h, &pOutputData, &outputLength, false );
// 
// 			// Clean up
// 			delete [] pOutputData;
// 			pOutputData = NULL;
// 		}

		// Print total stats
//		printf( "\n\n****Total stats on %d iterations****\n", numIterations );
// 		DXTCompressor::PrintPerformanceLog();

		// Now test saving to DDS memory file
		{
			// Compress the data
// 			void* pCompressedOutput;
// 			int compressedOutputLength;
// 			bool bCompressSuccess = DXTCompressor::CompressRGBAImageData( DXT1, pSourceData, w, h, &pCompressedOutput, &compressedOutputLength, false );

			char *outputData;
			int bufferSize = DXTCompressor::GetBufferSize(DXT1,
				w,
				h);
			int ddsHeaderSize = DXTCompressor::GetDDSHeaderSize();
			outputData = (char*) malloc(bufferSize + ddsHeaderSize );
			bool bCompressSuccess = DXTCompressor::CompressRGBAImageData(
				DXT1,
				pSourceData,
				w,
				h,
				outputData+ddsHeaderSize, false );


			if( bCompressSuccess )
			{
				// Save DDS file
// 				void* pOutputDDSFile;
// 				int outputDDSFileLength;
// 				DXTCompressor::WriteDDSMemoryFile( DXT1, w, h, pCompressedOutput, compressedOutputLength, &pOutputDDSFile, &outputDDSFileLength ); 

				// Clean up
// 				delete [] pCompressedOutput;
// 				pCompressedOutput = NULL;
// 				delete [] pOutputDDSFile;
// 				pOutputDDSFile = NULL;

				DXTCompressor::WriteDDSHeader(DXT1,
					w,
					h,
					bufferSize,
					outputData);

				FILE *fp = fopen("DXTCompressorTGAtoDDS.dds", "wb");
				fwrite(outputData,1,bufferSize + ddsHeaderSize,fp);
				fclose(fp);

				free(outputData);
			}
		}
	}

	// Shutdown the compressor
	DXTCompressor::Shutdown();

	return 0;
}