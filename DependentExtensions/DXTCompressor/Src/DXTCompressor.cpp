/* ------------------------------------------------------------------------------------------------------------------------------------ */
// Portions of this file have been written by using the "Compress YCOCgDXT" sample as a reference.
// Please refer to http://developer.download.nvidia.com/SDK/10/opengl/samples.html#compress_YCoCgDXT for more information.
/* ------------------------------------------------------------------------------------------------------------------------------------ */


/* ------------------------------------------------------------------------------------------------------------------------------------ */
// Includes
#include <windows.h>
#include <assert.h>
#include "DXTCompressor.h"
#include "FrameBufferRenderBuffer.hpp"
#include "ShaderSource.h"
#include "OpenGLWindow.hpp"
#include <map>


/* ------------------------------------------------------------------------------------------------------------------------------------ */
// Enable performance timing
// #define DXTCOMPRESSOR_ENABLE_PERFORMANCE_TIMING


/* ------------------------------------------------------------------------------------------------------------------------------------ */
// Link with these libraries
#pragma comment(lib, "glutstatic")
#pragma comment(lib, "glew32")
#pragma comment(lib, "cg")
#pragma comment(lib, "cgGL")


/* ------------------------------------------------------------------------------------------------------------------------------------ */
// Defines
#define DXTCOMPRESSOR_WINDOW_WIDTH		640
#define DXTCOMPRESSOR_WINDOW_HEIGHT		480


/* ------------------------------------------------------------------------------------------------------------------------------------ */
// Static class data
int			DXTCompressor::m_initRefCount = 0;
CGcontext	DXTCompressor::m_cgContext; 
CGprofile	DXTCompressor::m_cgVProfile;
CGprofile	DXTCompressor::m_cgFProfile;
CGprogram	DXTCompressor::m_compressVProg;
CGprogram	DXTCompressor::m_compressDXT1RGBAFProg;
CGprogram	DXTCompressor::m_compressDXT5RGBAFProg;
CGprogram	DXTCompressor::m_compressDXT1BGRAFProg;
CGprogram	DXTCompressor::m_compressDXT5BGRAFProg;

int			DXTCompressor::m_numIterations = 0;
int			DXTCompressor::m_currentImageWidth = 0;
int			DXTCompressor::m_currentImageHeight = 0;
float		DXTCompressor::m_lastCompressionTime = 0;
float		DXTCompressor::m_accumulatedTime = 0;
float		DXTCompressor::m_timeRunningCompressionShader = 0;
float		DXTCompressor::m_timeCopyingPixelDataToCPU = 0;


/* ------------------------------------------------------------------------------------------------------------------------------------ */
// Hash table to store framebuffer objects

struct FramebufferObjectKeyCompare
{
	bool operator()(int s1, int s2) const
	{
		return s1 < s2;
	}
};

typedef int FramebufferObjectKey;
typedef class map< FramebufferObjectKey, FramebufferObject*, FramebufferObjectKeyCompare > FramebufferObjectHashtable;

// The hashtable
FramebufferObjectHashtable	s_frameBuffersHash;

// Hashkey generation
FramebufferObjectKey GetHashKey( CompressionType compressionType, int width, int height )
{
	return (int(compressionType) + width + (width*height));
}



/* ------------------------------------------------------------------------------------------------------------------------------------ */

#ifdef DXTCOMPRESSOR_ENABLE_PERFORMANCE_TIMING
// Typedef ticks
typedef __int64	Ticks;

inline Ticks GetTicks()
{
	Ticks ticks;
	QueryPerformanceCounter( (LARGE_INTEGER*) &ticks);
	return ticks;
}

inline float TicksToMilliseconds( Ticks ticks )
{
	LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq);
	return float(float(ticks) * 1000.0f / float(freq.QuadPart));
}

const int			TIMER_STACK_SIZE = 256;
static int			g_currentStackId = 0;
static Ticks		g_startTrack[ TIMER_STACK_SIZE ];
static Ticks		g_endTrack[ TIMER_STACK_SIZE ];

