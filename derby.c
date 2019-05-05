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
#define TRACK_DISTANCE 20
#define HORSE_COUNTDOWN_TIME 3
#define HORSE_TRACK_MICROSECONDS 100000 // 0.1 seconds
#define HORSE_WAIT_TIME 2 //give horses enough time to prepare for the race

pthread_mutex_t start_gate_mutex;
pthread_cond_t starting_pistol = PTHREAD_COND_INITIALIZER;

pthread_mutex_t track_mutex;
pthread_cond_t track_position_busy;

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

void *race_horse(void *arguments){
	
	int index = *((int *) arguments);
	int action;
	int laneX, positionY;
	
	pthread_mutex_lock(&start_gate_mutex);
	pthread_cond_wait(&starting_pistol, &start_gate_mutex); //wait for starting pistol
	pthread_mutex_unlock(&start_gate_mutex); // unlocking for all other threads
	
	laneX = index;
	positionY = 0;
	printf("Horse %d starts\n", index);
	
	while(positionY < TRACK_DISTANCE)
	{
		action = get_random_action();
		
		/*
		pthread_mutex_lock(&track_mutex);
		while (track_position[laneX][positionY])
			pthread_cond_wait(&track_position_busy, &track_mutex);
		*/
		
		switch (action)
		{
			case MOVE_FORWARD:
				positionY += 2;
				break;
				
			case MOVE_LEFT:
				if(laneX > 0) //cannot be on a negative track
					laneX -= 1;
				
				positionY += 1;
				break;
				
			case MOVE_RIGHT:
				if(laneX < NUM_TRACKS) //cannot run off track
					laneX += 1;
				
				positionY += 1;
				break;
		}
		
		track_position[laneX][positionY] = 1;
		//pthread_mutex_unlock(&track_mutex);
		
		usleep(HORSE_TRACK_MICROSECONDS);
		printf("Horse: %d, Lane: %d, Position: %d, Lap: %d\n", index, laneX, positionY, track_position[laneX][positionY]);
		track_position[laneX][positionY] = 0;
	}
	printf("Horse %d: Ended.\n", index); //horse reached the end of the track
	
	//dont use pthreads on mac, but I code on a mac so this is so my IDE does not complain
	pthread_exit(NULL);
}

int main(void) {
	pthread_t horse[NUM_TRACKS];
	int thread_id[NUM_TRACKS];
	int i, j;
	int ret;
	
	for(i = 0; i < NUM_TRACKS; i++) { //prepare track for race
		for(j = 0; j < TRACK_DISTANCE; j++) {
				track_position[i][j] = 0;
		}
	}
	
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
	
	//sleep(HORSE_WAIT_TIME);
	
	for (i = 0; i < NUM_TRACKS; i++) {
		ret = pthread_join(horse[i], NULL);
		assert(!ret);
	}
	
	printf("The race finishes!\n");
	return 0;
}
