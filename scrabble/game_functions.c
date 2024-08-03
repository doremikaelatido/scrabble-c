//
//  game_functions.c
//  scrabble
//
//  Created by Mikaela Nicole Ramos on 7/27/24.
//

#include <stdlib.h>
#include <string.h>

#include "game_functions.h"

const int letterScores[27] = {1, 3, 3, 2, 1, 4, 2, 4, 1, 8, 5, 1, 3, 1, 1, 3, 10, 1, 1, 1, 1, 4, 4, 8, 4, 10, 0};
const int letterProbabilities[27] = {9, 11, 13, 17, 29, 31, 34, 36, 45, 46, 47, 51, 53, 59, 67, 69, 70, 76, 80, 86, 90, 92, 94, 95, 97, 98, 100};

bool isInsideBoard(PlayerTurn playerTurn){
    if (playerTurn.endX > 15 || playerTurn.endY > 15){
        printf("Word %s cannot fit the selected tiles, choose again\n", playerTurn.word);
        return false;
    }
    return true;
}

bool playerHasEnoughTiles(char letterNeeded, char *playerLetters, bool *letterUsed){
    bool letterFound = false;
    
    int blankTiles = 0, blankTileIndex = 0;
    for (int l = 0; l < LETTERS_ON_DECK; l++){
        if (letterNeeded == playerLetters[l] && !letterUsed[l]){
            *(letterUsed + l) = true;
            letterFound = true;
            break;
        }
        if (playerLetters[l] == '_' && !letterUsed[l]){
            blankTiles++;
            blankTileIndex = l;
        }
    }

    if (!letterFound && blankTiles > 0){
        *(letterUsed + blankTileIndex) = true;
        letterFound = true;
    }
    if(!letterFound) {
        printf("Missing a letter: %c\n", letterNeeded);
    }
        
    return letterFound;
}


bool isValidConnectedWord(int position, int newTilePosition, char* newLetters, int newLettersLength, bool isHorizontal){
    char connectedWord[15];
    int row = position, col = position;
    int newLetterIdx = 0;
    
    Tile** board = getBoard();
    for (int p = 0; p < BOARD_DIMENSION; p++){
        if (isHorizontal){
            col = p;
        } else {
            row = p;
        }
        
        if (p >= newTilePosition && p < newTilePosition + newLettersLength){
            strncat(connectedWord, &newLetters[newLetterIdx], 1);
            newLetterIdx += 1;
        }
        else if (board[row][col].letterPlaced == '\0') {
            if (p < newTilePosition)
                connectedWord[0] = '\0';
            else
                break;
        } else
            strncat(connectedWord, &board[row][col].letterPlaced, 1);
    }
    
    printf("Connected word found: %s\n", connectedWord);
    bool connectedWordExists = wordExists(connectedWord);

    return connectedWordExists;
}

bool connectedWordsAreValid(int r, int c, PlayerTurn playerTurn, int wordIdx, bool isHorizontal){
    bool validConnectedWords = true, isEndOfWord = wordIdx == playerTurn.wordLength-1, isStartOfWord = wordIdx == 0;
    char *horizontalConnect = &playerTurn.word[wordIdx], *verticalConnect = &playerTurn.word[wordIdx];
    int horizontalLettersLength = 1, verticalLettersLength = 1;
    
    Tile** board = getBoard();
    if (isEndOfWord || isStartOfWord){
        if (isHorizontal){
            horizontalConnect = playerTurn.word;
            horizontalLettersLength = playerTurn.wordLength;
        } else {
            verticalConnect = playerTurn.word;
            verticalLettersLength = playerTurn.wordLength;
        }
    }
    
    if ((isEndOfWord || isHorizontal) && (r < 14 && board[r+1][c].letterPlaced != '\0'))
        validConnectedWords &= isValidConnectedWord(c, r, verticalConnect, verticalLettersLength, false);
    if ((isStartOfWord || isHorizontal) && (r > 0 && board[r-1][c].letterPlaced != '\0'))
        validConnectedWords &= isValidConnectedWord(c, r, verticalConnect, verticalLettersLength, false);
    if ((isEndOfWord || !isHorizontal) && (c < 14 && board[r][c+1].letterPlaced != '\0'))
        validConnectedWords &= isValidConnectedWord(r, c, horizontalConnect, horizontalLettersLength, true);
    if ((isStartOfWord || !isHorizontal) && (c > 0 && board[r][c-1].letterPlaced != '\0'))
        validConnectedWords &= isValidConnectedWord(r, c, horizontalConnect, horizontalLettersLength, true);
    
    return validConnectedWords;
}