// Used for tracking time
inline void StartTracking()
{
	g_startTrack[ g_currentStackId++ ] = GetTicks();
}

inline void EndTracking()
{
	g_endTrack[ --g_currentStackId ] = GetTicks();
}

inline float GetDeltaTimeInSeconds()
{
	Ticks delta = g_endTrack[ g_currentStackId ] - g_startTrack[ g_currentStackId ];
	return TicksToMilliseconds( delta ) / 1000.0f;
}

inline float GetDeltaTimeInMilliseconds()
{
	Ticks delta = g_endTrack[ g_currentStackId ] - g_startTrack[ g_currentStackId ];
	return TicksToMilliseconds( delta );
}

#endif // #ifdef DXTCOMPRESSOR_ENABLE_PERFORMANCE_TIMING

/* ------------------------------------------------------------------------------------------------------------------------------------ */

DXTCompressor::DXTCompressor()
{
	m_imageWidth = 0;
	m_imageHeight = 0;
	m_compressionType = DXT1;

	m_pCompressFbo = 0;
	m_compressFboTex = 0;
}

/* ------------------------------------------------------------------------------------------------------------------------------------ */

void DXTCompressor::cgErrorCallback()
{
	CGerror lastError = cgGetError();
	if( lastError )
	{
		printf( "%s\n", cgGetErrorString( lastError ) );
		printf( "%s\n", cgGetLastListing( m_cgContext ) );
		assert( false );
	}
}

/* ------------------------------------------------------------------------------------------------------------------------------------ */

bool DXTCompressor::Initialize()
{
	if( m_initRefCount == 0 )
	{
		if( !InitOpenGL() )
			return false;
		if( !InitCG() )
			return false;
	}

	m_initRefCount++;
	return true;
}

/* ------------------------------------------------------------------------------------------------------------------------------------ */

void DXTCompressor::Shutdown()
{
	if( m_initRefCount > 0 )
		m_initRefCount--;

	if( m_initRefCount == 0 )
	{
		// Deallocate buffers
		DeallocateBuffers();

		// Shutdown cg stuff
		cgGLDisableProfile(m_cgVProfile);
		cgGLDisableProfile(m_cgFProfile);
		cgGLUnloadProgram( m_compressVProg );
		cgGLUnloadProgram( m_compressDXT1RGBAFProg );
		cgGLUnloadProgram( m_compressDXT5RGBAFProg );
		cgGLUnloadProgram( m_compressDXT1BGRAFProg );
		cgGLUnloadProgram( m_compressDXT5BGRAFProg );
		cgDestroyContext(m_cgContext);

		// Kill the OpenGL window
		KillGLWindow();
	}
}


/* ------------------------------------------------------------------------------------------------------------------------------------ */

bool DXTCompressor::InitOpenGL()
{
	bool bFullscreen = false;
#if 1
	// Compute desktop window width and height
	HWND desktopHwnd = GetDesktopWindow();
	RECT windowRect;
	GetWindowRect( desktopHwnd, &windowRect );
	int windowWidth = windowRect.right - windowRect.left;
	int windowHeight = windowRect.bottom - windowRect.top;
	bFullscreen = true;
#else
	int windowWidth = 640;
	int windowHeight = 480;
#endif

	// Create the window; this is needed in order to invoke OpenGL commands
	BOOL windowCreateSuccess = CreateGLWindow( "Testing", windowWidth, windowHeight, 32, bFullscreen, true );
	if( !windowCreateSuccess )
		return false;

	// Int GLEW
	glewInit();

	// Make sure these extensions are supported
	if (!glewIsSupported(
		"GL_VERSION_2_0 "
		"GL_ARB_vertex_program "
		"GL_ARB_fragment_program "
		"GL_NV_gpu_program4 "
		"GL_ARB_pixel_buffer_object "
		"GL_EXT_framebuffer_object "
		"GL_ARB_texture_compression "
		"GL_EXT_texture_compression_s3tc "
		"GL_EXT_texture_integer "
		))
	{
		printf("Unable to load required OpenGL extension!\n");
		return false;
	}

	// Enable depth testing
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.2, 0.2, 0.2, 1.0);

	// Report any errors to the console screen
	glutReportErrors();

	// Success
	return true;
}

