#include "include.h"
#include "ble_service.h"
#include "ble_user_service.h"
#include "ble_fota_service.h"
#include "ble_hid_service.h"
#include "ble_battery_service.h"


void ble_service_init(void)
{
    printf("ble_service_init\n");

    ble_user_service_init();
    ble_battery_service_init();

#if LE_HID_EN
    ble_hid_service_init();
#endif // LE_HID_EN

#if LE_FOTA_EN
    ble_fota_service_init();
#endif
}

