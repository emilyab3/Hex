/*
 * winning.c
 *
 * handles finding a winning path in a game of hex
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "winning.h"

/* checks the given move to see if its addition to the board results in a win
 * for the currentPlayer
 *
 * game: stores information on the current game
 * grid: the current game grid
 * connectionGrid: a grid of all the existing connections to the start of the
 *                 board (left hand side for O, top for X)
 * move: the move to be checked
 * currentPlayer: the player who made the given move
 *
 * returns: WIN if the move results in a win for currentPlayer, 0 otherwise
 *
 */
int check_win(struct Game* game, char** grid, int** connectionGrid, int* move, 
        struct Player* currentPlayer) {

    int connecter;
    char symbol = currentPlayer->playerSymbol;

    // different symbols in the connectionGrid represent Os and Xs
    if (symbol == 'O') {
        connecter = O_CONNECTER;
    } else {
        connecter = X_CONNECTER;
    }
    
    // keeps track of positions that still need to be checked for connections
    int toCheck[game->size][2];
    int numElements = 0;
    
    // if move is at the start of a path, or if it has surrounding connecters,
    // a connecter is added to the connectionGrid at move, and move is added
    // to the toCheck list
    int newConnections = connections(game, move, connectionGrid, connecter);

    if ((move[1] == 0 && symbol == 'O') || (move[0] == 0 && symbol == 'X') || 
            (newConnections == 1)) {

        connectionGrid[move[0]][move[1]] = connecter;
        toCheck[numElements][0] = move[0];
        toCheck[numElements][1] = move[1];
        numElements++;

    }

    // while there are still positions to be checked in toCheck, visit all the
    // positions which could potentially result in new connections
    while (numElements > 0) {
        numElements = visit_position(game, currentPlayer, numElements, toCheck,
                connecter);
    }

    return is_winner(game, connectionGrid);
}

/* checks the given move against the connectionGrid to determine if it is has
 * any connections to its neighbours (that is, if any of its neighbours are
 * part of an existing path
 *
 * game: stores information on the current game
 * move: the move to be checked for connections
 * connectionGrid: a grid of all the existing connections to the start of the
 *                 board (left hand side for O, top for X)
 * connecter: the symbol placed on the connectionGrid for the player who made
 *            the move (O_CONNECTER or X_CONNECTER)
 * 
 * returns: SUCCESS if there are any connections between move and its
 *          neighbours, 0 otherwise
 */
int connections(struct Game* game, int* move, int** connectionGrid, 
        int connecter) {
    int i;
    int row;
    int column;
    int connections = 0;
    int numberOfNeighbours;

    // gets the neighbours of the move to check for connections
    int** neighbours = get_neighbours(game, move, &numberOfNeighbours);

    // check each neighbour for a matching connecter in connectionGrid
    for (i = 0; i < numberOfNeighbours; i++) {

        row = neighbours[i][0];
        column = neighbours[i][1];

        if (connectionGrid[row][column] == connecter) {
            connections = SUCCESS;
        }
    }
    // frees neighbours array before returning
    free_2d_int(neighbours, 6);
    return connections;
}

/* for each position in toCheck, if its neighbours in the grid have the 
 * currentPlayer symbol, but the connectionGrid does not yet have a
 * connecter at that position, then a connecter is inserted and the position
 * is marked as visited
 *
 * game: stores information on the current game
 * currentPlayer: the player which has the symbol currently being checked
 * numElements: the current number of elements in toCheck
 * toCheck: contains all the positions which need to be visited
 * connecter: the symbol placed on the connectionGrid for the player who made
 *            the move (O_CONNECTER or X_CONNECTER)
 * 
 * returns: the updated number of elements in toCheck
 *
 */
int visit_position(struct Game* game, struct Player* currentPlayer, 
        int numElements, int toCheck[][2], int connecter) {
    
    int** neighbours;
    int* currentMove;
    int numNeighbours;
    int i;

    // compares the first element of toCheck to its neighbours
    currentMove = toCheck[0];
    neighbours = get_neighbours(game, currentMove, &numNeighbours);

    for (i = 0; i < numNeighbours; i++) {

        int row = neighbours[i][0];
        int column = neighbours[i][1];

        // if a neighbour has the current playerSymbol and has not yet been
        // visited (no connecter), add a connecter and add the neighbour to
        // toCheck
        if (game->grid[row][column] == currentPlayer->playerSymbol && 
                game->connectionGrid[row][column] != connecter) {

            game->connectionGrid[row][column] = connecter;
            toCheck[numElements][0] = neighbours[i][0];
            toCheck[numElements][1] = neighbours[i][1];
            numElements++;
        }
    }
    // mark the current element as having been checked
    remove_element(toCheck, numElements);
    numElements--;
    free_2d_int(neighbours, 6);

    return numElements;
}

