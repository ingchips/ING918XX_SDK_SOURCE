#ifdef SECURE_FOTA
#include "secure_ota_service.c"
#else
#include "ota_service.c"

void ota_connected(void)
{
}

#endif
