#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <math.h>

#include "crossing.h"
#include "animation.h"


// void newperson(person *p, int id) {
// 
// 	int type_aux = rand()%2;
// 	p->type = type_aux == 0 ? serf : hacker;
// 	//p->waiting = rand() % 10;
// 	p->waiting = 5;
// 	p->id = id;
// }
// 
// int main(){
// 
// 	srand(time(NULL)); 
// 
// 	screen *screen = generate_screen();
// 
// 	int id=0;
// 
// 	person people[8];
// 
// 	for(int i=0; i<8; i++){
// 
// 		newperson(&people[i], id++);
// 		adds_queue(screen, &people[i]);
// 		
// 	}
// 
// 	person person1, person2, person3, person4;
// 	newperson(&person1, 1);
// 	newperson(&person2, 2);
// 	newperson(&person3, 3);
// 	newperson(&person4, 4);
// 
// 	adds_boat(screen, &person1, 0);
// 	adds_boat(screen, &person2, 1);
// 	adds_boat(screen, &person3, 2);
// 	adds_boat(screen, &person4, 3);
// 
// 
// 	draw_boat(screen, BOAT_POSITION_VERTICAL, BOAT_POSITION_HORIZONTAL);
// 	draw_hackers_queue(screen, HACKERS_POSITION_VERTICAL, HACKERS_POSITION_HORIZONTAL); 
// 	draw_serfs_queue(screen, SERFS_POSITION_VERTICAL, SERFS_POSITION_HORIZONTAL); 
// 	print_screen(screen);
// 	removes_queue(screen, &people[0]);
// 	sleep(3);
// 
// 	for(int i=0; i < 80; i++){
// 		delete_screen(screen->height);
// 		draw_boat(screen, BOAT_POSITION_VERTICAL, BOAT_POSITION_HORIZONTAL+i);
// 		draw_hackers_queue(screen, HACKERS_POSITION_VERTICAL, HACKERS_POSITION_HORIZONTAL); 
// 		draw_serfs_queue(screen, SERFS_POSITION_VERTICAL, SERFS_POSITION_HORIZONTAL); 
// 		print_screen(screen);
// 		clean_screen(screen);
// 		/* Micro seconds sleep */
// 		usleep(100000);
// 	}
// 	free_screen(screen);
// 
// }

/* Creates a new screen */
screen* generate_screen(){

	screen *new_screen;
	new_screen = (screen*) malloc(sizeof(screen));
	new_screen->width = WIDTH;
	new_screen->height = HEIGHT;
	new_screen->hackers_queue=0;
	new_screen->serfs_queue=0;
	new_screen->screen_map = malloc_char_matrix(new_screen->height, new_screen->width);

	for(int i=0; i < MAX_HACKERS; i++){
		new_screen->hackers[i]=NULL;
	}
	for(int i=0; i < MAX_SERFS; i++){
		new_screen->serfs[i]=NULL;
	}
	for(int i=0; i < BOATS_CAPACITY; i++){
		new_screen->boat[i]=NULL;
	}

	clean_screen(new_screen);

	int i;
	for (i=0; i<MAX_HACKERS; i++){
		new_screen->hackers[i] = NULL;	
	}
	for (i=0; i<MAX_SERFS; i++){
		new_screen->serfs[i] = NULL;	
	}
	for (i=0; i<BOATS_CAPACITY; i++){
		new_screen->boat[i] = NULL;	
	}

	return new_screen;

}

/* Free a screen struct */
void free_screen(screen *screen){
	free_matrix((void **)(screen->screen_map), screen->height);	
	free(screen);
	screen=NULL;
}

/* Overrides the screen with */
void clean_screen( screen *a_screen){

	for (int i=0; i < a_screen->height; i++){
		for (int j=0; j < a_screen->width; j++){
			a_screen->screen_map[i][j]=' '; 
		}
	}

}

/* Prints the screen */
void print_screen(const screen *screen){

	for (int i=0; i < screen->height; i++){
		int j;
		for (j=0; j < screen->width; j++){
			printf("%c", screen->screen_map[i][j]);
		}
		printf("\n");
	}
	
}

/* Deletes the screen at the terminal */
void delete_screen(int height){
	for(int i=0; i < height; i++){
		printf("\033[1A\033[K");
	}
}

