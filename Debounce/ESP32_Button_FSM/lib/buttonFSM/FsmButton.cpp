#include "FsmButton.h"

// Конструктор класу
FsmButton::FsmButton(uint8_t pin, uint32_t debounceDelayMs, uint32_t longPressDelayMs, bool activeLow)
{
	_pin = pin;							// Номер фізичного контакту (GPIO)
	_debounceDelay = debounceDelayMs;	// час фільтрації (в мілісекундах) для ігнорування брязкіту
	_longPressDelay = longPressDelayMs; // Час (в мілісекундах) для утримання кнопки
	_activeLow = activeLow;				// true — INPUT_PULLUP

	_currentState = STATE_STABLE_RELEASED;
	_stateTimer = 0;	  //  відліку часу стабілізації
	_pressStartTimer = 0; // фіксує момент натискання

	_wasPressedEvent = false;	  // Подія натискання відсутня
	_wasReleasedEvent = false;	  // Подія відпускання відсутня
	_longPressTriggered = false;  // Довге натискання ще не зафіксовано
	_wasLongPressedEvent = false; // Подія довгого натискання відсутня
}

// Приведення фізичного сигналу до логічного
bool FsmButton::getRawTargetState(bool rawInput) const
{
	return _activeLow ? (rawInput == LOW) : (rawInput == HIGH);
}

// Налаштування піна мікроконтролера
void FsmButton::begin()
{
	if (_activeLow)
	{
		pinMode(_pin, INPUT_PULLUP);
	}
	else
	{
		pinMode(_pin, INPUT_PULLDOWN);
	}

	if (getRawTargetState(digitalRead(_pin)))
	{
		_currentState = STATE_STABLE_PRESSED;
	}
	else
	{
		_currentState = STATE_STABLE_RELEASED;
	}
}

// Ядро обробки автомата
void FsmButton::update()
{
	bool rawInput = digitalRead(_pin);
	bool isTargetPressed = getRawTargetState(rawInput);
	uint32_t now = millis();

	_wasPressedEvent = false;
	_wasReleasedEvent = false;
	_wasLongPressedEvent = false;

	switch (_currentState)
	{
	case STATE_STABLE_RELEASED:
		if (isTargetPressed)
		{
			_stateTimer = now;
			_currentState = STATE_DEBOUNCE_PRESS;
		}
		break;

	case STATE_DEBOUNCE_PRESS:
		if (!isTargetPressed)
		{
			_currentState = STATE_STABLE_RELEASED;
		}
		else if (now - _stateTimer >= _debounceDelay)
		{
			_currentState = STATE_STABLE_PRESSED;
			_pressStartTimer = now;
			_longPressTriggered = false;
			_wasPressedEvent = true;
		}
		break;

	case STATE_STABLE_PRESSED:
		if (!_longPressTriggered && (now - _pressStartTimer >= _longPressDelay))
		{
			_wasLongPressedEvent = true;
			_longPressTriggered = true;
		}

		if (!isTargetPressed)
		{
			_stateTimer = now;
			_currentState = STATE_DEBOUNCE_RELEASE;
		}
		break;

	case STATE_DEBOUNCE_RELEASE:
		if (isTargetPressed)
		{
			_currentState = STATE_STABLE_PRESSED;
		}
		else if (now - _stateTimer >= _debounceDelay)
		{
			_currentState = STATE_STABLE_RELEASED;
			_wasReleasedEvent = true;
		}
		break;
	}
}
// Реалізація публічних методів
//  Перевіряє, чи кнопка зараз вважається натиснутою (стабільний або debounce-стан)
bool FsmButton::isPressed() const
{
	return (_currentState == STATE_STABLE_PRESSED || _currentState == STATE_DEBOUNCE_RELEASE);
}
// Повертає подію: зафіксовано натискання кнопки
bool FsmButton::wasPressed() const { return _wasPressedEvent; }
// Повертає подію: кнопка відпущена
bool FsmButton::wasReleased() const { return _wasReleasedEvent; }
// Повертає подію: зафіксовано довге натискання кнопки
bool FsmButton::wasLongPressed() const { return _wasLongPressedEvent; }
