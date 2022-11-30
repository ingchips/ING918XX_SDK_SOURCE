#ifndef _btstack_sync_h
#define _btstack_sync_h

#include "btstack_defines.h"
#include "btstack_event.h"
#include "gap.h"
#include "att_db.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 ****************************************************************************************
 * @brief A collection of synchronized-style Bluetooth stack APIs
 *
 * Note:
 *   1. To using synced GAP APIs set `enable_gap_api` when create the runner;
 *   1. When use synced GAP APIs, avoid calling the original async ones.
 *      Because in the HCI event handler, it's impossible to determine which one
 *      triggers `HCI_EVENT_COMMAND_COMPLETE`.
 *
 ****************************************************************************************
 */

struct btstack_synced_runner;
typedef void (* f_btstack_synced_runnable)(struct btstack_synced_runner * runner, void *user_data);

#ifndef BTSTACK_SYNC_RUNNER_STACK_SIZE
#define BTSTACK_SYNC_RUNNER_STACK_SIZE  1024
#endif

#define BTSTACK_SYNCED_ERROR_RTOS                   (-1)
#define BTSTACK_SYNCED_ERROR_BUFFER_TOO_SMALL       (-2)
#define BTSTACK_SYNCED_ERROR_OUT_OF_MEM             (-3)

/**
 ****************************************************************************************
 * @brief Create a synchronized runner
 *
 * Note: There should only be one runner at most for each connection;
 *       The runner can't be freed.
 *
 * Note: There should be only one runner enabled for synced GAP APIs
 *
 * Note: When `enable_gap_api`, this function NOT thread-safe, and must be called
 *       within Host task.
 *
 * @param[in]   enable_gap_api      enable synced GAP APIs (i.e. `gap_sync_xxx`)
 * @return                          synced runner
 ****************************************************************************************
 */
struct btstack_synced_runner *btstack_create_sync_runner(int enable_gap_api);

/**
 ****************************************************************************************
 * @brief Run a function in synchronized manner
 *
 * Note: `runnable` can use SYNC CALL APIs.
 *
 * @param[in]   runner              the runner
 * @param[in]   runnable            the function to be called in synchronized manner
 * @param[in]   user_data           user data
 * @return                          error code (0: OK)
 ****************************************************************************************
 */
int btstack_sync_run(struct btstack_synced_runner *runner, f_btstack_synced_runnable runnable, void *user_data);

/**
 ****************************************************************************************
 * @brief SYNC CALL API: discover all
 *
 * Note: use `gatt_client_util_free` to free the discovering context.
 *
 * @param[in]   runner              the runner
 * @param[in]   con_handle          connection handle
 * @param[out]  err_code            error code (0: OK)
 * @return                          discovering context which is allocated on heap
 ****************************************************************************************
 */
struct gatt_client_discoverer * gatt_client_sync_discover_all(struct btstack_synced_runner *runner,
    hci_con_handle_t con_handle, int *err_code);

/**
 ****************************************************************************************
 * @brief SYNC CALL API: read value of a characteristic using its value handle
 *
 * @param[in]       runner              the runner
 * @param[in]       con_handle                      connection handle
 * @param[in]       characteristic_value_handle     handle of the value
 * @param[out]      data                            data read if no error
 * @param[in,out]   length                          in: buffer size of data; out: length of data if no error
 * @return                                          error code (0: OK)
 ****************************************************************************************
 */
int gatt_client_sync_read_value_of_characteristic(struct btstack_synced_runner *runner,
    hci_con_handle_t con_handle,
    uint16_t characteristic_value_handle, uint8_t *data, uint16_t *length);

/**
 ****************************************************************************************
 * @brief SYNC CALL API: read discriptor of a characteristic using its handle
 *
 * @param[in]       runner                          the runner
 * @param[in]       con_handle                      connection handle
 * @param[in]       descriptor_handle               handle of the discriptor
 * @param[out]      data                            data read if no error
 * @param[in,out]   length                          in: buffer size of data; out: length of data if no error
 * @return                                          error code (0: OK)
 ****************************************************************************************
 */
int gatt_client_sync_read_characteristic_descriptor(struct btstack_synced_runner *runner,
    hci_con_handle_t con_handle,
    uint16_t descriptor_handle, uint8_t *data, uint16_t *length);

/**
 ****************************************************************************************
 * @brief SYNC CALL API: write value of a characteristic using its value handle
 *
 * @param[in]       runner                          the runner
 * @param[in]       con_handle                      connection handle
 * @param[in]       characteristic_value_handle     handle of the value
 * @param[out]      data                            data to be written
 * @param[in]       length                          data length
 * @return                                          error code (0: OK)
 ****************************************************************************************
 */
int gatt_client_sync_write_value_of_characteristic(struct btstack_synced_runner *runner,
    hci_con_handle_t con_handle,
    uint16_t characteristic_value_handle, const uint8_t *data, uint16_t length);

/**
 ****************************************************************************************
 * @brief SYNC CALL API: write value of a characteristic without response using its value handle
 *
 * @param[in]       runner                          the runner
 * @param[in]       con_handle                      connection handle
 * @param[in]       characteristic_value_handle     handle of the value
 * @param[out]      data                            data to be written
 * @param[in]       length                          data length
 * @return                                          error code (0: OK)
 ****************************************************************************************
 */
