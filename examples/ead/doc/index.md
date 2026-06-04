# EAD

## Overview

This example shows how to use the controller CCM engine to implement Encrypted Advertising Data (EAD).

Two roles are supported in src/profile.c:

* APP_ROLE_TX: build nonce, encrypt payload, and send AD type 0x31.
* APP_ROLE_RX: scan reports, parse AD type 0x31, rebuild nonce, and decrypt payload.

The CCM operation is started by gap_start_ccm:

* TX: type = 0 (encrypt)
* RX: type = 1 (decrypt)

## Payload Format

Current plaintext payload length is 6 bytes:

* Byte 0..1: temperature, little-endian, unit 0.01 C
* Byte 2..5: counter, little-endian

EAD data section after AD type 0x31:

* Randomizer: 5 bytes
* Ciphertext: 6 bytes
* MIC: 4 bytes

Total EAD payload in AD type 0x31: 15 bytes.

## Key Parameters

The following fields in src/profile.c must match between TX and RX:

* session_key (16 bytes)
* iv (8 bytes)
* ead_aad (0xEA)
* BT_EAD_MIC_SIZE (4)

If any value is different, decrypt authentication fails.

## Role Selection

Set APP_ROLE in src/profile.c:

* APP_ROLE_TX for broadcaster board
* APP_ROLE_RX for scanner board

Default is APP_ROLE_RX in current code.

## Test

1. Build and download TX firmware to board A.
2. Build and download RX firmware to board B.
3. Open UART logs on both boards.
4. Power on TX first, then RX.
5. Confirm RX prints EAD decrypt result.

Typical RX output:

EAD RX ok: temp=23.50C counter=0

## Troubleshooting

### cmd_complete status=18

Status 18 means invalid HCI command parameters.

Check:

* APP_ROLE is correct for current firmware
* Scan address type and filter policy are valid for your board configuration
* Random address setup matches own address type

### CCM decrypt failed

Check TX and RX consistency:

* session_key
* iv
* ead_aad
* Nonce composition: randomizer plus iv

### ADV buffer too small

Legacy ADV limit is 31 bytes.

If ADV_DEVICE_NAME is too long, EAD does not fit. Reduce name length or remove other AD fields.

## Files

* src/profile.c: EAD TX/RX logic and CCM callbacks
* src/main.c: platform boot and callback registration
* data/gatt.profile: generated GATT database
