#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <math.h>

#include "crossing.h"

#ifndef ANIMATION_H
#define ANIMATION_H

#define HACKERS_POSITION_VERTICAL 0
#define HACKERS_POSITION_HORIZONTAL 0
#define SERFS_POSITION_VERTICAL 10
#define SERFS_POSITION_HORIZONTAL 0
#define BOAT_POSITION_VERTICAL 19
#define BOAT_POSITION_HORIZONTAL 0

screen* generate_screen();
void clean_screen( screen *a_screen);
void print_screen(const screen *screen);
void delete_screen(int height);
void free_screen(screen *screen);

int replace_char(screen *screen, int row, int column, char a_char);

void draw_boat(screen *screen, int row_start, int column_start);
void draw_hackers_queue(screen *screen, int row_start, int column_start);
void draw_serfs_queue(screen *screen, int row_start, int column_start);
void draw_boat_hull(screen *screen, int pos_vertical, int pos_horizontal);
void draw_person(screen *screen, int row_start, int column_start, const person *person);
void draw_penguin(screen *screen, int row_start, int column_start, int id);
void draw_windows_guy(screen *screen, int row_start, int column_start, int id);
void print_number(screen *screen, int number, int row, int column_start);

int adds_queue(screen *screen, person *person);
int removes_queue(screen *screen, person *person);
int adds_boat(screen *screen, person *person, int position);
void removes_all_boat(screen *screen);

char** malloc_char_matrix(int height, int width);
void free_matrix(void **matrix, int rows);

int get_number_of_digits(int number);

#endif
