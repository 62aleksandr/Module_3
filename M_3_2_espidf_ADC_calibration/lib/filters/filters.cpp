#include "filters.h"

void initEMAFilter(ExpMovingAverage *filter, float alpha)
{
	filter->y_prev = 0;
	filter->alpha = alpha;
}

// Функція фільтрації експоненціальним середнім EMA
int exponentialMovingAverage(ExpMovingAverage *filter, int x)
{
	filter->y_prev = static_cast<int>(filter->alpha * x +
									  (1.0f - filter->alpha) * filter->y_prev);

	return filter->y_prev;
}

void initMovingAverage(MovingAvg *filter, int size)
{
	if (size > WINDOW_SIZE)
		size = WINDOW_SIZE;

	if (size < 1)
		size = 1;

	filter->size = size;
	filter->index = 0;
	filter->sum = 0;

	for (int i = 0; i < WINDOW_SIZE; i++)
		filter->buffer[i] = 0;
}

//  Функція фільтрації ковзним середнім MA
int movingAverage(MovingAvg *filter, int x)
{

	// відняти старе значення з суми
	filter->sum -= filter->buffer[filter->index];

	// записати нове значення в буфер
	filter->buffer[filter->index] = x;

	// додати нове значення до суми
	filter->sum += x;

	// перейти до наступного елемента циклічно
	filter->index++;

	if (filter->index >= filter->size)
		filter->index = 0;

	// середнє значення
	return static_cast<int>(filter->sum / filter->size);
}
