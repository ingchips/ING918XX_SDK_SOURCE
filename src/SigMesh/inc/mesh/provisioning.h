/*
 *  provisioning.h
 */

#ifndef __PROVISIONING_H
#define __PROVISIONING_H

#include <stdint.h>

#include "btstack_defines.h"

#include "mesh/mesh_keys.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define PROVISIONING_PROTOCOL_TIMEOUT_MS  60000
#define MESH_PROV_MAX_PROXY_PDU              66 
#define MESH_PB_TRANSPORT_INVALID_CID    0xFFFF

// Provisioning Bearer Control

#define MESH_PROV_INVITE            0x00
#define MESH_PROV_CAPABILITIES      0x01
#define MESH_PROV_START             0x02
#define MESH_PROV_PUB_KEY           0x03
#define MESH_PROV_INPUT_COMPLETE    0x04
#define MESH_PROV_CONFIRM           0x05
#define MESH_PROV_RANDOM            0x06
#define MESH_PROV_DATA              0x07
#define MESH_PROV_COMPLETE          0x08
#define MESH_PROV_FAILED            0x09

// Provisioning Output OOB Actions
#define MESH_OUTPUT_OOB_BLINK       0x01
#define MESH_OUTPUT_OOB_BEEP        0x02
#define MESH_OUTPUT_OOB_VIBRATE     0x04
#define MESH_OUTPUT_OOB_NUMBER      0x08
#define MESH_OUTPUT_OOB_STRING      0x10

// Provisioning Input OOB Actions
#define MESH_INPUT_OOB_PUSH         0x01
#define MESH_INPUT_OOB_TWIST        0x02
#define MESH_INPUT_OOB_NUMBER       0x04
#define MESH_INPUT_OOB_STRING       0x08

typedef enum {
    MESH_PB_TYPE_ADV = 0,
    MESH_PB_TYPE_GATT
} mesh_pb_type_t;

typedef enum {
    MESH_OOB_INFORMATION_INDEX_OTHER = 0,
    MESH_OOB_INFORMATION_INDEX_ELECTRONIC_OR_URI,
    MESH_OOB_INFORMATION_INDEX_2D_MACHINE_READABLE_CODE,
    MESH_OOB_INFORMATION_INDEX_BAR_CODE,
    MESH_OOB_INFORMATION_INDEX_NEAR_FIELD_COMMUNICATION,
    MESH_OOB_INFORMATION_INDEX_NUMBER,
    MESH_OOB_INFORMATION_INDEX_STRING,
    MESH_OOB_INFORMATION_INDEX_RESERVED_7,
    MESH_OOB_INFORMATION_INDEX_RESERVED_8,
    MESH_OOB_INFORMATION_INDEX_RESERVED_9,
    MESH_OOB_INFORMATION_INDEX_RESERVED_10,
    MESH_OOB_INFORMATION_INDEX_ON_BOX,
    MESH_OOB_INFORMATION_INDEX_INSIDE_BOX,
    MESH_OOB_INFORMATION_INDEX_ON_PIECE_OF_PAPER,
    MESH_OOB_INFORMATION_INDEX_INSIDE_MANUAL,
    MESH_OOB_INFORMATION_INDEX_ON_DEVICE
} mesh_oob_information_index_t;

typedef enum {
    MESH_MSG_SAR_FIELD_COMPLETE_MSG = 0,
    MESH_MSG_SAR_FIELD_FIRST_SEGMENT,
    MESH_MSG_SAR_FIELD_CONTINUE,
    MESH_MSG_SAR_FIELD_LAST_SEGMENT
} mesh_msg_sar_field_t; // Message segmentation and reassembly information

typedef enum {
    MESH_MSG_TYPE_NETWORK_PDU = 0,
    MESH_MSG_TYPE_BEACON,
    MESH_MSG_TYPE_PROXY_CONFIGURATION,
    MESH_MSG_TYPE_PROVISIONING_PDU
} mesh_msg_type_t;

typedef enum {
    MESH_IDENTIFICATION_NETWORK_ID_TYPE = 0,
    MESH_IDENTIFICATION_NODE_IDENTIFY_TYPE 
} mesh_identification_type_t;

typedef struct {
    // DevKey = k1(ECDHSecret, ProvisioningSalt, “prdk”)
    uint8_t  device_key[16];

    // Unicast Address
    uint16_t unicast_address;

    // Key Refresh Phase 0 vs. 2, IV Update Active
    uint8_t  flags;

    // IV Index
    uint32_t iv_index;

    // Network Key (pass by reference)
    mesh_network_key_t * network_key;

} mesh_provisioning_data_t;

#ifdef __cplusplus
} /* end of extern "C" */
#endif

#endif