/* ------------------------------------------------------------------------------------------------------------------------------------ */

bool DXTCompressor::InitCG()
{
	// Create Cg Context
	m_cgContext = cgCreateContext();
	cgSetErrorCallback( cgErrorCallback );

	// Load Cg programs
	m_cgVProfile = cgGLGetLatestProfile( CG_GL_VERTEX );
	m_cgFProfile = cgGLGetLatestProfile( CG_GL_FRAGMENT );

	// Shader compile options...
	const char *args[] =
	{
		"-unroll", "all",
		0,
	};


	m_compressVProg = cgCreateProgram( m_cgContext, CG_SOURCE, pDXTCompressorShaderSource, m_cgVProfile, "compress_vp", args );
	cgGLLoadProgram( m_compressVProg );

	m_compressDXT1RGBAFProg = cgCreateProgram( m_cgContext, CG_SOURCE, pDXTCompressorShaderSource, m_cgFProfile, "compress_DXT1_RGBA_fp", args );
	cgGLLoadProgram( m_compressDXT1RGBAFProg );

	m_compressDXT1BGRAFProg = cgCreateProgram( m_cgContext, CG_SOURCE, pDXTCompressorShaderSource, m_cgFProfile, "compress_DXT1_BGRA_fp", args );
	cgGLLoadProgram( m_compressDXT1BGRAFProg );

	m_compressDXT5RGBAFProg = cgCreateProgram( m_cgContext, CG_SOURCE, pDXTCompressorShaderSource, m_cgFProfile, "compress_YCoCgDXT5_RGBA_fp", args );
	cgGLLoadProgram( m_compressDXT5RGBAFProg );

	m_compressDXT5BGRAFProg = cgCreateProgram( m_cgContext, CG_SOURCE, pDXTCompressorShaderSource, m_cgFProfile, "compress_YCoCgDXT5_BGRA_fp", args );
	cgGLLoadProgram( m_compressDXT5BGRAFProg );

	return true;
}

/* ------------------------------------------------------------------------------------------------------------------------------------ */

bool DXTCompressor::IsInitialized()
{
	return (m_initRefCount != 0);
}

/* ------------------------------------------------------------------------------------------------------------------------------------ */

static GLuint CreateTexture( GLenum target, GLint internalformat, GLenum format, GLenum type, int w, int h )
{
	GLuint tex;
	glGenTextures(1, &tex);
	glBindTexture(target, tex);
	glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(target, 0, internalformat, w, h, 0, format, type, 0);
	return tex;
}

/* ------------------------------------------------------------------------------------------------------------------------------------ */

FramebufferObject* DXTCompressor::RequestFBO( CompressionType compressionType, int width, int height )
{
	// Get hash key
	FramebufferObjectKey hashKey = GetHashKey( compressionType, width, height );

	// See if we have it in the hash
	if( s_frameBuffersHash.find( hashKey ) != s_frameBuffersHash.end() )
	{
		return s_frameBuffersHash[ hashKey ];
	}

	// Create the texture
	GLuint newTextureID = 0;
	FramebufferObject* pNewFrameBufferObject = new FramebufferObject();
	if( compressionType == DXT1 )
	{
		newTextureID = CreateTexture(GL_TEXTURE_2D, GL_LUMINANCE_ALPHA32UI_EXT, GL_LUMINANCE_ALPHA_INTEGER_EXT, GL_INT, width/4, height/4);
	}
	else if( compressionType == DXT5_YCOCG )
	{
		newTextureID = CreateTexture(GL_TEXTURE_2D, GL_RGBA32UI_EXT, GL_RGBA_INTEGER_EXT, GL_INT, width/4, height/4);
	}

	// Attach texture to framebuffer object
	pNewFrameBufferObject->Bind();
	pNewFrameBufferObject->AttachTexture(GL_TEXTURE_2D, newTextureID, GL_COLOR_ATTACHMENT0_EXT);
	pNewFrameBufferObject->IsValid();
	FramebufferObject::Disable();

	// Add to hash
	s_frameBuffersHash[ hashKey ] = pNewFrameBufferObject;

	// Return
	return pNewFrameBufferObject;
}

