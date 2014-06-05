// Copyright (C) 2002-2009 Nikolaus Gebhardt
// This file is part of the "irrKlang" library.
// For conditions of distribution and use, see copyright notice in irrKlang.h

#ifndef __I_IRRKLANG_IREFERENCE_COUNTED_H_INCLUDED__
#define __I_IRRKLANG_IREFERENCE_COUNTED_H_INCLUDED__

#include "ik_irrKlangTypes.h"

namespace irrklang
{
	//! Base class of most objects of the irrKlang.
	/** This class provides reference counting through the methods grab() and drop().
	It also is able to store a debug string for every instance of an object.
	Most objects of irrKlang are derived from IRefCounted, and so they are reference counted.

	When you create an object in irrKlang, calling a method
	which starts with 'create', an object is created, and you get a pointer
	to the new object. If you no longer need the object, you have 
	to call drop(). This will destroy the object, if grab() was not called
	in another part of you program, because this part still needs the object.
	Note, that you only need to call drop() to the object, if you created it,
	and the method had a 'create' in it. 

	A simple example:

	If you want to create a texture, you may want to call an imaginable method
	IDriver::createTexture. You call
	ITexture* texture = driver->createTexture(128, 128);
	If you no longer need the texture, call texture->drop().

	If you want to load a texture, you may want to call imaginable method
	IDriver::loadTexture. You do this like
	ITexture* texture = driver->loadTexture("example.jpg");
	You will not have to drop the pointer to the loaded texture, because
	the name of the method does not start with 'create'. The texture
	is stored somewhere by the driver.
	*/
	class IRefCounted
	{
	public:

		//! Constructor.
		IRefCounted()
			: ReferenceCounter(1)
		{
		}

		//! Destructor.
		virtual ~IRefCounted()
		{
		}

		//! Grabs the object. Increments the reference counter by one.
		//! Someone who calls grab() to an object, should later also call
		//! drop() to it. If an object never gets as much drop() as grab()
		//! calls, it will never be destroyed.
		//! The IRefCounted class provides a basic reference counting mechanism
		//! with its methods grab() and drop(). Most objects of irrklang
		//! are derived from IRefCounted, and so they are reference counted.
		//!
		//! When you create an object in irrKlang, calling a method
		//! which starts with 'create', an object is created, and you get a pointer
		//! to the new object. If you no longer need the object, you have 
		//! to call drop(). This will destroy the object, if grab() was not called
		//! in another part of you program, because this part still needs the object.
		//! Note, that you only need to call drop() to the object, if you created it,
		//! and the method had a 'create' in it. 
		//!
		//! A simple example:
		//!
		//! If you want to create a texture, you may want to call an imaginable method
		//! IDriver::createTexture. You call
		//! ITexture* texture = driver->createTexture(128, 128);
		//! If you no longer need the texture, call texture->drop().
		//! If you want to load a texture, you may want to call imaginable method
		//! IDriver::loadTexture. You do this like
		//! ITexture* texture = driver->loadTexture("example.jpg");
		//! You will not have to drop the pointer to the loaded texture, because
		//! the name of the method does not start with 'create'. The texture
		//! is stored somewhere by the driver.
		void grab() { ++ReferenceCounter; }

		//! Drops the object. Decrements the reference counter by one.
		//! Returns true, if the object was deleted.
		//! The IRefCounted class provides a basic reference counting mechanism
		//! with its methods grab() and drop(). Most objects of irrKlang
		//! Engine are derived from IRefCounted, and so they are reference counted.
		//!
		//! When you create an object in irrKlang, calling a method
		//! which starts with 'create', an object is created, and you get a pointer
		//! to the new object. If you no longer need the object, you have 
		//! to call drop(). This will destroy the object, if grab() was not called
		//! in another part of you program, because this part still needs the object.
		//! Note, that you only need to call drop() to the object, if you created it,
		//! and the method had a 'create' in it. 
		//!
		//! A simple example:
		//!
		//! If you want to create a texture, you may want to call an imaginable method
		//! IDriver::createTexture. You call
		//! ITexture* texture = driver->createTexture(128, 128);
		//! If you no longer need the texture, call texture->drop().
		//! If you want to load a texture, you may want to call imaginable method
		//! IDriver::loadTexture. You do this like
		//! ITexture* texture = driver->loadTexture("example.jpg");
		//! You will not have to drop the pointer to the loaded texture, because
		//! the name of the method does not start with 'create'. The texture
		//! is stored somewhere by the driver.
		bool drop()
		{
			--ReferenceCounter;

			if (!ReferenceCounter)
			{
				delete this;
				return true;
			}

			return false;
		}

	private:

		ik_s32	ReferenceCounter;
	};

} // end namespace irr

#endif

