#ifndef _btstack_sync_h
#define _btstack_sync_h

#include <stdint.h>
#include <string.h>

#include "bt_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 ****************************************************************************************
 * @brief A set of Bluetooth helpers
 *
 * These helpers are intended to demonstrate usage of Bluetooth APIs from another
 * perspective.
 *
 * NOTE:
 * 1. all device addresses are using big-endian byte order.
 ****************************************************************************************
 */

#define BT_HELPER_DEFAULT_ADV_SET           0
#define BT_HELPER_HCI_CHANNEL               1

/**
 ****************************************************************************************
 * @brief set random address
 *
 * This address can be used for scanning, initiating and advertising.
 *
 * @param address               random address
 * @return                      0: Message is sent to controller; others: failed
 ****************************************************************************************
 */
uint8_t bt_helper_set_random_address(const uint8_t *address);

/**
 ****************************************************************************************
 * @brief set public address
 *
 * This function can only be used in `app_main`.
 *
 * @param address               address
 ****************************************************************************************
 */
void bt_helper_set_public_address(const uint8_t *address);

/**
 ****************************************************************************************
 * @brief init HCI event converter
 *
 * This function converts device addresses in HCI events from little-endian.
 * to **big-endian**.
 *
 * This can be used as a replacement of `hci_add_event_handler`, i.e. to use this,
 * replace `hci_add_event_handler` with `bt_helper_hci_add_event_handler`.
 * The `channel` param of the callback will receive `BT_HELPER_HCI_CHANNEL`
 * instead of 0 in the case of `hci_add_event_handler`.
 *
 * This function can only be called once.
 *
 * @param callback_handler      the callback function to receive the converted HCI
 *                              events
 ****************************************************************************************
 */
void bt_helper_hci_add_event_handler(btstack_packet_callback_registration_t *callback_handler);

struct bt_le_adv_param;

/**
 ****************************************************************************************
 * @brief Start advertising
 *
 * This function simulates `bt_le_adv_start()` in Zephyr.
 *
 * Note: This does not support all functionalities of `bt_le_adv_start()`.
 *
 * Set advertisement data, scan response data, advertisement parameters
 * and start advertising.
 *
 * When the advertisement parameter peer address has been set the advertising
 * will be directed to the peer. In this case advertisement data and scan
 * response data parameters are ignored. If the mode is high duty cycle
 * the timeout will be @ref BT_GAP_ADV_HIGH_DUTY_CYCLE_MAX_TIMEOUT.
 *
 * This function cannot be used with @ref BT_LE_ADV_OPT_EXT_ADV in the @p param.options.
 * For extended advertising, the bt_le_ext_adv_* functions must be used.
 *
 * @param param Advertising parameters.
 * @param ad Data to be used in advertisement packets.
 * @param ad_len Number of elements in ad
 * @param sd Data to be used in scan response packets.
 * @param sd_len Number of elements in sd
 *
 * @return Zero on success or (negative) error code otherwise.
 ****************************************************************************************
 */
int bt_helper_adv_start(const struct bt_le_adv_param *param,
		    const uint8_t *ad, size_t ad_len,
		    const uint8_t *sd, size_t sd_len);

/** Bluetooth LE Device Address */
typedef struct {
	uint8_t      type;
	bd_addr_t a;
} bt_addr_le_t;

/** LE Advertising Parameters. */
struct bt_le_adv_param {
	/**
	 * @brief Local identity.
	 *
	 * @note When extended advertising @kconfig{CONFIG_BT_EXT_ADV} is not
	 *       enabled or not supported by the controller it is not possible
	 *       to scan and advertise simultaneously using two different
	 *       random addresses.
	 */
	uint8_t  id;

	/**
	 * @brief Advertising Set Identifier, valid range 0x00 - 0x0f.
	 *
	 * @note Requires @ref BT_LE_ADV_OPT_EXT_ADV
	 **/
	uint8_t  sid;

