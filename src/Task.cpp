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
 *
 * Half the time, this returns the parent directory (if it exists, and is
 * viable). The other half, it chooses a viable subdirectory (if one exists).
 *
 * If none of the above options are viable, this returns the HOME directory.
 * 
 * @return [description]
 */
std::string Task::getNext() const {

#ifdef _WIN32
  const static char FILE_SEP ='\\';
#else
  const static char FILE_SEP = '/';
#endif

  // Copy prgName to char buffer because dirname isn't guaranteed not to modify
  // its parameter
  char absPath[prgName.length() + 1];
  strcpy(absPath, prgName.c_str());
  absPath[prgName.length()] = '\0';

  std::string nextDir;

  const auto getParentDir = [&absPath] () -> std::string {

    auto dir = dirname(absPath);

    if (!isRWX(dir)) return {};
    if (dir == absPath) return {};

    return dir;
  };

  const auto getSubDir = [&absPath, this] () -> std::string {

    auto dirs = getSubdirs(absPath);

    if (dirs.empty()) return {};

    auto dir = *choose(std::begin(dirs), std::end(dirs), g);

    if (!isRWX(dir)) return {};

    return dir;
  };

  if (std::uniform_int_distribution<int>(0, 1)(g)) {

    nextDir = getParentDir();

    if (nextDir.empty()) nextDir = getSubDir();
  } else { 

    nextDir = getSubDir();

    if (nextDir.empty()) nextDir = getParentDir();
  }

  if (nextDir.empty()) nextDir = getHomeDir();

  nextDir += FILE_SEP;

  const auto maxBaseName = PATH_MAX - nextDir.length();

  return nextDir + randomFilename(maxBaseName, g);
}