/* ------------------------------------------------------------------------------------------------------------------------------------ */

void DXTCompressor::DeallocateBuffers()
{
	FramebufferObjectHashtable::iterator iter = s_frameBuffersHash.begin();
	while( iter != s_frameBuffersHash.end() )
	{
		// Delete object
		FramebufferObject* pFrameBufferObject = iter->second;
		GLuint textureID = pFrameBufferObject->GetAttachedTextureID();
		glDeleteTextures(1, &textureID);
		delete pFrameBufferObject;

		// Bump
		++iter;
	}
}

/* ------------------------------------------------------------------------------------------------------------------------------------ */

static inline void SetParameter( CGprogram prog, char *name, float x, float y=0.0, float z=0.0, float w=0.0 )
{
	CGparameter param = cgGetNamedParameter( prog, name );
	if( param )
	{
		cgGLSetParameter4f( param, x, y, z, w );
	}
}

/* ------------------------------------------------------------------------------------------------------------------------------------ */

void DXTCompressor::SetShaderConstants()
{
	SetParameter( m_compressDXT1RGBAFProg, "imageSize", m_imageWidth, m_imageHeight );
	SetParameter( m_compressDXT5RGBAFProg, "imageSize", m_imageWidth, m_imageHeight );
	SetParameter( m_compressDXT1BGRAFProg, "imageSize", m_imageWidth, m_imageHeight );
	SetParameter( m_compressDXT5BGRAFProg, "imageSize", m_imageWidth, m_imageHeight );
}

/* ------------------------------------------------------------------------------------------------------------------------------------ */

static inline void DrawQuad()
{
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0); glVertex2f(-1.0, -1.0);
	glTexCoord2f(1.0, 0.0); glVertex2f(1.0, -1.0);
	glTexCoord2f(1.0, 1.0); glVertex2f(1.0, 1.0);
	glTexCoord2f(0.0, 1.0); glVertex2f(-1.0, 1.0);
	glEnd();
}

/* ------------------------------------------------------------------------------------------------------------------------------------ */

void DXTCompressor::CompressInternal(bool sourceFormatIsBGRA)
{
#ifdef DXTCOMPRESSOR_ENABLE_PERFORMANCE_TIMING
	StartTracking();
#endif

	glViewport(0, 0, m_imageWidth/4, m_imageHeight/4);
	glDisable(GL_DEPTH_TEST);

	cgGLBindProgram(m_compressVProg);
	cgGLEnableProfile(m_cgVProfile);

	if( m_compressionType == DXT5_YCOCG )
	{
		if (sourceFormatIsBGRA==false)
			cgGLBindProgram(m_compressDXT5RGBAFProg);
		else
			cgGLBindProgram(m_compressDXT5BGRAFProg);
	}
	else if( m_compressionType == DXT1 )
	{
		if (sourceFormatIsBGRA==false)
			cgGLBindProgram(m_compressDXT1RGBAFProg);
		else
			cgGLBindProgram(m_compressDXT1BGRAFProg);
	}
	else
	{
		assert(false );
	}

	cgGLEnableProfile(m_cgFProfile);

	glBindTexture(GL_TEXTURE_2D, m_imageTexId);
	SetShaderConstants();

	DrawQuad();

	cgGLDisableProfile(m_cgVProfile);
	cgGLDisableProfile(m_cgFProfile);

#ifdef DXTCOMPRESSOR_ENABLE_PERFORMANCE_TIMING
	glFinish();
	EndTracking();
	m_timeRunningCompressionShader += GetDeltaTimeInMilliseconds();
#endif
}

