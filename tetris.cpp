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

  static PieceSet I() {
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

  static PieceSet O() {
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

  static PieceSet T() {
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

  static PieceSet L() {
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

  static PieceSet J() {
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

  static PieceSet S() {
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

  static PieceSet Z() {
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

struct Board {
  std::array<uint16_t, 20> array;
  int width, height;
  int lastEmptyRow;

  Board(): array({}), width(10), height(20), lastEmptyRow(19) {}
  Board(const Board &b) = default;
  Board(Board &&b) = default;

  Board& operator=(Board other) {
    array = other.array;
    width = other.width;
    height = other.height;
    return *this;
  }

  inline const int getCell(int row, int col) const {
    return (array[row] >> col) & 1;
  }

  int getDropRow(const Piece &piece, int col) const {
    int startRow = lastEmptyRow-piece.height+1 >= 0 ? lastEmptyRow-piece.height+1 : 0;

    // for (int row = 0; row < height-piece.height+1; row++) {
    for (int row = startRow; row < height-piece.height+1; row++) {
      for (int i = 0; i < piece.height; i++) {
        if ((array[row + i] & (piece.array[i] << col)) != 0) return row-1;
      }
    }
    return height-piece.height;
  }

  int dropPiece(const Piece &piece, int col) {
    auto dropRow = getDropRow(piece, col);
    if (dropRow < 0) return -1;

    for (int i = 0; i < piece.height; i++) {
      array[dropRow + i] |= (piece.array[i] << col);
    }

    return dropRow;
  }

  int clearLines() {
    int full = (1 << width)-1;
    int linesCleared = 0;

    // Single pass, but requires copy

    std::array<uint16_t, 20> updated = {};
    int current = height-1;

    for (int i = height-1; i >= 0; i--) {
      if (array[i] == full) {
        linesCleared++;
        continue;
      }

      updated[current] = array[i];
      current--;
    }

    array = std::move(updated);

    lastEmptyRow = height-1;
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

  Move playMove(const PieceSet &pieceSet, DropMove move) {
    if (move.rot >= pieceSet.rotations.size()) return Move::invalid();

    const auto &piece = pieceSet.rotations[move.rot];
    if (move.col + piece.width > width) return Move::invalid();

    auto dropRow = dropPiece(piece, move.col);
    if (dropRow < 0) return Move::invalid();

    auto linesCleared = clearLines();
    return Move(dropRow, move.col, move.rot, linesCleared, piece.height);
  }

  void setCell(int row, int col, uint8_t b) {
    array[row] |= (b > 0) << col;
  }

  void print() {
    for (int i = 0; i < height; i++) {
      for (int j = 0; j < width; j++) {
        auto c = getCell(i, j);
        std::cout << (c == 1 ? 'O' : '.');
      }
      std::cout << std::endl;
    }
  }
};

using PlayerFunc = std::function<DropMove(const Board &, const PieceSet &)>;

const std::array<PieceSet, 7> pieceSets = {
  PieceSet::I(), PieceSet::O(), PieceSet::T(),
  PieceSet::L(), PieceSet::J(),
  PieceSet::S(), PieceSet::Z(),
};

struct Game {
  enum TickResult { Ok, GameOver };

  std::default_random_engine rng;
  int seed;
  Board board;
  int bagIndex;
  int linesCleared;
  int pieces;
  Move lastMove;
  std::array<int, 7> indices;

  Game(int seed):
    rng(std::default_random_engine(seed)), seed(seed), linesCleared(0), pieces(0), indices({0, 1, 2, 3, 4, 5, 6}), bagIndex(7) {}

  TickResult tick(PlayerFunc player) {
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

  void generatePieces() {
    for (int i = 0; i < indices.size(); i++) indices[i] = i;
    std::shuffle(indices.begin(), indices.end(), rng);
    bagIndex = 0;
  }

  inline const PieceSet &getNextPiece() {
    return pieceSets[bagIndex];
  }
};

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

double landingHeight(const Board &board, Move move) {
  return board.height-move.row + ((move.pieceHeight-1)/2.0);
}

int rowTransitions(const Board &board) {
  int transitions = 0;
  int last = 1;
  int current = 0;

  for (int i = board.height-1; i >= 0; i--) {
    for (int j = 0; j < board.width; j++) {
      current = board.getCell(i, j);
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

  for (int j = 0; j < board.width; j++) {
    for (int i = board.height-1; i >= 0; i--) {
      int current = board.getCell(i, j);
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

  int prev = board.array[0];
  for (int i = 1; i < board.height; i++) {
    rowHoles = ~board.array[i] & (prev | rowHoles);

    for (int j = 0; j < board.width; j++) {
      holes += ((rowHoles >> j) & 1);
    }

    prev = board.array[i];
  }

  return holes;
}

int wellSums(const Board &board) {
  int sums = 0;
  int current = 0;

  for (int j = 1; j < board.width-1; j++) {
    for (int i = 0; i < board.height; i++) {
      if (board.getCell(i, j) == 0 &&
          board.getCell(i, j-1) == 1 &&
          board.getCell(i, j+1) == 1) {

          sums++;

          // for (int k = i+1; k < board.height; k++) {
          //   if (board.getCell(k, j) == 0) sums++;
          //   else break;
          // }
      }
    }
  }

  for (int i = 0; i < board.height; i++) {
    if (board.getCell(i, 0) == 0 && board.getCell(i, 1) == 1) {
      sums++;

      // for (int k = i+1; k < board.height; k++) {
      //   if (board.getCell(k, 0) == 0) sums++;
      //   else break;
      // }
    }
  }

  for (int i = 0; i < board.height; i++) {
    if (board.getCell(i, board.width-1) == 0 && board.getCell(i, board.width-2) == 1) {
      sums++;

      // for (int k = i+1; k < board.height; k++) {
      //   if (board.getCell(k, board.width-1) == 0) sums++;
      //   else break;
      // }
    }
  }

  return sums;
}

struct Evaluation {
  double score;
  bool valid;

  Evaluation(double score): score(score), valid(true) {}
  Evaluation(double score, bool valid): score(score), valid(valid) {}

  static Evaluation invalid() {
    return Evaluation(0.0, false);
  }
};

Evaluation evaluateBoard(const Board &board, const PieceSet &pieceSet, DropMove move) {
  auto updated = board;
  auto overallMove = updated.playMove(pieceSet, move);
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

DropMove elTetris(const Board &board, const PieceSet &pieceSet) {
  double bestScore = -10000000000000000.0;
  auto bestMove = DropMove::invalid();

  // Serial
  for (int rot = 0; rot < pieceSet.rotations.size(); rot++) {
    const auto &piece = pieceSet.rotations[rot];
    for (int col = 0; col < board.width-piece.width+1; col++) {
      auto move = DropMove(col, rot);

      auto eval = evaluateBoard(board, pieceSet, move);
      if (!eval.valid) continue;

      if (eval.score > bestScore) {
        bestScore = eval.score;
        bestMove = move;
      }
    }
  }

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

int main(int argc, char **argv) {
  int seed = 0;
  if (argc > 1) {
    seed = atoi(argv[1]);
  }

  std::cout << "seed=" << seed << std::endl;

  Game game (seed);

  // auto player = dummy;

  // auto rng = std::default_random_engine(seed);
  // auto player = randomDummy(rng);

  auto player = elTetris;

  for (int i = 0; i < 1000000; i++) {
    if (game.tick(player) == Game::GameOver) break;
    if (i > 0 && i % 100000 == 0) {
      std::cout << i << std::endl;
    }
  }

  std::cout << "pieces=" << game.pieces;
  std::cout << ", lines cleared=" << game.linesCleared << std::endl;

  game.board.print();

  return 0;
}
