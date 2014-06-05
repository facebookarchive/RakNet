/*
Copyright (c) 2005, 
Aaron Lefohn	(lefohn@cs.ucdavis.edu)
Adam Moerschell (atmoerschell@ucdavis.edu)
All rights reserved.

This software is licensed under the BSD open-source license. See
http://www.opensource.org/licenses/bsd-license.php for more detail.

*************************************************************
Redistribution and use in source and binary forms, with or 
without modification, are permitted provided that the following 
conditions are met:

Redistributions of source code must retain the above copyright notice, 
this list of conditions and the following disclaimer. 

Redistributions in binary form must reproduce the above copyright notice, 
this list of conditions and the following disclaimer in the documentation 
and/or other materials provided with the distribution. 

Neither the name of the University of Californa, Davis nor the names of 
the contributors may be used to endorse or promote products derived 
from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS 
FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL 
THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE 
GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF 
THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
OF SUCH DAMAGE.
*/

#ifndef __FRAMEBUFFERRENDERBUFFER_HPP__
#define __FRAMEBUFFERRENDERBUFFER_HPP__


#include <GL/glew.h>
#include <iostream>

/*!
FramebufferObject Class. This class encapsulates the FramebufferObject
(FBO) OpenGL spec. See the official spec at:
http://oss.sgi.com/projects/ogl-sample/registry/EXT/framebuffer_object.txt

for details.

A framebuffer object (FBO) is conceptually a structure containing pointers
to GPU memory. The memory pointed to is either an OpenGL texture or an
OpenGL RenderBuffer. FBOs can be used to render to one or more textures,
share depth buffers between multiple sets of color buffers/textures and
are a complete replacement for pbuffers.

Performance Notes:
1) It is more efficient (but not required) to call Bind() 
on an FBO before making multiple method calls. For example:

FramebufferObject fbo;
fbo.Bind();
fbo.AttachTexture(GL_TEXTURE_2D, texId0, GL_COLOR_ATTACHMENT0_EXT);
fbo.AttachTexture(GL_TEXTURE_2D, texId1, GL_COLOR_ATTACHMENT1_EXT);
fbo.IsValid();

To provide a complete encapsulation, the following usage
pattern works correctly but is less efficient:

FramebufferObject fbo;
// NOTE : No Bind() call
fbo.AttachTexture(GL_TEXTURE_2D, texId0, GL_COLOR_ATTACHMENT0_EXT);
fbo.AttachTexture(GL_TEXTURE_2D, texId1, GL_COLOR_ATTACHMENT1_EXT);
fbo.IsValid();

The first usage pattern binds the FBO only once, whereas
the second usage binds/unbinds the FBO for each method call.

2) Use FramebufferObject::Disable() sparingly. We have intentionally
left out an "Unbind()" method because it is largely unnecessary
and encourages rendundant Bind/Unbind coding. Binding an FBO is
usually much faster than enabling/disabling a pbuffer, but is
still a costly operation. When switching between multiple FBOs
and a visible OpenGL framebuffer, the following usage pattern 
is recommended:

FramebufferObject fbo1, fbo2;
fbo1.Bind();
... Render ...
// NOTE : No Unbind/Disable here...

fbo2.Bind();
... Render ...

// Disable FBO rendering and return to visible window
// OpenGL framebuffer.
FramebufferObject::Disable();
*/
class FramebufferObject
{
public:
	/// Ctor/Dtor
	FramebufferObject();
	virtual ~FramebufferObject();

	/// Bind this FBO as current render target
	void Bind();

	/// Bind a texture to the "attachment" point of this FBO
	virtual void AttachTexture( GLenum texTarget, 
		GLuint texId,
		GLenum attachment = GL_COLOR_ATTACHMENT0_EXT,
		int mipLevel      = 0,
		int zSlice        = 0 );

