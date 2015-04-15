project(sqlite3pp)
cmake_minimum_required(VERSION 2.8)
add_definitions(-std=c++11)

include_directories(src/)

aux_source_directory(src/ SRC_LIST)
add_library(${PROJECT_NAME} SHARED ${SRC_LIST})

install(DIRECTORY src/ DESTINATION include/sqlite3pp/ FILES_MATCHING PATTERN "*.h")
install(TARGETS ${PROJECT_NAME} DESTINATION lib/)

