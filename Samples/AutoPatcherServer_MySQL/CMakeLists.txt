cmake_minimum_required(VERSION 2.6)
project("AutoPatcherServer_MySQL")
IF(WIN32 AND NOT UNIX)
	FILE(GLOB AUTOSRC "${Autopatcher_SOURCE_DIR}/AutopatcherServer.cpp" "${Autopatcher_SOURCE_DIR}/MemoryCompressor.cpp" "${Autopatcher_SOURCE_DIR}/CreatePatch.cpp" "${Autopatcher_SOURCE_DIR}/AutopatcherServer.h")
	FILE(GLOB BZSRC "${BZip2_SOURCE_DIR}/*.c" "${BZip2_SOURCE_DIR}/*.h")
	LIST(REMOVE_ITEM BZSRC "${BZip2_SOURCE_DIR}/dlltest.c" "${BZip2_SOURCE_DIR}/mk251.c" "${BZip2_SOURCE_DIR}/bzip2recover.c")
	SOURCE_GROUP(BZip FILES ${BZSRC})
	SOURCE_GROUP("Source Files" FILES ${AUTOSRC})
	SOURCE_GROUP(Main "AutopatcherServerTest_MySQL.cpp")
	include_directories(${RAKNETHEADERFILES} ./ ${AutopatcherMySQLRepository_SOURCE_DIR} ${MySQLInterface_SOURCE_DIR} ${Autopatcher_SOURCE_DIR} ${BZip2_SOURCE_DIR}) 
	add_executable("AutoPatcherServer_MySQL" "AutopatcherServerTest_MySQL.cpp" readme.txt ${AUTOSRC} ${BZSRC})
	target_link_libraries("AutoPatcherServer_MySQL" ${RAKNET_COMMON_LIBS} AutoPatcherMySQLRepository)
	VSUBFOLDER(AutoPatcherServer_MySQL "Samples/AutoPatcher/Server/MySQL")
ELSE(WIN32 AND NOT UNIX)
	include_directories(${RAKNETHEADERFILES} ./ ${AutopatcherMySQLRepository_SOURCE_DIR} ${MySQLInterface_SOURCE_DIR} ${Autopatcher_SOURCE_DIR}) 
	add_executable("AutoPatcherServer_MySQL" "AutopatcherServerTest_MySQL.cpp" readme.txt)
	target_link_libraries("AutoPatcherServer_MySQL" ${RAKNET_COMMON_LIBS} LibAutopatcher AutoPatcherMySQLRepository LibMySQLInterface)
ENDIF(WIN32 AND NOT UNIX)






