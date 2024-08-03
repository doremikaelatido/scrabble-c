//
//  word_functions.c
//  scrabble
//
//  Created by Mikaela Nicole Ramos on 7/27/24.
//
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>

#include "word_functions.h"

#define MAXCHAR 1000

TrieNode trie;

void loadWordsToTrie(void){
    char cwd[MAXCHAR];
    /* use relative path
     if (getcwd(cwd, sizeof(cwd)) != NULL) {
        //printf("Current working dir: %s\n", cwd);
        DIR *d;
        struct dirent *dir;
        d = opendir("../Debug/scrabble");
          if (d) {
            while ((dir = readdir(d)) != NULL) {
              printf("%s\n", dir->d_name);
            }
            closedir(d);
          }
    }*/
    FILE *filePointer = fopen("/Users/mikaelanicoleramos/Documents/GitHub/scrabble/dictionary.csv","r");
    char row[MAXCHAR];

    trie = *initTrieNode('*');
    while (fgets(row, MAXCHAR, filePointer))
    {
        addToTrie(&trie, row);
    }
    printf("All words loaded. Program starting....\n");
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

TrieNode* initTrieNode(char letter){
    TrieNode *trieNode = malloc(sizeof(TrieNode) * LETTERS);
    for (int i=0; i < LETTERS; i++){
        trieNode->children[i] = NULL;
    }
    trieNode->letter = letter;
    trieNode->isLeaf = false;
    return trieNode;
}

void addToTrie(TrieNode *trie, char* word){
    TrieNode *temp = trie;
    for (int c = 0; word[c] != '\r' && word[c] != '\n' && word[c] != '\0'; c++){
        int letter = word[c] - 97;
        if (temp->children[letter] == NULL){
            temp->children[letter] = initTrieNode(word[c]);
        }
        temp = temp->children[letter];
    }
    temp->isLeaf = true;
}
