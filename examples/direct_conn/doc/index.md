# Direct Connection

This example demonstrates how to directly create a connection between
two devices using `ll_create_conn` without advertising.

Before calling `ll_create_conn`, all connection parameters (PHY, interval,
access address, CRC init value, master clock accuracy, etc) need to be exchanged using
other methods which is designed by developers. In this example, raw packet API is used
to pass these parameters to the Peripheral device in packet (named `paging_packet` here);
when Peripheral device receives such paging, it will respond with a another raw packet
(named `access_packet` here) carrying its address. When Central receives `access_packet`,
the connection is created by `ll_create_conn`.

## Test

Build this project twice with compiling switch `CONN_ROLE` set to `0` & `1` respectively,
and download it two Dev-Boards.

Check `printf` log to see if the connection can be established or not.