/*	Adds a person in your queue
 *	Return: 0: success
 *			1: the queue is full
*/
int adds_queue(screen *screen, person *person){

	if(person->type == hacker){
		if(screen->hackers_queue == MAX_HACKERS-1){
			return 1;
		}
		screen->hackers[screen->hackers_queue++]=person;
	}
	else if(person->type == serf){
		if(screen->serfs_queue == MAX_SERFS-1){
			return 1;
		}
		screen->serfs[screen->serfs_queue++]=person;

	}
	return 0;

}

/*	Remove a person from a queue
 *	Return: 0: success
 *			1: the person is not in the queue
*/
int removes_queue(screen *screen, person *person){

	if(person->type == hacker){

		int i;
		int person_position= -1;

		/* Searches for person and remove it*/
		for(i=0; i < MAX_HACKERS && person_position == -1; i++){
			if(screen->hackers[i]->id == person->id){
				screen->hackers[i] = NULL;
				person_position = i;
			}
		}

		/* Return -1 if the person is not in the queue */
		if(person_position == -1){
			return 1;
		}

		/* Move from the front backwards */
		for(i=person_position; i < MAX_HACKERS - 1; i++){
			screen->hackers[i] = screen->hackers[i+1];
		}
		screen->hackers[i] = NULL;

		screen->hackers_queue--;

	}
	else if(person->type == serf){

		int i;
		int person_position= -1;

		/* Searches for person and remove it*/
		for(i=0; i < MAX_SERFS && person_position == -1; i++){
			if(screen->serfs[i]->id == person->id){
				screen->serfs[i] = NULL;
				person_position = i;
			}
		}

		/* Return -1 if the person is not in the queue */
		if(person_position == -1){
			return 1;
		}

		/* Move from the front backwards */
		for(i=person_position; i < MAX_SERFS - 1; i++){
			screen->serfs[i] = screen->serfs[i+1];
		}
		screen->serfs[i] = NULL;

		screen->serfs_queue--;

	}

	return 0;

}

/*	Adds a person in the boat at position "position"
 *	Return: 0: success
 *			1: position is filled
 *			2: invalid position
*/
int adds_boat(screen *screen, person *person, int position){

	if (position < 0 || position >= BOATS_CAPACITY){
		return 2;
	}
	if (screen->boat[position] != NULL){
		return 1;
	}

	screen->boat[position] = person;	

	return 0;

}

/* Remove all people from the boat */
void removes_all_boat(screen *screen){
	
	for(int i=0; i < BOATS_CAPACITY; i++){
		screen->boat[i] = NULL;
	}

}

/* Draw boat and passengers*/
void draw_boat(screen *screen, int row_start, int column_start){
	/* Draw boat hull */
	draw_boat_hull(screen, row_start+4, column_start);

	/* Draw persons on the boat */
	for(int i=0; i < BOATS_CAPACITY; i++){
		if(screen->boat[i] != NULL){
			draw_person(screen,row_start, column_start+5+(i*10), screen->boat[i]); 	
		}
	}
}

/* Draw hackers queue */
void draw_hackers_queue(screen *screen, int row_start, int column_start){
	for(int i=0; i < MAX_HACKERS; i++){
		if(screen->hackers[i] != NULL){
			draw_person(screen, row_start, column_start+(i*10), screen->hackers[i]);
		}
	}
}

/* Draw serfs queue */
void draw_serfs_queue(screen *screen, int row_start, int column_start){
	for(int i=0; i < MAX_SERFS; i++){
		if(screen->serfs[i] != NULL){
			draw_person(screen, row_start, column_start+(i*10), screen->serfs[i]);
		}
	}
}

void draw_person(screen *screen, int row_start, int column_start, const person *person){
	if (person->type == hacker){
		draw_penguin(screen, row_start+1, column_start, person->id);
	}
	else if(person->type == serf){
		draw_windows_guy(screen, row_start, column_start,person->id);
	}
}

