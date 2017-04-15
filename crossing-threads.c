#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define N 16

typedef enum person_type {serf, hacker} person_type;

/*
 * Struct to define a new person who will make use of the ship to cross
 * the river 
 */
typedef struct person {

	person_type type;

	/* Sleep time for thread */
	int waiting;

	/* Id to identify person */
	int id;
} person;


/* Mutex semaphore */
sem_t mutex;

/* Hacker/Serf queue */
sem_t hacker_queue;
sem_t serf_queue;

sem_t _everybody_on_board;

/* Keeps track of how many serfs and hackers boarded the ship */
int _serfsb = 0;
int _hackersb = 0;
int _totalb=0;

/* Keeps track of how many serfs and hackers are enqueued */
int _serfs = 0;
int _hackers = 0;

/* Creates a new person */
void newperson(person *p, int id) {

	int type_aux = rand()%2;
	p->type = type_aux == 0 ? serf : hacker;
	//p->waiting = rand() % 10;
	p->waiting = 5;
	p->id = id;
}

/* Simple function to show queue */
void showqueue() {
	printf("Serf queue size: %d\n", _serfs);
	printf("Hacker queue size: %d\n\n", _hackers);
	//sleep(1);
}

void setsail() {
	printf("\n*******************\n");
	printf("\nSetting sail\n");
	printf("Hackers: %d\n", _hackersb);
	printf("Serfs: %d\n", _serfsb);
	printf("\n*******************\n");
	//sleep(1);
}

/* Make a person get on board. */
void board(person *p){

	if (p->type == hacker){
		printf("Hacker %d boarded.\n", p->id);
		//sleep(1);
		__sync_add_and_fetch(&_hackersb, 1);
	}
	else{
		printf("Serf %d boarded.\n", p->id);
		//sleep(1);
		__sync_add_and_fetch(&_serfsb, 1);
	}

	int total = __sync_add_and_fetch(&_totalb, 1);
	if(total == 4){
		sem_post(&_everybody_on_board);
	}

}

void serfjoin(person *p) {
	/* Waits if ship is free to join */
	sem_wait(&mutex);
	printf("Serf %d joined\n", p->id);
	//sleep(1);

	if(_serfs == 3 || (_serfs == 1 && _hackers >= 2)){

		if(_serfs == 3){
			_serfs -= 3;
			printf("Waking up 3 serfs.\n");
			sem_post(&serf_queue);
			sem_post(&serf_queue);
			sem_post(&serf_queue);
		}
		else{
			_serfs -= 1;
			_hackers -= 2;
			printf("Waking up 2 hackers and 1 serf.\n");
			sem_post(&serf_queue);
			sem_post(&hacker_queue);
			sem_post(&hacker_queue);
		}

		/* Multithread from here */
		/* Wait until everybody go onboard */
		board(p);
		sem_wait(&_everybody_on_board);
		_totalb=0;

		setsail();
		showqueue();

		_serfsb=0;
		_hackersb=0;

		/* Releases mutex */
		sem_post(&mutex);
	}

	/* Is not time to go on board */
	else {
		/* Increments serf count */
		_serfs++;
		printf("Serf %d enqueued\n", p->id);
		//sleep(1);
		showqueue();

		/* Releases mutex */
		sem_post(&mutex);

		/* Puts current serf to sleep */
		sem_wait(&serf_queue);

		/* Multithread from here */
		/* Serf gets on board when wakes up */
		board(p);

	}
}

void hackerjoin(person *p) {
	/* Waits if ship is free to join */
	sem_wait(&mutex);
	printf("Hacker %d joined\n", p->id);
	//sleep(1);

	if(_hackers == 3 || (_hackers == 1 && _serfs >= 2)){

		if(_hackers == 3){
			_hackers -= 3;
			printf("Waking up 3 hackers\n");
			sem_post(&hacker_queue);
			sem_post(&hacker_queue);
			sem_post(&hacker_queue);
		}
		else{
			_hackers -= 1;
			_serfs -= 2;
			printf("Waking up 2 serfs and 1 hacker.\n");
			sem_post(&hacker_queue);
			sem_post(&serf_queue);
			sem_post(&serf_queue);
		}

		/* Multithread from here */
		/* Wait until everybody go onboard */
		board(p);
		sem_wait(&_everybody_on_board);
		_totalb=0;

		setsail();
		showqueue();

		_serfsb=0;
		_hackersb=0;

		/* Releases mutex */
		sem_post(&mutex);
	}

	/* Is not time to go on board */
	else {
		/* Increments hacker count */
		_hackers++;
		printf("Hacker %d enqueued\n", p->id);
		//sleep(1);
		showqueue();

		/* Releases mutex */
		sem_post(&mutex);

		/* Puts current hacker to sleep */
		sem_wait(&hacker_queue);

		/* Multithread from here */
		/* Serf gets on board when wakes up */
		board(p);

	}

}


/* Passenger p enters the ship */
void *entership(void *p) {
	person passenger = *(person*) p;
	if (passenger.type == hacker) {
		hackerjoin((person*)p);
	} else {
		serfjoin((person*)p);
	}

	printf("Done with %d\n", passenger.id);
	return NULL;
}

int main() {

	/* Seeds rand */
	srand(time(NULL)); 

	pthread_t people[N];
	person passenger[N];
	int i = 0;

	sem_init(&mutex, 0, 1);
	sem_init(&hacker_queue, 0, 0);
	sem_init(&serf_queue, 0, 0);
	sem_init(&_everybody_on_board, 0, 0);

	for (i = 0 ; i < N ; i++) {
		newperson(&passenger[i], i);
		pthread_create(&people[i], NULL, entership, (void*) &passenger[i]);
	}

	for (i = 0 ; i < N ; i++) {
		pthread_join(people[i], NULL);
	}

	return 0;
}
