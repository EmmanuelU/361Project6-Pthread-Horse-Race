/*
 Emmanuel Utomi
 eutomi2
 655390858
 */

#define _XOPEN_SOURCE 600 // Required to use the barriers

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_TRACKS 10
#define NUM_LAPS 1
#define TRACK_DISTANCE 20
#define HORSE_COUNTDOWN_TIME 3
#define HORSE_TRACK_MICROSECONDS 100000 // 0.1 seconds
#define HORSE_WAIT_TIME 2 //give horses enough time to prepare for the race

pthread_mutex_t start_gate_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t starting_pistol = PTHREAD_COND_INITIALIZER;

pthread_mutex_t track_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t track_position_busy = PTHREAD_COND_INITIALIZER;

pthread_barrier_t barr;

int track_position[NUM_TRACKS][TRACK_DISTANCE];

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

int get_action_distance(int action, int forward)
{
	switch (action)
	{
		case MOVE_FORWARD:
			return forward ? 2 : 1;
			
		case MOVE_LEFT:
			return forward ? 1 : -1;
			
		case MOVE_RIGHT:
			return 1;
			
		default:
			return 0;
	}
}

void *race_horse(void *arguments){
	
	int index = *((int *) arguments);
	int action;
	int laneX, positionY, lap;
	
	laneX = index;
	positionY = 0;
	lap = 0;
	
	
	pthread_mutex_lock(&start_gate_mutex);
	pthread_cond_wait(&starting_pistol, &start_gate_mutex); //wait for starting pistol
	pthread_mutex_unlock(&start_gate_mutex); // unlock and pass to next thread to unlock
	
	
	printf("Horse %d starts\n", index);
	pthread_barrier_wait(&barr);
	
	usleep(HORSE_TRACK_MICROSECONDS);
	
	do
	{
		action = get_random_action();
		
		if(laneX > 0 && laneX < NUM_TRACKS) //stay within track bounds
			laneX += get_action_distance(action, 0);
		
		positionY += get_action_distance(action, 1);
		
		if(positionY >= TRACK_DISTANCE)
		{
			lap++;
			positionY = 0;
		}
		
		pthread_mutex_lock(&track_mutex);
		
		if(track_position[laneX][positionY])
		{
			if(laneX > 0 && laneX < NUM_TRACKS)
				laneX -= get_action_distance(action, 0);
			
			pthread_cond_wait(&track_position_busy, &track_mutex);
		}
			
		
		printf("Horse: %d, Lane: %d, Position: %d, Lap: %d\n", index, laneX, positionY, lap);
		track_position[laneX][positionY] = 1;
		pthread_mutex_unlock(&track_mutex);
		
		usleep(HORSE_TRACK_MICROSECONDS);
		
		pthread_mutex_lock(&track_mutex);
		track_position[laneX][positionY] = 0;
		pthread_cond_signal(&track_position_busy);
		pthread_mutex_unlock(&track_mutex);
		
	} while (lap < NUM_LAPS);
	
	printf("Horse %d: Ended.\n", index); //horse reached the end of the track
	
	//dont use pthreads on mac, but I code on a mac so this is so my IDE does not complain
	//pthread_exit(NULL);
}

int main(void) {
	pthread_t horse[NUM_TRACKS];
	int thread_id[NUM_TRACKS];
	int i, j;
	int ret;
	
	pthread_mutex_init(&track_mutex,NULL); //initiliaze the pthread mutex
	pthread_mutex_init(&start_gate_mutex,NULL); //initiliaze the pthread mutex
	
	pthread_barrier_init(&barr, NULL, NUM_TRACKS);
	
	for (i = 0; i < NUM_TRACKS; i++) {
		for(j = 0; j < TRACK_DISTANCE; j++) { //prepare track for race
			track_position[i][j] = 0;
		}
		thread_id[i] = i;
		ret = pthread_create(&horse[i], NULL, race_horse, &thread_id[i]);
		assert(!ret && "Horse injured during race, event canceled.");
	}
	
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
	
	pthread_mutex_destroy(&track_mutex);
	printf("The race finishes!\n");
	return 0;
}