/* Draws a penguin starting at position (row_start,column_start) with its ID */
void draw_penguin(screen *screen, int row_start, int column_start, int id){

	replace_char( screen, row_start+6, column_start+0, '/');
	replace_char( screen, row_start+7, column_start+0, '\\');

	replace_char( screen, row_start+2, column_start+1, '|');
	replace_char( screen, row_start+5, column_start+1, '/');
	replace_char( screen, row_start+7, column_start+1, '/');

	replace_char( screen, row_start+1, column_start+2, '/');
	replace_char( screen, row_start+3, column_start+2, '\\');
	replace_char( screen, row_start+4, column_start+2, '/');
	replace_char( screen, row_start+6, column_start+2, '/');
	replace_char( screen, row_start+7, column_start+2, '\\');

	replace_char( screen, row_start+0, column_start+3, '_');
	replace_char( screen, row_start+3, column_start+3, '_');
	replace_char( screen, row_start+5, column_start+3, '/');
	replace_char( screen, row_start+7, column_start+3, '_');

	replace_char( screen, row_start+0, column_start+4, '_');
	replace_char( screen, row_start+2, column_start+4, 'O');
	replace_char( screen, row_start+3, column_start+4, '_');
	replace_char( screen, row_start+4, column_start+4, '/');
	replace_char( screen, row_start+7, column_start+4, '_');

	replace_char( screen, row_start+0, column_start+5, '_');
	replace_char( screen, row_start+3, column_start+5, '_');
	replace_char( screen, row_start+4, column_start+5, '\\');
	replace_char( screen, row_start+7, column_start+5, '/');

	replace_char( screen, row_start+1, column_start+6, '\\');
	replace_char( screen, row_start+3, column_start+6, '/');
	replace_char( screen, row_start+5, column_start+6, '\\');
	replace_char( screen, row_start+6, column_start+6, '/');
	replace_char( screen, row_start+7, column_start+6, '_');

	replace_char( screen, row_start+2, column_start+7, '>');
	replace_char( screen, row_start+7, column_start+7, '_');

	print_number(screen, id, row_start + 6, column_start + 3);

}

/* Draws a windows guy starting at position (row_start,column_start) with its ID */
void draw_windows_guy(screen *screen, int row_start, int column_start, int id){

	replace_char(screen, row_start+1, column_start+0, '|');
	replace_char(screen, row_start+2, column_start+0, '|');
	replace_char(screen, row_start+5, column_start+0, '/');
	replace_char(screen, row_start+6, column_start+0, 'v');
	replace_char(screen, row_start+8, column_start+0, '/');

	replace_char(screen, row_start+0, column_start+1, '_');
	replace_char(screen, row_start+1, column_start+1, '_');
	replace_char(screen, row_start+2, column_start+1, '_');
	replace_char(screen, row_start+4, column_start+1, '/');
	replace_char(screen, row_start+5, column_start+1, '/');
	replace_char(screen, row_start+7, column_start+1, '/');

	replace_char(screen, row_start+0, column_start+2, '_');
	replace_char(screen, row_start+1, column_start+2, '_');
	replace_char(screen, row_start+2, column_start+2, '_');
	replace_char(screen, row_start+3, column_start+2, '|');
	replace_char(screen, row_start+4, column_start+2, '|');
	replace_char(screen, row_start+5, column_start+2, '|');
	replace_char(screen, row_start+6, column_start+2, '|');

	replace_char(screen, row_start+0, column_start+3, '_');
	replace_char(screen, row_start+1, column_start+3, '|');
	replace_char(screen, row_start+2, column_start+3, '|');

	replace_char(screen, row_start+0, column_start+4, '_');
	replace_char(screen, row_start+1, column_start+4, '_');
	replace_char(screen, row_start+2, column_start+4, '_');
	replace_char(screen, row_start+3, column_start+4, '|');
	replace_char(screen, row_start+4, column_start+4, '|');
	replace_char(screen, row_start+5, column_start+4, '|');
	replace_char(screen, row_start+6, column_start+4, '|');

	replace_char(screen, row_start+0, column_start+5, '_');
	replace_char(screen, row_start+1, column_start+5, '_');
	replace_char(screen, row_start+2, column_start+5, '_');
	replace_char(screen, row_start+4, column_start+5, '\\');
	replace_char(screen, row_start+5, column_start+5, '\\');
	replace_char(screen, row_start+7, column_start+5, '\\');

	replace_char(screen, row_start+1, column_start+6, '|');
	replace_char(screen, row_start+2, column_start+6, '|');
	replace_char(screen, row_start+5, column_start+6, '\\');
	replace_char(screen, row_start+6, column_start+6, 'v');
	replace_char(screen, row_start+8, column_start+6, '\\');

	print_number(screen, id, row_start+8,column_start+2); 

}

