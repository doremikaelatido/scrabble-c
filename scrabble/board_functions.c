//
//  board_functions.c
//  scrabble
//
//  Created by Mikaela Nicole Ramos on 7/27/24.
//
#include <stdlib.h>

#include "board_functions.h"

Tile** board;
PlayerTurn* bestWords;

void setBoard(Tile** newBoard){
    board = newBoard;
}

Tile** getBoard(void){
    return board;
}

Tile** createBoard(void){
    Tile** board = malloc(sizeof(Tile*) * 15);
    for (int row = 0; row<15; row++){
        board[row] = malloc(sizeof(Tile) * 15);
        for (int col = 0; col<15; col++){
            board[row][col].wordMultiplier = 1;
            board[row][col].letterMultiplier = 1;
            board[row][col].letterPlaced = '\0';
            
            if ((row > 0 && row < 5) || (row > 9 && row < 14)){
                if (row == col || 14-row == col || row == 14-col || 14-row == 14-col){
                    board[row][col].wordMultiplier = 2;
                } else if (abs(7 - row) == 6 || abs(7 - row) == 2){
                    if (abs(7 - col) == 6 || abs(7 - col) == 2){
                        board[row][col].letterMultiplier = 3;
                    }
                } else if (abs(7 - col) == 6 || abs(7 - col) == 2){
                    if (abs(7 - row) == 6 || abs(7 - row) == 2){
                        board[row][col].letterMultiplier = 3;
                    }
                }
            }
            if (row % 7 == 0 && col % 7 == 0){
                board[row][col].wordMultiplier = 3;
            }
            if ((row % 7 == 0 && abs(7 - col) == 4) || (col % 7 == 0 && abs(7 - row) == 4)){
                board[row][col].letterMultiplier = 2;
            }
            if (abs(7 - row) == 1 && (abs(7 - col) == 1 || abs(7 - col) == 5)){
                board[row][col].letterMultiplier = 2;
            }
            if (abs(7 - col) == 1 && (abs(7 - row) == 1 || abs(7 - row) == 5)){
                board[row][col].letterMultiplier = 2;
            }
            
            if (row == 7 && col == 7){
                board[row][col].wordMultiplier = 1;
            }
        }
    }
    return board;
}

void prepareGame(void){
    loadWordsToTrie();
    board = createBoard();
}

void printBoard(void){
    printf("      ");
    for (int c = 1; c<BOARD_DIMENSION+1; c++){
        if (c > 9)
            printf(" %d  ", c);
        else
            printf("  %d  ", c);
    }
    printf("\n");
    for (int r = 0; r<BOARD_DIMENSION; r++){
        if (r + 1 <= 9)
            printf(" %d   |", r+1);
        else
            printf(" %d  |", r+1);
        for (int c = 0; c<BOARD_DIMENSION; c++){
            if (board[r][c].letterPlaced != '\0'){
                printf("  %c |", board[r][c].letterPlaced);
            } else {
                if (board[r][c].wordMultiplier == 3){
                    printf(" 3W |");
                } else if (board[r][c].wordMultiplier == 2) {
                    printf(" 2W |");
                } else if (board[r][c].letterMultiplier == 3){
                    printf(" 3L |");
                } else if (board[r][c].letterMultiplier == 2){
                    printf(" 2L |");
                } else {
                    printf("    |");
                }
            }
        }
        printf("\n");
    }
}

void printPlayerLetters(Player player){
    printf("=================================================================================\n");
    printf("Player %d letters available: \n", player.playerId);
    for (int l = 0; l<LETTERS_ON_DECK; l++){
        printf(" %c ", player.letters[l]);
    }
    printf("\n");
    for (int s = 0; s<LETTERS_ON_DECK; s++){
        int letterIdx = player.letters[s] - 65;
        if (letterIdx > 26)
            letterIdx = 26;
        printf(" %d ", letterScores[letterIdx]);
    }
    printf("\n=================================================================================\n");
}

void printPlayerScores(Player* players, int numPlayers){
    printf("==================================SCOREBOARD=====================================\n");
    for (int n = 0; n<numPlayers; n++){
        printf("Player %d: %d       ", players[n].playerId, players[n].totalScore);
    }
    printf("\n=================================================================================\n");
}

Player* rankPlayers(Player* players, int numPlayers){
    Player *rankings = malloc(sizeof(Player) * numPlayers);
    for (int i = 0; i<numPlayers; i++)
        rankings[i].playerId = 0;
    
    for (int n = 0; n<numPlayers; n++){
        int r = 0;
        while (rankings[r].playerId != 0 && rankings[r].totalScore > players[n].totalScore)
            r++;
        Player movePlayer = rankings[r];
        rankings[r] = players[n];
        while(movePlayer.playerId != 0){
            r++;
            Player newMovePlayer = players[r];
            rankings[r] = movePlayer;
            movePlayer = newMovePlayer;
        }
    }
    return rankings;
}

void displayResults(Player* players, int numPlayers){
    printf("==================================SCOREBOARD=====================================\n");
    printf("WINNERS:\n");
    Player *rankings = rankPlayers(players, numPlayers);
    for (int p = 0; p < numPlayers; p++){
        printf("Rank %i: Player %i with %i points\n", p+1, rankings[p].playerId, rankings[p].totalScore);
        printf("Player's best word is %s with the score %i\n", rankings[p].bestWord, rankings[p].bestWordScore);
    }
    printf("==================================SCOREBOARD=====================================\n");
}
