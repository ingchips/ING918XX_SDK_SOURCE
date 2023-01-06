#ifndef __APP_DEBUG_H
#define __APP_DEBUG_H



#define ENABLE_APP_ASSERT



#ifdef ENABLE_APP_ASSERT
    #include "platform_api.h"
    #ifndef app_assert
    #define app_assert(condition)         if (condition) {} else { platform_raise_assertion(__FILE__, __LINE__);  }    
    #endif
#else
    #define app_assert(condition)         {}
#endif





#endif // __APP_DEBUG_H
