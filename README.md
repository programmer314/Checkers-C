## Checkers AI (C)

A basic checkers command line game written in C. The player plays against a computer that calculates moves using the [minmax algorithm](https://en.wikipedia.org/wiki/Minimax).
This project was inspired by my friend who had the idea to use bits to represent squares on the board and bitwise operations to calculate moves, significantly improving the efficiency of the algorithm.
In `PlayerAI.c`, you will find several constants:
- DEPTH: This indicates how many moves ahead, or how deep, the algorithm will go when calculating minmax value
- ALPHABETA: Indicates whether [alpha-beta pruning](https://en.wikipedia.org/wiki/Alpha%E2%80%93beta_pruning) is enabled

The original program was written in C++, and is also available on my Github profile, but I rewrote it in C as my friend prefers it over C++.
