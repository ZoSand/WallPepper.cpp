#ifndef SHARED_LIBRARY_H
#define SHARED_LIBRARY_H

#   if defined(_WIN32)
#       if defined(SHARED_EXPORT)
#           define PEPPER_SHARED_LIB  __declspec( dllexport )
#       else
#           define PEPPER_SHARED_LIB  __declspec( dllimport )
#       endif
#   else
#       if defined(SHARED_EXPORT)
#           define PEPPER_SHARED_LIB  __attribute__((visibility("default")))
#       else
#           define PEPPER_SHARED_LIB
#       endif
#   endif

#include "ConstantDefinitions.h"

#endif //SHARED_LIBRARY_H
