cmake_minimum_required(VERSION 2.6)
GETCURRENTFOLDER()

project(${current_folder})
include_directories(${RAKNETHEADERFILES} ./) 
add_executable(${current_folder} DirectoryDeltaTransferTest.cpp readme.txt)
target_link_libraries(${current_folder} ${RAKNET_COMMON_LIBS})
set_target_properties(${current_folder} PROPERTIES PROJECT_GROUP Samples)