	/// Bind an array of textures to multiple "attachment" points of this FBO
	///  - By default, the first 'numTextures' attachments are used,
	///    starting with GL_COLOR_ATTACHMENT0_EXT
	virtual void AttachTextures( int numTextures, 
		GLenum texTarget[], 
		GLuint texId[],
		GLenum attachment[] = NULL,
		int mipLevel[]      = NULL,
		int zSlice[]        = NULL );

	/// Bind a render buffer to the "attachment" point of this FBO
	virtual void AttachRenderBuffer( GLuint buffId,
		GLenum attachment = GL_COLOR_ATTACHMENT0_EXT );

	/// Bind an array of render buffers to corresponding "attachment" points
	/// of this FBO.
	/// - By default, the first 'numBuffers' attachments are used,
	///   starting with GL_COLOR_ATTACHMENT0_EXT
	virtual void AttachRenderBuffers( int numBuffers, GLuint buffId[],
		GLenum attachment[] = NULL );

	/// Free any resource bound to the "attachment" point of this FBO
	void Unattach( GLenum attachment );

	/// Free any resources bound to any attachment points of this FBO
	void UnattachAll();

	/// Is this FBO currently a valid render target?
	///  - Sends output to std::cerr by default but can
	///    be a user-defined C++ stream
	///
	/// NOTE : This function works correctly in debug build
	///        mode but always returns "true" if NDEBUG is
	///        is defined (optimized builds)
#ifndef NDEBUG
	bool IsValid( std::ostream& ostr = std::cerr );
#else
	bool IsValid( std::ostream& ostr = std::cerr ) { 
		return true; 
	}
#endif

	/// BEGIN : Accessors
	/// Is attached type GL_RENDERBUFFER_EXT or GL_TEXTURE?
	GLenum GetAttachedType( GLenum attachment );

	/// What is the Id of Renderbuffer/texture currently 
	/// attached to "attachement?"
	GLuint GetAttachedId( GLenum attachment );

	/// Which mipmap level is currently attached to "attachement?"
	GLint  GetAttachedMipLevel( GLenum attachment );

	/// Which cube face is currently attached to "attachment?"
	GLint  GetAttachedCubeFace( GLenum attachment );

	/// Which z-slice is currently attached to "attachment?"
	GLint  GetAttachedZSlice( GLenum attachment );
	/// END : Accessors

	GLuint GetAttachedTextureID() const { return m_attachedTexture; }


	/// BEGIN : Static methods global to all FBOs
	/// Return number of color attachments permitted
	static int GetMaxColorAttachments();

	/// Disable all FBO rendering and return to traditional,
	/// windowing-system controlled framebuffer
	///  NOTE:
	///     This is NOT an "unbind" for this specific FBO, but rather
	///     disables all FBO rendering. This call is intentionally "static"
	///     and named "Disable" instead of "Unbind" for this reason. The
	///     motivation for this strange semantic is performance. Providing
	///     "Unbind" would likely lead to a large number of unnecessary
	///     FBO enablings/disabling.
	static void Disable();
	/// END : Static methods global to all FBOs

protected:
	void  _GuardedBind();
	void  _GuardedUnbind();
	void  _FramebufferTextureND( GLenum attachment, GLenum texTarget, 
		GLuint texId, int mipLevel, int zSlice );
	static GLuint _GenerateFboId();

private:
	GLuint m_fboId;
	GLint  m_savedFboId;
	GLuint m_attachedTexture;
};



#include <iostream>
using namespace std;

FramebufferObject::FramebufferObject()
: m_fboId(_GenerateFboId()),
m_savedFboId(0)
{
	// Bind this FBO so that it actually gets created now
	_GuardedBind();
	_GuardedUnbind();
}

FramebufferObject::~FramebufferObject() 
{
	glDeleteFramebuffersEXT(1, &m_fboId);
}

void FramebufferObject::Bind() 
{
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_fboId);
}

