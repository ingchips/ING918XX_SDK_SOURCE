#ifndef _gatt_client_h
#define _gatt_client_h

#include "gatt_client.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct desc_node
{
    struct desc_node *next;
    gatt_client_characteristic_descriptor_t desc;
} desc_node_t;

typedef struct char_node
{
    struct char_node *next;
    gatt_client_characteristic_t chara;
    gatt_client_notification_t *notification;
    desc_node_t *descs;
} char_node_t;

typedef struct service_node
{
    struct service_node *next;
    gatt_client_service_t service;
    char_node_t *chars;
} service_node_t;

struct gatt_client_discoverer;
typedef void (* f_on_fully_discovered)(service_node_t *first, void *user_data, int err_code);

/**
 ****************************************************************************************
 * @brief Start discover full profile
 *
 * @param[in] con_handle            the connection handle
 * @param[in] on_fully_discovered   callback when fully discovered
 * @return                          discovering context which is allocated on heap
 ****************************************************************************************
 */
struct gatt_client_discoverer *gatt_client_util_discover_all(hci_con_handle_t con_handle, f_on_fully_discovered on_fully_discovered, void *user_data);

/**
 ****************************************************************************************
 * @brief Free the discovering context on heap
 *
 * Note: this can only be freed after discover operation is completed.
 *
 * @param[in] ctx                   discovering context
 ****************************************************************************************
 */
void gatt_client_util_free(struct gatt_client_discoverer *ctx);

/**
 ****************************************************************************************
 * @brief Get the first service of a discovering context
 *
 * Note: this can only be freed after discover operation is completed.
 *
 * @param[in] ctx                   discovering context
 * @return                          the first service
 ****************************************************************************************
 */
service_node_t *gatt_client_util_get_first_service(struct gatt_client_discoverer *ctx);

/**
 ****************************************************************************************
 * @brief Dump profile with `iprintf`
 *
 * @param[in] first                 first service
 ****************************************************************************************
 */
void gatt_client_util_dump_profile(service_node_t *first, void *user_data, int err_code);

/**
 ****************************************************************************************
 * @brief Find charasteristic node
 *
 * @param[in] discoverer            discovering context
 * @param[in] handle                handle of the charasteristic
 * @return                          the found charasteristic node
 ****************************************************************************************
 */
char_node_t *gatt_client_util_find_char(struct gatt_client_discoverer *discoverer, uint16_t handle);

/**
 ****************************************************************************************
 * @brief Find charasteristic node from uuid
 *
 * @param[in] discoverer            discovering context
 * @param[in] uuid                  uuid
 * @return                          the found charasteristic node
 ****************************************************************************************
 */
char_node_t *gatt_client_util_find_char_uuid128(struct gatt_client_discoverer *discoverer,
                                                const uint8_t *uuid);

/**
 ****************************************************************************************
 * @brief Find charasteristic node from sig uuid
 *
 * @param[in] discoverer            discovering context
 * @param[in] uuid                  sig uuid
 * @return                          the found charasteristic node
 ****************************************************************************************
 */
char_node_t *gatt_client_util_find_char_uuid16(struct gatt_client_discoverer *discoverer,
                                                const uint16_t uuid);

/**
 ****************************************************************************************
 * @brief Find the client configuration descriptor of a charasteristic node
 *
 * @param[in] c                     the charasteristic node
 * @return                          the client configuration descriptor node
 ****************************************************************************************
 */
desc_node_t *gatt_client_util_find_config_desc(char_node_t *c);

/**
 ****************************************************************************************
 * @brief Print a 128-bit UUID
 *
 * Note: SIG UUID is printed as an uint16_t.
 *
 * @param[in] uuid                  UUID
 ****************************************************************************************
 */
void gatt_client_util_print_uuid(const uint8_t *uuid);

/**
 ****************************************************************************************
 * @brief Print properties of a charasteristic
 *
 * @param[in] v                     properties
 ****************************************************************************************
 */
void gatt_client_util_print_properties(uint16_t v);

#ifdef __cplusplus
}
#endif

#endif