int gatt_client_sync_write_value_of_characteristic_without_response(struct btstack_synced_runner *runner,
    hci_con_handle_t con_handle,
    uint16_t characteristic_value_handle, const uint8_t *data, uint16_t length);

/**
 ****************************************************************************************
 * @brief SYNC CALL API: write discriptor of a characteristic using its handle
 *
 * @param[in]       runner                          the runner
 * @param[in]       con_handle                      connection handle
 * @param[in]       descriptor_handle               handle of the discriptor
 * @param[out]      data                            data to be written
 * @param[in]       length                          data length
 * @return                                          error code (0: OK)
 ****************************************************************************************
 */
int gatt_client_sync_write_characteristic_descriptor(struct btstack_synced_runner *runner,
    hci_con_handle_t con_handle,
    uint16_t descriptor_handle, const uint8_t *data, uint16_t length);

/**
 ****************************************************************************************
 * @brief SYNC CALL API: LE Extended Create Connection command
 *
 * Note: when timed out, `gap_create_connection_cancel` is called to cancel the initiating
 * procedure.
 *
 * @param[in]       runner                          the runner
 * @param[in]       filter_policy                   see `gap_ext_create_connection(...)`
 * @param[in]       own_addr_type                   see `gap_ext_create_connection(...)`
 * @param[in]       peer_addr_type                  see `gap_ext_create_connection(...)`
 * @param[in]       peer_addr                       see `gap_ext_create_connection(...)`
 * @param[in]       initiating_phy_num              see `gap_ext_create_connection(...)`
 * @param[in]       phy_configs                     see `gap_ext_create_connection(...)`
 * @param[in]       timeout_ms                      time waiting for connection in milliseconds
 * @param[out]      complete                        the complete event
 * @return                                          error code (0: OK)
 ****************************************************************************************
 */
int gap_sync_ext_create_connection(struct btstack_synced_runner *runner,
                                  const initiating_filter_policy_t filter_policy,
                                  const bd_addr_type_t own_addr_type,
	                              const bd_addr_type_t peer_addr_type,
	                              const uint8_t *peer_addr,
                                  const uint8_t initiating_phy_num,
                                  const initiating_phy_config_t *phy_configs,
                                  uint32_t timeout_ms,
                                  le_meta_event_enh_create_conn_complete_t *complete);

/**
 ****************************************************************************************
 * @brief SYNC CALL API: read the channel map of an HCI connection
 *
 * @param[in]       runner                          the runner
 * @param[in]       con_handle                      connection handle
 * @param[out]      channel_map                     channel map
 * @return                                          error code (0: OK)
 ****************************************************************************************
 */
int gap_sync_le_read_channel_map(struct btstack_synced_runner *runner,
    hci_con_handle_t con_handle, uint8_t channel_map[5]);

/**
 ****************************************************************************************
 * @brief SYNC CALL API: read current RSSI of an HCI connection
 *
 * @param[in]       runner                          the runner
 * @param[in]       con_handle                      connection handle
 * @param[out]      rssi                            RSSI in dBm
 * @return                                          error code (0: OK)
 ****************************************************************************************
 */
int gap_sync_read_rssi(struct btstack_synced_runner *runner,
    hci_con_handle_t con_handle, int8_t *rssi);

/**
 ****************************************************************************************
 * @brief SYNC CALL API: read current PHY of an HCI connection
 *
 * @param[in]       runner                          the runner
 * @param[in]       con_handle                      connection handle
 * @param[out]      tx_phy                          PHY for Tx
 * @param[out]      rx_phy                          PHY for Rx
 * @return                                          error code (0: OK)
 ****************************************************************************************
 */
int gap_sync_read_phy(struct btstack_synced_runner *runner,
    hci_con_handle_t con_handle, phy_type_t *tx_phy, phy_type_t *rx_phy);

/**
 ****************************************************************************************
 * @brief SYNC CALL API: read remote version information of an HCI connection
 *
 * @param[in]       runner                          the runner
 * @param[in]       con_handle                      connection handle
 * @param[out]      version                         Core Spec. version
 * @param[out]      manufacturer_name               manufacturer name
 * @param[out]      subversion                      implementation revision
 * @return                                          error code (0: OK)
 ****************************************************************************************
 */
int gap_sync_read_remote_version(struct btstack_synced_runner *runner,
    hci_con_handle_t con_handle,
    uint8_t *version,
    uint16_t *manufacturer_name,
    uint16_t *subversion);

/**
 ****************************************************************************************
 * @brief SYNC CALL API: read remote used features of an HCI connection
 *
 * @param[in]       runner                          the runner
 * @param[in]       con_handle                      connection handle
 * @param[out]      features                        remote used features
 * @return                                          error code (0: OK)
 ****************************************************************************************
 */
int gap_sync_read_remote_used_features(struct btstack_synced_runner *runner,
    hci_con_handle_t con_handle, uint8_t features[8]);

#ifdef __cplusplus
}
#endif

#endif
