#include <iostream>
#include <map>

#include "tetris.h"
#include "randomizers.h"
#include "argparse.hpp"

#include "eltetris.h"
#include "yiyuan.h"

int main(int argc, char **argv) {
  const std::map<std::string, PlayerFunc> ai = {
    { "eltetris", elTetris },
    { "yiyuan", yiyuan },
  };

  const std::map<std::string, PieceRandomizer> randomizers = {
    { "uniform", uniform},
    { "nes", nes },
    { "nesApprox", nesApprox },
    { "7bag", sevenBag },
  };

  argparse::ArgumentParser program("tetris");

  program.add_argument("-a", "--ai")
    .default_value(std::string{"eltetris"})
    .help("AI to use");

  program.add_argument("-r", "--randomizer")
    .default_value(std::string{"7bag"})
    .help("randomizer to use");

  program.add_argument("-s", "--seed")
    .default_value(0)
    .help("RNG seed")
    .scan<'i', int>();

  program.add_argument("-p", "--pieces")
    .default_value(1000000)
    .help("Number of pieces to generate")
    .scan<'i', int>();

  try {
    program.parse_args(argc, argv);
  }
  catch (const std::runtime_error& err) {
    std::cerr << err.what() << std::endl;
    std::cerr << program;
    std::exit(1);
  }

  auto aiName = program.get<std::string>("--ai");
  if (ai.find(aiName) == ai.end()) {
    std::cerr << "invalid ai: " << aiName << std::endl;
    std::exit(1);
  }

  auto randomizerName = program.get<std::string>("--randomizer");
  if (randomizers.find(randomizerName) == randomizers.end()) {
    std::cerr << "invalid randomizer: " << randomizerName << std::endl;
    std::exit(1);
  }

  auto player = ai.at(aiName);
  auto randomizer = randomizers.at(randomizerName);
  auto seed = program.get<int>("--seed");
  auto pieces = program.get<int>("--pieces");

  std::cout << "ai=" << aiName << std::endl;
  std::cout << "randomizer=" << randomizerName << std::endl;
  std::cout << "seed=" << seed << std::endl;
  std::cout << "pieces=" << pieces << std::endl;
  std::cout << std::endl;

  Game game (seed, randomizer);

  auto step = pieces/10;

  for (int i = 0; i < pieces; i++) {
    if (game.tick(player) == Game::GameOver) break;
    const auto &stats = game.stats();
    if (stats.pieces > 0 && stats.pieces % step == 0) {
      std::cout << "pieces=" << stats.pieces;
      std::cout << ",lines_cleared=" << stats.linesCleared;

      std::cout << ",piece_frequency=";
      for (int p = 0; p < 7; p++) {
        std::cout << stats.pieceFrequency.at((PieceType)p) << ",";
      }

      std::cout << std::endl;
    }
  }

  game.print();

  return 0;
}