	/**
	 * @brief Secondary channel maximum skip count.
	 *
	 * Maximum advertising events the advertiser can skip before it must
	 * send advertising data on the secondary advertising channel.
	 *
	 * @note Requires @ref BT_LE_ADV_OPT_EXT_ADV
	 */
	uint8_t  secondary_max_skip;

	/** Bit-field of advertising options */
	uint32_t options;

	/** Minimum Advertising Interval (N * 0.625 milliseconds)
	 * Minimum Advertising Interval shall be less than or equal to the
	 * Maximum Advertising Interval. The Minimum Advertising Interval and
	 * Maximum Advertising Interval should not be the same value (as stated
	 * in Bluetooth Core Spec 5.2, section 7.8.5)
	 * Range: 0x0020 to 0x4000
	 */
	uint32_t interval_min;

	/** Maximum Advertising Interval (N * 0.625 milliseconds)
	 * Minimum Advertising Interval shall be less than or equal to the
	 * Maximum Advertising Interval. The Minimum Advertising Interval and
	 * Maximum Advertising Interval should not be the same value (as stated
	 * in Bluetooth Core Spec 5.2, section 7.8.5)
	 * Range: 0x0020 to 0x4000
	 */
	uint32_t interval_max;

	/**
	 * @brief Directed advertising to peer
	 *
	 * When this parameter is set the advertiser will send directed
	 * advertising to the remote device.
	 *
	 * The advertising type will either be high duty cycle, or low duty
	 * cycle if the BT_LE_ADV_OPT_DIR_MODE_LOW_DUTY option is enabled.
	 * When using @ref BT_LE_ADV_OPT_EXT_ADV then only low duty cycle is
	 * allowed.
	 *
	 * In case of connectable high duty cycle if the connection could not
	 * be established within the timeout the connected() callback will be
	 * called with the status set to @ref BT_HCI_ERR_ADV_TIMEOUT.
	 */
	const bt_addr_le_t *peer;
};

#define BT_ID_DEFAULT BT_HELPER_DEFAULT_ADV_SET

/** Advertising options */
enum {
	/** Convenience value when no options are specified. */
	BT_LE_ADV_OPT_NONE = 0,

	/**
	 * @brief Advertise as connectable.
	 *
	 * Advertise as connectable. If not connectable then the type of
	 * advertising is determined by providing scan response data.
	 * The advertiser address is determined by the type of advertising
	 * and/or enabling privacy @kconfig{CONFIG_BT_PRIVACY}.
	 */
	BT_LE_ADV_OPT_CONNECTABLE = BIT(0),

	/**
	 * @brief Advertise one time.
	 *
	 * Don't try to resume connectable advertising after a connection.
	 * This option is only meaningful when used together with
	 * BT_LE_ADV_OPT_CONNECTABLE. If set the advertising will be stopped
	 * when bt_le_adv_stop() is called or when an incoming (peripheral)
	 * connection happens. If this option is not set the stack will
	 * take care of keeping advertising enabled even as connections
	 * occur.
	 * If Advertising directed or the advertiser was started with
	 * @ref bt_le_ext_adv_start then this behavior is the default behavior
	 * and this flag has no effect.
	 */
	BT_LE_ADV_OPT_ONE_TIME = BIT(1),

	/**
	 * @brief Advertise using identity address.
	 *
	 * Advertise using the identity address as the advertiser address.
	 * @warning This will compromise the privacy of the device, so care
	 *          must be taken when using this option.
	 * @note The address used for advertising will not be the same as
	 *        returned by @ref bt_le_oob_get_local, instead @ref bt_id_get
	 *        should be used to get the LE address.
	 */
	BT_LE_ADV_OPT_USE_IDENTITY = BIT(2),

