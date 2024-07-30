
# Scrabble in C

This is a simple Command Line Scrabble game for me to get used to the C syntax.

It closely mirrors the traditional Scrabble board game, as well as its mechanics.

It uses a a trie created using words.csv to check if a given word exists. It also has checking if all connected letters/words for a given turn exists.

## TODO
- error handling
- enable skip once player has chosen to play
- ~~file segregation for functions~~
- use relative path for words.csv

## What can be improved
- End of game 
    - display the highest scoring word of each player
    - simple UI to show the ranking

## Some learnings
- There are many possible reasons for a duplicate symbol error. What I personally encountered while working on this are the following:
    - circular dependency - A.h includes B.h, and B.h includes A.h
    - const variables in a header file
