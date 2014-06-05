cmake_minimum_required(VERSION 2.6)
project(AutopatcherClientGFx3)
IF(WIN32 AND NOT UNIX)
	FINDD3D()
	FINDSCALEGFX()
	include_directories(${RAKNETHEADERFILES} ./ ${Autopatcher_SOURCE_DIR} ${BZip2_SOURCE_DIR} ${D3D_INCLUDE_DIR} ${SCALEGFX_INCLUDE_DIR} "${RakNet_SOURCE_DIR}/DependentExtensions/GFx3")
	FILE(GLOB GFXSRC GFxPlayerTinyD3D9.cpp "${RakNet_SOURCE_DIR}/DependentExtensions/GFx3/*.cpp" "${RakNet_SOURCE_DIR}/DependentExtensions/GFx3/*.h")
	FILE(GLOB AUTOSRC "${Autopatcher_SOURCE_DIR}/*.cpp" "${Autopatcher_SOURCE_DIR}/*.h")
	FILE(GLOB BZSRC "${BZip2_SOURCE_DIR}/*.c" "${BZip2_SOURCE_DIR}/*.h")
	LIST(REMOVE_ITEM BZSRC "${BZip2_SOURCE_DIR}/dlltest.c" "${BZip2_SOURCE_DIR}/mk251.c" "${BZip2_SOURCE_DIR}/bzip2recover.c")
	SOURCE_GROUP(BZip2 FILES ${BZSRC})
	SET(WRAPFILES "${Autopatcher_SOURCE_DIR}/MemoryCompressor.cpp" "${Autopatcher_SOURCE_DIR}/MemoryCompressor.h")
	LIST(REMOVE_ITEM AUTOSRC ${WRAPFILES})
	SOURCE_GROUP(Client_Files FILES ${AUTOSRC})
	SOURCE_GROUP(Main FILES "AutopatcherClientGFx3Impl.cpp" AutopatcherClientGFx3Impl.h)
	SOURCE_GROUP(BZip2Wrapper FILES ${WRAPFILES})
	SOURCE_GROUP(GFx3 FILES ${GFXSRC})
	add_executable(AutopatcherClientGFx3 WIN32 "AutopatcherClientGFx3Impl.cpp" AutopatcherClientGFx3Impl.h ${AUTOSRC} ${BZSRC} ${WRAPFILES} ${GFXSRC} "readme.txt")
	target_link_libraries(AutopatcherClientGFx3 ${RAKNET_COMMON_LIBS} ${D3D_LIBRARIES} winmm.lib imm32.lib ${SCALEGFX_DEBUG_LIBRARIES} ${SCALEGFX_LIBRARIES})
	VSUBFOLDER(AutopatcherClientGFx3 "Samples/AutoPatcher/Client")
ENDIF(WIN32 AND NOT UNIX)






