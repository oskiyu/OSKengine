#pragma once

#include "NumericTypes.h"

#define OSK_VULKAN_TYPEDEF(type) \
struct type##_T;\
using type = type##_T*

#define OSK_VULKAN_FLAGS_TYPEDEF(type) using type = ::OSK::USize32;
