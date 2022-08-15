#include "randomizers.h"
#include <random>
#include <array>
#include <algorithm>

const std::array<PieceType, 7> allPieces = {I, O, T, L, J, S, Z};

struct sevenBagState {
  std::default_random_engine rng;
  std::array<PieceType, 7> pieces;
  int bagIndex;

  sevenBagState(int seed):
    rng(std::default_random_engine(seed)),
    pieces(allPieces),
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

PieceGenerator original(int seed) {
  return [rng = std::default_random_engine(seed)]() mutable -> PieceType {
    int i = std::uniform_int_distribution<int>(0, 6)(rng);
    return allPieces[i];
  };
}

PieceGenerator sevenBag(int seed) {
  return [s = sevenBagState(seed)]() mutable -> PieceType {
    return s.nextPiece();
  };
}