/* ------------------------------------------------------------------------------------------------------------------------------------ */
int DXTCompressor::GetBufferSize( CompressionType compressionType, int inputWidth, int inputHeight )
{
	if( compressionType == DXT5_YCOCG )
	{
		int size = (inputWidth/4)*(inputHeight/4)*8;
		return sizeof(GLushort)*size;
	}
	else
	{
		int size = (inputWidth/4)*(inputHeight/4)*4;
		return sizeof(GLushort)*size;
	}
}
/* ------------------------------------------------------------------------------------------------------------------------------------ */
void DXTCompressor::DoCompression( void* ppOutputData, bool sourceFormatIsBGRA )
{
	if( m_compressionType == DXT5_YCOCG )
	{
		// Render to integer fbo
		m_pCompressFbo->Bind();
		CompressInternal(sourceFormatIsBGRA);

		// Readback data to host
		int size = (m_imageWidth/4)*(m_imageHeight/4)*8;
		GLushort *data = (GLushort *) ppOutputData;

		// Copy pixel data
#ifdef DXTCOMPRESSOR_ENABLE_PERFORMANCE_TIMING
		StartTracking();
#endif
		glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);
		glReadPixels(0, 0, m_imageWidth/4, m_imageHeight/4, GL_RGBA_INTEGER_EXT, GL_UNSIGNED_INT, data);
		FramebufferObject::Disable();
#ifdef DXTCOMPRESSOR_ENABLE_PERFORMANCE_TIMING
		EndTracking();
		m_timeCopyingPixelDataToCPU += GetDeltaTimeInMilliseconds();
#endif
	}
	else if( m_compressionType == DXT1 )
	{
		// Render to integer fbo
		m_pCompressFbo->Bind();
		CompressInternal(sourceFormatIsBGRA);

		// Readback data to host
		int size = (m_imageWidth/4)*(m_imageHeight/4)*4;
		GLushort *data = (GLushort *) ppOutputData;

		// Copy pixel data
#ifdef DXTCOMPRESSOR_ENABLE_PERFORMANCE_TIMING
		StartTracking();
#endif
		glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);
		glReadPixels(0, 0, m_imageWidth/4, m_imageHeight/4, GL_LUMINANCE_ALPHA_INTEGER_EXT, GL_UNSIGNED_INT, data);
		FramebufferObject::Disable();
#ifdef DXTCOMPRESSOR_ENABLE_PERFORMANCE_TIMING
		EndTracking();
		m_timeCopyingPixelDataToCPU += GetDeltaTimeInMilliseconds();
#endif
	}
}

/* ------------------------------------------------------------------------------------------------------------------------------------ */

bool DXTCompressor::CompressImageData( CompressionType compressionType, const void *inputRGBA, int inputWidth, int inputHeight, void *outputData, bool bDisplayResults, bool sourceFormatIsBGRA )
{
	// Make sure we're initialized
	if( !IsInitialized() )
	{
		printf( "You need to initialize DXTCompressor before calling compress!\n " );
		return false;
	}

	// Make sure the source width and height are divisible by 4
	// This is a requirement by the DXT compression algorithm
	if( !( (inputWidth%4)==0 && (inputHeight%4)==0 ) )
	{
		printf( "Error! Input image width and height must be multiple of 4, as required by DXT compression rules. You have passed in an image of %dx%d", inputWidth, inputHeight );
		return false;
	}

	// Accumulate width and heights
	m_numIterations++;
	m_currentImageWidth = inputWidth;
	m_currentImageHeight = inputHeight;

	// Instantiate the compressor
	DXTCompressor compressor;
	compressor.m_imageWidth = inputWidth;
	compressor.m_imageHeight = inputHeight;
	compressor.m_compressionType = compressionType;

	// Make a copy of the source data and flip the Y. OpenGL rendering has Y going down
	/*
	char* pFlippedData = new char[ inputWidth*inputHeight*4 ];
	{
		const int rowSize = inputWidth*4;
		char* pRunnerDest = pFlippedData + (rowSize*(inputHeight-1));
		const char* pRunnerSrc = (const char*)inputRGBA;
		for( int row = inputHeight-1; row >=0; row-- )
		{
			memcpy( pRunnerDest, pRunnerSrc, rowSize );
			pRunnerSrc += rowSize;
			pRunnerDest -= rowSize;
		}
	}
	*/
	
	// Generate a texture and bind it to the input source data
	glGenTextures(1, &compressor.m_imageTexId);
	glBindTexture(GL_TEXTURE_2D, compressor.m_imageTexId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, inputWidth, inputHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, (const GLvoid*)pFlippedData );
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, inputWidth, inputHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, (const GLvoid*)inputRGBA );

	// Request FBO
	compressor.m_pCompressFbo = RequestFBO( compressionType, inputWidth, inputHeight );
	compressor.m_compressFboTex = compressor.m_pCompressFbo->GetAttachedTextureID();

