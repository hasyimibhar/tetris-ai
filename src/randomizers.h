#ifndef _RANDOMIZERS_H_
#define _RANDOMIZERS_H_

#include "tetris.h"

// This basically generates a random number between 0 and 6, and use that
// as an index to a lookup table.
PieceGenerator uniform(int seed);

// Randomizer used by NES Tetris, which uses LFSR to generate random numbers.
PieceGenerator nes(int seed);

// Randomizer used by NES Tetris, but approximated as first-order Markov process.
PieceGenerator nesApprox(int seed);

// All 7 pieces are randomly shuffled inside a bag.
// This randomizer produces the most uniform distribution.
PieceGenerator sevenBag(int seed);

#endif
