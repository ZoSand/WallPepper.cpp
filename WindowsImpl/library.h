#ifndef WINDOWSIMPL_LIBRARY_H
#define WINDOWSIMPL_LIBRARY_H

#   ifdef _WIN32
#       ifdef WIN_EXPORT
#           define PEPPER_DYNAMIC_LIB  __declspec( dllexport )
#       else
#           define PEPPER_DYNAMIC_LIB  __declspec( dllimport )
#       endif
#   endif

#endif //WINDOWSIMPL_LIBRARY_H
