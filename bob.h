/*
 * bob.h
 *
 * function prototypes for bob.c
 *
 */

#ifndef BOB_H_
#define BOB_H_

#include "structs.h"

void init_game(struct Game* game, struct Player* player1, 
        struct Player* player2);

void start_game(int argc, char** argv, struct Game* game);

void start_with_dimensions(struct Game* game, char** argv);

void start_with_file(struct Game* game, char** argv);

void check_start(struct Game* game, char** grid, int** connectionGrid);

void draw_grid(struct Game* game, char** grid);

int manual_move(struct Game* game, struct Player* currentPlayer);

void make_move_manual(char** grid, int* userMove, struct Player* player);

int check_position(char** grid, int row, int column);

int auto_move(struct Game* game, struct Player* currentPlayer);

int* make_move_auto(struct Game* game, struct Player* player, char** grid);

#endif /* BOB_H_ */
