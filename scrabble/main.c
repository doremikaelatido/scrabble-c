#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <time.h>

#define MAXCHAR 1000
#define LETTERS 26

const int PLAYER_LETTERS = 30;
const int LETTERS_ON_DECK = 7;
const int BOARD_DIMENSION = 15;

const int letterScores[27] = {1, 3, 3, 2, 1, 4, 2, 4, 1, 8, 5, 1, 3, 1, 1, 3, 10, 1, 1, 1, 1, 4, 4, 8, 4, 10, 0};
const int letterProbabilities[27] = {9, 11, 13, 17, 29, 31, 34, 36, 45, 46, 47, 51, 53, 59, 67, 69, 70, 76, 80, 86, 90, 92, 94, 95, 97, 98, 100};

int totalTilesDistributed = 0;

typedef struct player {
    int playerId;
    int totalScore;
    char letters[7];
} Player;

// can even expand this to do a recall of the highest scored word
typedef struct playerTurn {
    Player* player;
    int startX;
    int endX;
    int startY;
    int endY;
    char* word;
    int score;
    int tilesUsed;
    bool done;
    bool isHorizontal;
} PlayerTurn;


typedef struct tile {
    int x;
    int y;
    int wordMultiplier;
    int letterMultiplier;
    char letterPlaced;
} Tile;

typedef struct trieNode {
    struct trieNode* children[26];
    bool isLeaf;
    char letter;
} TrieNode;

Tile** board;
TrieNode trie;

TrieNode* initTrieNode(char letter){
    TrieNode *trieNode = malloc(sizeof(TrieNode) * LETTERS);
    for (int i=0; i < LETTERS; i++){
        trieNode->children[i] = NULL;
    }
    trieNode->letter = letter;
    trieNode->isLeaf = false;
    return trieNode;
}

void addToTrie(char* word){
    TrieNode *temp = &trie;
    for (int c = 0; word[c] != '\r' && word[c] != '\n' && word[c] != '\0'; c++){
        int letter = word[c] - 97;
        if (temp->children[letter] == NULL){
            temp->children[letter] = initTrieNode(word[c]);
        }
        temp = temp->children[letter];
    }
    temp->isLeaf = true;
}

bool wordExists(char* word){
    TrieNode temp = trie;
    
    for (int c = 0; word[c] != '\n' && word[c] != '\r' && word[c] != '\0'; c++){
        int letter = tolower(word[c]) - 97;
        if (temp.children[letter] != NULL){
            temp = *temp.children[letter];
        } else {
            printf("Word %s does not exist\n", word);
            return false;
        }
    }
    
    return temp.isLeaf;
}

void loadWords(){
    FILE *filePointer = fopen("/Users/mikaelanicoleramos/Documents/GitHub/scrabble/dictionary.csv","r");
    char row[MAXCHAR];

    trie = *initTrieNode('*');
    while (fgets(row, MAXCHAR, filePointer))
    {
        addToTrie(row);
    }
    printf("All words loaded. Program starting....\n");
}

