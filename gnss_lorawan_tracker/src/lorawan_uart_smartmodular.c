#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <stdio.h>
#include <string.h>
#include <zephyr/logging/log.h>
#include "lorawan_uart_smartmodular.h"

LOG_MODULE_DECLARE(lorawan_gnss_tracker);

/* Define time to receive response from smart modular lorawan module */
#define TIME_TO_RECEIVE_RESPONSE   1000 //ms

/* Define LoRaWAN Port (for LoRaWAN communication) */
#define LORAWAN_PORT               12

/* Define size (bytes) of latitude and longitude */
#define SIZE_LAT_LONG_BYTES        sizeof(float)

/* Load device tree defs for lorawan uart */
const struct device *uart2 = DEVICE_DT_GET(DT_NODELABEL(uart2));

/* Struct for lorawan uart configuration */
struct uart_config uart_cfg = {
	.baudrate = 9600,
	.parity = PARITY_LORAWAN_UART,
	.stop_bits = STOP_BITS_LORAWAN_UART,
	.flow_ctrl = FLOW_CTRL_LORAWAN_UART,
	.data_bits = DATA_BITS_LORAWAN_UART,
};

/* Local functions prototypes */
void wake_up_lorawan_smartmodular_uart(void);
void reset_lorawan_smartmodular_uart(void);
void join_mode_abp_smartmodular_uart(void);
void class_A_smartmodular_uart(void);
void device_address_smartmodular_uart(void);
void appskey_smartmodular_uart(void);
void nwskey_smartmodular_uart(void);
void appeui_smartmodular_uart(void);
void send_custom_at_command_lorawan_smartmodular_uart(char *at_command);
void rcv_response_lorawan_smartmodular_uart(void);

/* Function: init smart modular lorawan module uart communication
 * Params: none
 * Return: 0: error
 *         1: ok
 */
int init_lorawan_smartmodular_uart(void)
{
    int rc;
	int init_lorawan_smartmodular_uart_status = 0;

	LOG_INF("Starting LoRaWAN UART Smart Modular module...");

    rc = uart_configure(uart2, &uart_cfg);
    
    if (rc)
    {
		LOG_INF("Failed to configure lorawan uart");
		init_lorawan_smartmodular_uart_status = 0;
		goto END_INIT_LORAWAN_SMART_MODULAR;
	}
        
    /* Wake up lorawan module */
	wake_up_lorawan_smartmodular_uart();

	/* Reset lorawan module */
	reset_lorawan_smartmodular_uart();

	/* Configure join mode as ABP */
	join_mode_abp_smartmodular_uart();

	/* Configure LoRaWAN class as A */
	class_A_smartmodular_uart();

	/* Configure Device Address */
	device_address_smartmodular_uart();

	/* Configure Application Session Key */
    appskey_smartmodular_uart();

	/* Configure Network Session Key */
	nwskey_smartmodular_uart();

	/* Configure Application EUI */
	appeui_smartmodular_uart();

    LOG_INF("LoRaWAN UART Smart Modular module has been started");
    init_lorawan_smartmodular_uart_status = 1;

END_INIT_LORAWAN_SMART_MODULAR:	
	return init_lorawan_smartmodular_uart_status;
}

/* Function: send AT command to wake up lorawan module
 * Params: none
 * Return: none
 */
void wake_up_lorawan_smartmodular_uart(void)
{
    char at_cmd[100] = {0};

	memset(at_cmd, 0x00, sizeof(at_cmd));
	snprintf(at_cmd, sizeof(at_cmd), "%s%s", AT_CMD_WAKE, AT_CMD_TERMINATOR);
	send_custom_at_command_lorawan_smartmodular_uart(at_cmd);
	k_msleep(TIME_TO_RECEIVE_RESPONSE);
    rcv_response_lorawan_smartmodular_uart();
}

/* Function: send AT command to reset lorawan module
 * Params: none
 * Return: none
 */
