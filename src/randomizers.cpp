#include "randomizers.h"
#include <random>
#include <array>
#include <algorithm>

struct state {
  std::default_random_engine rng;
  std::array<PieceType, 7> pieces;
  int bagIndex;

  state(int seed):
    rng(std::default_random_engine(seed)),
    pieces({I, O, T, L, J, S, Z}),
    bagIndex(7) {}

  void generate() {
    pieces[0] = I; pieces[1] = O; pieces[2] = T;
    pieces[3] = L; pieces[4] = J;
    pieces[5] = S; pieces[6] = Z;

    std::shuffle(pieces.begin(), pieces.end(), rng);
    bagIndex = 0;
  }

  PieceType nextPiece() {
    if (bagIndex == 7) generate();
    return pieces[bagIndex++];
  }
};

PieceGenerator sevenBag(int seed) {
  return [s = state(seed)]() mutable -> PieceType {
    return s.nextPiece();
  };
}