void createBoard(){
    board = malloc(sizeof(Tile*) * BOARD_DIMENSION);
    for (int row = 0; row<BOARD_DIMENSION; row++){
        board[row] = malloc(sizeof(Tile) * BOARD_DIMENSION);
        for (int col = 0; col<BOARD_DIMENSION; col++){
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
}

void printBoard(){
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

char getRandomCharacter(){
    int randomNumber = rand() % 100 + 1;
    for (int ind = 0; ind < 26; ind++){
        if (randomNumber < letterProbabilities[ind]){
            return ind + 65;
        }
    }
    return '_';
}

void replaceLetter(Player* player, char letterUsed){
    for (int c = 0; c < LETTERS_ON_DECK; c++){
        if (player->letters[c] == letterUsed){
            player->letters[c] = getRandomCharacter();
            break;
        }
    }
}

void initializePlayerLetters(Player* player){
    for (int c = 0; c<LETTERS_ON_DECK; c++){
        player->letters[c] = getRandomCharacter();
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

bool connectedWordsAreValid(int r, int c, char *word, int wordIdx, bool isHorizontal){
    bool validConnectedWords = true, isEndOfWord = wordIdx == strlen(word)-1, isStartOfWord = wordIdx == 0;
    char *horizontalConnect = &word[wordIdx], *verticalConnect = &word[wordIdx];
    int horizontalLettersLength = 1, verticalLettersLength = 1;
    
    if (isEndOfWord || isStartOfWord){
        if (isHorizontal){
            horizontalConnect = word;
            horizontalLettersLength = strlen(word);
        } else {
            verticalConnect = word;
            verticalLettersLength = strlen(word);
        }
    }
    
    if ((isEndOfWord || isHorizontal) && (board[r+1][c].letterPlaced != '\0'))
        validConnectedWords &= isValidConnectedWord(c, r, verticalConnect, verticalLettersLength, false);
    if ((isStartOfWord || isHorizontal) && (board[r-1][c].letterPlaced != '\0'))
        validConnectedWords &= isValidConnectedWord(c, r, verticalConnect, verticalLettersLength, false);
    if ((isEndOfWord || !isHorizontal) && (board[r][c+1].letterPlaced != '\0'))
        validConnectedWords &= isValidConnectedWord(r, c, horizontalConnect, horizontalLettersLength, true);
    if ((isStartOfWord || !isHorizontal) && (board[r][c-1].letterPlaced != '\0'))
        validConnectedWords &= isValidConnectedWord(r, c, horizontalConnect, horizontalLettersLength, true);
    
    return validConnectedWords;
}

bool wordPositionIsValidAndConnected(PlayerTurn playerTurn){
    bool connected = false, validConnectedWords = true, enoughLetters = true;

    if (playerTurn.startX == 7 && playerTurn.startY == 7)
        connected = true;
    
    int wordIdx = 0;
    bool letterUsed[7] = {false};
    
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
                validConnectedWords &= connectedWordsAreValid(r, c, playerTurn.word, wordIdx, playerTurn.isHorizontal);
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
    
    if (canPlayThisMove(*playerTurn)){
        printf("Can play this turn for player %d with word %s\n", playerTurn->player->playerId, playerTurn->word);
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
    
    return *playerTurn;
}

PlayerTurn placeLetterTiles(PlayerTurn *playerTurn, bool isHorizontal){
    int wordLength = strlen(playerTurn->word);
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

void playTurn(Player* player){
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
            playerTurn = placeLetterTiles(&playerTurn, direction == 'h');
            printf("The player used %d tiles for the word %s\n", playerTurn.tilesUsed, playerTurn.word);
        }
    } while (playerTurn.tilesUsed == 0);
    
    player->totalScore += playerTurn.score;
    totalTilesDistributed += playerTurn.tilesUsed;
}
/**
 * returns the number of tiles used by the player
 */
bool play(Player* player){
    printPlayerLetters(*player);
    printf("Enter p to proceed, and s to skip:");
    char decision;
    scanf(" %c", &decision);
    
    if (decision == 'p'){
        playTurn(player);
        return true;
    }
    
    return false;
}

/**
 this is where all the program starts
 sha yung nagccall nung functionalities na kailangan gawin sunud sunod
 */
void printPlayerScores(Player* players, int numPlayers){
    printf("==================================SCOREBOARD=====================================\n");
    for (int n = 0; n<numPlayers; n++){
        printf("Player %d: %d       ", players[n].playerId, players[n].totalScore);
    }
    printf("\n=================================================================================\n");
}

int main() {
    loadWords();

    srand(time(NULL));

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

    createBoard();
    int nthPlayer = 0, skipCounter = 0, tilesDistributed = 0;
    while (totalTilesDistributed != 100 && skipCounter != numPlayers){
        printPlayerScores(players, numPlayers);
        printBoard();
        bool userPlayed = play(&players[nthPlayer]);
        if (!userPlayed){
            skipCounter += 1;
        } else {
            skipCounter = 0;
        }
        totalTilesDistributed += tilesDistributed;
        nthPlayer = (nthPlayer + 1) % numPlayers;
    }
    
    return 0;
}

//int canPlaceWord(char* word, int x, int y){
    
//}
/*
 returns the number of letters placed
 in turn, this will be the number of NEW letters for player to choose
 */
//int placeLetters(Player player, char* word, int x, int y){
    
//}

// read text file to get a list of "valid words"
int isValidWord(char* word){
    return 1;
}

//Letter* createLetters(){
//}

/** TODO:
  -
 - word checker functionality
 - read txt file
 - create a trie for words - efficient word checking
**/
