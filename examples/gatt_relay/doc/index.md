# GATT Relay

This example scans for the device with `peer_addr`, copies advertising data
to its own advertising, then connects to it and copies its GATT profile.
Other devices can now connect to this example, and everything appears to be
the same as connected to the device with `peer_addr`.

## Design Details

[`gatt_client_util`](../../../src/Tools/gatt_client_util.c) module is used for dumping the whole profile.

Deferred read is used to support reading the value of a characteristic:

1. Return `ATT_DEFERRED_READ` in the read callback querying for data size;
1. Use `att_server_deferred_read_response` to send value when data is ready.