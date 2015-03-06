cmake_minimum_required(VERSION 2.6)

project(RakNet)

if( NOT APPLE )
# check 64 bit
if( CMAKE_SIZEOF_VOID_P MATCHES "4" )
  set( HAVE_64_BIT 0 )
else(  CMAKE_SIZEOF_VOID_P MATCHES "4")
  set( HAVE_64_BIT 1 )
endif(  CMAKE_SIZEOF_VOID_P MATCHES "4")
endif( NOT APPLE )

IF (WIN32 AND NOT UNIX)
	set (PROGRAMFILESX86 $ENV{PROGRAMFILES})
	string(REPLACE "\\" "/" PROGRAMFILESX86 ${PROGRAMFILESX86})
ENDIF(WIN32 AND NOT UNIX)

IF (WIN32 AND NOT UNIX)
	set(RAKNET_LIBRARY_LIBS ws2_32.lib)
ELSE(WIN32 AND NOT UNIX)
	set(RAKNET_LIBRARY_LIBS pthread)
ENDIF(WIN32 AND NOT UNIX)


# Options
option( RAKNET_ENABLE_SAMPLES 	"Generate RakNet sample projects if true." 		TRUE  )
option( RAKNET_ENABLE_DLL		"Generate the DLL project if true." 			TRUE )
option( RAKNET_ENABLE_STATIC	"Generate the static library project if true." 	TRUE )
option( RAKNET_GENERATE_INCLUDE_ONLY_DIR "Setup a include/RakNet/ directory in which all the headers are copied." FALSE )

set( RAKNETHEADERFILES ${RakNet_SOURCE_DIR}/Source ) #This name doesn't follow CMake conventions but for retro compatibility I'll let it there.

if( RAKNET_GENERATE_INCLUDE_ONLY_DIR )
	set( RAKNET_INCLUDE_ONLY_DIR ${RakNet_SOURCE_DIR}/include ) # this will be visible by client code
	set( RAKNET_NAMED_INCLUDE_ONLY_DIR ${RAKNET_INCLUDE_ONLY_DIR}/RakNet )
	message( STATUS "Setting up the ${RAKNET_NAMED_INCLUDE_ONLY_DIR} directory..." )
	# Now setup the include/RakNet/*.h files.
	file( MAKE_DIRECTORY ${RAKNET_NAMED_INCLUDE_ONLY_DIR} )
	file( COPY ${RAKNETHEADERFILES}/ DESTINATION ${RAKNET_NAMED_INCLUDE_ONLY_DIR} FILES_MATCHING PATTERN "*.h"  )
	message( STATUS "DONE: Setting up the ${RAKNET_NAMED_INCLUDE_ONLY_DIR} directory." )
endif()

set( RAKNET_INCLUDE_DIRS ${RAKNETHEADERFILES} ${RAKNET_INCLUDE_ONLY_DIR} PARENT_SCOPE ) # Visible from outside

include(./CmakeIncludes/CmakeMacros.txt)
FIXLINKOPTIONS()
FIXCOMPILEOPTIONS()


add_subdirectory(Lib)


set(RAKNET_COMMON_LIBS RakNetLibStatic)

if( RAKNET_ENABLE_SAMPLES )
	add_subdirectory(Samples)
endif()