void FramebufferObject::Disable() 
{
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

void
FramebufferObject::AttachTexture( GLenum texTarget, GLuint texId, 
								 GLenum attachment, int mipLevel, int zSlice )
{
	_GuardedBind();

	/*
	#ifndef NDEBUG
	if( GetAttachedId(attachment) != texId ) {
	#endif
	*/

	_FramebufferTextureND( attachment, texTarget,
		texId, mipLevel, zSlice );

	m_attachedTexture = texId;

	/*
	#ifndef NDEBUG
	}
	else {
	cerr << "FramebufferObject::AttachTexture PERFORMANCE WARNING:\n"
	<< "\tRedundant bind of texture (id = " << texId << ").\n"
	<< "\tHINT : Compile with -DNDEBUG to remove this warning.\n";
	}
	#endif
	*/

	_GuardedUnbind();
}

void
FramebufferObject::AttachTextures( int numTextures, GLenum texTarget[], GLuint texId[],
								  GLenum attachment[], int mipLevel[], int zSlice[] )
{
	for(int i = 0; i < numTextures; ++i) {
		AttachTexture( texTarget[i], texId[i], 
			attachment ? attachment[i] : (GL_COLOR_ATTACHMENT0_EXT + i), 
			mipLevel ? mipLevel[i] : 0, 
			zSlice ? zSlice[i] : 0 );
	}
}

void
FramebufferObject::AttachRenderBuffer( GLuint buffId, GLenum attachment )
{
	_GuardedBind();

#ifndef NDEBUG
	if( GetAttachedId(attachment) != buffId ) {
#endif

		glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, attachment, 
			GL_RENDERBUFFER_EXT, buffId);

#ifndef NDEBUG
	}
	else {
		cerr << "FramebufferObject::AttachRenderBuffer PERFORMANCE WARNING:\n"
			<< "\tRedundant bind of Renderbuffer (id = " << buffId << ")\n"
			<< "\tHINT : Compile with -DNDEBUG to remove this warning.\n";
	}
#endif

	_GuardedUnbind();
}

void
FramebufferObject::AttachRenderBuffers( int numBuffers, GLuint buffId[], GLenum attachment[] )
{
	for(int i = 0; i < numBuffers; ++i) {
		AttachRenderBuffer( buffId[i], 
			attachment ? attachment[i] : (GL_COLOR_ATTACHMENT0_EXT + i) );
	}
}

void
FramebufferObject::Unattach( GLenum attachment )
{
	_GuardedBind();
	GLenum type = GetAttachedType(attachment);

	switch(type) {
case GL_NONE:
	break;
case GL_RENDERBUFFER_EXT:
	AttachRenderBuffer( 0, attachment );
	break;
case GL_TEXTURE:
	AttachTexture( GL_TEXTURE_2D, 0, attachment );
	break;
default:
	cerr << "FramebufferObject::unbind_attachment ERROR: Unknown attached resource type\n";
	}
	_GuardedUnbind();
}

void
FramebufferObject::UnattachAll()
{
	int numAttachments = GetMaxColorAttachments();
	for(int i = 0; i < numAttachments; ++i) {
		Unattach( GL_COLOR_ATTACHMENT0_EXT + i );
	}
}

GLint FramebufferObject::GetMaxColorAttachments()
{
	GLint maxAttach = 0;
	glGetIntegerv( GL_MAX_COLOR_ATTACHMENTS_EXT, &maxAttach );
	return maxAttach;
}

GLuint FramebufferObject::_GenerateFboId()
{
	GLuint id = 0;
	glGenFramebuffersEXT(1, &id);
	return id;
}

void FramebufferObject::_GuardedBind() 
{
	// Only binds if m_fboId is different than the currently bound FBO
	glGetIntegerv( GL_FRAMEBUFFER_BINDING_EXT, &m_savedFboId );
	if (m_fboId != (GLuint)m_savedFboId) {
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_fboId);
	}
}

void FramebufferObject::_GuardedUnbind() 
{
	// Returns FBO binding to the previously enabled FBO
	if (m_fboId != (GLuint)m_savedFboId) {
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, (GLuint)m_savedFboId);
	}
}

