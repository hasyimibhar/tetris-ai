Various Tetris AI implemented in C++. The code has been fine-tuned to avoid unnecessary copying and memory allocations.

## Tetris rules used

Tetris varies a lot between implementations. The following rules are used:

- 10x20 board
- 1 piece at a time, 0 piece lookahead, no holding piece
- Piece drops straight down, so moves like T-spin, L-spin, and wall kicks are not allowed. Essentially the game is turn-based and not "real-time".
- Piece starts from row 0 (some implementations use hidden row 21-23)
- Game is over when piece overflows row 0

## Randomizers

- `uniform`: Randomly pick any of the 7 piece using a uniform PRNG
- `7bag`: The Random Generator
- `nes`: NES randomizer, but LFSR is only scrambled on piece generation (in original NES Tetris, LFSR is scrambled many times in between spawns)
- `nesApprox`: NES randomizer, but approximated as first-order Markov process

## AI

- `eltetris`: [El-Tetris](https://imake.ninja/el-tetris-an-improvement-on-pierre-dellacheries-algorithm/)
- `yiyuan`: [The (Near) Perfect Bot](https://codemyroad.wordpress.com/2013/04/14/tetris-ai-the-near-perfect-player/)

## Usage

Compile:

```sh
$ make
```

```
Usage: tetris [options]

Optional arguments:
-h --help       	shows help message and exits [default: false]
-v --version    	prints version information and exits [default: false]
-a --ai         	AI to use [default: "eltetris"]
-r --randomizer 	randomizer to use [default: "7bag"]
-s --seed       	RNG seed [default: 0]
-p --pieces     	Number of pieces to generate [default: 1000000]
```

Example:

```sh
$ bin/tetris -a eltetris -randomizer 7bag -s 123 -p 100000
ai=eltetris
randomizer=7bag
seed=123
pieces=100000

10000
20000
30000
40000
50000
60000
70000
80000
90000
pieces=100000, lines cleared=39999
..........
..........
..........
..........
..........
..........
..........
..........
..........
..........
..........
..........
..........
..........
..........
..........
..........
..........
.O..OO....
OOOOOO.O..
```
