cmake_minimum_required(VERSION 2.6)
project(AutopatcherClient)

set(Autopatcher_SOURCE_DIR ${RakNet_SOURCE_DIR}/DependentExtensions/Autopatcher)
set(BZip2_SOURCE_DIR ${RakNet_SOURCE_DIR}/DependentExtensions/bzip2-1.0.6)

include_directories(${RAKNETHEADERFILES} ./ ${Autopatcher_SOURCE_DIR} ${BZip2_SOURCE_DIR} )
FILE(GLOB AUTOSRC "${Autopatcher_SOURCE_DIR}/*.cpp" "${Autopatcher_SOURCE_DIR}/*.h")
LIST(REMOVE_ITEM AUTOSRC "${Autopatcher_SOURCE_DIR}/AutopatcherServer.cpp" "${Autopatcher_SOURCE_DIR}/AutopatcherServer.h" )
FILE(GLOB BZSRC "${BZip2_SOURCE_DIR}/*.c" "${BZip2_SOURCE_DIR}/*.h")
LIST(REMOVE_ITEM BZSRC "${BZip2_SOURCE_DIR}/dlltest.c" "${BZip2_SOURCE_DIR}/mk251.c" "${BZip2_SOURCE_DIR}/bzip2recover.c")
SOURCE_GROUP(BZip2 FILES ${BZSRC})
SET(WRAPFILES "${Autopatcher_SOURCE_DIR}/MemoryCompressor.cpp" "${Autopatcher_SOURCE_DIR}/MemoryCompressor.h")
LIST(REMOVE_ITEM AUTOSRC ${WRAPFILES})
SOURCE_GROUP(Client_Files FILES ${AUTOSRC})
SOURCE_GROUP(MAIN FILES "AutopatcherClientTest.cpp")
SOURCE_GROUP(BZip2Wrapper FILES ${WRAPFILES})
add_executable(AutopatcherClient "AutopatcherClientTest.cpp" ${AUTOSRC} ${BZSRC} ${WRAPFILES} "readme.txt")
target_link_libraries(AutopatcherClient ${RAKNET_COMMON_LIBS})

##VSUBFOLDER(AutopatcherClient "Samples/AutoPatcher/Client")