void
FramebufferObject::_FramebufferTextureND( GLenum attachment, GLenum texTarget,
										 GLuint texId, int mipLevel,
										 int zSlice )
{
	if (texTarget == GL_TEXTURE_1D) {
		glFramebufferTexture1DEXT( GL_FRAMEBUFFER_EXT, attachment,
			GL_TEXTURE_1D, texId, mipLevel );
	}
	else if (texTarget == GL_TEXTURE_3D) {
		glFramebufferTexture3DEXT( GL_FRAMEBUFFER_EXT, attachment,
			GL_TEXTURE_3D, texId, mipLevel, zSlice );
	}
	else {
		// Default is GL_TEXTURE_2D, GL_TEXTURE_RECTANGLE_ARB, or cube faces
		glFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT, attachment,
			texTarget, texId, mipLevel );
	}
}

#ifndef NDEBUG
bool FramebufferObject::IsValid( ostream& ostr )
{
	_GuardedBind();

	bool isOK = false;

	GLenum status;                                            
	status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	switch(status) {                                          
case GL_FRAMEBUFFER_COMPLETE_EXT: // Everything's OK
	isOK = true;
	break;
case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
	ostr << "glift::CheckFramebufferStatus() ERROR:\n\t"
		<< "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT\n";
	isOK = false;
	break;
case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
	ostr << "glift::CheckFramebufferStatus() ERROR:\n\t"
		<< "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT\n";
	isOK = false;
	break;
case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
	ostr << "glift::CheckFramebufferStatus() ERROR:\n\t"
		<< "GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT\n";
	isOK = false;
	break;
case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
	ostr << "glift::CheckFramebufferStatus() ERROR:\n\t"
		<< "GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT\n";
	isOK = false;
	break;
case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
	ostr << "glift::CheckFramebufferStatus() ERROR:\n\t"
		<< "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT\n";
	isOK = false;
	break;
case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
	ostr << "glift::CheckFramebufferStatus() ERROR:\n\t"
		<< "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT\n";
	isOK = false;
	break;
case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
	ostr << "glift::CheckFramebufferStatus() ERROR:\n\t"
		<< "GL_FRAMEBUFFER_UNSUPPORTED_EXT\n";
	isOK = false;
	break;
default:
	ostr << "glift::CheckFramebufferStatus() ERROR:\n\t"
		<< "Unknown ERROR\n";
	isOK = false;
	}

	_GuardedUnbind();
	return isOK;
}
#endif // NDEBUG

/// Accessors
GLenum FramebufferObject::GetAttachedType( GLenum attachment )
{
	// Returns GL_RENDERBUFFER_EXT or GL_TEXTURE
	_GuardedBind();
	GLint type = 0;
	glGetFramebufferAttachmentParameterivEXT(GL_FRAMEBUFFER_EXT, attachment,
		GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE_EXT, 
		&type);
	_GuardedUnbind();
	return GLenum(type);
}

GLuint FramebufferObject::GetAttachedId( GLenum attachment )
{
	_GuardedBind();
	GLint id = 0;
	glGetFramebufferAttachmentParameterivEXT(GL_FRAMEBUFFER_EXT, attachment,
		GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME_EXT,
		&id);
	_GuardedUnbind();
	return GLuint(id);
}

GLint FramebufferObject::GetAttachedMipLevel( GLenum attachment )
{
	_GuardedBind();
	GLint level = 0;
	glGetFramebufferAttachmentParameterivEXT(GL_FRAMEBUFFER_EXT, attachment,
		GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL_EXT, 
		&level);
	_GuardedUnbind();
	return level;
}

GLint FramebufferObject::GetAttachedCubeFace( GLenum attachment )
{
	_GuardedBind();
	GLint level = 0;
	glGetFramebufferAttachmentParameterivEXT(GL_FRAMEBUFFER_EXT, attachment,
		GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE_EXT,
		&level);
	_GuardedUnbind();
	return level;
}

