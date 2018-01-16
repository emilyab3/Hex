/*
 * gameIO.c
 *
 * handles IO for both saving/loading games, and for user input
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gameIO.h"

#define MAX_INPUT 70

/* SAVED GAME IO */

/* saves the current game info and grid state to a file address specified in
 * input
 *
 * input: user input to be read in order to extract the filename
 * size: the length of the input
 * grid: the current game grid to be saved to the file
 * game: stores information on the current game
 *
 * returns: SUCCESS if game save was successful, ERROR otherwise
 */
int save_game(char* input, int size, char** grid, struct Game* game) {

    // stores the file address without the letter 's'
    char fileAddress[70];
    fileAddress[0] = '\0';
    strcat(fileAddress, &input[1]);

    // attempts to open the given file for saving
    FILE* gameFile = fopen(fileAddress, "w");
    if (gameFile == NULL) {
        return ERROR;
    }

    // saves game info to the first line of the file
    int hasNextMove = game->player2->hasNextMove;

    fprintf(gameFile, "%d,", hasNextMove);
    fprintf(gameFile, "%d,", game->height);
    fprintf(gameFile, "%d,", game->width);
    fprintf(gameFile, "%d,", game->player1->moveNumber);
    fprintf(gameFile, "%d\n", game->player2->moveNumber);
    
    int i;
    int j;
    int counter = 0;

    // prints the game grid to the file
    for (i = 0; i < game->height; i++) {

        for (j = 0; j < game->width; j++) {

            fprintf(gameFile, "%c", grid[i][j]);
            counter++;

            if (counter == game->width) {
                fprintf(gameFile, "%c", '\n');
                counter = 0;
            }
        }
    }

    fflush(gameFile);
    return SUCCESS;
}

/* loads information from a saved game file into game
 *
 * gameFile: file which the game information is stored in
 * game: stores information on the current game
 *
 * returns: a newly loaded game grid
 *
 * error conditions: incorrect file contents in gameFile
 *
 */
char** load_file(FILE* gameFile, struct Game* game) {
    int infoLength = 5;
    int next;
    int fileInfo[infoLength];
    char** grid;
    char infoEntry[infoLength];
    int digits = 0, commaCounter = 0, lineCounter = 0, indexCounter = 0;
    while (1) {
        next = fgetc(gameFile);
        if (next == EOF) {
            // incorrect number of grid lines
            if (lineCounter != (game->height + 1)) {
                exit_with_error("Incorrect file contents", 5);
            }
            return grid;
        } else if (next == '\n') {
            if (lineCounter == 0) {
                // file info line is the wrong length
                if (commaCounter != infoLength - 1) {
                    exit_with_error("Incorrect file contents", 5);
                }
                record_number(infoEntry, &digits, fileInfo, commaCounter);
                grid = init_saved_game(game, fileInfo);

            } else if ((lineCounter > 0) && (indexCounter != game->width)) {
                // grid line too short or too long
                exit_with_error("Incorrect file contents", 5);
            }
            lineCounter++;
            indexCounter = 0;

        } else if (lineCounter == 0 && (char)next == ',') {
            record_number(infoEntry, &digits, fileInfo, commaCounter);
            commaCounter++;

        } else if (lineCounter == 0 && (char)next != ',') {
            // adds each digit of a file info number to a string
            char charNext = (char)next;
            infoEntry[digits] = charNext;
            digits++;

        } else if (lineCounter > 0) {
            // bad characters in input grid
            if ((char)next != 'O' && (char)next != 'X' && (char)next != '.') {
                exit_with_error("Incorrect file contents", 5);
            }
            grid[lineCounter - 1][indexCounter] = (char)next;
            indexCounter++;
        }
    }
}

/* initialises a new game based on the information in a saved file (stored
 * in fileInfo)
 * helper method to load_file
 *
 * game: will be used to store information about the new game being
 *       initialised
 * fileInfo: contains information about the saved game which will be used to
 *           initialise a new game
 *
 * returns: a new game grid which has been allocated according to the
 *          dimensions given in fileInfo
 *
 * error conditions: incorrect file contents
 *
 */
