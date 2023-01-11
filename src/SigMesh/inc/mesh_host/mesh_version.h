#ifndef MESH_VERSION_H
#define MESH_VERSION_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>


/** @brief get version information.
 *  @param[out] str: version string buffer.
 *  @param[in]  str_len: the str buffer length, must enough large to get version info. Different str_len may lead to different result.
 *       if str_len >= 7,  then just return mesh libaray version, such as: v1.0.0
 *       if str_len >= 16, then return mesh libaray version and compile time, such as: v1.0.0 11:24:23
 *       if str_len >= 28, then return mesh libaray version, compile time, and compile date. Such as: v1.0.0 11:24:23 Jan 11 2023
 *  @return  
 *       -1      : params can not null.
 *       -2      : buffer size not enough.
 *       >0      : return version inforamtion, belongs to the size of str_len.
 *  @examp
        char version[28];
        int ver_len = mesh_get_lib_version_info(version, sizeof(version));
        if(ver_len > 0){
            platform_printf("mesh version info: %s\n", version); // Print: mesh version info: v1.0.0 11:24:23 Jan 11 2023
        }
 */
int mesh_get_lib_version_info(char * str, uint8_t str_len);


#endif
