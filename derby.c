/*
 Emmanuel Utomi
 eutomi2
 655390858
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_TRACKS 10

//give horses enough time to prepare for the race
#define HORSE_WAIT_TIME 1

pthread_mutex_t start_gate_mutex;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void *race_horse(void *arguments){
	
	int index = *((int *) arguments);
	pthread_mutex_lock(&start_gate_mutex);
	
	printf("Horse %d: Ready.\n", index);
	
	pthread_cond_wait(&cond, &start_gate_mutex);
	pthread_mutex_unlock(&start_gate_mutex); // unlocking for all other threads
	
	printf("Horse %d: has begun.\n", index);
	
	sleep(2);
	
	printf("Horse %d: Ended.\n", index);
	
	
	pthread_exit(NULL);
}

int main(void) {
	pthread_t horse[NUM_TRACKS];
	int thread_id[NUM_TRACKS];
	int i;
	int ret;
	
	
	for (i = 0; i < NUM_TRACKS; i++) {
		thread_id[i] = i;
		ret = pthread_create(&horse[i], NULL, race_horse, &thread_id[i]);
		assert(!ret);
	}
	
	sleep(HORSE_WAIT_TIME);
	printf("MAIN: All threads are created.\n");
	printf("Starting in ...\n");
	
	for(i = 3; i > 0; i--)
	{
		printf("%d seconds\n", i);
		sleep(1);
	}
	
	printf("*starting pistol fired* \n");
	pthread_cond_broadcast(&cond);
	
	for (i = 0; i < NUM_TRACKS; i++) {
		ret = pthread_join(horse[i], NULL);
		assert(!ret);
		//printf("MAIN: Thread %d has ended.\n", i);
	}
	
	sleep(HORSE_WAIT_TIME);
	
	printf("Race has ended. \n");
	return 0;
}