	/** Advertise using GAP device name.
	 *
	 *  Include the GAP device name automatically when advertising.
	 *  By default the GAP device name is put at the end of the scan
	 *  response data.
	 *  When advertising using @ref BT_LE_ADV_OPT_EXT_ADV and not
	 *  @ref BT_LE_ADV_OPT_SCANNABLE then it will be put at the end of the
	 *  advertising data.
	 *  If the GAP device name does not fit into advertising data it will be
	 *  converted to a shortened name if possible.
	 *  @ref BT_LE_ADV_OPT_FORCE_NAME_IN_AD can be used to force the device
	 *  name to appear in the advertising data of an advert with scan
	 *  response data.
	 *
	 *  The application can set the device name itself by including the
	 *  following in the advertising data.
	 *  @code
	 *  BT_DATA(BT_DATA_NAME_COMPLETE, name, sizeof(name) - 1)
	 *  @endcode
	 */
	BT_LE_ADV_OPT_USE_NAME = BIT(3),

	/**
	 * @brief Low duty cycle directed advertising.
	 *
	 * Use low duty directed advertising mode, otherwise high duty mode
	 * will be used.
	 */
	BT_LE_ADV_OPT_DIR_MODE_LOW_DUTY = BIT(4),

	/**
	 * @brief Directed advertising to privacy-enabled peer.
	 *
	 * Enable use of Resolvable Private Address (RPA) as the target address
	 * in directed advertisements.
	 * This is required if the remote device is privacy-enabled and
	 * supports address resolution of the target address in directed
	 * advertisement.
	 * It is the responsibility of the application to check that the remote
	 * device supports address resolution of directed advertisements by
	 * reading its Central Address Resolution characteristic.
	 */
	BT_LE_ADV_OPT_DIR_ADDR_RPA = BIT(5),

	/** Use filter accept list to filter devices that can request scan
	 *  response data.
	 */
	BT_LE_ADV_OPT_FILTER_SCAN_REQ = BIT(6),

	/** Use filter accept list to filter devices that can connect. */
	BT_LE_ADV_OPT_FILTER_CONN = BIT(7),

	/** Notify the application when a scan response data has been sent to an
	 *  active scanner.
	 */
	BT_LE_ADV_OPT_NOTIFY_SCAN_REQ = BIT(8),

	/**
	 * @brief Support scan response data.
	 *
	 * When used together with @ref BT_LE_ADV_OPT_EXT_ADV then this option
	 * cannot be used together with the @ref BT_LE_ADV_OPT_CONNECTABLE
	 * option.
	 * When used together with @ref BT_LE_ADV_OPT_EXT_ADV then scan
	 * response data must be set.
	 */
	BT_LE_ADV_OPT_SCANNABLE = BIT(9),

	/**
	 * @brief Advertise with extended advertising.
	 *
	 * This options enables extended advertising in the advertising set.
	 * In extended advertising the advertising set will send a small header
	 * packet on the three primary advertising channels. This small header
	 * points to the advertising data packet that will be sent on one of
	 * the 37 secondary advertising channels.
	 * The advertiser will send primary advertising on LE 1M PHY, and
	 * secondary advertising on LE 2M PHY.
	 * Connections will be established on LE 2M PHY.
	 *
	 * Without this option the advertiser will send advertising data on the
	 * three primary advertising channels.
	 *
	 * @note Enabling this option requires extended advertising support in
	 *       the peer devices scanning for advertisement packets.
	 *
	 * @note This cannot be used with bt_le_adv_start().
	 */
	BT_LE_ADV_OPT_EXT_ADV = BIT(10),

	/**
	 * @brief Disable use of LE 2M PHY on the secondary advertising
	 * channel.
	 *
	 * Disabling the use of LE 2M PHY could be necessary if scanners don't
	 * support the LE 2M PHY.
	 * The advertiser will send primary advertising on LE 1M PHY, and
	 * secondary advertising on LE 1M PHY.
	 * Connections will be established on LE 1M PHY.
	 *
	 * @note Cannot be set if BT_LE_ADV_OPT_CODED is set.
	 *
	 * @note Requires @ref BT_LE_ADV_OPT_EXT_ADV.
	 */
	BT_LE_ADV_OPT_NO_2M = BIT(11),

	/**
	 * @brief Advertise on the LE Coded PHY (Long Range).
	 *
	 * The advertiser will send both primary and secondary advertising
	 * on the LE Coded PHY. This gives the advertiser increased range with
	 * the trade-off of lower data rate and higher power consumption.
	 * Connections will be established on LE Coded PHY.
	 *
	 * @note Requires @ref BT_LE_ADV_OPT_EXT_ADV
	 */
	BT_LE_ADV_OPT_CODED = BIT(12),

