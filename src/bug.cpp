#include "Task.h"

#include <cstdio>

/**
 * Expected arguments:
 *
 * ./bug 
 * ./bug [PREVIOUS FILE] [STATE NUMBER]
 */
int main(int argc, char* argv[]) {

  std::unique_ptr<Task> task = Task::newTask(argv[0]);

  int nextState, currState;
  std::string currFile, prevFile;

  if (argc == 1) {
    
    currState = 0;
    currFile = argv[0];
  } else if (argc == 3) {

    try {

      currState = std::stoi(argv[2]);
    } catch (const std::exception& e) {

      currState = 0;
    }
    currFile = argv[0];
    prevFile = argv[1];
  } else {

    fprintf(stderr, "Bad arguments: ");

    for (int i = 0; i < argc; ++i)
      fprintf(stderr, "\"%s\" ", argv[i]);

    fprintf(stderr, "\n");
  }

  nextState = task->run(currState);

  remove(prevFile.c_str());

  task->jump(currFile, nextState);
}
