# CMake build file list for sqlite3pp

cmake_minimum_required (VERSION 3.2)

PROJECT(sqlite3pp)

set(PROJECT_NAME "sqlite3pp")
set(PROJECT_DESCRIPTION "SQLite3++ is a C++ wrapper of SQLite3 API.")
string(TIMESTAMP PROJECT_VERSION "%Y-%m-%d %H:%M")
if(NOT DEFINED CMAKE_INSTALL_LIBDIR)
  set(CMAKE_INSTALL_LIBDIR lib)
endif(NOT DEFINED CMAKE_INSTALL_LIBDIR)
set(PKG_CONFIG_CFLAGS "-I${CMAKE_INSTALL_PREFIX}/include")
set(PKG_CONFIG_EXEC_PREFIX ${CMAKE_INSTALL_PREFIX})
set(PKG_CONFIG_INCLUDEDIR ${CMAKE_INSTALL_PREFIX}/include)
set(PKG_CONFIG_LIBDIR ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR})
set(PKG_CONFIG_LIBS "-L${PKG_CONFIG_LIBDIR} -lsqlite3")

if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
   set(CMAKE_BUILD_TYPE Release)
endif(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)

if(CMAKE_COMPILER_IS_GNUCXX OR "${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
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
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fexceptions -O0 -D_DEBUG -DDEBUG")
endif()

include_directories("${PROJECT_SOURCE_DIR}/src")

add_library(sqlite3pp STATIC ${PROJECT_SOURCE_DIR}/src/sqlite3pp.cpp ${PROJECT_SOURCE_DIR}/src/sqlite3ppext.cpp)
set_property(TARGET sqlite3pp PROPERTY INTERFACE_POSITION_INDEPENDENT_CODE ON)

add_executable(testaggregate ${PROJECT_SOURCE_DIR}/test/testaggregate.cpp)
target_link_libraries(testaggregate sqlite3pp sqlite3)
set_property(TARGET testaggregate PROPERTY POSITION_INDEPENDENT_CODE ON)

add_executable(testattach ${PROJECT_SOURCE_DIR}/test/testattach.cpp)
target_link_libraries(testattach sqlite3pp sqlite3)
set_property(TARGET testattach PROPERTY POSITION_INDEPENDENT_CODE ON)

add_executable(testcallback ${PROJECT_SOURCE_DIR}/test/testcallback.cpp)
target_link_libraries(testcallback sqlite3pp sqlite3)
set_property(TARGET testcallback PROPERTY POSITION_INDEPENDENT_CODE ON)

add_executable(testdisconnect ${PROJECT_SOURCE_DIR}/test/testdisconnect.cpp)
target_link_libraries(testdisconnect sqlite3pp sqlite3)
set_property(TARGET testdisconnect PROPERTY POSITION_INDEPENDENT_CODE ON)

add_executable(testfunction ${PROJECT_SOURCE_DIR}/test/testfunction.cpp)
target_link_libraries(testfunction sqlite3pp sqlite3)
set_property(TARGET testfunction PROPERTY POSITION_INDEPENDENT_CODE ON)

add_executable(testinsert ${PROJECT_SOURCE_DIR}/test/testinsert.cpp)
target_link_libraries(testinsert sqlite3pp sqlite3)
set_property(TARGET testinsert PROPERTY POSITION_INDEPENDENT_CODE ON)

add_executable(testinsertall ${PROJECT_SOURCE_DIR}/test/testinsertall.cpp)
target_link_libraries(testinsertall sqlite3pp sqlite3)
set_property(TARGET testinsertall PROPERTY POSITION_INDEPENDENT_CODE ON)

add_executable(testselect ${PROJECT_SOURCE_DIR}/test/testselect.cpp)
target_link_libraries(testselect sqlite3pp sqlite3)
set_property(TARGET testselect PROPERTY POSITION_INDEPENDENT_CODE ON)

install(TARGETS sqlite3pp DESTINATION lib)
install(FILES "${PROJECT_SOURCE_DIR}/src/sqlite3pp.h" DESTINATION include)
install(FILES "${PROJECT_SOURCE_DIR}/src/sqlite3ppext.h" DESTINATION include)
install(FILES "${PROJECT_SOURCE_DIR}/README.md" DESTINATION share/doc/${PROJECT_NAME})

include(FindPkgConfig QUIET)
if(PKG_CONFIG_FOUND)
	configure_file(
		"${PROJECT_SOURCE_DIR}/${PROJECT_NAME}.pc.in"
		"${CMAKE_BINARY_DIR}/${PROJECT_NAME}.pc")
	install(FILES "${CMAKE_BINARY_DIR}/${PROJECT_NAME}.pc" DESTINATION lib/pkgconfig)
endif()
