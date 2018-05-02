/*
CMPT 300_Project 3
Created by YUHAO HE 
*/
#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>
#include "bbuff.h"
#include "stats.h"

_Bool stop_thread = false;
// define candy type
typedef struct {
	// factory_number tracks which factory thread produced the candy item
	int factory_number;
	// time_stamp_in_ms tracks when the item was created
	double time_stamp_in_ms;
} candy_t;

double current_time_in_ms() {
	struct timespec now;
	clock_gettime(CLOCK_REALTIME, &now);
	return now.tv_sec * 1000.0 + now.tv_nsec / 1000000.0;
}

// define stats type
typedef struct {
	int FACTORY_NUMBER;
	int CANDY_MADE;
	int CANDY_EATEN;
	double MIN_DELAY;
	double AVG_DELAY;
	double MAX_DELAY;
	double TOT_DELAY;
} stat_t

// wait for requested time
void wait_requestTime(int NUM_SECONDS) {
	int i = 0;
	while(i < NUM_SECONDS) {
		printf("Time 	%ds\n", i);
		sleep(1);
		i ++;
	}
}

void stop_factory(int NUM_FACTORIES, pthread_t *factory_threads) {
	int i = 0;
	while(i < NUM_FACTORIES) {
		// join factory_thread
		pthread_join(factory_threads[i], NULL);
	}
}

void stop_kid(int NUM_KIDS, pthread_t *kid_threads) {
	int i = 0;
	while(i < NUM_KIDS) {
		// send cancellation request to the kid_thread
		pthread_cancel(kid_threads[i], NULL);
		// join kid_thread
		pthread_join(kid_threads[i], NULL);
	}
}

// the place where each factory_thread will produce candy item and put them into bounded_buffer
void *candy_factory(void *arg) {

	int wait_time;
	// factory_id
	int factory_num = *((int *)arg);
	while(!stop_thread) {
		// produce candy and put candy into bounded_buffer
		// generate a random wait_time ranging from 0 to 3
		wait_time = (rand() % (3 + 1 - 0) + 0);
		// print factory_thread message
		printf("\tFactory %d ships candy & waits %ds\n", factory_num, wait_time);
		// dynamically allocate a new candy item and populate its fields
		// initialize a candy
		candy_t *candy = malloc(sizeof(candy_t));
		candy -> factory_number = factory_num;
		candy -> time_stamp_in_ms = current_time_in_ms();
		// insert the new candy into bounded_buffer
		bbuff_blocking_insert(candy);
		// modify the corresponding stat_array
		stats_record_produced(factory_num);
		// free candy
		free(candy);
		// put factory_thread to sleep
		sleep(wait_time);
	}
	// the factory_thread is done
	printf("Candy-factory %d done\n", factory_num);
	pthread_exit(NULL);
}

// the place where kids consume candy item from the bounded_buffer
void *kid_consumer(void *arg) {

	int wait_time;
	int factory_num;
	double delay_in_ms;
	// kid_thread loops forever
	while(true) {
		// wait_time: either 0 or 1
		wait_time = (rand() % (1 + 1 - 0) + 0);
		// get the info for the candy to be consumed
		candy_t *candy = bbuff_blocking_extract();
		// get the id of which factory produce this candy
		factory_num = candy -> factory_number;
		// get the delay between when this candy is created and consumed
		delay_in_ms = current_time_in_ms() - (candy -> time_stamp_in_ms);
		// pass factory number and delay into stat to be printed
		stats_record_consumed(factory_num, delay_in_ms);
		// put kid into sleeping
		sleep(wait_time);
	}

}

int main(int argc, char *argv[]) {


	// argc indicates the number of parameters passed into program
	// check if the # of arguments is 3
	if(argc <= 3) {
		printf("Please pass all three parameters!\n");
		exit(1);
	}

	// extract all three parameters
	int NUM_FACTORIES = atoi(argv[1]);
	int NUM_KIDS = atoi(argv[2]);
	int NUM_SECONDS = atoi(argv[3]);

	// check all three parameters are greater than 0
	if(NUM_FACTORIES <= 0 || NUM_KIDS <= 0 || NUM_SECONDS <= 0) {
		printf("Invalid input. Please try again.\n");
		exit(1);
	}

	// initialize modules
	// initialize bounded_buffer
	bbuff_init();
	// initialize stats
	stats_init(NUM_FACTORIES);

	// launch factory threads
	pthread_t factory_threads[NUM_FACTORIES];
	int factory_id[NUM_FACTORIES];
	for(int i = 0; i < NUM_FACTORIES; i++) {
		// assign id to each factory thread
		factory_id[i] = i;
		// pthread_create(pthread_t *tid, pthread_attr_t *attr, start_routine(), void *arg)
		// create factory_thread to produce candy item
		pthread_create(&factory_threads[i], NULL, candy_factory, &factory_id[i]);
	}

	// lauch kids threads
	pthread_t kid_threads[NUM_KIDS];
	for(int j = 0; j < NUM_KIDS; j++) {
		// create kid_thread to consume candy item
		pthread_create(&kid_threads[j], NULL, kid_consumer, NULL)
	}

	// wait for requested time
	wait_requestTime(NUM_SECONDS);

	//print message: stop candy factories ...
	printf("Stopping candy factories...\n"); 
	// set stop_thread to true so factory_thread is done
	stop_thread = true;
	// after NUM_SECONDS
	// stop all the running factory threads
	stop_factory(NUM_FACTORIES, factory_threads);

	// stop producing candies
	// check if there are candies left in bounded_buffer
	// if there is, wait kids to consume them  all
	if(bbuff_is_empty() == false) {
		printf("Waiting for all candy to be consumed.\n");
		// sleep for 1s
		sleep(1);
	}

	// stop all the running kid threads
	printf("Stopping kids.\n");
	stop_kid(NUM_KIDS, kid_threads);

	// display statistics
	stats_display();
	// clean up stats
	stats_cleanup();
}