#ifndef MESH_VERSION_H
#define MESH_VERSION_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>


/** @brief get version information, format: hex = 0xABC, version: vA.B.C
 *  @param[out] return version in hex format.
 *  @return  
 *       -1      : params error.
 *       >0      : version length.
 *  @examp
        uint16_t ver_hex;
        int ver_len = mesh_get_version_info_hex(&ver_hex);
        if(ver_len > 0){
          printf("mesh version[hex]: 0x%x\n", ver_hex); 
        }
        // print: mesh version[hex]: 0x100
 */
int mesh_get_version_info_hex(uint16_t * ver_hex);


/** @brief get version information, format: string[5] = "A.B.C" , version: vA.B.C
 *  @param[out] str: version string buffer.
 *  @param[in]  str_len: the str buffer length, must enough large to get version, exceed 5.
 *  @return  
 *       -1      : params error.
 *       -2      : buffer not enough.
 *       >0      : version length.
 *  @examp
        char version[8];
        int ver_len = mesh_get_version_info_str(version, sizeof(version));
        if(ver_len > 0 && ver_len < sizeof(version)-1){
            version[ver_len] = '\0';
            printf("mesh ver: v%s\n", version); 
        }
        // print: mesh ver: v1.0.0
 */
int mesh_get_version_info_str(char * str, uint8_t str_len);


/** @brief get mesh lib compile date and time, format: string.
 *  @param[out] str: date and time string buffer.
 *  @param[in]  str_len: the str buffer length, must enough large to get date and time, exceed 21 bytes.
 *  @return  
 *       -1      : params error.
 *       -2      : buffer not enough.
 *       >0      : date and time string length.
 *  @examp
        char date_time[30];
        int str_len = mesh_get_lib_compile_date_time(date_time, sizeof(date_time));
        if(str_len > 0 ){
            printf("mesh date: %s\n", date_time); 
        }
        // print: mesh date: 15:38:31 Jan  9 2023
 */
int mesh_get_lib_compile_date_time(char *str, uint8_t str_len);


#endif
