#ifndef _ANCS_H
#define _ANCS_H

#include <stdint.h>

typedef enum
{
    ANCS_ACTION_ID_POSITIVE = 0,
    ANCS_ACTION_ID_NEGATIVE
} ANCS_ACTION_ID_VALUES;

/**
 *@brief Reset state of ANCS
 *
 *  This is called automatically in `ancs_discover`.
 */
void ancs_reset(void);

/**
 *@brief Initialize ANCS on a connection
 *
 *@param conn_handle: connection handle
 *
 *  Note: Call this after security is established.
 */
void ancs_discover(uint16_t conn_handle);

/**
 *@brief User action input
 *
 *@param actionId: user's action
 *@return 0: if action sent to server, else action is allowed at present
 */
int ancs_user_action(ANCS_ACTION_ID_VALUES actionId);

#endif
