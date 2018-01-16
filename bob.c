/* 
 * bob.c
 *
 * contains the main function, plus other functions required to run a game of
 * hex
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bob.h"
#include "gameIO.h"
#include "winning.h"

#define EMPTY 0

int main(int argc, char** argv) {

    // initialises the game
    struct Game game;
    struct Player playerO;
    struct Player playerX;
    init_game(&game, &playerO, &playerX);

    start_game(argc, argv, &game);
    draw_grid(&game, game.grid);

    // keeps track of which player is making the current move
    struct Player* currentPlayer;
    struct Player* inactivePlayer;

    int gameOver = 0;

    while (1) {
        // sets the current player
        if (playerO.hasNextMove == 1) {
            currentPlayer = &playerO;
            inactivePlayer = &playerX;

        } else {
            currentPlayer = &playerX;
            inactivePlayer = &playerO;

        }
        if (currentPlayer->type == 'm') {
            // handles moves made by manual players
            gameOver = manual_move(&game, currentPlayer);
            if (gameOver == ERROR) {
                continue;
            }

        } else if (currentPlayer->type == 'a') {
            // handles moves made by auto players
            gameOver = auto_move(&game, currentPlayer);
        }
        
        // if the game has a winner, end the game
        if (gameOver == WIN) {
            return 0;
        }
        // after a successful move, swap which player is currently active
        currentPlayer->hasNextMove = 0;
        inactivePlayer->hasNextMove = 1;
    }
}

/* initialises the game and players
 *
 * game: the game to be initialised
 * player1, player2: the players to be initialised
 *
 */
void init_game(struct Game* game, struct Player* player1, 
        struct Player* player2) {

    // sets player 1 to be O, and player2 to be X
    player1->playerSymbol = 'O';
    player2->playerSymbol = 'X';

    game->player1 = player1;
    game->player2 = player2;

    game->checkEOF = 0;
}

/* checks the given arguments to the program for validity, and starts loading
 * the game based on either board dimensions or a file name
 *
 * argc: argument counter from running the program
 * argv: the arguments given to the program
 * game: stores information on the current game
 *
 * error conditions: invalid argc value (too many/too few arguments)
 *                   invalid values stored in argv
 *
 */
void start_game(int argc, char** argv, struct Game* game) {

    // checks for incorrect number of starting arguments
    if (argc != 4 && argc != 5) {
        exit_with_error("Usage: bob p1type p2type "
                "[height width | filename]", 1);
    }

    // checks that arguments 2 and 3 indicate either an auto or manual player
    if ((strcmp(argv[1], "a") != 0 && strcmp(argv[1], "m") != 0) ||
            (strcmp(argv[2], "a") != 0 && strcmp(argv[2], "m") != 0)) {

        exit_with_error("Invalid type", 2);
    }

    game->player1->type = *argv[1];
    game->player2->type = *argv[2];
    
    if (argc == 5) {
        // start a new game using the given board dimensions
        start_with_dimensions(game, argv);

    } else if (argc == 4) {
        // load a saved game using the given filename
        start_with_file(game, argv);
    }
}

/* starts a new game based on given board dimensions
 *
 * game: stores information on the current game
 * argv: the arguments given to the program
 *
 * error conditions: invalid board dimensions in argv
 *
 */
void start_with_dimensions(struct Game* game, char** argv) {
    
    // check for invalid board dimensions
    int checkHeight = check_int(argv[3]);
    int checkWidth = check_int(argv[4]);

    if (checkHeight < MIN_BOARD_WIDTH || checkHeight > MAX_BOARD_WIDTH ||
            checkWidth < MIN_BOARD_WIDTH || checkWidth > MAX_BOARD_WIDTH) {

        exit_with_error("Sensible board dimensions please!", 3);
    }
    // finishes initialising game and player structs
    game->height = checkHeight;
    game->width = checkWidth;
    game->size = game->width * game->height;

    game->player1->hasNextMove = 1;
    game->player2->hasNextMove = 0;

    game->player1->moveNumber = 0;
    game->player2->moveNumber = 0;
    
    // allocates memory for both the game grid and connectionGrid
    game->grid = malloc(sizeof(char*) * game->height);
    game->connectionGrid = malloc(sizeof(int*) * game->height);

    int i;
    int j;

    for (i = 0; i < game->height; i++) {
        game->grid[i] = malloc(sizeof(char) * game->width);
        game->connectionGrid[i] = malloc(sizeof(int) * game->width);
    }
    for (i = 0; i < game->height; i++) {
        for (j = 0; j < game->width; j++) {
            game->grid[i][j] = '.';
            game->connectionGrid[i][j] = EMPTY;
        }
    }
}

