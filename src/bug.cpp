#include "name.h"

#include <iostream>
#include <cstdio>
#include <string>
#include <sys/shm.h>
#include <cstring>
#include <unistd.h>

/**
 * User-defined function.
 * 
 * @param  state The current state.
 * @param  r     A source of randomness, just in case.
 * @return       The next state.
 */
int runTask(int state, std::default_random_engine& r);

/**
 * Creates a new copy of the program and starts it.
 * 
 * @param prgData The program's binary code.
 * @param size    The size of the program's binary code.
 */
template <typename Random>
auto copyData(const void* prgData, const size_t size, Random& r) ->
  std::string {

  while (true) {

    const std::string next = randomFilename(r);

    auto f = fopen(next.c_str(), "wb");
    auto ret = fwrite(prgData, 1, size, f);
    fclose(f);

    if (ret != size) {
#ifdef DEBUG
      std::cerr << "Could not write data to file " << next << std::endl;
#endif
      continue;
    }

    return next;
  }
}

/**
 * Runs the next program in the sequence. Returns on failure.
 * 
 * @param nextFile The name of the next file to run.
 * @param shmid    The ID of the shared memory containing the program data.
 * @param state    The state at which the next program should start.
 */
void runNext(const std::string nextFile, int shmid, int state) {

  char* nextStr = new char[nextFile.size() + 1];
  char  shmidStr[30];
  char  stateStr[30];

  strcpy(nextStr,  nextFile.c_str());
  strcpy(shmidStr, std::to_string(shmid).c_str());
  strcpy(stateStr, std::to_string(state).c_str());

  char* argv[] = {nextStr, shmidStr, stateStr, nullptr};
  char* envp[]  = {nullptr};

  execve(nextStr, argv, envp);

#ifdef DEBUG
  perror("execve");
#endif

  return;
}

/**
 * Waits for another program to access the shared memory, then returns.
 * 
 * @param shmid The ID of the shared memory.
 */
void waitForShmAccess(int shmid) {

  struct shmid_ds info;

  while (true) {

    shmctl(shmid, IPC_STAT, &info);

    if (info.shm_cpid != info.shm_lpid) return;

    sleep(1);
  }
}

/**
 * Expected arguments:
 *
 * ./bug 
 * ./bug [SHMID] [STATE]
 */
int main(int argc, char* argv[]) {

  int shmid;
  void* prgData;
  size_t prgDataSize;
  int state;

  std::default_random_engine g(std::random_device{}());

  if (argc == 1) {
    /*
    Initial call -- need to grab the data from the file, create a shared memory
    segment, and write the former into the latter.
     */
    auto f = fopen(argv[0], "rb");
    fseek(f, 0, SEEK_END);
    prgDataSize = ftell(f);
    fseek(f, 0, SEEK_SET);

    shmid = shmget(IPC_PRIVATE, prgDataSize, 0777);
    prgData = shmat(shmid, nullptr, 0);

    fread(prgData, sizeof(char), prgDataSize, f);

    fclose(f);

    state = 0;
  } else try {

    shmid = std::stoi(argv[2]);

    prgData = shmat(shmid, nullptr, 0);

    struct shmid_ds info;
    shmctl(shmid, IPC_STAT, &info);
    prgDataSize = info.shm_segsz;

    state = std::stoi(argv[3]);
  } catch (const std::exception& e) {
#ifdef DEBUG
    std::cerr << "Bad args: ";

    for (int i = 0; i < argc; ++i) 
      std::cerr << argv[i] << " ";

    std::cerr << std::endl;

    return EXIT_FAILURE;
#endif
  }

  state = runTask(state, g);

  if (fork()) {

    waitForShmAccess(shmid);
    unlink(argv[0]);
    return EXIT_SUCCESS;
  } else {

    while (true) {

      std::string next = copyData(prgData, prgDataSize, g);
      runNext(next, shmid, state);
    }
  }

  // This should never be reached.
  return EXIT_FAILURE;
}
