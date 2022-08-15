#ifndef _TETRIS_H_
#define _TETRIS_H_

#include <array>
#include <vector>
#include <random>
#include <string>
#include <functional>
#include <map>

enum PieceType {
  I = 0,
  O, T,
  L, J,
  S, Z,
};

using PieceGenerator = std::function<PieceType()>;
using PieceRandomizer = std::function<PieceGenerator(int seed)>;

const std::map<PieceType, int> pieceRotations = {
  { PieceType::I, 2 },
  { PieceType::O, 1 },
  { PieceType::T, 4 },
  { PieceType::L, 4 },
  { PieceType::J, 4 },
  { PieceType::S, 2 },
  { PieceType::Z, 2 },
};

const std::map<std::pair<PieceType, int>, std::pair<int, int>> pieceSizes = {
  { { PieceType::I, 0 }, {4, 1} },
  { { PieceType::I, 1 }, {1, 4} },

  { { PieceType::O, 0 }, {2, 2} },

  { { PieceType::T, 0 }, {3, 2} },
  { { PieceType::T, 1 }, {2, 3} },
  { { PieceType::T, 2 }, {3, 2} },
  { { PieceType::T, 3 }, {2, 3} },

  { { PieceType::L, 0 }, {3, 2} },
  { { PieceType::L, 1 }, {2, 3} },
  { { PieceType::L, 2 }, {3, 2} },
  { { PieceType::L, 3 }, {2, 3} },

  { { PieceType::J, 0 }, {3, 2} },
  { { PieceType::J, 1 }, {2, 3} },
  { { PieceType::J, 2 }, {3, 2} },
  { { PieceType::J, 3 }, {2, 3} },

  { { PieceType::S, 0 }, {3, 2} },
  { { PieceType::S, 1 }, {2, 3} },

  { { PieceType::Z, 0 }, {3, 2} },
  { { PieceType::Z, 1 }, {2, 3} },
};

struct DropMove {
  int col, rot;

  DropMove(int col, int rot): col(col), rot(rot) {}

  static DropMove invalid() {
    return DropMove(-1, -1);
  }

  bool valid() const {
    return col >= 0;
  }
};

struct Move {
  PieceType piece;
  int row, col, rot, linesCleared, pieceHeight;

  Move(): piece(PieceType::I), row(-1), col(-1), rot(-1), linesCleared(0) {}
  Move(PieceType piece, int row, int col, int rot, int linesCleared)
    : piece(piece), row(row), col(col), rot(rot), linesCleared(linesCleared) {}

  static Move invalid() {
    return Move(PieceType::I, -1, -1, -1, 0);
  }

  bool valid() const {
    return row >= 0;
  }
};

class Board {
private:
  std::array<uint16_t, 20> array;
  int _width, _height;
  int lastEmptyRow;

  int getDropRow(PieceType piece, DropMove move) const;
  int dropPiece(PieceType piece, DropMove move);
  int clearLines();

public:
  Board(): array({}), _width(10), _height(20), lastEmptyRow(19) {}
  Board(const Board &b) = default;
  Board(Board &&b) = default;

  inline int width() const { return _width; }
  inline int height() const { return _height; }

  inline const int cell(int row, int col) const {
    return (array[row] >> col) & 1;
  }

  inline uint16_t row(int i) const { return array[i]; }

  Move playMove(PieceType piece, DropMove move);
  void print();
};

// Function signature for AI to implement.
// Given current state of the board, and a piece to place, return the move to play.
// DropMove is just a tuple of column and rotation.
using PlayerFunc = std::function<DropMove(const Board &, PieceType)>;

// Game is deterministic state machine.
// Given the same seed and randomizer, it should always produce the same sequence of pieces.
class Game {
private:
  std::default_random_engine rng;
  int seed;
  Board board;
  int linesCleared;
  int pieces;
  Move lastMove;
  PieceGenerator nextPiece;

public:
  enum TickResult { Ok, GameOver };

  Game(int seed, PieceRandomizer randomizer):
    rng(std::default_random_engine(seed)), seed(seed), nextPiece(randomizer(seed)),
    linesCleared(0), pieces(0) {}

  TickResult tick(PlayerFunc player);
  void print();
};

#endif
