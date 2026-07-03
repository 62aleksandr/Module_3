#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "sdkconfig.h"

// Define the GPIO pin for the LED (GPIO 2 is common for onboard LEDs)
#define BLINK_GPIO 16

void app_main(void)
{
	// Configure the GPIO pin
	// gpio_reset_pin(BLINK_GPIO);
	// gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);

	gpio_config_t io_conf = {
		.pin_bit_mask = (1ULL << BLINK_GPIO),
		.mode = GPIO_MODE_OUTPUT,
		.pull_up_en = GPIO_PULLUP_DISABLE,
		.pull_down_en = GPIO_PULLDOWN_DISABLE,
		.intr_type = GPIO_INTR_DISABLE};

	gpio_config(&io_conf);

	// Blink loop
	while (1)
	{
		// Turn LED ON
		printf("LED ON\n");
		gpio_set_level(BLINK_GPIO, 1);
		vTaskDelay(1000 / portTICK_PERIOD_MS); // Delay 1 second

		// Turn LED OFF
		printf("LED OFF\n");
		gpio_set_level(BLINK_GPIO, 0);
		vTaskDelay(1000 / portTICK_PERIOD_MS); // Delay 1 second
	}
}
