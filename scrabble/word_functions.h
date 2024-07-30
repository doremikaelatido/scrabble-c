//
//  word_functions.h
//  scrabble
//
//  Created by Mikaela Nicole Ramos on 7/27/24.
//

#ifndef word_functions_h
#define word_functions_h

#include <stdio.h>
#include <stdbool.h>

#define LETTERS 26

typedef struct trieNode {
    struct trieNode* children[LETTERS];
    bool isLeaf;
    char letter;
} TrieNode;

void loadWordsToTrie(void);
bool wordExists(char* word);
TrieNode* initTrieNode(char letter);
void addToTrie(TrieNode *trie, char* word);

#endif /* word_functions_h */
