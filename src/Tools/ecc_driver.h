#ifndef _ecc_driver_h
#define _ecc_driver_h

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Install a software-implemented ECC engine for Link Layer
 *
 * This is used for LE Secure Connection pairing.
 *
*/
void install_ecc_driver(void);

#ifdef __cplusplus
}
#endif

#endif
