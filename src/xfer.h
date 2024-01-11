#ifndef INF3173_TP2_XFER_H
#define INF3173_TP2_XFER_H

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*xfer_progress_t)(int percent, void *data);
typedef int (*xfer_function)(int fd, void* args);

struct xfer_file {
  const char* fname;
  int block_size;
  int verbose;
  xfer_progress_t progress;
};

int xfer_send_file(int sockfd, void* args);
int xfer_recv_file(int sockfd, void* args);

#ifdef __cplusplus
}
#endif

#endif