char** init_saved_game(struct Game* game, int* fileInfo) {

    // checks to see which player has the next move
    if (fileInfo[0] == 1) {
        game->player1->hasNextMove = 0;
        game->player2->hasNextMove = 1;

    } else if (fileInfo[0] == 0) {
        game->player1->hasNextMove = 1;
        game->player2->hasNextMove = 0;

    } else {
        // if the first number is neither 0 nor 1, exit
        exit_with_error("Incorrect file contents", 5);
    }

    // check for invalid saved board dimensions
    if (fileInfo[1] < MIN_BOARD_WIDTH || fileInfo[1] > MAX_BOARD_WIDTH ||
            fileInfo[2] < MIN_BOARD_WIDTH || fileInfo[2] > MAX_BOARD_WIDTH) {

        exit_with_error("Incorrect file contents", 5);
    }
    // initialising the game struct
    game->height = fileInfo[1];
    game->width = fileInfo[2];
    game->size = game->height * game->width;
    game->player1->moveNumber = fileInfo[3];
    game->player2->moveNumber = fileInfo[4];

    // initialising the game grid
    char** grid = malloc(sizeof(char*) * (game->height + 1));
    int i;
    for (i = 0; i < game->height + 1; i++) {
        grid[i] = malloc(sizeof(char) * game->width + 1);
    }
    return grid;
}

/* checks if infoEntry is a number, and if it is, adds it to fileInfo
 * (helper method to load_file)
 *
 * infoEntry: a string to be checked to see if it is an integer
 * digits: the number of digits in infoEntry
 * fileInfo: holds values required to initialise a game from a saved file
 * commaCounter: counter for the number of entries already in fileInfo
 *
 * error conditions: invalid file contents
 *
 */
void record_number(char* infoEntry, int* digits, int* fileInfo, 
        int commaCounter) {

    // null terminates the string
    infoEntry[*digits] = '\0';

    // checks that the given string is an integer
    int integer = check_int(infoEntry);

    if (integer == ERROR) {
        exit_with_error("Incorrect file contents", 5);
    }

    fileInfo[commaCounter] = integer;
    *digits = 0;
}

/* checks if a given string is an integer or not
 *
 * numberString: the string to be checked to see if it is an integer
 * 
 * returns: the integer value of numberString if it can be converted, ERROR
 *          otherwise
 *
 */
int check_int(char* numberString) {

    char* end;

    int result = (int) strtol(numberString, &end, 10);

    // uses strtol functionality to check for errors in converting the string
    // to an int
    if (*end != '\0') {
        return ERROR;
    }

    return result;
}

/* exits the program with the given error message and exitStatus
 *
 * message: the message to be printed to stderr on exiting
 * exitStatus: the status with which the program should exit
 *
 */
void exit_with_error(char* message, int exitStatus) {

    fprintf(stderr, "%s\n", message);
    exit(exitStatus);
}

/* USER INPUT */

/* reads user input to stdin
 *
 * file: the file to be read (in this case, stdin)
 * length: stores the length of the file's contents
 * game: stores information on the current game
 *
 * returns: an array containing the input
 *
 * error conditions: EOF from user while waiting for input
 *
 */
char* read_line(FILE* file, int* length, struct Game* game) {
    // starting size
    int size = 10;
    char* result = malloc(sizeof(char) * size);
    int position = 0;
    int next = 0;

    while (1) {
        // checks that the previous move was not ended with EOF
        if (game->checkEOF == 1) {
            free(result);
            exit_with_error("EOF from user", 6);
        }

        next = fgetc(file);
        
        if ((next == EOF && position == 0)) {
            // EOF as the first character in the file
            free(result);
            exit_with_error("EOF from user", 6);

        } else if (next == EOF && position > 0) {
            // EOF after other characters have been entered
            result[position] = '\0';
            *length = position;
            game->checkEOF = 1;
            return result;

        } else if (next == '\n') {
            result[position] = '\0';
            *length = position;
            return result;

        } else {
            // resize result according to how much space is needed
            if (position > size - 5) {
                size = size * 2;
                result = realloc(result, (sizeof(char) * size));

            } else if (position < (size * 0.25)) {
                size = size * 0.5;
                result = realloc(result, (sizeof(char) * size));

            }
            result[position] = (char)next;
            position++;
        }
    }
}

/* checks the raw user input to ensure that it has a valid length and valid
 * number of individual arguments
 * also checks for the presence of an 's' at the start of the input to
 * indicate that the game should be saved
 *
 * input: the user input being checked
 * size: the length of the user input
 * game: stores information on the current game
 *
 * returns: SUCCESS if the input is valid, SAVE_ATTEMPT if the input started
 *          with an 's', and ERROR otherwise
 *
 * error conditions: unable to save to file given in input
 *
 */
