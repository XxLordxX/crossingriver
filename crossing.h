#ifndef CROSSING_H
#define CROSSING_H

#define WIDTH 78
#define HEIGHT 31
#define MAX_HACKERS 8
#define MAX_SERFS 8
#define BOATS_CAPACITY 4

typedef enum person_type {serf, hacker} person_type;

/*
 * Struct to define a new person who will make use of the ship to cross
 * the river 
 */
typedef struct person {

	person_type type;

	/* Id to identify person */
	int id;
} person;

typedef struct screen {

	/* Saves the next position available to each queue */
	int hackers_queue;
	int serfs_queue;

	/* Arrays that points to the persons in some location */
	const person *hackers[MAX_HACKERS];
	const person *serfs[MAX_SERFS];
	const person *boat[BOATS_CAPACITY];
	
	/* Matrix printed on screen */
	char **screen_map;

	int width;
	int height;

}screen;

#endif
