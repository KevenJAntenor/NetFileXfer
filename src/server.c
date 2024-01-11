#include <getopt.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "core.h"
#include "xfer.h"

sem_t sem;
int count = 0;
int repeat = 1;
pthread_mutex_t lock;

int handle_server(int fd, void* args) {
  struct xfer_file* x = args;
  if (xfer_recv_file(fd, x) < 0) {
    printf("server transfer error\n");
  } else {
    printf("server transfer success\n");
  }
  pthread_mutex_lock(&lock);
  if (++count == repeat) {
    sem_post(&sem);
  }
  pthread_mutex_unlock(&lock);
  return 0;
}

void print_usage() {
  fprintf(stderr, "Usage: %s [-orpHsvh]\n", "server");
  exit(1);
}

int main(int argc, char** argv) {
  struct option options[] = {{"output", 1, 0, 'o'},   //
                             {"repeat", 1, 0, 'r'},   //
                             {"port", 1, 0, 'p'},     //
                             {"host", 1, 0, 'H'},     //
                             {"bsize", 1, 0, 's'},    //
                             {"verbose", 0, 0, 'v'},  //
                             {"help", 0, 0, 'h'},     //
                             {0, 0, 0, 0}};

  const char* output = "output.dat";
  const char* host = "127.0.0.1";
  int port = 8080;
  int bs = 1024;
  int verbose = 0;

  int opt;
  int idx;
  while ((opt = getopt_long(argc, argv, "o:r:p:H:s:vh", options, &idx)) != -1) {
    switch (opt) {
      case 'o':
        output = optarg;
        break;
      case 'r':
        repeat = atoi(optarg);
        break;
      case 'p':
        port = atoi(optarg);
        break;
      case 'H':
        host = optarg;
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

  pthread_mutex_init(&lock, NULL);
  sem_init(&sem, 0, 0);
  struct xfer_file srv_data = {
      .fname = output, .block_size = bs, .verbose = verbose};
  struct xfer_server_state* state;
  xfer_server_init(&state, host, port, handle_server, &srv_data);
  xfer_server_start(state);
  printf("server started...\n");
  sem_wait(&sem);
  xfer_server_stop(state);
  return 0;
}
