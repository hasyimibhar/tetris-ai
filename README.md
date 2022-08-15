Various Tetris AI implemented in C++. The code has been fine-tuned to avoid unnecessary copying and memory allocations.

AI currently implemented:

- [El-Tetris](https://imake.ninja/el-tetris-an-improvement-on-pierre-dellacheries-algorithm/)

## Tetris rules used

Tetris varies a lot between implementations. The following rules are used:

- 10x20 board
- 7-bag random generator with uniform piece distribution
- 1 piece at a time, 0 piece lookahead, no holding piece
- Piece drops straight down, so moves like T-spin, L-spin, and wall kicks are not allowed. Essentially the game is turn-based and not "real-time".
- Piece starts from row 0 (some implementations use hidden row 21-23)
- Game is over when piece overflows row 0

## Usage

Compile:

```sh
$ make
```

Then run (`123` is the RNG seed used to generate the pieces):

```sh
$ bin/tetris 123
```

Sample output:

```
seed=123
100000
200000
300000
400000
500000
600000
700000
800000
900000
pieces=1000000, lines cleared=399998
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
....O.....
.O..OO....
OOOOOOOO..
OOOOOOOO..
```
