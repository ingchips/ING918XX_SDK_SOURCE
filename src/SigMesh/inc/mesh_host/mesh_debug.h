#ifndef _MESH_DEBUG_H
#define _MESH_DEBUG_H

#include "bt_types.h"

/**
 * @enum trace_layer
 * @brief bit sets of mesh trace layer or functional trace
 */
enum trace_layer{
    ADV_LAYER   = BIT(0),    ///< bit set of advertise layer for trace
    NET_LAYER   = BIT(1),    ///< bit set of network layer for trace
    TRANS_LAYER = BIT(2),    ///< bit set of transport layer for trace
    ACC_LAYER   = BIT(3),    ///< bit set of access layer for trace
    BEA_FEA     = BIT(4),    ///< bit set of beacon message for trace
    MESH_FEA    = BIT(5),    ///< bit set of mesh related message for trace
    CRYPTO_FEA  = BIT(6),    ///< bit set of encryption or decryption message for trace
    PROV_FEA    = BIT(7),    ///< bit set of proving message for trace
    PROXY_FEA   = BIT(8),    ///< bit set of proxy message for trace
    CFG_FEA     = BIT(9),    ///< bit set of configure server mssage for trace
    HEALTH_FEA  = BIT(10),   ///< bit set of health model message for trace
    NODE_FEA    = BIT(11),   ///< bit set of node message for trace
    FLASH_FEA   = BIT(12),   ///< bit set of mesh tlv storage operation for trace
    ALL_LAYER   = 0x7FFFFFFF,///< set all bits for trace.
};

/**
 * @enum trace_level
 * @brief set the trace level, in mesh trace design there are 3 levels log.
 */
enum trace_lvl{
    DEBUG_LVL = BIT(0),
    INFO_LVL  = BIT(1),
    WARN_LVL  = BIT(2),
    ALL_LVL   = 0xFF,
};


/**
 * @brief Config mesh trace system. Only the trace that selected layer and selected class will print.
 * @param sel_bits select trace layer, reference @enum trace_layer, not selected layer will not print.
 * @param level_bits select trace level, ref @enum trace_lvl, not selected level will not print.
 * @return none.
 */
void mesh_trace_config(uint32_t sel_bits, uint8_t level_bits);


#endif // _MESH_DEBUG_H