#ifdef DXTCOMPRESSOR_ENABLE_PERFORMANCE_TIMING
	// Start the clock
	StartTracking();
#endif

	// Do the compression
	compressor.DoCompression( outputData, sourceFormatIsBGRA );

#ifdef DXTCOMPRESSOR_ENABLE_PERFORMANCE_TIMING
	// Stop the clock
	EndTracking();
	m_lastCompressionTime = GetDeltaTimeInMilliseconds();
	m_accumulatedTime += m_lastCompressionTime;
	printf( "Compression time: %f ms\n", m_lastCompressionTime );
#endif

	// Display texture? Only DXT1 supported here.
	if( bDisplayResults && compressionType == DXT1 )
	{
		// Create empty dxt1 compressed texture
		GLuint tempDisplayTexture;
		int dxt1Size = (inputWidth/4)*(inputHeight/4)*8;
		GLubyte * tempPadData = new GLubyte [dxt1Size];
		memset(tempPadData, 0, dxt1Size);
		glGenTextures(1, &tempDisplayTexture);
		glBindTexture(GL_TEXTURE_2D, tempDisplayTexture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glCompressedTexImage2DARB(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGB_S3TC_DXT1_EXT, inputWidth, inputHeight, 0, dxt1Size, tempPadData);
		delete [] tempPadData;

		// Re-upload the texture to VRAM for display
		int size = (inputWidth/4)*(inputHeight/4);
		glBindTexture(GL_TEXTURE_2D, tempDisplayTexture);
		glCompressedTexImage2DARB(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGB_S3TC_DXT1_EXT, inputWidth, inputHeight, 0, size*8, outputData);

		// Display the texture
		DisplayTexture( tempDisplayTexture, inputWidth, inputHeight );

		// Get rid of the texture
		glDeleteTextures(1, &tempDisplayTexture);
	}

	// Clean up
// 	delete [] pFlippedData;
// 	pFlippedData = NULL;
	glDeleteTextures( 1, &compressor.m_imageTexId );

	// Done
	return true;
}

#include <stdlib.h>
#include "DDSHeader.h"

/* ------------------------------------------------------------------------------------------------------------------------------------ */

int DXTCompressor::GetDDSHeaderSize(void)
{
	return sizeof(DDS_header);
}

/* ------------------------------------------------------------------------------------------------------------------------------------ */

void DXTCompressor::WriteDDSHeader( CompressionType compressionType, int width, int height, int compresedDataLength, void *outputData )
{
	DDS_header* pHdr = (DDS_header*)outputData;
	memset( pHdr, 0, sizeof(DDS_header) );

	pHdr->dwMagic = DDS_MAGIC;
	pHdr->dwSize = 124;
	pHdr->dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
	pHdr->dwWidth = width;
	pHdr->dwHeight = height;

	pHdr->sCaps.dwCaps1 = DDSCAPS_TEXTURE | DDSD_CAPS;

	pHdr->sPixelFormat.dwSize = 32;
	pHdr->sPixelFormat.dwFlags = DDPF_FOURCC;

	if( compressionType == DXT1 )
		pHdr->sPixelFormat.dwFourCC = D3DFMT_DXT1;
	else if( compressionType == DXT5_YCOCG )
		pHdr->sPixelFormat.dwFourCC = D3DFMT_DXT5;
}

/* ------------------------------------------------------------------------------------------------------------------------------------ */

