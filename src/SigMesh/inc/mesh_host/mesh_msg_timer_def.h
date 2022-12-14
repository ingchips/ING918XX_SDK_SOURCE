/*
 *  mesh_msg_timer_def.h
 *
 *  MESH timer ID macro definition declare.
 *
 *  Created by Lizhk on 07-Dec-2022.
 */

#ifndef MESH_MSG_TIMER_DEF_H
#define MESH_MSG_TIMER_DEF_H


#include "bt_types.h"



//Begain: MESH Timer define, added by lizhk.
#define MESH_ADV_TIMER_ID                               0x0681u
#define MESH_BEACON_TIMER_ID                            0x0682u
#define MESH_ACCESS_ACKNOWLEDGED_TIMER_ID               0x0683u
#define MESH_BASE_TRANSITION_TIMER_ID                   0x0684u
#define MESH_ACCESS_PUBLICATION_TIMER_ID                0x0685u
#define MESH_HEARTBEAT_PUBLICATION_TIMER_ID             0x0686u
#define MESH_SEGMENTED_PDU_ACKNOWLEDGEMENT_TIMER_ID     0x0687u
#define MESH_SEGMENTED_PDU_INCOMPLETE_TIMER_ID          0x0688u
#define MESH_ATTENTION_TIMER_TIMER_ID                   0x0689u
#define MESH_PB_ADV_LINK_TIMER_ID                       0x0690u
#define MESH_PROV_PROTOCOL_TIMER_ID                     0x0691u
#define MESH_PROV_ATTENTION_TIMER_ID                    0x0692u
#define MESH_PROXY_NODE_ID_TIMER_ID                     0x0693u
#define MESH_NODE_RESET_DELAY_TIMER_ID                  0x0694u
#define MESH_BLE_PARAMS_RESET_DELAY_TIMER_ID            0x0695u
#define MESH_CONN_PARAM_UPDATE_TIMER_ID                 0x0696u
//End: MESH Timer define, added by lizhk.


#endif
