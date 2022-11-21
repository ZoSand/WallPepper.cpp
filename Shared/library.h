#ifndef SHARED_LIBRARY_H
#define SHARED_LIBRARY_H

#   ifdef _WIN32
#       ifdef WIN_EXPORT
#           define EXPORTED  __declspec( dllexport )
#       else
#           define EXPORTED  __declspec( dllimport )
#       endif
#   else
#       define EXPORTED
#   endif


#endif //SHARED_LIBRARY_H
