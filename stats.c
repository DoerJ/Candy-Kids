#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include "stats.h"

// define mutex used for modifying stats
sem_t mutex;
int factory_amount;
/*
typedef struct {
	int FACTORY_NUMBER;
	int CANDY_MADE;
	int CANDY_EATEN;
	double MIN_DELAY;
	double AVG_DELAY;
	double MAX_DELAY;
	double TOT_DELAY;
} stat_t
*/

void stats_init(int num_producers) {
	// initialize semaphore mutex
	// the initial value of semaphore is 1 
	// critical section needs to be protected, since there are
	// cases where kids consume candies which are produced from the same factory
	sem_init(&mutex, 0, 1);
	// initialize factory_amount for stats purpose
	factory_amount = num_producers;
	// initialize an array of stat_t
	stat_t *stat_array = (stat_t *)malloc(num_producers * sizeof(stat_t));
	// initialize all elements of stat_t
	for(int i = 0; i < num_producers; i++) {
		stat_array[i] -> FACTORY_NUMBER = i;
		stat_array[i] -> CANDY_MADE = 0;
		stat_array[i] -> CANDY_EATEN = 0;
		stat_array[i] -> MIN_DELAY = 0;
		stat_array[i] -> AVG_DELAY = 0;
		stat_array[i] -> MAX_DELAY = 0;
		// tot_delay is used for calculating the average delay
		stat_array[i] -> TOT_DELAY = 0;
	}
}

void stats_cleanup(void) {
	// free stat_array
	free(stat_array);
}
// count the number of candies each factory creates
void stats_record_produced(int factory_number) {
	// protect the critical section
	sem_wait(&mutex);
	atat_array[factory_number] -> CANDY_MADE ++;
	sem_signal(&mutex);
}
// count the number of candies that were consumed from each factory
void stats_record_consumed(int factory_number, double delay_in_ms) {
	// protext the critical section
	sem_wait(&mutex);
	// number of candy consumed increments
	stat_array[factory_number] -> CANDY_EATEN ++;
	// check if min_delay and max_delay have been defined
	if(stat_array[factory_number] -> MIN_DELAY == 0 && stat_array[factory_number] -> MAX_DELAY == 0) {
		stat_array[factory_number] -> MIN_DELAY = delay_in_ms;
		stat_array[factory_number] -> MAX_DELAY = delay_in_ms;
		stat_array[factory_number] -> TOT_DELAY = delay_in_ms;
	}
	// check if min_delay is the min and max_delay is the max
	else{
		// check if min_delay has the min delay
		if(delay_in_ms < stat_array[factory_number] -> MIN_DELAY) {
			stat_array[factory_number] -> MIN_DELAY = delay_in_ms;
		}
		// check if max_delay has the max delay
		if(delay_in_ms > stat_array[factory_number] -> MAX_DELAY) {
			stat_array[factory_number] -> MAX_DELAY = delay_in_ms;
		}
		// add onto total_delay
		stat_array[factory_number] -> TOT_DELAY += delay_in_ms;
		// get the current average delay
		stat_array[factory_number] -> AVG_DELAY = (stat_array[factory_number] -> TOT_DELAY) / (stat_array[factory_number] -> CANDY_EATEN);
	}
	sem_signal(&mutex);
}
// print out stats
void stats_display(void) {
	printf("Statistics: \n");
	// print titles
	printf("%8s %8s %8s %15s %15s %15s\n", "Factory#", "#Made", "#Eaten", "Min Delay[ms]", "Avg Delay[ms]", "Max Delay[ms]");
	// print out stat info for each stat_array[i]\
	for(int i = 0; i < factory_amount; i++) {
		// check if candy made mismatches with candy eaten
		if(stat_array[i] -> CANDY_MADE != stat_array[i] -> CANDY_EATEN) {
			printf("ERROR: Mismatch between number made and eaten.\n");
		}
		else {
		printf("%8d %8d %8d %10.5f %10.5f %10.5f\n", i, stat_array[i] -> CANDY_MADE, stat_array[i] -> CANDY_EATEN, 
			stat_array[i] -> MIN_DELAY, stat_array[i] -> AVG_DELAY, stat_array[i] -> MAX_DELAY);
		}
	}

	printf("\n");
}
