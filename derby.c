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
#define HORSE_TRACK_MICROSECONDS 100000 // 1/10 seconds

//give horses enough time to prepare for the race
#define HORSE_WAIT_TIME 1

pthread_mutex_t start_gate_mutex;
pthread_cond_t starting_pistol = PTHREAD_COND_INITIALIZER;

enum
{
	MOVE_FORWARD,
	MOVE_LEFT,
	MOVE_RIGHT
	
} HORSE_ACTIONS;

int get_random_action()
{
	return (rand() % 3);
}

void *race_horse(void *arguments){
	
	int index = *((int *) arguments);
	int action;
	pthread_mutex_lock(&start_gate_mutex);
	
	pthread_cond_wait(&starting_pistol, &start_gate_mutex); //wait for starting pistol
	pthread_mutex_unlock(&start_gate_mutex); // unlocking for all other threads
	
	printf("Horse %d starts\n", index);
	
	usleep(HORSE_TRACK_MICROSECONDS);
	action = get_random_action();
	
	switch (action)
	{
		case MOVE_FORWARD:
			printf("Horse %d: Forward.\n", index);
			break;
			
		case MOVE_LEFT:
			printf("Horse %d: Left.\n", index);
			break;
			
		case MOVE_RIGHT:
			printf("Horse %d: Right.\n", index);
			break;
	}
	
	printf("Horse %d: Ended.\n", index);
	
	//dont use pthreads on mac, but I code on a mac so this is so my IDE does not complain
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
