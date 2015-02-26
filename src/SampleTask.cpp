#include "Task.h"

#include <cstdio>

class SampleTask :
  public Task {

public:

  SampleTask(const std::string& prgName) :
    Task(prgName) {}
  ~SampleTask() = default;

  virtual int run(int);
};

int SampleTask::run(int state) {

  printf("I'm in state %d!\n", state);

  return state == 0 ? 1 : 0;
}

std::unique_ptr<Task> Task::newTask(const std::string& prgName) {

  return std::make_unique<SampleTask>(prgName);
}