void reset_lorawan_smartmodular_uart(void)
{
    char at_cmd[100] = {0};

	memset(at_cmd, 0x00, sizeof(at_cmd));
	snprintf(at_cmd, sizeof(at_cmd), "%s%s", AT_CMD_RESET, AT_CMD_TERMINATOR);
	send_custom_at_command_lorawan_smartmodular_uart(at_cmd);
	k_msleep(TIME_TO_RECEIVE_RESPONSE);
    rcv_response_lorawan_smartmodular_uart();
}

/* Function: send AT command to configure join mode as ABP
 * Params: none
 * Return: none
 */
void join_mode_abp_smartmodular_uart(void)
{
    char at_cmd[100] = {0};

	memset(at_cmd, 0x00, sizeof(at_cmd));
	snprintf(at_cmd, sizeof(at_cmd), "%s%s", AT_CMD_JOIN_MODE_ABP, AT_CMD_TERMINATOR);
	send_custom_at_command_lorawan_smartmodular_uart(at_cmd);
	k_msleep(TIME_TO_RECEIVE_RESPONSE);
    rcv_response_lorawan_smartmodular_uart();
}

/* Function: send AT command to configure lorawan class as A
 * Params: none
 * Return: none
 */
void class_A_smartmodular_uart(void)
{
	char at_cmd[100] = {0};

	memset(at_cmd, 0x00, sizeof(at_cmd));
	snprintf(at_cmd, sizeof(at_cmd), "%s%s", AT_CMD_CLASS, AT_CMD_TERMINATOR);
	send_custom_at_command_lorawan_smartmodular_uart(at_cmd);
	k_msleep(TIME_TO_RECEIVE_RESPONSE);
    rcv_response_lorawan_smartmodular_uart();
}

/* Function: send AT command to configure lorawan class as A
 * Params: none
 * Return: none
 */
void device_address_smartmodular_uart(void)
{
	char at_cmd[100] = {0};

	memset(at_cmd, 0x00, sizeof(at_cmd));
	snprintf(at_cmd, sizeof(at_cmd), "%s=%s%s", AT_CMD_DEVADDR, CONFIG_LORAWAN_UART_SMARTMODULAR_DEVADDR, AT_CMD_TERMINATOR);
	send_custom_at_command_lorawan_smartmodular_uart(at_cmd);
	k_msleep(TIME_TO_RECEIVE_RESPONSE);
    rcv_response_lorawan_smartmodular_uart();
}

/* Function: send AT command to configure lorawan application session key
 * Params: none
 * Return: none
 */
void appskey_smartmodular_uart(void)
{
    char at_cmd[100] = {0};

	memset(at_cmd, 0x00, sizeof(at_cmd));
	snprintf(at_cmd, sizeof(at_cmd), "%s=%s%s", AT_CMD_APPSKEY, CONFIG_LORAWAN_UART_SMARTMODULAR_APPSKEY, AT_CMD_TERMINATOR);
	send_custom_at_command_lorawan_smartmodular_uart(at_cmd);
	k_msleep(TIME_TO_RECEIVE_RESPONSE);
    rcv_response_lorawan_smartmodular_uart();
}

/* Function: send AT command to configure lorawan network session key
 * Params: none
 * Return: none
 */
void nwskey_smartmodular_uart(void)
{
    char at_cmd[100] = {0};

	memset(at_cmd, 0x00, sizeof(at_cmd));
	snprintf(at_cmd, sizeof(at_cmd), "%s=%s%s", AT_CMD_NWSKEY, CONFIG_LORAWAN_UART_SMARTMODULAR_NWKSKEY, AT_CMD_TERMINATOR);
	send_custom_at_command_lorawan_smartmodular_uart(at_cmd);
	k_msleep(TIME_TO_RECEIVE_RESPONSE);
    rcv_response_lorawan_smartmodular_uart();
}