	/**
	 * @brief Advertise without a device address (identity or RPA).
	 *
	 * @note Requires @ref BT_LE_ADV_OPT_EXT_ADV
	 */
	BT_LE_ADV_OPT_ANONYMOUS = BIT(13),

	/**
	 * @brief Advertise with transmit power.
	 *
	 * @note Requires @ref BT_LE_ADV_OPT_EXT_ADV
	 */
	BT_LE_ADV_OPT_USE_TX_POWER = BIT(14),

	/** Disable advertising on channel index 37. */
	BT_LE_ADV_OPT_DISABLE_CHAN_37 = BIT(15),

	/** Disable advertising on channel index 38. */
	BT_LE_ADV_OPT_DISABLE_CHAN_38 = BIT(16),

	/** Disable advertising on channel index 39. */
	BT_LE_ADV_OPT_DISABLE_CHAN_39 = BIT(17),

	/**
	 * @brief Put GAP device name into advert data
	 *
	 * Will place the GAP device name into the advertising data rather
	 * than the scan response data.
	 *
	 * @note Requires @ref BT_LE_ADV_OPT_USE_NAME
	 */
	BT_LE_ADV_OPT_FORCE_NAME_IN_AD = BIT(18),

	/**
	 * @brief Advertise using a Non-Resolvable Private Address.
	 *
	 * A new NRPA is set when updating the advertising parameters.
	 *
	 * This is an advanced feature; most users will want to enable
	 * @kconfig{CONFIG_BT_EXT_ADV} instead.
	 *
	 * @note Not implemented when @kconfig{CONFIG_BT_PRIVACY}.
	 *
	 * @note Mutually exclusive with BT_LE_ADV_OPT_USE_IDENTITY.
	 */
	BT_LE_ADV_OPT_USE_NRPA = BIT(19),
};

/**
 * @brief Initialize advertising parameters
 *
 * @param _options   Advertising Options
 * @param _int_min   Minimum advertising interval
 * @param _int_max   Maximum advertising interval
 * @param _peer      Peer address, set to NULL for undirected advertising or
 *                   address of peer for directed advertising.
 */
#define BT_LE_ADV_PARAM_INIT(_options, _int_min, _int_max, _peer) \
{ \
	.id = BT_ID_DEFAULT, \
	.sid = 0, \
	.secondary_max_skip = 0, \
	.options = (_options), \
	.interval_min = (_int_min), \
	.interval_max = (_int_max), \
	.peer = (_peer), \
}

/**
 * @brief Helper to declare advertising parameters inline
 *
 * @param _options   Advertising Options
 * @param _int_min   Minimum advertising interval
 * @param _int_max   Maximum advertising interval
 * @param _peer      Peer address, set to NULL for undirected advertising or
 *                   address of peer for directed advertising.
 */
#define BT_LE_ADV_PARAM(_options, _int_min, _int_max, _peer) \
	((struct bt_le_adv_param[]) { \
		BT_LE_ADV_PARAM_INIT(_options, _int_min, _int_max, _peer) \
	 })

#define BT_GAP_ADV_FAST_INT_MIN_1               0x0030  /* 30 ms    */
#define BT_GAP_ADV_FAST_INT_MAX_1               0x0060  /* 60 ms    */
#define BT_GAP_ADV_FAST_INT_MIN_2               0x00a0  /* 100 ms   */
#define BT_GAP_ADV_FAST_INT_MAX_2               0x00f0  /* 150 ms   */

#define BT_LE_ADV_CONN BT_LE_ADV_PARAM(BT_LE_ADV_OPT_CONNECTABLE |  \
                       BT_LE_ADV_OPT_SCANNABLE,   \
				       BT_GAP_ADV_FAST_INT_MIN_2, \
				       BT_GAP_ADV_FAST_INT_MAX_2, NULL)

#ifdef __cplusplus
}
#endif

#endif
