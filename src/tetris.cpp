#include "tetris.h"

#include <string>
#include <array>
#include <vector>
#include <list>
#include <tuple>
#include <functional>
#include <random>
#include <algorithm>
#include <map>
#include <iostream>

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

const std::map<PieceType, PieceSet> pieceSets = {
  { PieceType::I, PieceSet::I() },
  { PieceType::O, PieceSet::O() },
  { PieceType::T, PieceSet::T() },
  { PieceType::L, PieceSet::L() },
  { PieceType::J, PieceSet::J() },
  { PieceType::S, PieceSet::S() },
  { PieceType::Z, PieceSet::Z() },
};

int Board::getDropRow(PieceType pieceType, DropMove move) const {
  const auto &piece = pieceSets.at(pieceType).rotations[move.rot];

  int startRow = lastEmptyRow-piece.height+1 >= 0 ? lastEmptyRow-piece.height+1 : 0;

  // for (int row = 0; row < height-piece.height+1; row++) {
  for (int row = startRow; row < _height-piece.height+1; row++) {
    for (int i = 0; i < piece.height; i++) {
      if ((array[row + i] & (piece.array[i] << move.col)) != 0) return row-1;
    }
  }
  return _height-piece.height;
}

int Board::dropPiece(PieceType pieceType, DropMove move) {
  auto dropRow = getDropRow(pieceType, move);
  if (dropRow < 0) return -1;

  const auto &piece = pieceSets.at(pieceType).rotations[move.rot];

  for (int i = 0; i < piece.height; i++) {
    array[dropRow + i] |= (piece.array[i] << move.col);
  }

  return dropRow;
}

int Board::clearLines() {
  int full = (1 << _width)-1;
  int linesCleared = 0;

  // Single pass, but requires copy

  int current = _height-1;

  for (int i = _height-1; i >= 0; i--) {
    if (array[i] == 0) break;
    if (array[i] == full) {
      linesCleared++;
      continue;
    }

    arrayCopy[current] = array[i];
    current--;
  }
  while (current >= 0) arrayCopy[current--] = 0;

  std::swap(array, arrayCopy);

  lastEmptyRow = _height-1;
  while (lastEmptyRow >= 0 && array[lastEmptyRow] > 0) lastEmptyRow--;

  // Avoid copying

  // for (int i = 0; i < height; i++) {
  //   if (array[i] < full) continue;

  //   linesCleared++;
  //   for (int j = i; j > 0; j--) {
  //     array[j] = array[j-1];
  //   }
  //   array[0] = 0;
  // }

  return linesCleared;
}

Move Board::playMove(PieceType pieceType, DropMove move) {
  const auto &pieceSet = pieceSets.at(pieceType);
  if (move.rot >= pieceSet.rotations.size()) return Move::invalid();

  const auto &piece = pieceSet.rotations[move.rot];
  if (move.col + piece.width > _width) return Move::invalid();

  auto dropRow = dropPiece(pieceType, move);
  if (dropRow < 0) return Move::invalid();

  auto linesCleared = clearLines();
  return Move(pieceType, dropRow, move.col, move.rot, linesCleared);
}

void Board::print() {
  for (int i = 0; i < _height; i++) {
    for (int j = 0; j < _width; j++) {
      auto c = cell(i, j);
      std::cout << (c == 1 ? 'O' : '.');
    }
    std::cout << std::endl;
  }
}

Game::TickResult Game::tick(PlayerFunc player) {
  auto piece = nextPiece();
  // std::cout << piece << std::endl;

  // Create a copy to avoid cheating
  // auto boardCopy = board;
  // auto dropMove = player(boardCopy, piece);

  auto dropMove = player(board, piece);

  if (!dropMove.valid()) {
    return GameOver;
  }

  auto move = board.playMove(piece, dropMove);
  if (!move.valid()) {
    return GameOver;
  }

  lastMove = move;
  linesCleared += lastMove.linesCleared;
  pieces++;

  return Ok;
}

void Game::print() {
  std::cout << "pieces=" << pieces;
  std::cout << ", lines cleared=" << linesCleared << std::endl;

  board.print();
}

PieceSet PieceSet::I() {
  return PieceSet(
    std::string("I"),
    {
      Piece(
        {
          0b1111,
        },
        4, 1),
      Piece(
        {
          0b1,
          0b1,
          0b1,
          0b1,
        },
        1, 4),
    });
}

PieceSet PieceSet::O() {
  return PieceSet(
    std::string("O"),
    {
      Piece(
        {
          0b11,
          0b11,
        },
        2, 2),
    });
}

PieceSet PieceSet::T() {
  return PieceSet(
    std::string("T"),
    {
      Piece(
        {
          0b111,
          0b010,
        },
        3, 2),
      Piece(
        {
          0b01,
          0b11,
          0b01,
        },
        2, 3),
      Piece(
        {
          0b010,
          0b111,
        },
        3, 2),
      Piece(
        {
          0b10,
          0b11,
          0b10,
        },
        2, 3),
    });
}

PieceSet PieceSet::L() {
  return PieceSet(
    std::string("L"),
    {
      Piece(
        {
          0b001,
          0b111,
        },
        3, 2),
      Piece(
        {
          0b10,
          0b10,
          0b11,
        },
        2, 3),
      Piece(
        {
          0b111,
          0b100,
        },
        3, 2),
      Piece(
        {
          0b11,
          0b01,
          0b01,
        },
        2, 3),
    });
}

PieceSet PieceSet::J() {
  return PieceSet(
    std::string("J"),
    {
      Piece(
        {
          0b100,
          0b111,
        },
        3, 2),
      Piece(
        {
          0b11,
          0b10,
          0b10,
        },
        2, 3),
      Piece(
        {
          0b111,
          0b001,
        },
        3, 2),
      Piece(
        {
          0b01,
          0b01,
          0b11,
        },
        2, 3),
    });
}

PieceSet PieceSet::S() {
  return PieceSet(
    std::string("S"),
    {
      Piece(
        {
          0b011,
          0b110,
        },
        3, 2),
      Piece(
        {
          0b10,
          0b11,
          0b01,
        },
        2, 3),
    });
}

PieceSet PieceSet::Z() {
  return PieceSet(
    std::string("Z"),
    {
      Piece(
        {
          0b110,
          0b011,
        },
        3, 2),
      Piece(
        {
          0b01,
          0b11,
          0b10,
        },
        2, 3),
    });
}
