#pragma once

#include "driver/gpio.h"
#include "esp_err.h"

enum ButtonState
{
	BUTTON_ERROR = -1,
	RELEASED, // кнопка відпущена
	PRESSED	  // кнопка натиснута
};

enum ButtonEvent
{
	NONE_EVENT,				// подія відсутня
	PRESSED_EVENT,			// момент натискання
	RELEASED_EVENT,			// момент відпускання
	CLICK_EVENT,			// коротке натискання
	LONG_PRESS_EVENT,		// довге утримання
	LONG_PRESS_REPEAT_EVENT // повтор довгого утримання
};

typedef struct
{
	gpio_num_t pin;			   // номер GPIO піна кнопки
	uint16_t debounceDelay;	   // час антидребезгу кнопки (мс)
	uint16_t longPressTime;	   // час довгого натискання (мс)
	uint16_t repeatInterval;   // інтервал повторення події (мс)
	bool lastPinState;		   // попередній  стан кнопки
	ButtonState pinState;	   // поточний стан кнопки
	ButtonEvent event;		   // подія кнопки
	uint32_t lastDebounceTime; // час останньої зміни стану
	uint32_t pressStartTime;   // час початку натискання кнопки
	uint32_t lastRepeatTime;   // час останнього повтору довгого натискання
	bool longPressActive;	   // прапорець довгого натискання
	bool initialized;

} deb;

// Debounce API
esp_err_t deb_btns_update(deb *btns, uint8_t btns_count);
ButtonEvent deb_get_btn_event(const deb *btn);
ButtonState deb_get_btn_status(const deb *btn);
