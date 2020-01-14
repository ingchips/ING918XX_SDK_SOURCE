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


// *****************************************************************************
//
// Advertising Data Parser 
//
// *****************************************************************************

#include "btstack_util.h"
#include "ad_parser.h"
#include <string.h>

typedef enum {
    IncompleteList16 = 0x02, 
    CompleteList16 = 0x03, 
    IncompleteList128 = 0x06, 
    CompleteList128 = 0x07
} UUID_TYPE;

void ad_iterator_init(ad_context_t *context, uint16_t ad_len, const uint8_t * ad_data){
    context->data = ad_data;
    context->length = ad_len;
    context->offset = 0;
}

int  ad_iterator_has_more(ad_context_t * context){
    return context->offset < context->length;
}

void ad_iterator_next(ad_context_t * context){
    int chunk_len = context->data[context->offset];
    int new_offset = context->offset + 1 + chunk_len;
    // avoid uint8_t overrun
    if (new_offset > 0xff){
        new_offset = 0xff;
    }
    context->offset = new_offset;
}

uint8_t   ad_iterator_get_data_len(ad_context_t * context){
    return context->data[context->offset] - 1;
}

uint8_t   ad_iterator_get_data_type(ad_context_t * context){
    return context->data[context->offset + 1];
}

const uint8_t * ad_iterator_get_data(ad_context_t * context){
    return &context->data[context->offset + 2];
}

int ad_data_contains_uuid16(uint16_t ad_len, const uint8_t * ad_data, const uint16_t uuid16){
    ad_context_t context;
    for (ad_iterator_init(&context, ad_len, ad_data) ; ad_iterator_has_more(&context) ; ad_iterator_next(&context)){
        uint8_t data_type = ad_iterator_get_data_type(&context);
        uint8_t data_len  = ad_iterator_get_data_len(&context);
        const uint8_t * data    = ad_iterator_get_data(&context);

        int i;
        uint8_t ad_uuid128[16], uuid128_bt[16];
                
        switch (data_type){
            case IncompleteList16:
            case CompleteList16:
                for (i=0; i<data_len; i+=2){
                    uint16_t uuid = little_endian_read_16(data, i);
                    if ( uuid == uuid16 ) return 1;
                }
                break;
            case IncompleteList128:
            case CompleteList128:
                uuid_add_bluetooth_prefix(ad_uuid128, uuid16);
                reverse_128(ad_uuid128, uuid128_bt);
            
                for (i=0; i<data_len; i+=16){
                    if (memcmp(uuid128_bt, &data[i], 16) == 0) return 1;
                }
                break;
            default:
                break;
        }  
    }
    return 0;
}

int ad_data_contains_uuid128(uint16_t ad_len, const uint8_t * ad_data, const uint8_t * uuid128){
    ad_context_t context;
    // input in big endian/network order, bluetooth data in little endian
    uint8_t uuid128_le[16];
    reverse_128(uuid128, uuid128_le);
    for (ad_iterator_init(&context, ad_len, ad_data) ; ad_iterator_has_more(&context) ; ad_iterator_next(&context)){
        uint8_t data_type = ad_iterator_get_data_type(&context);
        uint8_t data_len  = ad_iterator_get_data_len(&context);
        const uint8_t *data = ad_iterator_get_data(&context);
        
        int i;
        uint8_t ad_uuid128[16];

        switch (data_type){
            case IncompleteList16:
            case CompleteList16:
                for (i=0; i<data_len; i+=2){
                    uint16_t uuid16 = little_endian_read_16(data, i);
                    uuid_add_bluetooth_prefix(ad_uuid128, uuid16);
                    
                    if (memcmp(ad_uuid128, uuid128_le, 16) == 0) return 1;
                }

                break;
            case IncompleteList128:
            case CompleteList128:
                for (i=0; i<data_len; i+=16){
                    if (memcmp(uuid128_le, &data[i], 16) == 0) return 1;
                }
                break;
            default:
                break;
        }  
    }
    return 0;
}

const uint8_t *ad_data_from_type(uint16_t ad_len, uint8_t * ad_data, uint8_t ad_type, uint16_t *found_length)
{
    ad_context_t context;
    for (ad_iterator_init(&context, ad_len, ad_data) ; ad_iterator_has_more(&context) ; ad_iterator_next(&context)){
        if(ad_type == ad_iterator_get_data_type(&context))
        {
            *found_length = ad_iterator_get_data_len(&context);
            return ad_iterator_get_data(&context);
        }
        continue;
    }
    
    *found_length = 0;
    return NULL;
}


