#include <cstdlib>
#include "yiyuan.h"
#include "ai.h"

int aggregateHeight(const Board &board);
int holesYy(const Board &board);
int bumpiness(const Board &board);

Evaluation evaluateBoardYy(const Board &board, PieceType piece, DropMove move) {
  auto updated = board;
  auto overallMove = updated.playMove(piece, move);
  if (!overallMove.valid()) return Evaluation::invalid();

  auto score =
    (aggregateHeight(updated) * -0.510066) +
    (overallMove.linesCleared * 0.760666) +
    (holesYy(updated) * -0.35663) +
    (bumpiness(updated) * -0.184483);

  return Evaluation(score);
}

DropMove yiyuan(const Board &board, PieceType piece) {
  double bestScore = -10000000000000000.0;
  auto bestMove = DropMove::invalid();

  enumerateMoves(board, piece,
      [&](DropMove move) {
        auto eval = evaluateBoardYy(board, piece, move);
        if (!eval.valid) return;

        if (eval.score > bestScore) {
          bestScore = eval.score;
          bestMove = move;
        }
      });

  return bestMove;
}

int aggregateHeight(const Board &board) {
  int aggregate = 0;

  for (int j = 0; j < board.width(); j++) {
    int height = 0;
    for (int i = 0; i < board.height(); i++) {
      if (board.cell(i, j) == 1) {
        height = board.height()-i;
        break;
      }
    }
    aggregate += height;
  }

  return aggregate;
}

int holesYy(const Board &board) {
  int holes = 0;

  for (int j = 0; j < board.width(); j++) {
    int block = 0;
    for (int i = 0; i < board.height(); i++) {
      if (board.cell(i, j) == 1) {
        block = 1;
      } else if (board.cell(i, j) == 0 && block) {
        holes++;
      }
    }
  }

  return holes;
}

int bumpiness(const Board &board) {
  int bumpiness = 0;

  int prev = 0;
  for (int j = 0; j < board.width(); j++) {
    int height = 0;
    for (int i = 0; i < board.height(); i++) {
      if (board.cell(i, j) == 1) {
        height = board.height()-i;
        break;
      }
    }

    if (j > 0) {
      bumpiness += abs(height-prev);
    }

    prev = height;
  }

  return bumpiness;
}
