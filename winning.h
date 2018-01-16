/*
 * winning.h
 *
 * function prototypes for winning.c
 *
 */

#ifndef WINNING_H_
#define WINNING_H_

#include "structs.h"

int check_win(struct Game* game, char** grid, int** connectionGrid, int* move, 
        struct Player* currentPlayer);

int connections(struct Game* game, int* move, int** connectionGrid, 
        int connecter);

int visit_position(struct Game* game, struct Player* currentPlayer, 
        int numElements, int toCheck[][2], int connecter);

int is_winner(struct Game* game, int** connectionGrid);

int** get_neighbours(struct Game* game, int* move, int* size);

int check_neighbour(int row, int column, struct Game* game);

void free_2d_int(int** array, int size);

void remove_element(int toCheck[][2], int size);

#endif /* WINNING_H_ */
