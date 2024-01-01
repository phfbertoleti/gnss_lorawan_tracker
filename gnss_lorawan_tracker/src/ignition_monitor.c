#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/util.h>
#include <zephyr/logging/log.h>
#include "ignition_monitor.h"

LOG_MODULE_DECLARE(lorawan_gnss_tracker);

/* Define time used for debounce ignition input */
#define IGNITION_DEBOUNCE_TIME    400  //ms

/* Ignition input debounce timer */
static struct k_timer ignition_debounce_timer;

/* Variable to indicate ignition status */
static bool ignition_status = false;

/*  Variable to indicate timestamp (uptime, in milisseconds) when ignition state became on */
int64_t timestamp_ignition_on = 0;

/*  Variable to indicate how many time (in milisseconds) ignition was on */
int64_t total_time_ignition_on = 0;

/*
 * Get ignition signal input configuration from the devicetree ignition alias.
 */
#define IGNITIONINPUT_NODE	DT_ALIAS(ignition)

#if !DT_NODE_HAS_STATUS(IGNITIONINPUT_NODE, okay)
#error "Unsupported board: ignition devicetree alias is not defined"
#endif
static const struct gpio_dt_spec ignition = GPIO_DT_SPEC_GET_OR(IGNITIONINPUT_NODE, gpios,
							      {0});
static struct gpio_callback ignition_cb_data;

/* Ignition input debounce timer callback */
static void ignition_debounce_timer_callback(struct k_timer *timer);

/* Function: ignition detection callback
 * Params: callback params
 * Return: none
 */
void ignition_detected(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
 	k_timer_start(&ignition_debounce_timer, K_MSEC(IGNITION_DEBOUNCE_TIME), K_NO_WAIT);
}

/* Function: timer callback, used for debouncing ignition input
 * Params: callback params
 * Return: none
 */
static void ignition_debounce_timer_callback(struct k_timer *timer)
{
    int current_state = gpio_pin_get_dt(&ignition);
    
    switch (current_state)
    {
        case 1:
            
            timestamp_ignition_on = k_uptime_get();
            ignition_status = true;
            LOG_INF("Ignition is on");
            break;

        case 0:
            total_time_ignition_on = k_uptime_get() - timestamp_ignition_on;
            ignition_status = false;
            LOG_INF("Ignition is off. Time with ignition on = %lld ms", total_time_ignition_on);            
            break;

        default:
            break;    
    }
}

/* Function: init ignition monitor
 * Params: none
 * Return: 0: error
 *         1: ok
 */
int init_ignition_monitor(void)
{
    int ret;
    int ignition_module_init_status = 0;
    
    LOG_INF("Starting ignition monitor module...");

    if (!gpio_is_ready_dt(&ignition)) 
	{
        LOG_ERR("Error: ignition input device %s is not ready", ignition.port->name);
        ignition_module_init_status = 0;
        goto END_IGNITION_MODULE_INIT;
	}

	ret = gpio_pin_configure_dt(&ignition, GPIO_INPUT);
	if (ret != 0) 
	{
        LOG_ERR("Error %d: failed to configure %s pin %d", ret, ignition.port->name, ignition.pin);
		ignition_module_init_status = 0;
        goto END_IGNITION_MODULE_INIT;
	}

    /* Configure ignition input interrupt edge as rising edge */
	ret = gpio_pin_interrupt_configure_dt(&ignition,
					                      GPIO_INT_EDGE_BOTH);
	if (ret != 0) 
	{
        LOG_ERR("Error %d: failed to configure interrupt on %s pin %d", ret, ignition.port->name, ignition.pin);
		ignition_module_init_status = 0;
        goto END_IGNITION_MODULE_INIT;
	}

	gpio_init_callback(&ignition_cb_data, ignition_detected, BIT(ignition.pin));
	gpio_add_callback(ignition.port, &ignition_cb_data);
	LOG_INF("Finished setting up ignition input at %s pin %d", ignition.port->name, ignition.pin);

    /* Init ignition input debounce timer */
    k_timer_init(&ignition_debounce_timer, ignition_debounce_timer_callback, NULL);

    LOG_INF("Ignition monitor module has been started");
    ignition_module_init_status = 1;

END_IGNITION_MODULE_INIT:
    return ignition_module_init_status;
}

/* Function: obtain ignition status
 * Params: none
 * Return: ignition status (true: ignition on / false: ignition off)
 */
bool get_ignition_staus(void)
{
    return ignition_status;
}

/* Function: get time with ignition on
 * Params: none
 * Return: time with ignition on
 */
int64_t get_time_with_ignition_on(void)
{
    return total_time_ignition_on;
}