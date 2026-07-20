// Метод опитування кнопки (Polling) Time-Based Debounce
// https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/api-reference/peripherals/gpio.html?utm_source=chatgpt.com
// https://github.com/espressif/esp-idf/tree/master/examples/peripherals/gpio/generic_gpio?utm_source=chatgpt.com

#include "debounce.h"
#include "esp_timer.h"

void deb_init(deb *btn)
{
	if (btn == NULL)
	{
		return;
	}

	// 2. Внутрішній стан
	btn->lastPinState = gpio_get_level(btn->pin);
	btn->pinState =
		(btn->lastPinState == btn->activeLevel) ? PRESSED : RELEASED;
	btn->event = NONE_EVENT;
	btn->lastDebounceTime = 0;
	btn->pressStartTime = 0;
	btn->lastRepeatTime = 0;
	btn->stateChanged = false;
	btn->longPressActive = true;
	btn->initialized = true;
}

esp_err_t deb_update_state(deb *btns, uint8_t btns_count)
{
	if (btns == NULL || btns_count == 0)
	{
		return ESP_ERR_INVALID_ARG;
	}

	// Отримуємо час один раз на початку для всього масиву
	uint64_t now = (uint64_t)esp_timer_get_time() / 1000ULL;

	esp_err_t err = ESP_OK;

	for (uint8_t i = 0; i < btns_count; i++)
	{
		if (!btns[i].initialized)
		{
			err = ESP_ERR_INVALID_STATE;
			continue;
		}

		btns[i].stateChanged = false;

		// bool currentPinState = digitalRead(btn.pin);
		int currentPinState = gpio_get_level(btns[i].pin);

		// debounce trigger
		if (currentPinState != btns[i].lastPinState)
		{
			btns[i].lastDebounceTime = now;
		}

		btns[i].lastPinState = currentPinState;

		// stable state check
		if ((now - btns[i].lastDebounceTime) >= btns[i].debounceDelay)
		{
			ButtonState newPinState = (currentPinState == btns[i].activeLevel) ? PRESSED : RELEASED;

			// state changed
			if (newPinState != btns[i].pinState)
			{
				btns[i].stateChanged = true;
				btns[i].pinState = newPinState;

				if (newPinState == PRESSED)
				{
					btns[i].pressStartTime = now;
					btns[i].longPressActive = false;
					btns[i].lastRepeatTime = 0;
				}
				continue;
			}
		}
	}
	return err;
}

esp_err_t deb_process_events(deb *btns, uint8_t btns_count)
{
	if (btns == NULL || btns_count == 0)
	{
		return ESP_ERR_INVALID_ARG;
	}

	uint64_t now = (uint64_t)esp_timer_get_time() / 1000ULL;
	esp_err_t err = ESP_OK;

	for (uint8_t i = 0; i < btns_count; i++)
	{
		if (!btns[i].initialized)
		{
			err = ESP_ERR_INVALID_STATE;
			continue;
		}

		// За замовчуванням кожну ітерацію подія відсутня
		btns[i].event = NONE_EVENT;

		// Перевірка зміни стану кнопки
		if (btns[i].stateChanged)
		{
			if (btns[i].pinState == PRESSED)
			{
				btns[i].event = PRESSED_EVENT;
			}
			else
			{
				btns[i].event = btns[i].longPressActive ? RELEASED_EVENT : CLICK_EVENT;
			}

			continue;
		}

		// LONG PRESS
		if (btns[i].pinState == PRESSED)
		{
			// Перевірка 1: Перше тривале затискання кнопки
			if (!btns[i].longPressActive)
			{
				uint64_t heldTime = now - btns[i].pressStartTime;

				if (heldTime >= btns[i].longPressTime)
				{
					btns[i].longPressActive = true;
					btns[i].lastRepeatTime = now;
					btns[i].event = LONG_PRESS_EVENT;
				}
			}

			// Перевірка 2: Повторення події (Auto-repeat)
			else
			{
				if (now - btns[i].lastRepeatTime >= btns[i].repeatInterval)
				{
					btns[i].lastRepeatTime = now;
					btns[i].event = LONG_PRESS_REPEAT_EVENT;
				}
			}
		}
	}

	return err;
}

ButtonEvent deb_get_btn_event(deb *btn)
{
	if (btn == NULL || !btn->initialized)
	{
		return NONE_EVENT;
	}

	ButtonEvent current_event = btn->event;
	btn->event = NONE_EVENT;

	return current_event;
}

ButtonState deb_get_btn_status(const deb *btn)
{
	if (btn == NULL || !btn->initialized)
	{
		return BUTTON_ERROR;
	}

	return btn->pinState;
}
