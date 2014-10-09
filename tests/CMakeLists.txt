# vim: set ts=4 sw=4 sts=4 et:

INCLUDE_DIRECTORIES (${CMAKE_SOURCE_DIR}/src)

ADD_EXECUTABLE (testaggregate testaggregate.cpp)
ADD_EXECUTABLE (testattach testattach.cpp)
ADD_EXECUTABLE (testcallback testcallback.cpp)
ADD_EXECUTABLE (testdisconnect testdisconnect.cpp)
ADD_EXECUTABLE (testfunction testfunction.cpp)
ADD_EXECUTABLE (testinsertall testinsertall.cpp)
ADD_EXECUTABLE (testinsert testinsert.cpp)
ADD_EXECUTABLE (testselect testselect.cpp)

ADD_DEPENDENCIES (testaggregate ${SQLITE3PP_NAME})
ADD_DEPENDENCIES (testattach ${SQLITE3PP_NAME})
ADD_DEPENDENCIES (testcallback ${SQLITE3PP_NAME})
ADD_DEPENDENCIES (testdisconnect ${SQLITE3PP_NAME})
ADD_DEPENDENCIES (testfunction ${SQLITE3PP_NAME})
ADD_DEPENDENCIES (testinsertall ${SQLITE3PP_NAME})
ADD_DEPENDENCIES (testinsert ${SQLITE3PP_NAME})
ADD_DEPENDENCIES (testselect ${SQLITE3PP_NAME})

TARGET_LINK_LIBRARIES (testaggregate ${SQLITE3PP_NAME})
TARGET_LINK_LIBRARIES (testattach ${SQLITE3PP_NAME})
TARGET_LINK_LIBRARIES (testcallback ${SQLITE3PP_NAME})
TARGET_LINK_LIBRARIES (testdisconnect ${SQLITE3PP_NAME})
TARGET_LINK_LIBRARIES (testfunction ${SQLITE3PP_NAME})
TARGET_LINK_LIBRARIES (testinsertall ${SQLITE3PP_NAME})
TARGET_LINK_LIBRARIES (testinsert ${SQLITE3PP_NAME})
TARGET_LINK_LIBRARIES (testselect ${SQLITE3PP_NAME})

IF (ENABLE_SQLCIPHER)
    ADD_EXECUTABLE (testinsertwithkey testinsertwithkey.cpp)
    ADD_DEPENDENCIES (testinsertwithkey ${SQLITE3PP_NAME})
    TARGET_LINK_LIBRARIES (testinsertwithkey ${SQLITE3PP_NAME})
ENDIF (ENABLE_SQLCIPHER)

