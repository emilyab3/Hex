/*
 * structs.h
 *
 * contains both the structs required for a game of hex, as well as
 * preprocessor constants needed in all of the game files
 * 
 */

#ifndef STRUCTS_H_
#define STRUCTS_H_

#define ERROR -1
#define SAVE_ATTEMPT 0
#define SUCCESS 1
#define WIN 2

#define MIN_BOARD_WIDTH 1
#define MAX_BOARD_WIDTH 1000

#define O_CONNECTER 1
#define X_CONNECTER 2

/* Represents a player within the game */
struct Player {
    int hasNextMove;
    int moveNumber;
    char type;
    char playerSymbol;
};

/* Represents a game of hex */
struct Game {
    int height;
    int width;
    int size;
    char** grid;
    int** connectionGrid;
    int checkEOF;
    struct Player* player1;
    struct Player* player2;
};

#endif /* STRUCTS_H_ */
