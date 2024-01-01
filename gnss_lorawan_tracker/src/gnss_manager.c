#include <zephyr/drivers/gnss.h>
#include <zephyr/device.h>
#include <zephyr/sys/printk.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include "lorawan_uart_smartmodular.h"
#include "ignition_monitor.h"

LOG_MODULE_DECLARE(lorawan_gnss_tracker);

/* Define thread stack size */
K_THREAD_STACK_DEFINE(gnss_manager_thread_stack, CONFIG_BREATHING_LIGHT_TASK_STACK_SIZE);

/* Pulse counter thread data */
struct k_thread gnss_manager_thread_data;

/* Last latitude and longitude */
static float last_latitude;
static float last_longitude;

/* Variable to indicate fix status*/
static bool gnss_fix_status = false;

/* Prototype of GNSS manager task */
void gnss_manager_task(void *p1, void *p2, void *p3);

/* Function - GNSS data received */
static void gnss_data_cb(const struct device *dev, const struct gnss_data *data)
{
	/* If GNSS module position is fixed, prints it in verbose debug */
	if (data->info.fix_status != GNSS_FIX_STATUS_NO_FIX) 
	{
        last_latitude = data->nav_data.latitude / 1000000000.0;
        last_longitude = data->nav_data.longitude / 1000000000.0;
        LOG_INF("%s has fix!", dev->name);
        gnss_fix_status = true;
	}
    else
    {
        gnss_fix_status = false;
    }
}

GNSS_DATA_CALLBACK_DEFINE(DEVICE_DT_GET(DT_NODELABEL(gnss)), gnss_data_cb);

/* Function: callback - satellites found */
#if CONFIG_GNSS_SATELLITES
static void gnss_satellites_cb(const struct device *dev, const struct gnss_satellite *satellites,
			       uint16_t size)
{
	LOG_INF("%u satellites have been found.", size);
}
#endif

GNSS_SATELLITES_CALLBACK_DEFINE(DEVICE_DT_GET(DT_NODELABEL(gnss)), gnss_satellites_cb);

/* Function: init GNSS manager
 * Params: none
 * Return: 0: failure
 *         1: success
 */
int init_gnss_manager(void)
{
    LOG_INF("Starting GNSS manager module...");

    last_latitude = 0.0;
    last_longitude = 0.0;

    /* Create GNSS manager thread */
    k_thread_create(&gnss_manager_thread_data, gnss_manager_thread_stack,
			      K_THREAD_STACK_SIZEOF(gnss_manager_thread_stack),
			      gnss_manager_task, NULL, NULL, NULL,
			      CONFIG_GNSS_MANAGER_TASK_PRIO, 0, K_NO_WAIT);

    LOG_INF("GNSS manager module has been started");    

    return 1;              
}

/* Function: get last latitude value
 * Params: none
 * Return: last latitude
 */
float get_last_latitude(void)
{
    return last_latitude;
}

/* Function: get last longitude value
 * Params: none
 * Return: last longitude
 */
float get_last_longitude(void)
{
    return last_longitude;
}

/* GNSS manager thread implementation */
void gnss_manager_task(void *p1, void *p2, void *p3)
{
    while(1)
    {
        LOG_INF("Sending tracking data through LoRaWAN...");
        send_tracking_data(get_last_latitude(), get_last_longitude(), get_ignition_staus(), gnss_fix_status);
        LOG_INF("Tracking data sent through LoRaWAN. Waiting for %d ms to send it again.", CONFIG_GNSS_MANAGER_TIME_TO_SEND_LORAWAN_MESSAGE);
        k_msleep(CONFIG_GNSS_MANAGER_TIME_TO_SEND_LORAWAN_MESSAGE);
    }
}