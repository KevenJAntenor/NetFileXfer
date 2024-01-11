#include <arpa/inet.h>
#include <getopt.h>
#include <netinet/in.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "core.h"
#include "xfer.h"

struct timespec ts;

void show_progress(int progress, void *args) {
  printf("send progress %d%%\n", progress);
  nanosleep(&ts, NULL);
}

int handle(int fd, void *args) {
  struct xfer_file *x = args;
  if (xfer_send_file(fd, x) < 0) {
    printf("client transfer error\n");
  } else {
    printf("client transfer success\n");
  }
  return 0;
}

void print_usage() {
  fprintf(stderr, "Usage: %s [-orpHsvh]\n", "client");
  exit(1);
}

int main(int argc, char **argv) {
  struct option options[] = {{"input", 1, 0, 'i'},   //
                             {"port", 1, 0, 'p'},    //
                             {"host", 1, 0, 'H'},    //
                             {"delay", 1, 0, 'd'},   //
                             {"bsize", 1, 0, 's'},   //
                             {"verbose", 0, 0, 'v'}, //
                             {"help", 0, 0, 'h'},    //
                             {0, 0, 0, 0}};

  const char *input = "input.dat";
  const char *host = "127.0.0.1";
  int port = 8080;
  int bs = 1024;
  int delay = 0;
  int verbose = 0;

  int opt;
  int idx;
  while ((opt = getopt_long(argc, argv, "i:p:H:d:s:vh", options, &idx)) != -1) {
    switch (opt) {
    case 'i':
      input = optarg;
      break;
    case 'p':
      port = atoi(optarg);
      break;
    case 'H':
      host = optarg;
      break;
    case 'd':
      delay = atoi(optarg);
      break;
    case 's':
      bs = atoi(optarg);
      break;
    case 'v':
      verbose = 1;
      break;
    case 'h':
    default:
      print_usage();
    }
  }

  // Délai artificiel pour tester la robustesse de la réception
  ts.tv_nsec = delay;

  struct xfer_file x = {.fname = input,
                        .block_size = bs,
                        .verbose = verbose,
                        .progress = show_progress};
  xfer_client(host, port, handle, &x);
  return 0;
}
