#include "cy_retarget_io.h"
#include "cybsp.h"
#include "cyhal.h"
#include <stdlib.h>
#include <string.h>

#define BUF_SIZE 128

#define PWM_DUTY_CYCLE_ON (0.0f)
#define PWM_DUTY_CYCLE_OFF (100.0f)
#define PWM_ON_OFF_FREQUENCY (2u)

uint8_t buffer[BUF_SIZE];
uint8_t idx;

uint8_t handled_flag;
uint8_t blink_flag;

uint32_t ticks;
uint32_t last_toggle;
uint32_t fq;

float duty_cycle;

cyhal_pwm_t pwm_led_control;

void reset_buffer(void) {
  memset(buffer, 0, BUF_SIZE);
  idx = 0;
}

void handle_error(cy_rslt_t status) {
  if (CY_RSLT_SUCCESS != status) {

    __disable_irq();

    CY_ASSERT(0);
  }
}

void handle_blink() {
  if (blink_flag) {

    if (last_toggle + fq < ticks) {
      last_toggle = ticks;

      duty_cycle = (duty_cycle == 100) ? 0 : 100;

      cyhal_pwm_set_duty_cycle(&pwm_led_control, duty_cycle, 8);
    }
  }
}

void send_uart_if_pressed(void) {

  if (cyhal_gpio_read(CYBSP_SW2) == 0) {
    fq = 500;
    blink_flag = 1;

    cyhal_uart_putc(&cy_retarget_io_uart_obj, 's');
    cyhal_uart_putc(&cy_retarget_io_uart_obj, 'w');
    cyhal_uart_putc(&cy_retarget_io_uart_obj, '2');
    cyhal_uart_putc(&cy_retarget_io_uart_obj, '\n');
  }
}

void handle_buffer(void) {
  if (handled_flag == 0)
    return;
  handled_flag = 0;

  if (idx > 0) {
    char *ch;

    ch = strstr((const char *)buffer, "On");
    if (ch) {
      blink_flag = 0;

      cyhal_pwm_set_duty_cycle(&pwm_led_control, PWM_DUTY_CYCLE_ON,
                               PWM_ON_OFF_FREQUENCY);

      reset_buffer();
      return;
    }

    ch = strstr((const char *)buffer, "Off");
    if (ch) {
      blink_flag = 0;
      cyhal_pwm_set_duty_cycle(&pwm_led_control, PWM_DUTY_CYCLE_OFF,
                               PWM_ON_OFF_FREQUENCY);
      reset_buffer();
      return;
    }

    ch = strstr((const char *)buffer, "Blink");
    if (ch) {
      cyhal_uart_putc(&cy_retarget_io_uart_obj, fq);
      blink_flag = 1;
      reset_buffer();
      return;
    }

    ch = strstr((const char *)buffer, "FREQ");
    if (ch) {
      blink_flag = 1;

      char *z_position =
          strchr(ch, 'Z'); // Find the position of the character 'Z'

      size_t freq_length =
          z_position - (ch + 4); // Length of the frequency (from FREQ to Z)

      if (freq_length >= 3 && freq_length <= 4) {
        char freq_str[5];

        strncpy(freq_str, ch + 4,
                freq_length);         // Copy the frequency from the buffer
        freq_str[freq_length] = '\0'; // Add the null terminator

        fq = atoi(freq_str); // Use atoi to convert the string to an integer

        reset_buffer();
      }

      return;
    }

    ch = strstr((const char *)buffer, "BRIGHT");
    if (ch) {
      blink_flag = 0;
      char *z_position = strchr(ch, 'Z');

      size_t bright_length = z_position - (ch + 7);

      if (bright_length >= 2 && bright_length <= 4) {
        char bright_str[6] = {0};
        strncpy(bright_str, ch + 6, bright_length);
        bright_str[bright_length] = '\0';

        duty_cycle = atof(bright_str);

        cyhal_pwm_set_duty_cycle(&pwm_led_control, duty_cycle, 50);
        reset_buffer();
      }

      return;
    }
  }
}

void handle_uart(void) {
  uint8_t read_data;

  if (CY_RSLT_SUCCESS ==
      cyhal_uart_getc(&cy_retarget_io_uart_obj, &read_data, 1)) {
    buffer[idx] = read_data;
    idx++;
    handled_flag = 1;
  }
}

int main(void) {
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

  result = cy_retarget_io_init_fc(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX,
                                  CYBSP_DEBUG_UART_CTS, CYBSP_DEBUG_UART_RTS,
                                  CY_RETARGET_IO_BAUDRATE);

  handle_error(result);

  result = cyhal_pwm_init(&pwm_led_control, CYBSP_USER_LED, NULL);
  handle_error(result);

  cyhal_pwm_set_duty_cycle(&pwm_led_control, PWM_DUTY_CYCLE_OFF,
                           PWM_ON_OFF_FREQUENCY);
  cyhal_pwm_start(&pwm_led_control);

  result = cyhal_gpio_init(CYBSP_SW2, CYHAL_GPIO_DIR_INPUT,
                           CYHAL_GPIO_DRIVE_PULLUP, CYBSP_BTN_OFF);
  handle_error(result);

  memset(buffer, 0, BUF_SIZE);

  fq = 400;

  __enable_irq();

  for (;;) {
    handle_uart();
    handle_buffer();
    handle_blink();
    send_uart_if_pressed();
    ticks++;
  }
}

/* [] END OF FILE */