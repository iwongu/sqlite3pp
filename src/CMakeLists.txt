# vim: set ts=4 sw=4 sts=4 et:

SET (SQLITE3PP_SRCS
	 sqlite3pp.cpp
	 sqlite3ppext.cpp
)

ADD_LIBRARY (sqlite3pp SHARED ${SQLITE3PP_SRCS})
TARGET_LINK_LIBRARIES (sqlite3pp sqlite3)

# at build time you may specify the cmake variable LIB_SUFFIX to handle
# 64-bit systems which use 'lib64'
INSTALL (TARGETS sqlite3pp
		 RUNTIME DESTINATION bin
		 LIBRARY DESTINATION lib${LIB_SUFFIX}
		 ARCHIVE DESTINATION lib${LIB_SUFFIX})

INSTALL (FILES sqlite3ppext.h sqlite3pp.h
         DESTINATION include/sqlite3pp)
