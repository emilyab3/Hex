/*
 * gameIO.h
 *
 * function prototypes for gameIO.h
 *
 */

#ifndef GAME_IO_H_
#define GAME_IO_H_

#include "structs.h"

/* saved game IO */

int save_game(char* input, int size, char** grid, struct Game* game);

char** load_file(FILE* gameFile, struct Game* game);

char** init_saved_game(struct Game* game, int* fileInfo);

void record_number(char* infoEntry, int* digits, int* fileInfo, 
        int commaCounter);

int check_int(char* numberString);

void exit_with_error(char* message, int exitStatus);

/* user input */

char* read_line(FILE* file, int* length, struct Game* game);

int check_input(char* input, int size, struct Game* game);

int check_arguments(char* input, int* size);

int* process_input(char* input, int* size);

int check_user_move(int* userMove, struct Game* game);

int check_position(char** grid, int row, int column);

#endif /* GAME_IO */
