#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
#include <stdbool.h>
#include "bbuff.h"

// define semaphores full, empty
sem_t full;
sem_t empty;

// define mutex lock
pthread_mutex_t lock;
int read_count;

// define bounded buffer
// note: BUFFER_SIZE is defined as 10 in bbuff.h
void* bounded_buffer[BUFFER_SIZE];

// initialize bounded buffer
void bbuff_init(void) {
	// initialize semaphore empty and full
	// sem_init(full, whether the semaphore is shared by all threads, initial value of semaphore)
	sem_init(&full, 0, 0);
	sem_init(&empty, 0, BUFFER_SIZE);
	read_count = 0;
}

// produce candy to bounded_buffer
void bbuff_blocking_insert(void* item) {
	// wait empty
	// note: sem_wait() decrement the semaphore
	// if S -> value < 0, the current thread will be put into waiting list
	sem_wait(&empty);
	// lock the mutex lock
	// if mutex lock is not freed, the calling thread blocks until the lock is freed
	pthread_mutex_lock(&lock);

	// critical section
	// insert candy into bounded_buffer
	bounded_buffer[read_count] = item;
	read_count ++;

	// free lock
	pthread_mutex_unlock(&lock);
	// semaphore full increments, means one more candy produced 
	sem_signal(&full);
}

// consume candy from boudned_buffer
// return the candy consumed by kids
void* bbuff_blocking_extract(void) {
	// wait full
	sem_wait(&full);
	// gain lock
	pthread_mutex_lock(&lock);

	// critical section
	// comsume candy from bounded_buffer
	read_count --;
	void *candy = bounded_buffer[read_count];

	// set lock free
	pthread_mutex_unlock(&lock);
	sem_signal(&empty);

	return candy;
}

// check if bounded_buffer is empty 
_Bool bbuff_is_empty(void) {
	if(read_count == 0) {
		return true;
	}
	else {
		return false;
	}
}