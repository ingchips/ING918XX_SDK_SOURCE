#pragma once

#include "ingsoc.h"
#include "peripheral_ssp.h"

#include "profile.h"

#define CS_HIGH()	    GIO_WriteValue(SPI_PIN_CS, 1)
#define CS_LOW()	    GIO_WriteValue(SPI_PIN_CS, 0)
#define MMC_CD          1   /* Card detect (yes:true, no:false, default:true) */
#define MMC_WP          0   /* Write protected (yes:true, no:false, default:false) */
