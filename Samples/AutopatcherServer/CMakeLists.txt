cmake_minimum_required(VERSION 2.6)
project(AutopatcherServer)

IF(WIN32 AND NOT UNIX)
	FILE(GLOB AUTOSRC "${Autopatcher_SOURCE_DIR}/AutopatcherServer.cpp" "${Autopatcher_SOURCE_DIR}/MemoryCompressor.cpp" "${Autopatcher_SOURCE_DIR}/CreatePatch.cpp" "${Autopatcher_SOURCE_DIR}/AutopatcherServer.h")
	FILE(GLOB BZSRC "${BZip2_SOURCE_DIR}/*.c" "${BZip2_SOURCE_DIR}/*.h")
	LIST(REMOVE_ITEM BZSRC "${BZip2_SOURCE_DIR}/dlltest.c" "${BZip2_SOURCE_DIR}/mk251.c" "${BZip2_SOURCE_DIR}/bzip2recover.c")
	SOURCE_GROUP(BZip FILES ${BZSRC})
	SOURCE_GROUP("Source Files" FILES ${AUTOSRC})
	SOURCE_GROUP(Main "AutopatcherServerTest.cpp")
	include_directories(${RAKNETHEADERFILES} ./ ${AutopatcherPostgreRepository_SOURCE_DIR} ${PostgreSQLInterface_SOURCE_DIR} ${Autopatcher_SOURCE_DIR} ${BZip2_SOURCE_DIR}) 
	add_executable(AutopatcherServer_PostgreSQL "AutopatcherServerTest.cpp" ${AUTOSRC} ${BZSRC} readme.txt)
	target_link_libraries(AutopatcherServer_PostgreSQL ${RAKNET_COMMON_LIBS} AutopatcherPostgreRepository)
	VSUBFOLDER(AutopatcherServer_PostgreSQL "Samples/AutoPatcher/Server/PostgreSQL")
ELSE(WIN32 AND NOT UNIX)
	include_directories(${RAKNETHEADERFILES} ./ ${AutopatcherPostgreRepository_SOURCE_DIR} ${PostgreSQLInterface_SOURCE_DIR} ${Autopatcher_SOURCE_DIR}) 
	add_executable(AutopatcherServer_PostgreSQL "AutopatcherServerTest.cpp")
	target_link_libraries(AutopatcherServer_PostgreSQL ${RAKNET_COMMON_LIBS} LibAutopatcher AutopatcherPostgreRepository LibPostgreSQLInterface)
ENDIF(WIN32 AND NOT UNIX)







