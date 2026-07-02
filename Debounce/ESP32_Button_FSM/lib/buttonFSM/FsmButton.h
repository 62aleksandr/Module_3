#ifndef FSM_BUTTON_H
#define FSM_BUTTON_H

#include <Arduino.h>

class FsmButton
{
private:
	// Стани скінченного автомата (FSM)
	enum State
	{
		STATE_STABLE_RELEASED,
		STATE_DEBOUNCE_PRESS,
		STATE_STABLE_PRESSED,
		STATE_DEBOUNCE_RELEASE
	};

	uint8_t _pin;
	uint32_t _debounceDelay;
	uint32_t _longPressDelay;

	uint32_t _stateTimer;
	uint32_t _pressStartTimer;

	State _currentState;
	bool _activeLow;

	bool _wasPressedEvent;
	bool _wasReleasedEvent;
	bool _longPressTriggered;
	bool _wasLongPressedEvent;

	// Метод приведення фізичного сигналу до логічного
	bool getRawTargetState(bool rawInput) const;

public:
	// Конструктор
	FsmButton(uint8_t pin, uint32_t debounceDelayMs = 30, uint32_t longPressDelayMs = 1000, bool activeLow = true);

	// Ініціалізація аппаратного рівня
	void begin();

	// Головний неблокуючий движок FSM
	void update();

	// Публічний інтерфейс (API)
	bool isPressed() const;
	bool wasPressed() const;
	bool wasReleased() const;
	bool wasLongPressed() const;
};

#endif // FSM_BUTTON_H