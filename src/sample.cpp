#include <iostream>
#include <random>

int runTask(int state, std::default_random_engine& g) {

  std::uniform_int_distribution<int> stateDist(0, 10);

  std::cout << "I am in state " << state << std::endl;

  return stateDist(g);
}
