# Periodic Scanner

This example scans for the periodic advertising from `target_addr`, and then synchronizes to it.

The temperature contained in the advertising is printed to UART.
The CTE signal attached to the advertising is also sampled and printed to UART in Base64 encoding.

## Test

Download _Periodic Advertiser_ and _Periodic Scanner_ to two Dev-Boards, and check the UART output
from _Periodic Scanner_.

_Periodic Scanner_ needs to be downloaded to an array board when AoA calculation is going to be performed
on the CTE samples.