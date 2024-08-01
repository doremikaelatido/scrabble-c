//
//  board_functions.h
//  scrabble
//
//  Created by Mikaela Nicole Ramos on 7/27/24.
//

#ifndef board_functions_h
#define board_functions_h

#include "player_functions.h"
#include "word_functions.h"

#define BOARD_DIMENSION 15

extern const int letterScores[27];

typedef struct tile {
    int x;
    int y;
    int wordMultiplier;
    int letterMultiplier;
    char letterPlaced;
} Tile;

void setBoard(Tile** board);
Tile** getBoard(void);

void prepareGame(void);
Tile** createBoard(void);
void printBoard(void);
void printPlayerLetters(Player player);
void printPlayerScores(Player* players, int numPlayers);
void displayWinner(Player* players, int numPlayers);

#endif /* board_functions_h */
