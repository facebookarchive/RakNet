#ifndef __DDS_HEADER_H__
#define __DDS_HEADER_H__ 

//  little-endian, of course
#define DDS_MAGIC 0x20534444


//  DDS_header.dwFlags
#define DDSD_CAPS                   0x00000001 
#define DDSD_HEIGHT                 0x00000002 
#define DDSD_WIDTH                  0x00000004 
#define DDSD_PITCH                  0x00000008 
#define DDSD_PIXELFORMAT            0x00001000 
#define DDSD_MIPMAPCOUNT            0x00020000 
#define DDSD_LINEARSIZE             0x00080000 
#define DDSD_DEPTH                  0x00800000 

//  DDS_header.sPixelFormat.dwFlags
#define DDPF_ALPHAPIXELS            0x00000001 
#define DDPF_FOURCC                 0x00000004 
#define DDPF_INDEXED                0x00000020 
#define DDPF_RGB                    0x00000040 

//  DDS_header.sCaps.dwCaps1
#define DDSCAPS_COMPLEX             0x00000008 
#define DDSCAPS_TEXTURE             0x00001000 
#define DDSCAPS_MIPMAP              0x00400000 

//  DDS_header.sCaps.dwCaps2
#define DDSCAPS2_CUBEMAP            0x00000200 
#define DDSCAPS2_CUBEMAP_POSITIVEX  0x00000400 
#define DDSCAPS2_CUBEMAP_NEGATIVEX  0x00000800 
#define DDSCAPS2_CUBEMAP_POSITIVEY  0x00001000 
#define DDSCAPS2_CUBEMAP_NEGATIVEY  0x00002000 
#define DDSCAPS2_CUBEMAP_POSITIVEZ  0x00004000 
#define DDSCAPS2_CUBEMAP_NEGATIVEZ  0x00008000 
#define DDSCAPS2_VOLUME             0x00200000 

#define D3DFMT_DXT1     '1TXD'    //  DXT1 compression texture format 
#define D3DFMT_DXT2     '2TXD'    //  DXT2 compression texture format 
#define D3DFMT_DXT3     '3TXD'    //  DXT3 compression texture format 
#define D3DFMT_DXT4     '4TXD'    //  DXT4 compression texture format 
#define D3DFMT_DXT5     '5TXD'    //  DXT5 compression texture format 

#define PF_IS_DXT1(pf) \
	((pf.dwFlags & DDPF_FOURCC) && \
	(pf.dwFourCC == D3DFMT_DXT1))

#define PF_IS_DXT3(pf) \
	((pf.dwFlags & DDPF_FOURCC) && \
	(pf.dwFourCC == D3DFMT_DXT3))

#define PF_IS_DXT5(pf) \
	((pf.dwFlags & DDPF_FOURCC) && \
	(pf.dwFourCC == D3DFMT_DXT5))

#define PF_IS_BGRA8(pf) \
	((pf.dwFlags & DDPF_RGB) && \
	(pf.dwFlags & DDPF_ALPHAPIXELS) && \
	(pf.dwRGBBitCount == 32) && \
	(pf.dwRBitMask == 0xff0000) && \
	(pf.dwGBitMask == 0xff00) && \
	(pf.dwBBitMask == 0xff) && \
	(pf.dwAlphaBitMask == 0xff000000U))

#define PF_IS_BGR8(pf) \
	((pf.dwFlags & DDPF_ALPHAPIXELS) && \
	!(pf.dwFlags & DDPF_ALPHAPIXELS) && \
	(pf.dwRGBBitCount == 24) && \
	(pf.dwRBitMask == 0xff0000) && \
	(pf.dwGBitMask == 0xff00) && \
	(pf.dwBBitMask == 0xff))

#define PF_IS_BGR5A1(pf) \
	((pf.dwFlags & DDPF_RGB) && \
	(pf.dwFlags & DDPF_ALPHAPIXELS) && \
	(pf.dwRGBBitCount == 16) && \
	(pf.dwRBitMask == 0x00007c00) && \
	(pf.dwGBitMask == 0x000003e0) && \
	(pf.dwBBitMask == 0x0000001f) && \
	(pf.dwAlphaBitMask == 0x00008000))

#define PF_IS_BGR565(pf) \
	((pf.dwFlags & DDPF_RGB) && \
	!(pf.dwFlags & DDPF_ALPHAPIXELS) && \
	(pf.dwRGBBitCount == 16) && \
	(pf.dwRBitMask == 0x0000f800) && \
	(pf.dwGBitMask == 0x000007e0) && \
	(pf.dwBBitMask == 0x0000001f))

#define PF_IS_INDEX8(pf) \
	((pf.dwFlags & DDPF_INDEXED) && \
	(pf.dwRGBBitCount == 8))


union DDS_header
{
	struct
	{
		unsigned int    dwMagic;
		unsigned int    dwSize;
		unsigned int    dwFlags;
		unsigned int    dwHeight;
		unsigned int    dwWidth;
		unsigned int    dwPitchOrLinearSize;
		unsigned int    dwDepth;
		unsigned int    dwMipMapCount;
		unsigned int    dwReserved1[ 11 ];

		//  DDPIXELFORMAT
		struct
		{
			unsigned int    dwSize;
			unsigned int    dwFlags;
			unsigned int    dwFourCC;
			unsigned int    dwRGBBitCount;
			unsigned int    dwRBitMask;
			unsigned int    dwGBitMask;
			unsigned int    dwBBitMask;
			unsigned int    dwAlphaBitMask;
		} sPixelFormat;

		//  DDCAPS2
		struct
		{
			unsigned int    dwCaps1;
			unsigned int    dwCaps2;
			unsigned int    dwDDSX;
			unsigned int    dwReserved;
		} sCaps;
		unsigned int    dwReserved2;
	};

	char data[ 128 ];
};

#endif  //  mydds_h



