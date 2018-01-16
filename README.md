Hex

C99 program called bob which simulates a game of Hex between two players

bob will display a grid of cells like thıs:
   . . . .
  . . . .
 . . . .
. . . .

The two plàyers in the game will take turns putting “stones” into empty cells. The first player will indicate
their stones with O and the second player will use X. The cells are numbered from the top left corner as 0,0
(row, then column). Note that columns are numbered from zero for the leftmost cell in each row, regardless of
how far it is indented.

To win the game, a player needs to connect opposite sides of the grid. Player O aims to connect the left and
right edges with Os, while Player X aims to connect top and bottom edges with Xs.

Each cell is considered adjacent to up to six other cells, so, in the picture below, the O would be directly
connected to an O in any of the cells marked with ?.
   . . . .
  . ? ? .
 . ? O ?
. . ? ?

In the game below, O wins by connecting left to right..
   . . X .
  O O X .
 . O X X
. . O O
