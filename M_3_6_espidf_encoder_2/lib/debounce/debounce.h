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
	// 1. Конфігураційні параметри (Заповнює користувач)
	gpio_num_t pin;			 // Номер GPIO піна кнопки
	uint16_t debounceDelay;	 // Час антидребезгу кнопки (мс)
	uint16_t longPressTime;	 // Час довгого натискання (мс)
	uint16_t repeatInterval; // Інтервал повторення події (мс)

	// 2. Внутрішній стан (Керується алгоритмом, групуємо по 32 біта
	uint32_t lastDebounceTime; // Час останньої зміни стану
	uint32_t pressStartTime;   // Час початку натискання кнопки
	uint32_t lastRepeatTime;   // Час останнього повтору довгого натискання

	ButtonState pinState; // Стабільний поточний логічний стан кнопки
	ButtonEvent event;	  // Поточна подія кнопки
	int lastPinState;	  // тип int для сирого рівня з gpio_get_level()

	bool longPressActive; // Прапорець довгого натискання
	bool initialized;	  // Прапорець успішної ініціалізації модуля

} deb;

// Debounce API
esp_err_t deb_btn_init(deb *btn);
esp_err_t deb_btns_update(deb *btns, uint8_t btns_count);
ButtonEvent deb_get_btn_event(deb *btn);
ButtonState deb_get_btn_status(const deb *btn);