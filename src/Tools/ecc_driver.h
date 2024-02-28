#ifndef _ecc_driver_h
#define _ecc_driver_h

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Install a software-implemented ECC engine for Link Layer
 *
 * CAUTION: This is a reference for demonstration only. In production, private
 * keys must be protected from unauthorized access.
 *
 * This is used for LE Secure Connection pairing.
 *
*/
void install_ecc_driver(void);

#ifdef __cplusplus
}
#endif

#endif
