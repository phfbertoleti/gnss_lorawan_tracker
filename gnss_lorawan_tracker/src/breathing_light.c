#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/__assert.h>
#include <zephyr/logging/log.h>
#include "breathing_light.h"

LOG_MODULE_DECLARE(lorawan_gnss_tracker);

/* Specify which node is used for LED in device tree*/
#define LED0_NODE DT_ALIAS(led0)

/* Define time for toggling breathing light LED */
#define BREATHING_LIGHT_TIME_MS              1000

/* Define thread stack size */
K_THREAD_STACK_DEFINE(breathing_light_thread_stack, CONFIG_BREATHING_LIGHT_TASK_STACK_SIZE);

/* Load beathing light LED device tree data */
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

/* Pulse counter thread data */
struct k_thread breathing_light_thread_data;

/* Prototype of breathing light task */
void breathing_light_task(void *p1, void *p2, void *p3);

/* Function: init breathing light, by setting up LED output
 * Params: none
 * Return: 0: error
 *         1: ok
 */
int init_breathing_light(void)
{
    int ret;
	int init_breathing_light_status = 0;

    LOG_INF("Starting breathing light module...");

	if (!gpio_is_ready_dt(&led)) 
    {
		init_breathing_light_status = 0;
		goto END_BREATHING_LIGHT_INIT;
	}

	ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) 
    {
		init_breathing_light_status = 0;
		goto END_BREATHING_LIGHT_INIT;
	}

    /* Create pulse counter thread */
    k_thread_create(&breathing_light_thread_data, breathing_light_thread_stack,
			      K_THREAD_STACK_SIZEOF(breathing_light_thread_stack),
			      breathing_light_task, NULL, NULL, NULL,
			      CONFIG_BREATHING_LIGHT_TASK_PRIO, 0, K_NO_WAIT);

    LOG_INF("Breathing light module has been started");

    init_breathing_light_status = 1;
	
END_BREATHING_LIGHT_INIT:
    return init_breathing_light_status;
}

/* Function: init breathing light, by setting up LED output
 * Params: none
 * Return: none
 */
void breathing_light_task(void *p1, void *p2, void *p3)
{
    while (1) 
    {
		gpio_pin_toggle_dt(&led);
		k_msleep(BREATHING_LIGHT_TIME_MS);
	}
}