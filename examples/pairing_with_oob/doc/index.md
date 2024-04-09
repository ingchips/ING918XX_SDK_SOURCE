# Pairing with OOB

## Test

Build this project twice with compiling switch `ROLE` set to `0` & `1` respectively,
and download it two Dev-Boards.

To test LE Secure Connection pairing, define `LE_SECURE_CONNECTION_PAIRING`; to test
LE legacy pairing, do not define `LE_SECURE_CONNECTION_PAIRING`.

Check that those two Dev-Boards can be paired.