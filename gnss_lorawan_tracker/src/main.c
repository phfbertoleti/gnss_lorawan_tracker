/* Project: GNSS LoRaWAN tracker
 * Author: Pedro Bertoleti
 */

#include <stdio.h>
#include <zephyr/device.h>
#include <zephyr/logging/log.h>
#include "breathing_light.h"
#include "lorawan_uart_smartmodular.h"
#include "gnss_manager.h"
#include "ignition_monitor.h"

LOG_MODULE_REGISTER(lorawan_gnss_tracker);

int main(void)
{
	int ret_breathing_light = 0;
	int ret_ignition_monitor = 0;
	int ret_lorawan_smartmodular_uart = 0;
	int ret_gnss_manager = 0;

	LOG_INF("Starting LoRaWAN GNSS tracker...");
	
	ret_breathing_light = init_breathing_light();
	ret_ignition_monitor = init_ignition_monitor();
	ret_lorawan_smartmodular_uart = init_lorawan_smartmodular_uart();
	ret_gnss_manager = init_gnss_manager();

	if ( (ret_breathing_light == 1) && (ret_ignition_monitor == 1) && 
	     (ret_lorawan_smartmodular_uart == 1) && (ret_gnss_manager == 1) )
	{
        LOG_INF("All LoRaWAN GNSS tracker modules have been successfully started");
	}
	else
	{
		LOG_ERR("Some modules experienced errors in their init routines.");
	}
	
	return 0;
}
