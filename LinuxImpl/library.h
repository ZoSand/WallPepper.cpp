#ifndef LINUXIMPL_LIBRARY_H
#define LINUXIMPL_LIBRARY_H

#   ifdef LINUX_EXPORT
#       define PEPPER_DYNAMIC_LIB __attribute__((visibility("default")))
#   else
#       define PEPPER_DYNAMIC_LIB
#   endif

#endif //LINUXIMPL_LIBRARY_H
