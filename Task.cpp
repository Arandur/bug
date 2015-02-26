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

Task::Task(const std::string& prgName) :
  prg([prgName = prgName] {

    struct stat st;
    stat(prgName.c_str(), &st);

    std::ifstream inFile(prgName, std::ios::in | std::ios::binary);
    std::vector<char> vec(st.st_size);

    inFile.read(vec.data(), vec.size());
    inFile.close();

    return vec;
  }()),
  g(std::random_device{}()) {

    // Remove controlling terminal -- now Ctrl-C and Ctrl-Z won't work.
    setsid();
    if (fork() != 0) exit(0);
  }

void Task::jump(const std::string& currFile, int state) const {

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
      const_cast<char*>(currFile.c_str()),
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

  char path[PATH_MAX + 1];

  char* absPath = realpath(prg.data(), path);
  std::string nextDir;

  if (std::uniform_int_distribution<int>(0, 1)(g)) {
    // Parent directory
    nextDir = dirname(absPath);

    if (!isRWX(nextDir))    goto _else;
    if (nextDir == absPath) goto _else;

    if (nextDir.back() != FILE_SEP)

      nextDir += FILE_SEP;
  } else { _else:
    // Subdirectory
    auto subdirs  = getSubdirs(absPath);
    nextDir       = std::string(absPath) + FILE_SEP + 
                    *choose(std::begin(subdirs), std::end(subdirs), g) +
                    FILE_SEP;

    if (!isRWX(nextDir)) {
      // HOME directory
      nextDir = getHomeDir() + FILE_SEP;
    }
  }

  const auto maxBaseName = PATH_MAX - nextDir.length();

  return nextDir + randomFilename(maxBaseName, g);
}
