#ifndef _TETRIS_H_
#define _TETRIS_H_

#include <array>
#include <vector>
#include <random>
#include <string>
#include <functional>

struct Piece {
  std::vector<uint16_t> array;
  int width, height;

  Piece(const std::vector<uint16_t> &array, int width, int height)
    : array(array), width(width), height(height) {}

  Piece(const Piece &p) = default;
  Piece(Piece &&p) = default;

  Piece& operator=(Piece other) {
    array.assign(other.array.begin(), other.array.end());
    width = other.width;
    height = other.height;
    return *this;
  }
};

struct PieceSet {
  std::string name;
  std::vector<Piece> rotations;

  PieceSet() {}

  PieceSet(
      const std::string &name, 
      const std::vector<Piece> &rotations)
    : name(name), rotations(rotations) {}

  PieceSet(const PieceSet &p) = default;
  PieceSet(PieceSet &&p) = default;

  PieceSet& operator=(const PieceSet &other) {
    name = other.name;
    rotations.assign(other.rotations.begin(), other.rotations.end());
    return *this;
  }

  static PieceSet I();
  static PieceSet O();
  static PieceSet T();
  static PieceSet L();
  static PieceSet J();
  static PieceSet S();
  static PieceSet Z();
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
  int row, col, rot, linesCleared, pieceHeight;

  Move(): row(-1), col(-1), rot(-1), linesCleared(0), pieceHeight(0) {}
  Move(int row, int col, int rot, int linesCleared, int pieceHeight)
    : row(row), col(col), rot(rot), linesCleared(linesCleared), pieceHeight(pieceHeight) {}

  static Move invalid() {
    return Move(-1, -1, -1, 0, 0);
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

  int getDropRow(const Piece &piece, int col) const;
  int dropPiece(const Piece &piece, int col);
  int clearLines();

public:
  Board(): array({}), _width(10), _height(20), lastEmptyRow(19) {}
  Board(const Board &b) = default;
  Board(Board &&b) = default;

  Board& operator=(Board other) {
    array = other.array;
    _width = other._width;
    _height = other._height;
    return *this;
  }

  inline int width() const { return _width; }
  inline int height() const { return _height; }

  inline const int getCell(int row, int col) const {
    return (array[row] >> col) & 1;
  }

  inline uint16_t row(int i) const { return array[i]; }

  Move playMove(const PieceSet &pieceSet, DropMove move);
  void print();
};

// Function signature for AI to implement.
// Given current state of the board, and a piece to place, return the move to play.
// DropMove is just a tuple of column and rotation.
using PlayerFunc = std::function<DropMove(const Board &, const PieceSet &)>;

class Game {
private:
  std::default_random_engine rng;
  int seed;
  Board board;
  int bagIndex;
  int linesCleared;
  int pieces;
  Move lastMove;
  std::array<int, 7> indices;

  void generatePieces();
  const PieceSet &getNextPiece();

public:
  enum TickResult { Ok, GameOver };

  Game(int seed):
    rng(std::default_random_engine(seed)), seed(seed), linesCleared(0), pieces(0), indices({0, 1, 2, 3, 4, 5, 6}), bagIndex(7) {}

  TickResult tick(PlayerFunc player);
  void print();
};

#endif