GLint FramebufferObject::GetAttachedZSlice( GLenum attachment )
{
	_GuardedBind();
	GLint slice = 0;
	glGetFramebufferAttachmentParameterivEXT(GL_FRAMEBUFFER_EXT, attachment,
		GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_3D_ZOFFSET_EXT,
		&slice);
	_GuardedUnbind();
	return slice;
}



/*!
Renderbuffer Class. This class encapsulates the Renderbuffer OpenGL
object described in the FramebufferObject (FBO) OpenGL spec. 
See the official spec at:
http://oss.sgi.com/projects/ogl-sample/registry/EXT/framebuffer_object.txt
for complete details.

A "Renderbuffer" is a chunk of GPU memory used by FramebufferObjects to
represent "traditional" framebuffer memory (depth, stencil, and color buffers).
By "traditional," we mean that the memory cannot be bound as a texture. 
With respect to GPU shaders, Renderbuffer memory is "write-only." Framebuffer
operations such as alpha blending, depth test, alpha test, stencil test, etc.
read from this memory in post-fragement-shader (ROP) operations.

The most common use of Renderbuffers is to create depth and stencil buffers.
Note that as of 7/1/05, NVIDIA drivers to do not support stencil Renderbuffers.

Usage Notes:
1) "internalFormat" can be any of the following:
Valid OpenGL internal formats beginning with:
RGB, RGBA, DEPTH_COMPONENT

or a stencil buffer format (not currently supported 
in NVIDIA drivers as of 7/1/05).
STENCIL_INDEX1_EXT 
STENCIL_INDEX4_EXT     
STENCIL_INDEX8_EXT     
STENCIL_INDEX16_EXT
*/
class Renderbuffer
{
public:
	/// Ctors/Dtors
	Renderbuffer();
	Renderbuffer(GLenum internalFormat, int width, int height);
	~Renderbuffer();

	void   Bind();
	void   Unbind();
	void   Set(GLenum internalFormat, int width, int height);
	GLuint GetId() const;

	static GLint GetMaxSize();

private:
	GLuint m_bufId;
	static GLuint _CreateBufferId();
};

#include <iostream>
using namespace std;

Renderbuffer::Renderbuffer()
: m_bufId(_CreateBufferId())
{}

Renderbuffer::Renderbuffer(GLenum internalFormat, int width, int height)
: m_bufId(_CreateBufferId())
{
	Set(internalFormat, width, height);
}

Renderbuffer::~Renderbuffer()
{
	glDeleteRenderbuffersEXT(1, &m_bufId);
}

void Renderbuffer::Bind() 
{
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, m_bufId);
}

void Renderbuffer::Unbind() 
{
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);
}

void Renderbuffer::Set(GLenum internalFormat, int width, int height)
{
	int maxSize = Renderbuffer::GetMaxSize();
	if (width > maxSize || height > maxSize ) {
		cerr << "Renderbuffer::Renderbuffer() ERROR:\n\t"
			<< "Size too big (" << width << ", " << height << ")\n";
		return;
	}

	// Guarded bind
	GLint savedId = 0;
	glGetIntegerv( GL_RENDERBUFFER_BINDING_EXT, &savedId );
	if (savedId != (GLint)m_bufId) {
		Bind();
	}

	// Allocate memory for renderBuffer
	glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, internalFormat, width, height );

	// Guarded unbind
	if (savedId != (GLint)m_bufId) {
		glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, savedId);
	}
}

GLuint Renderbuffer::GetId() const 
{
	return m_bufId;
}

GLint Renderbuffer::GetMaxSize()
{
	GLint maxAttach = 0;
	glGetIntegerv( GL_MAX_RENDERBUFFER_SIZE_EXT, &maxAttach );
	return maxAttach;
}

GLuint Renderbuffer::_CreateBufferId() 
{
	GLuint id = 0;
	glGenRenderbuffersEXT(1, &id);
	return id;
}


#endif // __FRAMEBUFFERRENDERBUFFER_HPP__

