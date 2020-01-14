// ----------------------------------------------------------------------------
// Copyright Message                                                           
// ----------------------------------------------------------------------------
//                                                                             
// INGCHIPS confidential and proprietary.                                      
// COPYRIGHT (c) 2018 by INGCHIPS                                              
//                                                                             
// All rights are reserved. Reproduction in whole or in part is                
// prohibited without the written consent of the copyright owner.              
//                                                                             
// ----------------------------------------------------------------------------

#ifndef __AD_PARSER_H
#define __AD_PARSER_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* API_START */

typedef struct ad_context {
     const uint8_t  *data;
     uint16_t  offset;
     uint16_t  length;
} ad_context_t;

// Advertising or Scan Response data iterator
void ad_iterator_init(ad_context_t *context, uint16_t ad_len, const uint8_t * ad_data);
int  ad_iterator_has_more(ad_context_t * context);
void ad_iterator_next(ad_context_t * context);

// Access functions
uint8_t   ad_iterator_get_data_type(ad_context_t * context);
uint8_t   ad_iterator_get_data_len(ad_context_t * context);
const uint8_t * ad_iterator_get_data(ad_context_t * context);

// convenience function on complete advertisements
int ad_data_contains_uuid16(uint16_t ad_len, const uint8_t * ad_data, const uint16_t uuid);
int ad_data_contains_uuid128(uint16_t ad_len, const uint8_t * ad_data, const uint8_t * uuid128);

// find data with type "ad_type"
// output: found_length: data length of "ad_type", 0 if not found
// return: pointer to the start of data within "ad_data", NULL if not found
const uint8_t *ad_data_from_type(uint16_t ad_len, uint8_t * ad_data, uint8_t ad_type, uint16_t *found_length);

/* API_END */

#ifdef __cplusplus
}
#endif
#endif // __AD_PARSER_H
