cmake_minimum_required(VERSION 2.6)
project(Lobby2Client)
include_directories(${RAKNETHEADERFILES} ./ ${RakNet_SOURCE_DIR}/DependentExtensions/Lobby2 ${RakNet_SOURCE_DIR}/DependentExtensions/Lobby2/Rooms)
FILE(GLOB LOBBYFILES ${RakNet_SOURCE_DIR}/DependentExtensions/Lobby2/*.cpp ${RakNet_SOURCE_DIR}/DependentExtensions/Lobby2/*.h)
SOURCE_GROUP(Main FILES "Lobby2ClientSample.cpp")
add_executable(Lobby2Client "Lobby2ClientSample.cpp" ${LOBBYFILES})
target_link_libraries(Lobby2Client ${RAKNET_COMMON_LIBS})
IF(WIN32 AND NOT UNIX)
	VSUBFOLDER(Lobby2Client "Samples/Lobby2")
ENDIF(WIN32 AND NOT UNIX)