/* Function: send AT command to configure lorawan application eui
 * Params: none
 * Return: none
 */
void appeui_smartmodular_uart(void)
{
    char at_cmd[100] = {0};

	memset(at_cmd, 0x00, sizeof(at_cmd));
	snprintf(at_cmd, sizeof(at_cmd), "%s=%s%s", AT_CMD_APPEUI, CONFIG_LORAWAN_UART_SMARTMODULAR_APPEUI, AT_CMD_TERMINATOR);
	send_custom_at_command_lorawan_smartmodular_uart(at_cmd);
	k_msleep(TIME_TO_RECEIVE_RESPONSE);
    rcv_response_lorawan_smartmodular_uart();
}

/* Function: send tracking data as LoRaWAN message
 * Params: - tracking data (latitude and longitude)
 *         - ignition status
 * Return: none
 */
void send_tracking_data(float latitude, float longitude, bool ignition_status, bool gnss_fix_status)
{
    char at_cmd[100] = {0};
	char latitude_byte_array[SIZE_LAT_LONG_BYTES] = {0};
	char longitude_byte_array[SIZE_LAT_LONG_BYTES] = {0};
	char tracker_status_byte = 0x00;

	/* Tracker status byte breakdown:
	 *
	 * Bit 0: ignition status (0: off / 1: on)
	 * Bit 1: GNSS fix status
	 * Bit 2: reserved
	 * Bit 3: reserved
	 * Bit 4: reserved
	 * Bit 5: reserved
	 * Bit 6: reserved
	 * Bit 7: reserved
	*/

    /* Format tracket status byte */
	/* Ignition status */
	if (ignition_status == true)
	{
		tracker_status_byte = tracker_status_byte | IGNITION_STATUS_MASK;
	}

    /* GNSS fix status */
	if (gnss_fix_status == true)
	{
		tracker_status_byte = tracker_status_byte | GNSS_STATUS_MASK;
	}


	memset(at_cmd, 0x00, sizeof(at_cmd));
	memcpy(latitude_byte_array, (char *)&latitude, SIZE_LAT_LONG_BYTES);
	memcpy(longitude_byte_array, (char *)&longitude, SIZE_LAT_LONG_BYTES);
	snprintf(at_cmd, sizeof(at_cmd), "%s=%d:%02X%02X%02X%02X%02X%02X%02X%02X%02X%s", AT_CMD_SEND_BYTE, 
	                                                                                 LORAWAN_PORT, 
													                                 latitude_byte_array[0],
														                             latitude_byte_array[1],
														                             latitude_byte_array[2],
														                             latitude_byte_array[3],
																				     longitude_byte_array[0],
														                             longitude_byte_array[1],
														                             longitude_byte_array[2],
														                             longitude_byte_array[3],
																			     	 tracker_status_byte,
														                             AT_CMD_TERMINATOR);

    LOG_INF("Full AT CMD(send lorawan message): %s", at_cmd);

	send_custom_at_command_lorawan_smartmodular_uart(at_cmd);
	k_msleep(TIME_TO_RECEIVE_RESPONSE);
    rcv_response_lorawan_smartmodular_uart();
}

/* Function: send custom AT command to smart modular lorawan module
 * Params: pointer to char array containing string
 * Return: none
 */
void send_custom_at_command_lorawan_smartmodular_uart(char *at_command)
{
	int msg_len = strlen(at_command);
    int i = 0;

	for (i=0; i<msg_len; i++) 
    {
		uart_poll_out(uart2, at_command[i]);
	}
}

/* Function: receive response from smart modular lorawan module
 * Params: 
 * Return: none
 */
void rcv_response_lorawan_smartmodular_uart(void)
{
    char c;
		
    LOG_INF("Bytes received from LoRaWAN module:");

	while (!uart_poll_in(uart2, &c)) 
	{
        LOG_INF("%c", c);
	}
}