#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>

#define NUMBER_OF_PHILOSOPHERS 5
void think(int);
void eat(int);
void philosopher(int);
void pickup(int);
void putdown(int);

extern pthread_mutex_t chopsticks[NUMBER_OF_PHILOSOPHERS];
extern pthread_t philosophers[NUMBER_OF_PHILOSOPHERS];
extern pthread_attr_t attributes[NUMBER_OF_PHILOSOPHERS];

#define LEFT(x) ((x) > 0 ? (x)-1 : NUMBER_OF_PHILOSOPHERS - 1)
#define RIGHT(x) (x)
