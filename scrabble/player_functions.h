//
//  player_functions.h
//  scrabble
//
//  Created by Mikaela Nicole Ramos on 7/27/24.
//

#ifndef player_functions_h
#define player_functions_h

#include <stdbool.h>

#define LETTERS_ON_DECK 7

typedef struct player {
    int playerId;
    int totalScore;
    char letters[LETTERS_ON_DECK];
} Player;

typedef struct playerTurn {
    Player* player;
    int startX;
    int endX;
    int startY;
    int endY;
    char* word;
    int wordLength;
    int score;
    int tilesUsed;
    bool isHorizontal;
} PlayerTurn;

#endif /* player_functions_h */
