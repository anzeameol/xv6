#include "philosopher.h"

pthread_mutex_t chopsticks[NUMBER_OF_PHILOSOPHERS];
pthread_t philosophers[NUMBER_OF_PHILOSOPHERS];
pthread_attr_t attributes[NUMBER_OF_PHILOSOPHERS];

void philosopher(int philosopherNumber)
{
	while (1)
	{
		int i = (int)philosopherNumber;
		think(philosopherNumber);
		// TODO
		pickup(philosopherNumber);
		// 实现pickUp()函数
		eat(philosopherNumber);
		// TODO
		putdown(philosopherNumber);
		// 实现putDown()函数
	}
}

void think(int philosopherNumber)
{
	int sleepTime = rand() % 3 + 1;
	printf("Philosopher %d will think for %d seconds\n", philosopherNumber, sleepTime);
	sleep(sleepTime);
}

void eat(int philosopherNumber)
{
	int eatTime = rand() % 3 + 1;
	printf("Philosopher %d will eat for %d seconds\n", philosopherNumber, eatTime);
	sleep(eatTime);
}

void pickup(int philosopherNumber)
{
	if (philosopherNumber > 0)
	{
		pthread_mutex_lock(&chopsticks[LEFT(philosopherNumber)]);
		pthread_mutex_lock(&chopsticks[RIGHT(philosopherNumber)]);
	}
	else
	{
		pthread_mutex_lock(&chopsticks[RIGHT(philosopherNumber)]);
		pthread_mutex_lock(&chopsticks[LEFT(philosopherNumber)]);
	}
}

void putdown(int philosopherNumber)
{
	pthread_mutex_unlock(&chopsticks[LEFT(philosopherNumber)]);
	pthread_mutex_unlock(&chopsticks[RIGHT(philosopherNumber)]);
}