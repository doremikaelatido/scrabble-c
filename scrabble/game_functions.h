//
//  game_functions.h
//  scrabble
//
//  Created by Mikaela Nicole Ramos on 7/27/24.
//

#ifndef game_functions_h
#define game_functions_h

#define PLAYER_LETTERS 30
#define BOARD_DIMENSION 15

#include "board_functions.h"

extern const int letterScores[27];
extern const int letterProbabilities[27];

PlayerTurn placeTileAndGetScore(PlayerTurn *playerTurn);
PlayerTurn placeLetterTiles(PlayerTurn *playerTurn, bool isHorizontal);

PlayerTurn getBestWord(void);

char getRandomCharacter(void);
void replaceLetter(Player* player, char letterUsed);
bool isInsideBoard(PlayerTurn playerTurn);
void initializePlayerLetters(Player* player);
bool canPlayThisMove(PlayerTurn playerTurn);
void playTurn(Player* player, int* totalTilesDistributed);
#endif /* game_functions_h */
