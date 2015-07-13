# CMake build file list for sqlite3pp

cmake_minimum_required (VERSION 3.2)

PROJECT(sqlite3pp)

if(CMAKE_COMPILER_IS_GNUCXX)
	include(CheckCXXCompilerFlag)
	CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
	CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
	if(COMPILER_SUPPORTS_CXX11)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
	elseif(COMPILER_SUPPORTS_CXX0X)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
	else()
        message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
	endif()
endif()

include_directories ("${PROJECT_SOURCE_DIR}/src")

add_library(sqlite3pp STATIC ${PROJECT_SOURCE_DIR}/src/sqlite3pp.cpp ${PROJECT_SOURCE_DIR}/src/sqlite3ppext.cpp)

add_executable (testaggregate ${PROJECT_SOURCE_DIR}/test/testaggregate.cpp)
target_link_libraries (testaggregate sqlite3pp sqlite3)

add_executable (testattach ${PROJECT_SOURCE_DIR}/test/testattach.cpp)
target_link_libraries (testattach sqlite3pp sqlite3)

add_executable (testcallback ${PROJECT_SOURCE_DIR}/test/testcallback.cpp)
target_link_libraries (testcallback sqlite3pp sqlite3)

add_executable (testdisconnect ${PROJECT_SOURCE_DIR}/test/testdisconnect.cpp)
target_link_libraries (testdisconnect sqlite3pp sqlite3)

add_executable (testfunction ${PROJECT_SOURCE_DIR}/test/testfunction.cpp)
target_link_libraries (testfunction sqlite3pp sqlite3)

add_executable (testinsert ${PROJECT_SOURCE_DIR}/test/testinsert.cpp)
target_link_libraries (testinsert sqlite3pp sqlite3)

add_executable (testinsertall ${PROJECT_SOURCE_DIR}/test/testinsertall.cpp)
target_link_libraries (testinsertall sqlite3pp sqlite3)

add_executable (testselect ${PROJECT_SOURCE_DIR}/test/testselect.cpp)
target_link_libraries (testselect sqlite3pp sqlite3)

install (TARGETS sqlite3pp DESTINATION lib)
install (FILES "${PROJECT_SOURCE_DIR}/src/sqlite3pp.h" DESTINATION include)
install (FILES "${PROJECT_SOURCE_DIR}/src/sqlite3ppext.h" DESTINATION include)
