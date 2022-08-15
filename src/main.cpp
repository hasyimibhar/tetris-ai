#include <iostream>
#include "tetris.h"
#include "eltetris.h"
#include "randomizers.h"

int main(int argc, char **argv) {
  int seed = 0;
  if (argc > 1) {
    seed = atoi(argv[1]);
  }

  std::cout << "seed=" << seed << std::endl;

  Game game (seed, sevenBag);

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

  game.print();

  return 0;
}
