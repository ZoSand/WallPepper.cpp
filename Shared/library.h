#ifndef SHARED_LIBRARY_H
#define SHARED_LIBRARY_H

#   ifdef _WIN32
#       ifdef WIN_EXPORT
#           define PEPPER_SHARED_LIB  __declspec( dllexport )
#       else
#           define PEPPER_SHARED_LIB  __declspec( dllimport )
#       endif
#   else
#       define PEPPER_SHARED_LIB
#   endif

#endif //SHARED_LIBRARY_H
