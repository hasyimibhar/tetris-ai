#ifndef _DUMMY_H_
#define _DUMMY_H_

#include "tetris.h"
#include <random>

DropMove dummy(const Board &board, const PieceSet &pieceSet) {
  return DropMove(0, 0);
}

PlayerFunc randomDummy(std::default_random_engine &rng) {
  return [&rng](const Board &board, const PieceSet &pieceSet) -> DropMove {
    int rot = std::uniform_int_distribution<int>(0, pieceSet.rotations.size()-1)(rng);
    const auto &piece = pieceSet.rotations[rot];

    int col = std::uniform_int_distribution<int>(0, board.width-piece.width)(rng);
    return DropMove(col, rot);
  };
}

#endif
