#include "eltetris.h"
#include "ai.h"

double landingHeight(const Board &board, Move move);
int rowTransitions(const Board &board);
int colTransitions(const Board &board);
int holes(const Board &board);
int wellSums(const Board &board);

Evaluation evaluateBoard(const Board &board, PieceType piece, DropMove move) {
  auto updated = board;
  auto overallMove = updated.playMove(piece, move);
  if (!overallMove.valid()) return Evaluation::invalid();

  auto score =
    (overallMove.linesCleared * 3.4181268101392694) +
    (landingHeight(updated, overallMove) * -4.500158825082766) +
    (rowTransitions(updated) * -3.2178882868487753) +
    (colTransitions(updated) * -9.348695305445199) +
    (holes(updated) * -7.899265427351652) +
    (wellSums(updated) * -3.3855972247263626);

  return Evaluation(score);
}

DropMove elTetris(const Board &board, PieceType piece) {
  double bestScore = -10000000000000000.0;
  auto bestMove = DropMove::invalid();

  enumerateMoves(board, piece,
      [&](DropMove move) {
        auto eval = evaluateBoard(board, piece, move);
        if (!eval.valid) return;

        if (eval.score > bestScore) {
          bestScore = eval.score;
          bestMove = move;
        }
      });

  // Serial
  // for (int rot = 0; rot < pieceRotations.at(piece); rot++) {
  //   for (int col = 0; col < board.width()-pieceSizes.at({piece, rot}).first+1; col++) {
  //     auto move = DropMove(col, rot);

  //     auto eval = evaluateBoard(board, piece, move);
  //     if (!eval.valid) continue;

  //     if (eval.score > bestScore) {
  //       bestScore = eval.score;
  //       bestMove = move;
  //     }
  //   }
  // }

  // Threads

  // std::vector<std::tuple<double, bool>> results;
  // for (auto move : moves) {
  //   results.push_back(std::make_tuple(0.0, false));
  // }

  // std::vector<std::thread> threads;
  // for (int i = 0; i < moves.size(); i++) {
  //   auto move = moves[i];
  //   threads.push_back(std::thread([&results, i, board, pieceSet, move]() {
  //     results[i] = evaluateBoard(board, pieceSet, move);
  //   }));
  // }

  // for (auto &t : threads) t.join();

  // for (int i = 0 ; i < results.size(); i++) {
  //   auto result = results[i];
  //   if (!std::get<1>(result)) continue;

  //   auto score = std::get<0>(result);
  //   if (score > bestScore) {
  //     bestScore = score;
  //     bestMove = moves[i];
  //   }
  // }

  // Futures

  // std::vector<std::future<std::tuple<double, bool>>> results;
  // for (auto move : moves) {
  //   results.push_back(std::async(std::launch::async, evaluateBoard, board, pieceSet, move));
  // }

  // for (int i = 0 ; i < results.size(); i++) {
  //   auto &result = results[i];
  //   result.wait();

  //   auto val = result.get();

  //   if (!std::get<1>(val)) continue;

  //   auto score = std::get<0>(val);
  //   if (score > bestScore) {
  //     bestScore = score;
  //     bestMove = moves[i];
  //   }
  // }

  return bestMove;
}

double landingHeight(const Board &board, Move move) {
  int pieceHeight = pieceSizes.at({move.piece, move.rot}).second;
  return board.height()-move.row + ((pieceHeight-1)/2.0);
}

int rowTransitions(const Board &board) {
  int transitions = 0;
  int last = 1;
  int current = 0;

  for (int i = board.height()-1; i >= 0; i--) {
    for (int j = 0; j < board.width(); j++) {
      current = board.cell(i, j);
      if (current != last) transitions++;

      last = current;
    }

    if (current == 0) transitions++;
    last = 1;
  }

  return transitions;
}

int colTransitions(const Board &board) {
  int transitions = 0;
  int last = 1;

  for (int j = 0; j < board.width(); j++) {
    for (int i = board.height()-1; i >= 0; i--) {
      int current = board.cell(i, j);
      if (current != last) transitions++;

      last = current;
    }

    last = 1;
  }

  return transitions;
}

int holes(const Board &board) {
  int holes = 0;
  int rowHoles = 0b0000;

  int prev = board.row(0);
  for (int i = 1; i < board.height(); i++) {
    rowHoles = ~board.row(i) & (prev | rowHoles);

    for (int j = 0; j < board.width(); j++) {
      holes += ((rowHoles >> j) & 1);
    }

    prev = board.row(i);
  }

  return holes;
}

int wellSums(const Board &board) {
  int sums = 0;
  int current = 0;

  for (int j = 1; j < board.width()-1; j++) {
    for (int i = 0; i < board.height(); i++) {
      if (board.cell(i, j) == 0 &&
          board.cell(i, j-1) == 1 &&
          board.cell(i, j+1) == 1) {

          sums++;

          for (int k = i+1; k < board.height(); k++) {
            if (board.cell(k, j) == 0) sums++;
            else break;
          }
      }
    }
  }

  for (int i = 0; i < board.height(); i++) {
    if (board.cell(i, 0) == 0 && board.cell(i, 1) == 1) {
      sums++;

      for (int k = i+1; k < board.height(); k++) {
        if (board.cell(k, 0) == 0) sums++;
        else break;
      }
    }
  }

  for (int i = 0; i < board.height(); i++) {
    if (board.cell(i, board.width()-1) == 0 && board.cell(i, board.width()-2) == 1) {
      sums++;

      for (int k = i+1; k < board.height(); k++) {
        if (board.cell(k, board.width()-1) == 0) sums++;
        else break;
      }
    }
  }

  return sums;
}
