#include <stdlib.h>
#include <time.h>

#include "game_functions.h"

#define TOTAL_TILES 30

int totalTilesDistributed = 0;

bool play(Player* player){
    printPlayerLetters(*player);
    printf("Enter p to proceed, and s to skip:");
    char decision;
    scanf(" %c", &decision);
    
    if (decision == 'p'){
        playTurn(player, &totalTilesDistributed);
    }
    
    printf("Tiles left: %i\n", TOTAL_TILES - totalTilesDistributed);
    
    return decision == 'p';
}

int main(void) {
    srand((unsigned)time(NULL));

    int numPlayers;
    printf("Number of players: ");
    scanf("%d", &numPlayers);
    Player *players = malloc(numPlayers * sizeof(Player));
    for (int p = 0; p<numPlayers; p++){
        players[p].playerId = p + 1;
        players[p].totalScore = 0;
        initializePlayerLetters(&players[p]);
        totalTilesDistributed += LETTERS_ON_DECK;
    }

    prepareGame();
    int nthPlayer = 0, skipCounter = 0;
    while (totalTilesDistributed < TOTAL_TILES && skipCounter < 6){
        printPlayerScores(players, numPlayers);
        printBoard();
        bool userPlayed = play(&players[nthPlayer]);
        if (!userPlayed){
            skipCounter += 1;
        } else {
            skipCounter = 0;
        }
        nthPlayer = (nthPlayer + 1) % numPlayers;
    }
    
    displayWinner(players, numPlayers);
    
    return 0;
}
