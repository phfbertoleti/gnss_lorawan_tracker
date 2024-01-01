/* Header file: smart modular (lorawan module) */
#include <zephyr/drivers/uart.h>

#ifndef HEADER_LORAWAN_UART
#define HEADER_LORAWAN_UART

/* UART parametrization */
#define BAUDRATE_LORAWAN_UART    9600
#define PARITY_LORAWAN_UART      UART_CFG_PARITY_NONE
#define STOP_BITS_LORAWAN_UART   UART_CFG_STOP_BITS_1
#define FLOW_CTRL_LORAWAN_UART   UART_CFG_FLOW_CTRL_NONE
#define DATA_BITS_LORAWAN_UART   UART_CFG_DATA_BITS_8

/* AT Commands */
#define AT_CMD_TERMINATOR        "\n"
#define AT_CMD_WAKE              "AT"
#define AT_CMD_RESET             "ATZ"
#define AT_CMD_JOIN_MODE_ABP     "AT+NJM=0"
#define AT_CMD_CLASS             "AT+CLASS=A"
#define AT_CMD_DEVADDR           "AT+DADDR"
#define AT_CMD_APPSKEY           "AT+APPSKEY"
#define AT_CMD_NWSKEY            "AT+NWKSKEY"
#define AT_CMD_APPEUI            "AT+APPEUI"
#define AT_CMD_SEND_BYTE         "AT+SENDB"

/*  Tracker status masks */
#define IGNITION_STATUS_MASK    0x01
#define GNSS_STATUS_MASK        0x02
#define BIT_2_STATUS_MASK       0x04
#define BIT_3_STATUS_MASK       0x08
#define BIT_4_STATUS_MASK       0x10
#define BIT_5_STATUS_MASK       0x20
#define BIT_6_STATUS_MASK       0x40
#define BIT_7_STATUS_MASK       0x80

#endif

/* Prototypes */
int init_lorawan_smartmodular_uart(void);
void send_tracking_data(float latitude, float longitude, bool ignition_status, bool gnss_fix_status);