/* performs the final check to determine if there is a winner in the game
 * (helper method to check_win)
 *
 * game: stores information on the current game
 * connectionGrid: a grid of all the existing connections to the start of the
 *                 board (left hand side for O, top for X)
 *
 * returns: WIN if there is a winner, 0 otherwise
 *
 */
int is_winner(struct Game* game, int** connectionGrid) {

    int i;

    // if there is a complete path across the board, the game has been won
    // (a complete path is indicated by either an O_CONNECTER in the last
    // column, or an X_CONNECTER in the last row)

    for (i = 0; i < game->height; i++) {
        if (connectionGrid[i][game->width - 1] == O_CONNECTER) {
            return WIN;
        }
    }
    for (i = 0; i < game->width; i++) {
        if (connectionGrid[game->height - 1][i] == X_CONNECTER) {
            return WIN;
        }
    }
    return 0;
}

/* gets the positions immediately surrounding the given move (its neighbours)
 *
 * game: stores information on the current game
 * move: the position to get the neighbours of
 * size: the number of neighbours the given move has
 *
 * returns: the grid coordinates of the neighbours of the move
 *
 */
int** get_neighbours(struct Game* game, int* move, int* size) {
    int maxNeighbours = 6;
    int row = move[0], column = move[1];
    int i, index = 0;

    int** neighbours = malloc(sizeof(int*) * maxNeighbours);

    for (i = 0; i < maxNeighbours; i++) {
        neighbours[i] = malloc(sizeof(int) * 2);
    }

    int check = check_neighbour(row - 1, column, game);
    if (check == SUCCESS) {
        neighbours[index][0] = row - 1;
        neighbours[index][1] = column;
        index++;
    }
    check = check_neighbour(row, column + 1, game);
    if (check == SUCCESS) {
        neighbours[index][0] = row;
        neighbours[index][1] = column + 1;
        index++;
    }
    check = check_neighbour(row + 1, column + 1, game);
    if (check == SUCCESS) {
        neighbours[index][0] = row + 1;
        neighbours[index][1] = column + 1;
        index++;
    }
    check = check_neighbour(row + 1, column, game);
    if (check == SUCCESS) {
        neighbours[index][0] = row + 1;
        neighbours[index][1] = column;
        index++;
    }
    check = check_neighbour(row, column - 1, game);
    if (check == SUCCESS) {
        neighbours[index][0] = row;
        neighbours[index][1] = column - 1;
        index++;
    }
    check = check_neighbour(row - 1, column - 1, game);
    if (check == SUCCESS) {
        neighbours[index][0] = row - 1;
        neighbours[index][1] = column - 1;
        index++;
    }
    *size = index;
    return neighbours;
}

/* checks the given row column pair to see if it is a valid position on the
 * grid
 *
 * row: the x-coordinate of the position to be checked
 * column: the y-coordinate of the position to be checked
 * game: stores information on the current game
 *
 * returns: SUCCESS if the row column pair is a valid position, ERROR otherwise
 *
 */
int check_neighbour(int row, int column, struct Game* game) {

    if (row < 0 || row >= game->height) {
        return ERROR;

    } else if (column < 0 || column >= game->width) {
        return ERROR;

    }

    return SUCCESS;
}

/* frees the memory allocated to a given multidimensional array
 *
 * array: the array to be freed
 * size: the size of the array
 *
 */
void free_2d_int(int** array, int size) {

    int i;

    for (i = 0; i < size; i++) {
        free(array[i]);
    }

    free(array);
}

/* removes the first element from toCheck
 *
 * toCheck: the array from which the element is being removed
 * size: the size of toCheck
 *
 */
void remove_element(int toCheck[][2], int size) {

    int i;

    // shifts each element in toCheck one position to the left
    for (i = 0; i < size - 1; i++) {

        toCheck[i][0] = toCheck[i + 1][0];
        toCheck[i][1] = toCheck[i + 1][1];
        
    }
}
