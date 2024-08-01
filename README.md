
# Scrabble in C

This is a simple Command Line Scrabble game for me to get used to the C syntax.

It closely mirrors the traditional Scrabble board game, as well as its mechanics.

It uses a a trie created using words.csv to check if a given word exists. It also has checking if all connected letters/words for a given turn exists.

It also implements a few structures that serve specific purposes:
- Player - information about the players of the game, such as tiles and current score
- PlayerTurn - information about the words played by the players, where they are positioned etc
- TrieNode - used for setting up the data structure to be used for efficient word search

## TODO
- input error handling
- enable skip once player has chosen to play
- ~~file segregation for functions~~
- stop distributing tiles once TOTAL_TILES exceeded - we only want to distribute up to the limit
- use relative path for words.csv

## What can be improved
- End of game 
    - display the highest scoring word of each player
    - ~~simple UI to show the ranking~~
- Improve location of variables/macros, file/folder hierarchies
- Tests? Already looked into gtest but I'm encountering errors - incompatible versions perhaps - something to investigate
- Improve letter distribution - don't rely on probabilities, in traditional scrabble, there are N tiles allocated to each letter

## Some learnings
- There are many possible reasons for a duplicate symbol error. What I personally encountered while working on this are the following:
    - circular dependency - A.h includes B.h, and B.h includes A.h
    - const variables in a header
- Here are some ways to share a variable outside a .c file
    - Getter/setter functions are also a thing in C
    - Initializing the variable in .c and using "extern" in its header file will expose this variable to others
- Never forget to include the header file in the .c file. It's good practice to always #include first one's own header file
