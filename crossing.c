#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define N 16

/*
 * Struct to define a new person who will make use of the ship to cross
 * the river 
 */
typedef struct person {
    /* 
     * Type == 0 -> Linux Hacker
     * Type == 1 -> Microsoft Programmer (serf)
     */
    int type;

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

/* Keeps track of how many serfs and hackers boarded the ship */
int _serfsb = 0;
int _hackersb = 0;

/* Keeps track of how many serfs and hackers are enqueued */
int _serfs = 0;
int _hackers = 0;

/* Creates a new person */
void newPerson(person *p, int id) {
    p->type = rand() % 2;
    //p->waiting = rand() % 10;
    p->waiting = 5;
    p->id = id;
}

/* Simple function to show queue */
void showqueue() {
   printf("Serf queue size: %d\n", _serfs);
   printf("Hacker queue size: %d\n\n", _hackers);
   sleep(3);
}


void setsail() {
    printf("\n*******************\n");
    printf("\nSetting sail\n");
    printf("Hackers: %d\n", _hackersb);
    printf("Serfs: %d\n", _serfsb);
    printf("\n*******************\n");
    sleep(10);
    showqueue();
}

void serfboard() {
    sem_post(&serf_queue);
    printf("Serf boarded\n");
    sleep(1);
    _serfsb++;
}

void hackerboard() {
    sem_post(&hacker_queue);
    printf("Hacker boarded\n");
    sleep(1);
    _hackersb++;
}

void serfjoin() {
    /* Waits if ship is free to join */
    sem_wait(&mutex);
    printf("Serf joined\n");
    sleep(3);

    /* Verifies boarding count */
    if (_serfs == 3) {
        _serfs -= 3;

        printf("Serf boarded\n");
        sleep(1);
        _serfsb++;
        serfboard();
        serfboard();
        serfboard();

        showqueue();

        setsail();

        _serfsb = 0;
        _hackersb = 0;

        /* Releases mutex */
        sem_post(&mutex);
    } else if (_serfs == 1 && _hackers >= 2) {
        _serfs -= 1;
        _hackers -= 2;

        printf("Serf boarded\n");
        sleep(1);
        _serfsb++;
        serfboard();
        hackerboard();
        hackerboard();

        showqueue();

        setsail();

        _serfsb = 0;
        _hackersb = 0;
        /* Releases mutex */
        sem_post(&mutex);
    } else {
        /* Increments serf count */
        _serfs++;
        printf("Serf enqueued\n");
        sleep(1);
        showqueue();

        /* Releases mutex */
        sem_post(&mutex);

        /* Puts current serf to sleep */
        sem_wait(&serf_queue);
    }
}

void hackerjoin() {
    /* Waits if ship is free to join */
    sem_wait(&mutex);
    printf("Hacker joined\n");
    sleep(3);

    /* Verifies boarding count */
    if (_hackers == 3) {
        _hackers -= 3;

        printf("Hacker boarded\n");
        sleep(1);
        _hackersb++;
        hackerboard();
        hackerboard();
        hackerboard();

        setsail();
        _serfsb = 0;
        _hackersb = 0;
        /* Releases mutex */
        sem_post(&mutex);
    } else if (_hackers == 1 && _serfs >= 2) {
        _hackers -= 1;
        _serfs -= 2;

        printf("Hacker boarded\n");
        sleep(1);
        _hackersb++;
        hackerboard();
        serfboard();
        serfboard();

        setsail();
        _serfsb = 0;
        _hackersb = 0;
        /* Releases mutex */
        sem_post(&mutex);
    } else {
        /* Increments hacker count */
        _hackers++;
        printf("Hacker enqueued\n");
        sleep(1);
        showqueue();

        /* Releases mutex */
        sem_post(&mutex);

        /* Puts current hacker to sleep */
        sem_wait(&hacker_queue);
    }
}
    

/* Passenger p enters the ship */
void *entership(void *p) {
    person passenger = *(person*) p;
    if (passenger.type == 0) {
        hackerjoin();
    } else {
        serfjoin();
    }

    printf("Done with %d\n", passenger.id);
}

int main() {
    pthread_t people[N];
    person passenger[N];
    int i = 0;

    sem_init(&mutex, 0, 1);
    sem_init(&hacker_queue, 0, 0);
    sem_init(&serf_queue, 0, 0);

    for (i = 0 ; i < N ; i++) {
       newPerson(&passenger[i], i);
       pthread_create(&people[i], NULL, entership, (void*) &passenger[i]);
    }

    for (i = 0 ; i < N ; i++) {
        pthread_join(people[i], NULL);
    }

    return 0;
}
