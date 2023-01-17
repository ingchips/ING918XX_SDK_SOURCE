#ifndef __MESH_PEER_H
#define __MESH_PEER_H

#include "mesh/mesh_network.h"

#if defined __cplusplus
extern "C" {
#endif

// mesh seq auth validation
typedef struct {
    // primary element address
    uint16_t address;
    // next expected seq number
    uint32_t seq;

    // segmented transport message
    mesh_segmented_pdu_t * message_pdu;
    // seq_zero
    uint16_t seq_zero;
    // seq_auth
    uint32_t seq_auth;
    // block ack
    uint32_t block_ack;
} mesh_peer_t;

// get peer info for address
mesh_peer_t * mesh_peer_for_addr(uint16_t address);

#if defined __cplusplus
}
#endif

#endif //__MESH_PEER_H