void DXTCompressor::WriteDDSMemoryFile( CompressionType compressionType, int width, int height, const void* pCompressedData, int compresedDataLength, void **outputData, int *outputLength )
{
	// Allocate the header + data
	int totalSize = sizeof(DDS_header) + compresedDataLength;
	void* pMemFile = new char[ totalSize ];

	// Write the header
	WriteDDSHeader(compressionType, width, height, compresedDataLength, pMemFile );
	
	// Write the data
	void* pData = ((char*)pMemFile + sizeof(DDS_header));
	memcpy( pData, pCompressedData, compresedDataLength );

	// Return data to user
	*outputData = pMemFile;
	*outputLength = totalSize;
}

/* ------------------------------------------------------------------------------------------------------------------------------------ */

void DXTCompressor::PrintPerformanceLog()
{
#ifdef DXTCOMPRESSOR_ENABLE_PERFORMANCE_TIMING
	// Compute
	float mPixelsPerSec = (m_currentImageWidth*m_currentImageHeight*m_numIterations) / (m_accumulatedTime*1e6/1000.0f);
	printf( "For %dx%d image, compression took %f ms, Average of %f mPixelsPerSec (mPixels/sec)\n", m_currentImageWidth, m_currentImageHeight, m_accumulatedTime/m_numIterations, mPixelsPerSec );
	printf( "%f ms was spent in running compression shader, %f ms was spent copying pixel data to main memory for the cpu\n", m_timeRunningCompressionShader/m_numIterations, m_timeCopyingPixelDataToCPU/m_numIterations );

	// Reset stats
	m_currentImageWidth = 0;
	m_currentImageHeight = 0;
	m_numIterations = 0;
	m_accumulatedTime = 0;
	m_timeCopyingPixelDataToCPU = 0;
	m_timeRunningCompressionShader = 0;
#endif
}

/* ------------------------------------------------------------------------------------------------------------------------------------ */

static void glutPrint(float x, float y, const char *s, void *font)
{
	int i, len;

	glRasterPos2f(x, y);
	len = (int) strlen(s);
	for (i = 0; i < len; i++) {
		glutBitmapCharacter(font, s[i]);
	}
}

static bool DrawTexture( GLuint textureID )
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0,0,windowWidth,windowHeight);	
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glEnable(GL_TEXTURE_2D);

	glDisable(GL_DEPTH_TEST);
	glColor3f(1.0, 1.0, 1.0);

	DrawQuad();

	glDisable(GL_TEXTURE_2D);

	glLoadIdentity();

	glutPrint(-0.95, -0.95, "DXT1 compressed (push ESC to close)", GLUT_BITMAP_9_BY_15);

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	return true;
}

void ClientResize(HWND hWnd, int nWidth, int nHeight)
{
	RECT rcClient, rcWindow;
	POINT ptDiff;
	GetClientRect(hWnd, &rcClient);
	GetWindowRect(hWnd, &rcWindow);
	ptDiff.x = (rcWindow.right - rcWindow.left) - rcClient.right;
	ptDiff.y = (rcWindow.bottom - rcWindow.top) - rcClient.bottom;
	MoveWindow(hWnd,rcWindow.left, rcWindow.top, nWidth + ptDiff.x, nHeight + ptDiff.y, TRUE);
}


void DXTCompressor::DisplayTexture( GLuint textureID, int texW, int texH )
{
	// Show the window
	ShowWindow(hWnd,SW_SHOW);

	// Resize the window to match the size of the texture
	ClientResize( hWnd, texW, texH );

	// Windows message pump
	MSG		msg;
	BOOL	done=FALSE;
	while(!done)
	{
		if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))
		{
			if (msg.message==WM_QUIT)
			{
				done=TRUE;
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			if ((active && !DrawTexture( textureID )) || keys[VK_ESCAPE])
			{
				keys[VK_ESCAPE] = FALSE;
				done=TRUE;
			}
			else
			{
				SwapBuffers(hDC);
			}
		}
	}

	// Hide the window
	ShowWindow(hWnd,SW_HIDE);
}