bool wordPositionIsValidAndConnected(PlayerTurn playerTurn){
    bool connected = false, validConnectedWords = true, enoughLetters = true;

    if (playerTurn.startX == 7 && playerTurn.startY == 7)
        connected = true;
    
    int wordIdx = 0;
    bool letterUsed[7] = {false};
    
    Tile** board = getBoard();
    for(int r = playerTurn.startX; r <= playerTurn.endX; r++){
        for (int c = playerTurn.startY; c <= playerTurn.endY; c++){
            if (board[r][c].letterPlaced != '\0'){
                if (board[r][c].letterPlaced != playerTurn.word[wordIdx]){
                    printf("Tiles in board do not match");
                    return false;
                }
                connected |= true;
            }
            else{
                enoughLetters &= playerHasEnoughTiles(playerTurn.word[wordIdx], playerTurn.player->letters, letterUsed);
                validConnectedWords &= connectedWordsAreValid(r, c, playerTurn, wordIdx, playerTurn.isHorizontal);
            }
            wordIdx++;
        }
    }
    
    return connected && validConnectedWords && enoughLetters;
}

bool canPlayThisMove(PlayerTurn playerTurn){
    return isInsideBoard(playerTurn) && wordPositionIsValidAndConnected(playerTurn);
    
    return false;
}

PlayerTurn placeTileAndGetScore(PlayerTurn *playerTurn){
    int wordMultiplier = 1;
    int score = 0, i = 0, letterMultiplier = 0;
    
    Tile** board = getBoard();
    if (canPlayThisMove(*playerTurn)){
        for (int x = playerTurn->startX; x <= playerTurn->endX; x++){
            for (int y = playerTurn->startY; y <= playerTurn->endY; y++){
                letterMultiplier = 1;
                char letter = playerTurn->word[i];
                
                if (board[x][y].letterPlaced == '\0'){
                    board[x][y].letterPlaced = letter;
                    wordMultiplier *= board[x][y].wordMultiplier;
                    letterMultiplier = board[x][y].letterMultiplier;
                    replaceLetter(playerTurn->player, letter);
                    playerTurn->tilesUsed++;
                }
                i++;
                score += (letterScores[letter - 65] * letterMultiplier);
            }
        }
        
        playerTurn->score = score * wordMultiplier;
    }
    setBoard(board);
    
    return *playerTurn;
}

PlayerTurn placeLetterTiles(PlayerTurn *playerTurn, bool isHorizontal){
    int wordLength = playerTurn->wordLength;
    playerTurn->endX = playerTurn->startX;
    playerTurn->endY = playerTurn->startY;
    
    if (!isHorizontal){
        playerTurn->endX += wordLength - 1;
    } else {
        playerTurn->endY += wordLength - 1;
    }
    
    playerTurn->isHorizontal = isHorizontal;
    return placeTileAndGetScore(playerTurn);
}

/*PlayerTurn getBestWord(void){
    return bestWord;
}

void setBestWord(PlayerTurn playerTurn){
    if (bestWord.score < playerTurn.score){
        bestWord = playerTurn;
    }
}*/

void updateBestWord(PlayerTurn playerTurn){
    if (playerTurn.score > playerTurn.player->bestWordScore){
        printf("Setting player's best word to: %s\n", playerTurn.word);
        playerTurn.player->bestWord = playerTurn.word;
        playerTurn.player->bestWordScore = playerTurn.score;
    }
}

void playTurn(Player* player, int* totalTilesDistributed){
    char word[7];
    int row, column;
    char direction;
    
    PlayerTurn playerTurn;
    playerTurn.player = player;
    playerTurn.tilesUsed = 0;

    do {
        printf("Enter your word, row and column position <word row column h|v>: ");
        scanf(" %s %d %d %c", word, &row, &column, &direction);
        
        if (wordExists(word)){
            playerTurn.word = word;
            playerTurn.startX = row - 1;
            playerTurn.startY = column - 1;
            playerTurn.wordLength = (int)strlen(word);
            playerTurn = placeLetterTiles(&playerTurn, direction == 'h');
        }
    } while (playerTurn.tilesUsed == 0);

    printf("Player %i scored %i with the word: %s\n", playerTurn.player->playerId, playerTurn.score, playerTurn.word);

    updateBestWord(playerTurn);
    player->totalScore += playerTurn.score;
    *totalTilesDistributed += playerTurn.tilesUsed;
}

char getRandomCharacter(void){
    int randomNumber = rand() % 100 + 1;
    for (int ind = 0; ind < 26; ind++){
        if (randomNumber < letterProbabilities[ind]){
            return ind + 65;
        }
    }
    return '_';
}

void initializePlayerLetters(Player* player){
    for (int c = 0; c<LETTERS_ON_DECK; c++){
        player->letters[c] = getRandomCharacter();
    }
}

void replaceLetter(Player* player, char letterUsed){
    for (int c = 0; c < LETTERS_ON_DECK; c++){
        if (player->letters[c] == letterUsed){
            player->letters[c] = getRandomCharacter();
            break;
        }
    }
}

