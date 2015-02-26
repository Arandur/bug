#include "Task.h"

#include <iostream>

class SampleTask :
  public Task {

public:

  SampleTask(const std::string& prgName) :
    Task(prgName) {}
  ~SampleTask() = default;

  virtual int run(int);
};

int SampleTask::run(int) {

  std::cout << prgName << std::endl;

  return 0;
}

std::unique_ptr<Task> Task::newTask(const std::string& prgName) {

  return std::make_unique<SampleTask>(prgName);
}
