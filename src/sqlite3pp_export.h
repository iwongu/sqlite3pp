#ifndef SQLITE3PP_EXPORT_H
#define SQLITE3PP_EXPORT_H

#if defined(_WIN32) || defined(__CYGWIN__) || defined(__MINGW32__)
#   ifdef SQLITE3PP_BUILDING_DLL
#       ifdef __GNUC__
#           define SQLITE3PP_Export __attribute__((dllexport))
#       else /* !__GNUC__ */
#           define SQLITE3PP_Export __declspec(dllexport)
#       endif /* __GNUC__ */
#   else /* !SQLITE3PP_BUILDING_DLL */
#       ifdef __GNUC__
#           define SQLITE3PP_Export __attribute__((dllimport))
#       else /* !__GNUC__ */
#           define SQLITE3PP_Export __declspec(dllimport)
#       endif /* __GNUC__ */
#   endif /* SQLITE3PP_BUILDING_DLL */
#else /* *nix platform */
#   if __GNUC__ >= 4
#       define SQLITE3PP_Export __attribute__ ((visibility ("default")))
#   else /* __GNUC__ < 4 */
#       define SQLITE3PP_Export
#   endif /* __GNUC__ >= 4 */
#endif /* _WIN32 || __CYGWIN__ || __MINGW32__ */

#endif /* SQLITE3PP_EXPORT_H */
// vim: set ts=4 sw=4 sts=4 et:
