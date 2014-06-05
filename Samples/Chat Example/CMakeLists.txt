cmake_minimum_required(VERSION 2.6)
project("Chat Example")
include_directories(${RAKNETHEADERFILES} ./) 
add_executable(ChatServer "Chat Example Server.cpp" readme.txt)
add_executable(ChatClient "Chat Example Client.cpp" readme.txt)

target_link_libraries(ChatServer ${RAKNET_COMMON_LIBS})
VSUBFOLDER(ChatServer "Samples")

target_link_libraries(ChatClient ${RAKNET_COMMON_LIBS})
VSUBFOLDER(ChatClient "Samples")






