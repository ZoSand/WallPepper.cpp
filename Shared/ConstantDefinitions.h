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

#endif //SHARED_CONSTANT_DEFINITIONS_H