/* Draws a boat starting at position (row_start,column_start) */
void draw_boat_hull(screen *screen, int row_start, int column_start){

	replace_char(screen,row_start+3,column_start,'/');
	replace_char(screen,row_start+4,column_start,'|');
	replace_char(screen,row_start+5,column_start,'|');
	replace_char(screen,row_start+6,column_start,'\\');

	replace_char(screen,row_start+3,column_start+1,'\\');
	replace_char(screen,row_start+7,column_start+1,'\\');

	replace_char(screen,row_start+4,column_start+2,'\\');

	replace_char(screen,row_start+5,column_start+3,'\\');

	replace_char(screen,row_start+1,column_start+44,'|');
	replace_char(screen,row_start+2,column_start+44,'|');
	replace_char(screen,row_start+3,column_start+44,'|');

	replace_char(screen,row_start+0,column_start+45,'_');
	replace_char(screen,row_start+2,column_start+45,'.');
	replace_char(screen,row_start+3,column_start+45,'_');


	replace_char(screen,row_start+1,column_start+46,'|');
	replace_char(screen,row_start+2,column_start+46,'|');
	replace_char(screen,row_start+3,column_start+46,'|');

	replace_char(screen,row_start+1,column_start+47,'_');
	replace_char(screen,row_start+2,column_start+47,'_');

	replace_char(screen,row_start+1,column_start+48,'_');
	replace_char(screen,row_start+2,column_start+48,'_');

	replace_char(screen,row_start+1,column_start+49,'|');
	replace_char(screen,row_start+2,column_start+49,'|');
	replace_char(screen,row_start+3,column_start+49,'|');
	replace_char(screen,row_start+4,column_start+49,'|');
	replace_char(screen,row_start+5,column_start+49,'|');

	replace_char(screen,row_start+5,column_start+50,'_');

	replace_char(screen,row_start+5,column_start+51,'_');

	replace_char(screen,row_start+5,column_start+52,'/');

	replace_char(screen,row_start+4,column_start+53,'/');

	replace_char(screen,row_start+3,column_start+54,'/');
	replace_char(screen,row_start+7,column_start+54,'/');

	replace_char(screen,row_start+3,column_start+55,'\\');
	replace_char(screen,row_start+6,column_start+55,'/');

	replace_char(screen,row_start+4,column_start+56,'\\');
	replace_char(screen,row_start+5,column_start+56,'/');

	for(int j=4; j<=48; j++){
		replace_char(screen,row_start+5,column_start+j,'_');
	}

	for(int j=2; j<=53; j++){
		replace_char(screen,row_start+7,column_start+j,'_');
	}
}

/* Puts "a_char" in screen at position (row,column) if that position exists
 * 	Return: 1 if success
 * 			0 if position doesn't exist
*/
int replace_char(screen *screen, int row, int column, char a_char){

	if ((row < 0 || row >= screen->height) || (column < 0 || column >= screen->width)){
		return 0;
	}
	screen->screen_map[row][column]=a_char;
	return 1;

}

/* Alloc a char matrix with "rows" rows and "columns" columns */
char** malloc_char_matrix(int rows, int columns){
	
	char **new_matrix = (char**) malloc(rows * sizeof(char *));

	for(int i=0; i<rows; i++){
		new_matrix[i] = (char *) malloc(columns * sizeof(char));
	}

	return new_matrix;

}

/* Free a char matrix with "rows" row */
void free_matrix(void **matrix, int rows){

	for(int i=0; i<rows; i++){
		free(matrix[i]);
	}
	free(matrix);

}

/* Return the number of digits at "number'" */
int get_number_of_digits(int number){
	return number == 0 ? 1 : floor( log10( abs(number) )) + 1;
}

/* Prints "number" at screen at the start position (row, column_start)*/
void print_number(screen *screen, int number, int row, int column_start){

	int number_aux = number;
	int num_digts = get_number_of_digits(number);
	int aux=pow(10, num_digts-1);
	for (int offset = 0; offset < num_digts; offset++){
		
		int digit = number_aux / aux;
		number_aux = number_aux - digit*aux;
		aux = aux / 10;
		char char_digit = digit + '0';
		replace_char(screen, row, column_start+offset, char_digit);

	}	
}
