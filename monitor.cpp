#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <stdlib.h>

void makeSiblings();

void run_ping() {

  FILE* outFile = fopen("out.txt", "w");

  while (true) {

    sleep(2);
    fprintf(outFile, "%d: Ping.\n", getpid());
  }
}

void run_monitor(pid_t sibling) {

  while (kill(sibling, 0) != -1) {

    printf("%d: %d still exists.\n", getpid(), sibling);
    sleep(1);
  }

  makeSiblings();
}

void makeSiblings() {

  pid_t pid1;

  fflush(stdout);

  if ((pid1 = fork()) == -1) {

    perror("fork");
    exit(EXIT_FAILURE);
  }

  if (pid1 == 0) {

    setsid();
    run_ping();
    printf("%d: exiting.\n", getpid());
    exit(EXIT_SUCCESS);
  } else {

    pid_t pid2;

    if ((pid2 = fork()) == -1) {

      perror("fork");
      exit(EXIT_FAILURE);
    }

    if (pid2 == 0) {

      setsid();
      run_monitor(pid1);
      printf("%d: exiting.\n", getpid());
      exit(EXIT_SUCCESS);
    }
  }

  exit(EXIT_SUCCESS);
}

void sig_handler(int signo) {

  if (signo == SIGTERM) {

    printf("%d: Terminated with SIGTERM\n", getpid());
    exit(EXIT_FAILURE);
  }
}

int main() {

  if (signal(SIGTERM, sig_handler) == SIG_ERR) {

    perror("signal");
    exit(EXIT_FAILURE);
  }

  makeSiblings();
}
