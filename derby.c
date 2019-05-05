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
#define HORSE_COUNTDOWN_TIME 3
#define HORSE_TRACK_MICROSECONDS 1000000

//give horses enough time to prepare for the race
#define HORSE_WAIT_TIME 1

pthread_mutex_t start_gate_mutex;
pthread_cond_t starting_pistol = PTHREAD_COND_INITIALIZER;

void *race_horse(void *arguments){
	
	int index = *((int *) arguments);
	pthread_mutex_lock(&start_gate_mutex);
	
	pthread_cond_wait(&starting_pistol, &start_gate_mutex); //wait for starting pistol
	pthread_mutex_unlock(&start_gate_mutex); // unlocking for all other threads
	
	printf("Horse %d: has begun.\n", index);
	
	usleep(HORSE_TRACK_MICROSECONDS); //
	
	printf("Horse %d: Ended.\n", index);
	
	//Which C standard do you use?
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
		assert(!ret && "Horse injured during race, event canceled.");
	}
	
	sleep(HORSE_WAIT_TIME);
	
	//countdown sequence, formatted to match weird rubric
	i = HORSE_COUNTDOWN_TIME;
	printf("%d", i);
	while(i > 1)
	{
		sleep(1);
		i--;
		
		printf(", %d", i);
	}
	
	printf(" .. GO!\n");
	pthread_cond_broadcast(&starting_pistol);
	
	for (i = 0; i < NUM_TRACKS; i++) {
		ret = pthread_join(horse[i], NULL);
		assert(!ret);
	}
	
	sleep(HORSE_WAIT_TIME);
	
	printf("The race finishes!\n");
	return 0;
}
