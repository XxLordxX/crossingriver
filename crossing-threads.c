#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include "crossing.h"
#include "crossing-threads.h"
#include "animation.h"

/* Must be a 4 multiple */
#define N 4*4

/* Mutex semaphore */
sem_t mutex;

/* Hacker/Serf queue */
sem_t hacker_queue;
sem_t serf_queue;

sem_t _everybody_on_board;

/* Keeps track of how many serfs and hackers boarded the ship */
int _serfsb = 0;
int _hackersb = 0;
int _total_crew=0;

/* Keeps track of how many serfs and hackers are enqueued */
int _serfs = 0;
int _hackers = 0;

pthread_mutex_t screen_mutex;

screen *my_screen;

int main() {

	if (pthread_mutex_init(&screen_mutex, NULL) != 0){
		printf("screen_mutex init failed\n");
		return 1;
	}

	my_screen = generate_screen();
	draw_boat(my_screen, BOAT_POSITION_VERTICAL, BOAT_POSITION_HORIZONTAL);
	print_screen(my_screen);

	/* Seeds rand */
	srand(time(NULL)); 

	pthread_t people[N];
	person *passenger[N];

	for(int i =0; i < N; i++){
		passenger[i] = (person*) malloc(sizeof(person));
	}

	sem_init(&mutex, 0, 1);
	sem_init(&hacker_queue, 0, 0);
	sem_init(&serf_queue, 0, 0);
	sem_init(&_everybody_on_board, 0, 0);

	for (int i = 0 ; i < N ; i++) {
		newperson(passenger[i], i);
		pthread_create(&people[i], NULL, (void* (*) (void*)) entership, (void*) passenger[i]);
	}

	for (int i = 0 ; i < N ; i++) {
		pthread_join(people[i], NULL);
	}

	for(int i = 0; i < N; i++){
		free(passenger[i]);
	}
	free_screen(my_screen);

	return 0;
}

/* Creates a new person */
void newperson(person *p, int id) {

	p->type = semi_rand_type();
	p->id = id;
}

/* Random choose the person type, except the last one */
person_type semi_rand_type(){

	static int total=0;
	static int hackers=0;
	static int serfs=0;

	if( total == N-1 ){
		if( hackers%2 == 1){
			total++;
			hackers++;
	   		return hacker;
		}
		else if( serfs%2 == 1){
			total++;
			serfs++;
			return serf;
		}
	}
	
	int type_aux = rand()%2;

	if(type_aux == 0){
		serfs++;
	}
	else {
		hackers++;
	}
	total++;

	return type_aux == 0 ? serf : hacker;

}

void setsail() {

	pthread_mutex_lock(&screen_mutex);

	for(int i=0; i <= 80; i++){
		reprints_screen(my_screen, BOAT_POSITION_VERTICAL, BOAT_POSITION_HORIZONTAL+i, 40000);
	}
	usleep(10000);

	removes_all_boat(my_screen);
	reprints_screen(my_screen, BOAT_POSITION_VERTICAL, BOAT_POSITION_HORIZONTAL, 30000);


	pthread_mutex_unlock(&screen_mutex);

}

/* Make a person get on board. */
void board(person *p, int isCapitan){

	if(!isCapitan){
		int	total = __sync_add_and_fetch(&_total_crew, 1);

		/* Gets the screen lock, removes person from the queue,
		 *  adds on the boat and reprints screen */
		pthread_mutex_lock(&screen_mutex);
		removes_queue(my_screen, p);
		adds_boat(my_screen, p, total-1);
		reprints_screen(my_screen, BOAT_POSITION_VERTICAL, BOAT_POSITION_HORIZONTAL, 1000000);
		pthread_mutex_unlock(&screen_mutex);

		/* If threads is the last to get in, wakeup capitan */
		if(total == 3){
			sem_post(&_everybody_on_board);
		}
	}
	else {
		/* Gets the screen lock, adds capitan on the boat
		 * and reporints screen */
		pthread_mutex_lock(&screen_mutex);
		adds_boat(my_screen, p, BOATS_CAPACITY-1);
		reprints_screen(my_screen, BOAT_POSITION_VERTICAL, BOAT_POSITION_HORIZONTAL, 1000000);
		pthread_mutex_unlock(&screen_mutex);
	}

}