/* starts a game based on a given file name
 *
 * game: stores information on the current game
 * argv: the arguments given to the program
 *
 * error conditions: unable to start reading from savefile
 *
 */
void start_with_file(struct Game* game, char** argv) {

    // attempts to open saved game file
    FILE* savedGame = fopen(argv[3], "r");
    if (savedGame == NULL) {
        exit_with_error("Could not start reading from savefile", 4);
    }

    // initialises a game grid based on the contents of savedGame
    game->grid = load_file(savedGame, game);

    // initialises connectionGrid
    game->connectionGrid = malloc(sizeof(int*) * game->height);
    
    int i;
    int j;

    for (i = 0; i < game->height; i++) {
        game->connectionGrid[i] = malloc(sizeof(int) * game->width);
    }
    for (i = 0; i < game->height; i++) {
        for (j = 0; j < game->width; j++) {
            game->connectionGrid[i][j] = EMPTY;
        }
    }
    fclose(savedGame);
    // checks for existing connections in the savedGame file
    check_start(game, game->grid, game->connectionGrid);
}

/* checks for existing connections and paths in a loaded game
 *
 * game: stores information on the current game
 * grid: the game grid
 * connectionGrid: a grid of all the existing connections to the start of the
 *                 board (left hand side for O, top for X)
 */
void check_start(struct Game* game, char** grid, int** connectionGrid) {
    int i;
    int move[2];
    
    // if there is an existing symbol in the grid, but it is not recorded in
    // connectionGrid, then check that position for possible paths
    // (paths must start at column 0 for O, and row 0 for X)

    for (i = 0; i < game->height; i++) {
        if (grid[i][0] == 'O' && connectionGrid[i][0] == EMPTY) {
            // checks for paths with player O
            connectionGrid[i][0] = O_CONNECTER;
            move[0] = i;
            move[1] = 0;
            check_win(game, grid, connectionGrid, move, game->player1);
        }
    }
    for (i = 0; i < game->width; i++) {

        if (grid[0][i] == 'X' && connectionGrid[0][i] == EMPTY) {
            // checks for paths with player X
            connectionGrid[0][i] = X_CONNECTER;
            move[0] = 0;
            move[1] = i;
            check_win(game, grid, connectionGrid, move, game->player2);
        }
    } 
}

/* prints out a game grid with the appropriate spacing, and with Xs and Os 
 * where moves have been made
 *
 * game: stores information on the current game
 * grid: the game grid to be printed
 *
 */
void draw_grid(struct Game* game, char** grid) {

    // the spacing to print the game at the correct angle
    int spacing = game->height;
    int counter = 0;
    int i;
    int j;

    // prints the contents of the game grid in the correct format
    for (i = 0; i < game->height; i++) {

        for (j = 0; j < game->width; j++) {

            char currentEntry = grid[i][j];

            if (counter == game->width - 1) {
                // last character in a row to be printed
                printf("%c", currentEntry);

            } else if (counter == 0) {
                // first character in a row to be printed
                printf("%*c ", spacing, currentEntry);
                spacing -= 1;

            } else {
                printf("%c ", currentEntry);

            }

            counter++;

            if (counter == game->width) {
                // start new line of printing
                printf("%c", '\n');
                counter = 0;
            }
        }
    }
}

/* prompts the user to make a move, reads user input and checks to ensure it 
 * is a valid move, then either updates the grid accordingly and checks for a
 * win, or indicates that the user should be reprompted
 *
 * game: stores information on the current game
 * currentPlayer: the player who made the move (O or X)
 * 
 * returns: WIN if the move results in a win for currentPlayer, ERROR if the
 * move is invalid, and SUCCESS otherwise
 *
 */
