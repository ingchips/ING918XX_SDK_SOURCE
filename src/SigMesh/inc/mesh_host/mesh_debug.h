#ifndef _MESH_DEBUG_H
#define _MESH_DEBUG_H


#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "mesh_config.h"


// Avoid complaints of unused arguments when log levels are disabled.
static inline void __mesh_log_unused(const char *format, ...) {};

#define MESH_PRINTF(format, ...)            printf(format, ## __VA_ARGS__)


#ifdef ENABLE_LOG_DEBUG
#define MESH_LOG_DEBUG(format, ...)         MESH_PRINTF(format "\n",  ## __VA_ARGS__)
#else
#define MESH_LOG_DEBUG(...)                 __mesh_log_unused(__VA_ARGS__)
#endif


#ifdef ENABLE_LOG_INFO
#define MESH_LOG_INFO(format, ...)          MESH_PRINTF(format "\n",  ## __VA_ARGS__)

/**
 * @brief Log Security Manager key via log_info
 * @param key to log
 */
void MESH_LOG_INFO_KEY(const char * name, const sm_key_t key);

/**
 * @brief Hexdump via log_info
 * @param data
 * @param size
 */
void MESH_LOG_INFO_HEXDUMP(const void *data, int size);

#else
#define MESH_LOG_INFO(...)                  __mesh_log_unused(__VA_ARGS__)
#define MESH_LOG_INFO_KEY(name, key)
#define MESH_LOG_INFO_HEXDUMP(data, size)
#endif

#ifdef ENABLE_LOG_ERROR
#define MESH_LOG_ERR(format, ...)           MESH_PRINTF(format "\n",  ## __VA_ARGS__)
#else
#define MESH_LOG_ERR(...)                   __mesh_log_unused(__VA_ARGS__)
#endif



#ifdef ENABLE_MESH_ASSERT
    #include "platform_api.h"
    #ifndef mesh_assert
    #define mesh_assert(condition)         if (condition) {} else { platform_raise_assertion(__FILE__, __LINE__);  }    
    #endif
#else /* ENABLE_MESH_ASSERT */
    #define mesh_assert(condition)         {}
#endif /* ENABLE_MESH_ASSERT */



#endif // _MESH_DEBUG_H