int check_input(char* input, int size, struct Game* game) {

    // checks length of input;
    if (size > MAX_INPUT) {
        return ERROR;
    }
    
    // checks for input indicating a file save has been requested ('s')
    if (size > 0 && input[0] == 's') {
        int saveSuccess = save_game(input, size, game->grid, game);
        if (saveSuccess == ERROR) {
            fprintf(stderr, "%s\n", "Unable to save game");
        }
        return SAVE_ATTEMPT;
    }

    // checks for the correct number of arguments in the input
    int inputValid = check_arguments(input, &size);
    if (inputValid == ERROR) {
        return ERROR;
    }

    return SUCCESS;
}

/* checks that the user has entered exactly two space-separated values
 * helper method to check_input
 * 
 *
 * input: the user input to be checked
 * size: the length of the input
 *
 * returns: SUCCESS if there are exactly two individual values in input,
 *          ERROR otherwise
 */
int check_arguments(char* input, int* size) {

    int i;
    int numSpaces = 0;
    int numTokens = 0;
    char previous = ' ';

    // checks that the number of spaces and arguments (tokens) in the input
    // are correct
    for (i = 0; i < *size; i++) {

        if (input[i] == ' ') {
            numSpaces++;

        } else if (previous == ' ' && input[i] != ' ') {
            // indicates the end of a token
            numTokens++;

        }
        // allows us to compare the next character with the current one
        previous = input[i];
    }
    if (numSpaces != 1 || numTokens != 2) {
        return ERROR;
    }
    return SUCCESS;
}

/* processes input of exactly two arguments, checks that they are integers,
 * and if they are, returns them as a grid position
 *
 * input: the user input to be processed
 * size: the length of the input
 *
 * returns: a valid userMove if one was entered, otherwise an [ERROR, ERROR]
 *          pair
 *
 */
int* process_input(char* input, int* size) {

    int* userMove = malloc(sizeof(int) * 2);

    // to store each individual token of the user input
    char token1[MAX_INPUT / 2];
    char token2[MAX_INPUT / 2];

    int tokenCount = 0;
    int stringLength = 0;
    int i;

    for (i = 0; i < *size; i++) {

        if (input[i] != ' ' && tokenCount == 0) {
            // builds up token1
            token1[stringLength] = input[i];
            stringLength++;

        } else if (input[i] != ' ' && tokenCount == 1) {
            // builds up token2
            token2[stringLength] = input[i];
            stringLength++;

        } else {
            token1[stringLength] = '\0';
            stringLength = 0;
            tokenCount++;

        }
    }

    token2[stringLength] = '\0';

    // converts both tokens to integers (or ERROR, if they cannot be 
    // converted)
    int row = check_int(token1);
    int column = check_int(token2);

    userMove[0] = row;
    userMove[1] = column;

    return userMove;
}

/* checks the given userMove to ensure that its coordinates are valid grid
 * positions, and if so, that userMove is still free on the grid
 *
 * userMove: the given position to be validated
 * game: stores information on the current game
 *
 * returns: SUCCESS if userMove is valid, ERROR otherwise
 *
 */
int check_user_move(int* userMove, struct Game* game) {
    
    int row = userMove[0];
    int column = userMove[1];

    // input is not an integer
    if (row == ERROR || column == ERROR) {
        return ERROR;
    } 
    /* if row input is out of grid bounds */
    if (row < 0 || row >= game->width) {
        return ERROR;
    }
    /* if column input is out of grid bounds */
    if (column < 0 || column >= game->height) {
        return ERROR;
    }

    // checks that the position of userMove is not already occupied
    int isFree = check_position(game->grid, row, column);

    if (isFree == ERROR) {
        return ERROR;
    }

    return SUCCESS;
}

/* checks to see if a position in the game grid is already occupied
 *
 * grid: game grid to check position on
 * row: row of position being checked
 * column: column of position being checked
 *
 * returns: SUCCESS if position is free, ERROR if it is taken
 *
 */
int check_position(char** grid, int row, int column) {

    // a single '.' indicates an empty position
    if (grid[row][column] == '.') {
        return SUCCESS;

    }
    return ERROR;
}
