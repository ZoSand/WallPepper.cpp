//
// Created by ZoSand on 22/11/2022.
//

#ifndef SHARED_CONSTANT_DEFINITIONS_H
#define SHARED_CONSTANT_DEFINITIONS_H

#   if !defined(UNICODE)
#       define UNICODE
#   endif

#   if defined(NDEBUG)
#       define PEPPER_VULKAN_VALIDATE_LAYERS false
#   else
#       define PEPPER_VULKAN_VALIDATE_LAYERS true
#   endif

#   if !defined(NO_DISCARD)
#       define NO_DISCARD [[nodiscard]]
#   endif

#   if !defined(MAYBE_UNUSED)
#       define MAYBE_UNUSED [[maybe_unused]]
#   endif

#   if !defined (DEPRECATED)
#       define DEPRECATED [[deprecated]]
#   endif

#   if !defined(DEPRECATED_REASON)
#       define DEPRECATED_REASON(reason) [[deprecated(reason)]]
#   endif

#   if !defined(NO_DISCARD_UNUSED)
#       define NO_DISCARD_UNUSED NO_DISCARD MAYBE_UNUSED
#   endif

#endif //SHARED_CONSTANT_DEFINITIONS_H
