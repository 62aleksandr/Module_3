#include "debounce.h"
#include "esp_timer.h"

esp_err_t deb_btns_update(deb *btns, uint8_t btns_count)
{
	if (btns == NULL || btns_count == 0)
	{
		return ESP_ERR_INVALID_ARG;
	}

	uint32_t now = esp_timer_get_time() / 1000;

	esp_err_t err = ESP_OK;

	for (uint8_t i = 0; i < btns_count; i++)
	{

		if (!btns[i].initialized)
		{
			err = ESP_ERR_INVALID_STATE;
			continue;
		}

		btns[i].event = NONE_EVENT;

		// bool currentPinState = digitalRead(btn.pin);
		bool currentPinState = gpio_get_level(btns[i].pin);

		// debounce trigger
		if (currentPinState != btns[i].lastPinState)
		{
			btns[i].lastDebounceTime = now;
		}

		btns[i].lastPinState = currentPinState;

		// stable state check
		if ((now - btns[i].lastDebounceTime) >= btns[i].debounceDelay)
		{
			ButtonState newPinState = (currentPinState == 0) ? PRESSED : RELEASED;

			// state changed
			if (newPinState != btns[i].pinState)
			{
				btns[i].pinState = newPinState;

				if (newPinState == PRESSED)
				{
					btns[i].pressStartTime = now;
					btns[i].longPressActive = false;
					btns[i].lastRepeatTime = 0;

					btns[i].event = PRESSED_EVENT;
					continue;
				}
				else
				{
					btns[i].event = (!btns[i].longPressActive) ? CLICK_EVENT : RELEASED_EVENT;
					continue;
				}
			}
		}

		// LONG PRESS
		if (btns[i].pinState == PRESSED)
		{
			uint32_t heldTime = now - btns[i].pressStartTime;

			// Перший long press
			if (!btns[i].longPressActive && heldTime >= btns[i].longPressTime)
			{
				btns[i].longPressActive = true;
				btns[i].lastRepeatTime = now;

				btns[i].event = LONG_PRESS_EVENT;
				continue;
			}

			// повторний long press
			if (btns[i].longPressActive &&
				(now - btns[i].lastRepeatTime >= btns[i].repeatInterval))
			{
				btns[i].lastRepeatTime = now;

				btns[i].event = LONG_PRESS_REPEAT_EVENT;
				continue;
			}
		}
	}
	return err;
}

ButtonEvent deb_get_btn_event(const deb *btn)
{
	if (btn == NULL)
		return NONE_EVENT;
	return btn->event;
}

ButtonState deb_get_btn_status(const deb *btn)
{
	if (btn == NULL)
		return BUTTON_ERROR;

	return btn->pinState;
}