/* Serf function */
void serfjoin(person *p) {
	/* Waits if ship is free to join */
	sem_wait(&mutex);

	if(_serfs == 3 || (_serfs == 1 && _hackers >= 2)){

		if(_serfs == 3){
			_serfs -= 3;
			sem_post(&serf_queue);
			sem_post(&serf_queue);
			sem_post(&serf_queue);
		}
		else{
			_serfs -= 1;
			_hackers -= 2;
			sem_post(&serf_queue);
			sem_post(&hacker_queue);
			sem_post(&hacker_queue);
		}

		/* Multithread from here */
		/* Wait until everybody go onboard */
		board(p, 1);
		sem_wait(&_everybody_on_board);
		_total_crew=0;

		setsail();

		_serfsb=0;
		_hackersb=0;

		/* Releases mutex */
		sem_post(&mutex);
	}

	/* Is not time to go on board */
	else {
		/* Increments serf count */
		_serfs++;

		pthread_mutex_lock(&screen_mutex);
		adds_queue(my_screen, p);
		reprints_screen(my_screen, BOAT_POSITION_VERTICAL, BOAT_POSITION_HORIZONTAL, 1000000);
		pthread_mutex_unlock(&screen_mutex);


		/* Releases mutex */
		sem_post(&mutex);

		/* Puts current serf to sleep */
		sem_wait(&serf_queue);

		/* Multithread from here */
		/* Serf gets on board when wakes up */
		board(p, 0);

	}
}

/* Hacker function */
void hackerjoin(person *p) {
	/* Waits if ship is free to join */
	sem_wait(&mutex);

	if(_hackers == 3 || (_hackers == 1 && _serfs >= 2)){

		if(_hackers == 3){
			_hackers -= 3;
			sem_post(&hacker_queue);
			sem_post(&hacker_queue);
			sem_post(&hacker_queue);
		}
		else{
			_hackers -= 1;
			_serfs -= 2;
			sem_post(&hacker_queue);
			sem_post(&serf_queue);
			sem_post(&serf_queue);
		}

		/* Multithread from here */
		/* Wait until everybody go onboard */
		board(p, 1);
		sem_wait(&_everybody_on_board);
		_total_crew=0;

		setsail();

		_serfsb=0;
		_hackersb=0;

		/* Releases mutex */
		sem_post(&mutex);
	}

	/* Is not time to go on board */
	else {
		/* Increments hacker count */
		_hackers++;

		pthread_mutex_lock(&screen_mutex);
		adds_queue(my_screen, p);
		reprints_screen(my_screen, BOAT_POSITION_VERTICAL, BOAT_POSITION_HORIZONTAL, 1000000);
		pthread_mutex_unlock(&screen_mutex);

		/* Releases mutex */
		sem_post(&mutex);

		/* Puts current hacker to sleep */
		sem_wait(&hacker_queue);

		/* Multithread from here */
		/* Serf gets on board when wakes up */
		board(p, 0);

	}

}


/* Passenger p enters the ship */
void entership(person *p) {
	person passenger = * p;
	if (passenger.type == hacker) {
		hackerjoin((person*)p);
	} else {
		serfjoin((person*)p);
	}
}

void reprints_screen(screen *screen, int boat_position_vertical, int boat_position_horizontal, int microseconds){

	clean_screen(screen);
	draw_boat(screen, boat_position_vertical, boat_position_horizontal);
	draw_hackers_queue(screen, HACKERS_POSITION_VERTICAL, HACKERS_POSITION_HORIZONTAL);
	draw_serfs_queue(screen, SERFS_POSITION_VERTICAL, SERFS_POSITION_HORIZONTAL);

	delete_screen(screen->height);

	print_screen(screen);
	
	usleep(microseconds);
}
