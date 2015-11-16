#pragma once

#include "inet_hal.h"
#include "wiced.h"

#if 0
#ifdef __cplusplus
inline wiced_interface_t wiced_wlan_interface(network_interface_t nif)
{
    return nif ? WICED_AP_INTERFACE : WICED_STA_INTERFACE;
}
#else
    #define wiced_wlan_interface(nif) (nif ? WICED_AP_INTERFACE : WICED_STA_INTERFACE)
#endif
#else
// RT hack to use pre-defined network interface (extern current_network_if)
#ifdef __cplusplus
extern "C" {
extern wiced_interface_t current_network_if;
}
#else
extern wiced_interface_t current_network_if;
#endif
#define wiced_wlan_interface(nif) (current_network_if)
#endif
