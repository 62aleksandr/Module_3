#pragma once

#include <cstdint>

constexpr int WINDOW_SIZE = 10;

typedef struct
{
	int y_prev;
	float alpha;
} ExpMovingAverage;

typedef struct
{
	int size;
	int buffer[WINDOW_SIZE];
	int index;
	int sum;
} MovingAvg;

void initEMAFilter(ExpMovingAverage *filter, float alpha);
void initMovingAverage(MovingAvg *filter, int size);

int exponentialMovingAverage(ExpMovingAverage *filter, int x);
int movingAverage(MovingAvg *filter, int x);
