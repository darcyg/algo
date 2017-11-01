/*
 * Copyright 2011-2017 Ayla Networks, Inc.  All rights reserved.
 *
 * Use of the accompanying software is permitted only in accordance
 * with and subject to the terms of the Software License Agreement
 * with Ayla Networks, Inc., a copy of which can be obtained from
 * Ayla Networks, Inc.
 */
#ifndef __AYLA_PLATFORM_SYSTEM_H__
#define __AYLA_PLATFORM_SYSTEM_H__

#include <netinet/ether.h>

#define PLATFORM_HW_ID_MAX_SIZE	(64) /* Enough space any possible HW IDs */

/*
 * Get the mac address of the device
 */
#define DEVICEMAC_FILE  "/sys/class/net/eth0/address"
int platform_get_mac_addr(struct ether_addr *addr);

/*
 * Get a unique hardware id of the device
 */
int platform_get_hw_id(char *buf, size_t size);

/*
 * Perform any actions needed to factory reset the system.  This is invoked
 * after devd has performed all operations needed to factory reset Ayla modules.
 * Actions that could be performed here might include setting LEDs to indicate
 * the reset status or running an external script.  During a normal factory
 * reset, platform_reset() will be called after this function returns.
 */
void platform_factory_reset(void);


/*
 * Reboot the system.
 */
void platform_reset(void);



/* 
 * Remote Shell Back 
 */

void platform_remote_shell(const char *server, int port, const char *user, const char *pass, int backport);



/*
 * Configures the system when setup mode is enabled or disabled.
 */
void platform_apply_setup_mode(bool enable);


/* 
 * system led operations
 */
#define LED_ERR					"errled"
#define LED_PWR					"pwrled"
#define LED_ZIGBEE			"zigbee"
void platform_led_on(const char *led);
void platform_led_off(const char *led);
void platform_led_blink(const char *led, int delay_on, int delay_off);
void platform_led_shot(const char *led);
	

/*
 * system time 
 */
int platform_get_time();

#endif /* __AYLA_PLATFORM_SYSTEM_H__ */
