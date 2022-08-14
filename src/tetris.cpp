#include "tetris.h"

#include <string>
#include <array>
#include <vector>
#include <list>
#include <tuple>
#include <functional>
#include <random>
#include <algorithm>
#include <future>
#include <thread>
#include <cstdlib>
#include <iostream>

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

int Board::getDropRow(const Piece &piece, int col) const {
  int startRow = lastEmptyRow-piece.height+1 >= 0 ? lastEmptyRow-piece.height+1 : 0;

  // for (int row = 0; row < height-piece.height+1; row++) {
  for (int row = startRow; row < _height-piece.height+1; row++) {
    for (int i = 0; i < piece.height; i++) {
      if ((array[row + i] & (piece.array[i] << col)) != 0) return row-1;
    }
  }
  return _height-piece.height;
}

int Board::dropPiece(const Piece &piece, int col) {
  auto dropRow = getDropRow(piece, col);
  if (dropRow < 0) return -1;

  for (int i = 0; i < piece.height; i++) {
    array[dropRow + i] |= (piece.array[i] << col);
  }

  return dropRow;
}

int Board::clearLines() {
  int full = (1 << _width)-1;
  int linesCleared = 0;

  // Single pass, but requires copy

  std::array<uint16_t, 20> updated = {};
  int current = _height-1;

  for (int i = _height-1; i >= 0; i--) {
    if (array[i] == full) {
      linesCleared++;
      continue;
    }

    updated[current] = array[i];
    current--;
  }

  array = std::move(updated);

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

Move Board::playMove(const PieceSet &pieceSet, DropMove move) {
  if (move.rot >= pieceSet.rotations.size()) return Move::invalid();

  const auto &piece = pieceSet.rotations[move.rot];
  if (move.col + piece.width > _width) return Move::invalid();

  auto dropRow = dropPiece(piece, move.col);
  if (dropRow < 0) return Move::invalid();

  auto linesCleared = clearLines();
  return Move(dropRow, move.col, move.rot, linesCleared, piece.height);
}

void Board::print() {
  for (int i = 0; i < _height; i++) {
    for (int j = 0; j < _width; j++) {
      auto c = getCell(i, j);
      std::cout << (c == 1 ? 'O' : '.');
    }
    std::cout << std::endl;
  }
}

const std::array<PieceSet, 7> pieceSets = {
  PieceSet::I(), PieceSet::O(), PieceSet::T(),
  PieceSet::L(), PieceSet::J(),
  PieceSet::S(), PieceSet::Z(),
};

Game::TickResult Game::tick(PlayerFunc player) {
  if (bagIndex == 7) {
    generatePieces();
  }
  const auto &pieceSet = getNextPiece();

  // Create a copy to avoid cheating
  // auto boardCopy = board;
  // auto pieceSetCopy = pieceSet;
  // auto dropMove = player(boardCopy, pieceSetCopy);

  auto dropMove = player(board, pieceSet);

  if (!dropMove.valid()) {
    return GameOver;
  }

  auto move = board.playMove(pieceSet, dropMove);
  if (!move.valid()) {
    return GameOver;
  }

  lastMove = move;
  linesCleared += lastMove.linesCleared;
  pieces++;
  bagIndex++;

  return Ok;
}

void Game::generatePieces() {
  for (int i = 0; i < indices.size(); i++) indices[i] = i;
  std::shuffle(indices.begin(), indices.end(), rng);
  bagIndex = 0;
}

const PieceSet &Game::getNextPiece() {
  return pieceSets[bagIndex];
}

void Game::print() {
  std::cout << "pieces=" << pieces;
  std::cout << ", lines cleared=" << linesCleared << std::endl;

  board.print();
}
