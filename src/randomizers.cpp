#include "randomizers.h"
#include <random>
#include <array>
#include <algorithm>
#include <map>
#include <iostream>

const std::array<PieceType, 7> allPieces = {I, O, T, L, J, S, Z};

PieceGenerator uniform(int seed) {
  return [rng = std::default_random_engine(seed)]() mutable -> PieceType {
    int i = std::uniform_int_distribution<int>(0, 6)(rng);
    return allPieces[i];
  };
}

inline uint16_t nextRandomNumber(uint16_t value) {
  return ((((value >> 9) & 1) ^ ((value >> 1) & 1)) << 15) | (value >> 1);
}

const std::array<PieceType, 7> nesSpawnTable = {T, J, Z, O, S, L, I};
const std::array<uint8_t, 7> nesSpawnOrientationTable = {0x02, 0x07, 0x08, 0x0a, 0x0b, 0x0e, 0x12};

struct nesState {
  uint16_t rand;
  uint8_t spawnCount;
  int prevSpawnId;

  nesState(int seed): rand(nextRandomNumber(seed % 32767)), spawnCount(0), prevSpawnId(0) {}

  PieceType nextPiece() {
    uint8_t acc;
    int newSpawnId;
    PieceType piece;

    rand = nextRandomNumber(rand);
    spawnCount++;

    acc = (rand >> 8);
    acc += spawnCount;
    acc &= 7;

    if (acc == 7) goto invalidIndex;

    piece = nesSpawnTable[acc];
    newSpawnId = (int)nesSpawnOrientationTable[acc];
    if (newSpawnId != prevSpawnId) {
      goto useNewSpawnId;
    }

invalidIndex:
    rand = nextRandomNumber(rand);
    acc = (rand >> 8);
    acc &= 7;
    acc += prevSpawnId;
    acc %= 7;

    piece = nesSpawnTable[acc];
    newSpawnId = (int)nesSpawnOrientationTable[acc];

useNewSpawnId:
    prevSpawnId = newSpawnId;
    //std::cout << newSpawnId << std::endl;
    return piece;
  }
};

PieceGenerator nes(int seed) {
  return [s = nesState(seed)]() mutable -> PieceType {
    return s.nextPiece();
  };
}

// NES piece randomizer approximated as a first-order Markov process.

const std::map<PieceType, std::map<PieceType, double>> nesTransitionMatrix = {
  { I, { { I, 0.032 }, { J, 0.155 }, { L, 0.156 }, { O, 0.156 }, { S, 0.190 }, { T, 0.155 }, { Z, 0.156 } } },
  { J, { { I, 0.156 }, { J, 0.032 }, { L, 0.155 }, { O, 0.156 }, { S, 0.157 }, { T, 0.187 }, { Z, 0.158 } } },
  { L, { { I, 0.156 }, { J, 0.157 }, { L, 0.032 }, { O, 0.157 }, { S, 0.156 }, { T, 0.186 }, { Z, 0.157 } } },
  { O, { { I, 0.157 }, { J, 0.156 }, { L, 0.158 }, { O, 0.061 }, { S, 0.157 }, { T, 0.157 }, { Z, 0.155 } } },
  { S, { { I, 0.156 }, { J, 0.157 }, { L, 0.157 }, { O, 0.157 }, { S, 0.063 }, { T, 0.156 }, { Z, 0.155 } } },
  { T, { { I, 0.156 }, { J, 0.157 }, { L, 0.157 }, { O, 0.156 }, { S, 0.156 }, { T, 0.031 }, { Z, 0.187 } } },
  { Z, { { I, 0.158 }, { J, 0.187 }, { L, 0.157 }, { O, 0.155 }, { S, 0.155 }, { T, 0.157 }, { Z, 0.032 } } },
};

PieceGenerator nesApprox(int seed) {
  auto rng = std::default_random_engine(seed);
  PieceType start = allPieces[std::uniform_int_distribution<int>(0, 6)(rng)];

  return [rng = std::move(rng), prev = start]() mutable -> PieceType {
    auto p = std::uniform_real_distribution<double>(0, 1)(rng);
    double current = 0;

    PieceType next = (PieceType)6;
    for (int i = 0; i < 6; i++) {
      current += nesTransitionMatrix.at((PieceType)i).at(prev);
      if (p < current) {
        next = (PieceType)i;
        break;
      }
    }

    prev = next;
    return next;
  };
}

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

PieceGenerator sevenBag(int seed) {
  return [s = sevenBagState(seed)]() mutable -> PieceType {
    return s.nextPiece();
  };
}
