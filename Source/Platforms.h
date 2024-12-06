#pragma once

#define OSK_PLATFORM_CHECK_A 0
#define OSK_PLATFORM_CHECK_B 0
#define OSK_PLATFORM_CHECK_C 0

#ifdef OSK_ANDROID
#undef OSK_PLATFORM_CHECK_A
#define OSK_PLATFORM_CHECK_A 1
#endif
#ifdef OSK_WINDOWS
#undef OSK_PLATFORM_CHECK_B
#define OSK_PLATFORM_CHECK_B 1
#endif
#ifdef OSK_LINUX
#undef OSK_PLATFORM_CHECK_C
#define OSK_PLATFORM_CHECK_C 1
#endif


#ifdef OSK_ANDROID
#define OSK_USE_VULKAN_BACKEND
#define OSK_USE_VULKAN_1_0
#elif OSK_WINDOWS
#define OSK_USE_VULKAN_BACKEND
#define OSK_USE_VULKAN_1_3
#define OSK_USE_DIRECTX12_BACKEND
#define OSK_USE_PC_WINDOW
#elif OSK_LINUX
#define OSK_USE_VULKAN_BAKCEND
#define OSK_USE_VULKAN_1_3
#define OSK_USE_PC_WINDOW
#else
#error No se ha establecido la plataforma de destino.
#endif

#if OSK_PLATFORM_CHECK_A + OSK_PLATFORM_CHECK_B + OSK_PLATFORM_CHECK_C > 1
#error Sólamente se puede definir una única plataforma de destino.
#endif

#undef OSK_PLATFORM_CHECK_A
#undef OSK_PLATFORM_CHECK_B
#undef OSK_PLATFORM_CHECK_C
