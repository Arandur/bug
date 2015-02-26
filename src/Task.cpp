#include "Task.h"
#include "name.h"
#include "dir.h"
#include "util.h"

#include <iostream>
#include <fstream>
#include <iterator>
#include <algorithm>
#include <functional>
#include <unistd.h>
#include <sys/stat.h>
#include <limits.h>
#include <libgen.h>
#include <stdlib.h>
#include <string.h>

Task::Task(const std::string& prgName) :
  prgName([prgName = prgName] {
      
    char path[PATH_MAX];
    
    char* abspath = realpath(prgName.c_str(), path);
    
    // TODO: Check abspath for null

    return abspath;
  }()),
  g(std::random_device{}()),
  prg([prgName = prgName] {

    struct stat st;
    stat(prgName.c_str(), &st);

    std::ifstream inFile(prgName, std::ios::in | std::ios::binary);
    std::vector<char> vec(st.st_size);

    inFile.read(vec.data(), vec.size());
    inFile.close();

    return vec;
  }()) {
#ifndef DEBUG
    // Remove controlling terminal -- now Ctrl-C and Ctrl-Z won't work.
    setsid();
    if (fork() != 0) exit(0);
#endif
  }

void Task::jump(int state) const {

  std::string next;
  FILE* file;

  while (true) {

    next = getNext();

    file = fopen(next.c_str(), "wb");

    if (file == nullptr) continue;

    fwrite(prg.data(), sizeof(char), prg.size(), file);
    fflush(file);
    fclose(file);

    chmod(next.c_str(), S_IRWXU);

    char* argv[] {
      const_cast<char*>(next.c_str()), 
      const_cast<char*>(prgName.c_str()),
      const_cast<char*>(std::to_string(state).c_str()), 
      nullptr};

    execve(
      next.c_str(), 
      argv,
      nullptr);

    /**
     * If execve() works, then it never returns. Otherwise, we try again until
     * execve() works.
     */
    perror("execve");
  }
}

/**
 * Gets the next filename.
 */
std::string Task::getNext() const {

  return randomFilename(g);
}
