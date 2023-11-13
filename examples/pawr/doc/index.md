# Periodic Advertising with Response

This example demonstrates how to use PAwR API.

This example contains two roles, Advertiser and Scanner, which is defined by setting
compiling switch `ROLE` to `ROLE_ADVERTISER` and `ROLE_SCANNER`.

Advertiser broadcasts one periodic advertising set with two subevents. Data of these
subevents are updated from a counter. Once a response on a subevent is received,
it will try to initiate a connection (`gap_ext_create_connection_v2`).

Scanner tries to establish synchronization to the periodic advertising set from
Advertiser. Then, it synchronizes to the first two subevents. When it receives
data from subevent #0, it send a response on subevent #1.

## Test

Build this project twice with compiling switch `ROLE` set to `0` & `1` respectively,
and download it two Dev-Boards.

Check `printf` log to see if the connection can be established or not.

### `printf` log of Advertiser

* Subevents data request:

    ```shell
    SUBEVT_DATA_REQ:
       adv_handle          = 0
       subevent_start      = 0
       subevent_data_count = 1
    SUBEVT_DATA_REQ:
       adv_handle          = 0
       subevent_start      = 1
       subevent_data_count = 1
    ```

* Response of subevents:

    ```shell
    RSP_REPORT:
       subevent      = 1
       response_slot = 0
       data      : ..........
    ```

* Connected:

    ```shell
    CONNECTED
    ```

### `printf` log of Scanner

* Subevents data:

    ```shell
    SUBEVT REPORT (#10)
       subevent = 0
       data     : ......
    SUBEVT REPORT (#10)
       subevent = 1
       data     : ......
    ```

* Connected:

    ```shell
    CONNECTED
    ```