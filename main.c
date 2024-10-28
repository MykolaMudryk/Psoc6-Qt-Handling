#include "cyhal.h"
#include "cybsp.h"
#include "cy_retarget_io.h"
#include <string.h>

#define BUF_SIZE 128

uint8_t buffer[BUF_SIZE];
uint8_t idx;

uint8_t handled_flag;
uint8_t blink_flag;

uint32_t ticks;
uint32_t last_toggle;

uint32_t fq;

void reset_buffer(void)
{
	memset(buffer, 0, BUF_SIZE);
	idx = 0;
}

void handle_error(cy_rslt_t status)
{
    if (CY_RSLT_SUCCESS != status)
    {

    __disable_irq();

    CY_ASSERT(0);

    }
}

void handle_led()
{
	if (blink_flag)
	{
		if (last_toggle + fq < ticks)
		{
			last_toggle = ticks;
			cyhal_gpio_toggle(CYBSP_USER_LED);
		}
	}
}

void handle_buffer(void)
{
	if (handled_flag == 0) return;
	handled_flag = 0;
	cyhal_uart_putc(&cy_retarget_io_uart_obj, '\r');
	cyhal_uart_putc(&cy_retarget_io_uart_obj, '\n');

	if (idx > 0)
	{
		uint8_t * ch;

		ch = strstr(buffer, "On");
		if (ch)
		{
			blink_flag = 0;
			cyhal_gpio_write(CYBSP_USER_LED, CYBSP_LED_STATE_ON);
		    reset_buffer();
		    return;
		}

		ch = strstr(buffer, "Off");
		if (ch)
		{
			blink_flag = 0;
			cyhal_gpio_write(CYBSP_USER_LED, CYBSP_LED_STATE_OFF);
			reset_buffer();
			return;
		}

		ch = strstr(buffer, "Blink");
		if (ch)
		{
			blink_flag = 1;
			return;
		}

		ch = strstr(buffer, "FREQ");
		if (ch)
		{
			blink_flag = 1;

			if (*(ch + 4 + 4) == 'Z')
			{
				fq = (*(ch + 4 + 0) - 0x30) * 1000 + (*(ch + 4 + 1) - 0x30) * 100 + (*(ch + 4 + 2) - 0x30) * 10 + (*(ch + 4 + 3) - 0x30) * 1;
				reset_buffer();
			}

			return;
		}
	}
}

void handle_uart(void)
{
	uint8_t read_data;

	if (CY_RSLT_SUCCESS == cyhal_uart_getc(&cy_retarget_io_uart_obj, &read_data, 1))
	{
		buffer[idx] = read_data;
		idx++;
		handled_flag = 1;
	}
}

int main(void)
{
    cy_rslt_t result;
    #if defined(CY_DEVICE_SECURE)
        cyhal_wdt_t wdt_obj;
        /* Clear watchdog timer so that it doesn't trigger a reset */
        result = cyhal_wdt_init(&wdt_obj, cyhal_wdt_get_max_timeout_ms());
        CY_ASSERT(CY_RSLT_SUCCESS == result);
        cyhal_wdt_free(&wdt_obj);
    #endif


    /* Initialize the device and board peripherals */
    result = cybsp_init();

    handle_error(result);

    /* Initialize retarget-io to use the debug UART port */

    result = cy_retarget_io_init_fc(CYBSP_DEBUG_UART_TX,
                                    CYBSP_DEBUG_UART_RX,
                                    CYBSP_DEBUG_UART_CTS,
                                    CYBSP_DEBUG_UART_RTS,
                                    CY_RETARGET_IO_BAUDRATE);

	handle_error(result);

    result = cyhal_gpio_init(CYBSP_USER_LED, CYHAL_GPIO_DIR_OUTPUT,
                             CYHAL_GPIO_DRIVE_STRONG, CYBSP_LED_STATE_OFF);

    memset(buffer, 0, BUF_SIZE);
    fq = 500;

    /* \x1b[2J\x1b[;H - ANSI ESC sequence for clear screen */
//    printf("\x1b[2J\x1b[;H");
//
//    printf("***********************************************************\r\n");
//    printf("HAL: UART Transmit and Receive\r\n");
//    printf("***********************************************************\r\n\n");
//    printf(">> Start typing to see the echo on the screen \r\n\n");

    __enable_irq();

    for (;;)
    {
        handle_uart();
		handle_buffer();
		handle_led();
		ticks++;
    }
}

/* [] END OF FILE */