int manual_move(struct Game* game, struct Player* currentPlayer) {

    // prompts player to enter a move
    printf("Player %c] ", currentPlayer->playerSymbol);

    // read and process user input
    int size;
    char* input = read_line(stdin, &size, game);

    int check = check_input(input, size, game);
    if (check == ERROR || check == SAVE_ATTEMPT) {
        free(input);
        return ERROR;
    }
    int* userMove = process_input(input, &size);

    int validMove = check_user_move(userMove, game);
    if (validMove == ERROR) {
        free(input);
        free(userMove);
        return ERROR;

    } else {
        make_move_manual(game->grid, userMove, currentPlayer);
        draw_grid(game, game->grid);

        // checks if userMove results in a win
        int win = check_win(game, game->grid, game->connectionGrid, userMove, 
                currentPlayer);

        free(userMove);
        free(input);

        if (win == WIN) {
            // free all allocated memory
            int i;
            for (i = 0; i < game->height; i++) {
                free(game->grid[i]);
                free(game->connectionGrid[i]);
            }
            free(game->grid);
            free(game->connectionGrid);

            // announce winner
            printf("Player %c wins\n", currentPlayer->playerSymbol);
            return WIN;
        }
    }
    return SUCCESS;
}

/* places a valid userMove onto the game grid
 *
 * grid: game grid to place the move onto
 * userMove: the position at which the move is being made
 * player: the player who made the move (O or X)
 *
 */
void make_move_manual(char** grid, int* userMove, struct Player* player) {

    char playerSymbol = player->playerSymbol;

    //places the player symbol onto the grid at the given userMove
    grid[userMove[0]][userMove[1]] = playerSymbol;
}

/* updates the grid with the auto player move, and checks for a win
 *
 * game: stores information on the current game
 * currentPlayer: the player who made the move (O or X)
 *
 * returns: WIN if the move results in a win for currentPlayer, SUCCESS 
 *          otherwise
 *
 */
int auto_move(struct Game* game, struct Player* currentPlayer) {

    // generates an automatic move, and prints both the move and the grid
    int* autoMove = make_move_auto(game, currentPlayer, game->grid);

    printf("Player %c => %d %d\n", currentPlayer->playerSymbol, autoMove[0], 
            autoMove[1]);

    draw_grid(game, game->grid);

    // checks if autoMove results in a win
    int win = check_win(game, game->grid, game->connectionGrid, autoMove, 
            currentPlayer);

    free(autoMove);
    if (win == WIN) {
        // free all allocated memory
        int i;
        for (i = 0; i < game->height; i++) {
            free(game->grid[i]);
            free(game->connectionGrid[i]);
        }
        free(game->grid);
        free(game->connectionGrid);

        // announce winner
        printf("Player %c wins\n", currentPlayer->playerSymbol);
        return WIN;
    }
    return SUCCESS;
}

/* generates a valid move for an auto player according to pre-determined 
 * algorithms
 *
 * game: stores information on the current game
 * player: the auto player which the move is for
 * grid: the game grid which the move will be placed onto
 *
 * returns: the position of the generated move
 *
 */
int* make_move_auto(struct Game* game, struct Player* player, char** grid) {
    
    // variables m, n, and t form parts of the move generating algorithms
    int m;
    if (game->height > game->width) {
        m = game->height;
    } else {
        m = game->width;
    }

    int row;
    int column;
    int n = player->moveNumber;
    int t;

    // generate moves until a free position is found
    while (1) {
        if (player->playerSymbol == 'O') {
            // formula for automatic player O
            t = ((n * 9 % 1000037) + 17);

        } else {
            // formula for automatic player X
            t = ((n * 7 % 1000213) + 81);

        }
        row = (t / m) % game->height;
        column = t % game->width;
        n++;

        // checks generated position to see if it is free
        int isFree = check_position(grid, row, column);
        if (isFree == SUCCESS) {
            break;
        } 
    }

    // insert symbol into grid at generated position
    grid[row][column] = player->playerSymbol;
    player->moveNumber = n;

    int* move = malloc(sizeof(int) * 2);
    move[0] = row;
    move[1] = column;

    return move;
}
