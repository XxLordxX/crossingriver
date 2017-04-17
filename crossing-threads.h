#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include "crossing.h"

/* Creates a new person */
void newperson(person *p, int id);
void setsail();
/* Make a person get on board. */
void board(person *p, int isCapitan);
/* Serf function */
void serfjoin(person *p);
/* Hacker function */
void hackerjoin(person *p);
/* Passenger p enters the ship */
void entership(person *p);
void reprints_screen(screen *screen);
