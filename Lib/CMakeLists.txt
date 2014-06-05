cmake_minimum_required(VERSION 2.6)

set( RAKNET_INTERNAL_INCLUDE_DIRS 
	${RakNet_SOURCE_DIR}/Source 
	${RakNet_SOURCE_DIR}/DependentExtensions/openssl-1.0.0d/include 
	${RakNet_SOURCE_DIR}/DependentExtensions
)

if( RAKNET_ENABLE_STATIC )
	add_subdirectory(LibStatic)
endif()


if( RAKNET_ENABLE_DLL )
	add_subdirectory(DLL)
endif()
