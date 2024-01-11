#include "core.h"

#include <arpa/inet.h>
#include <inf3173/config.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "utils.h"

struct xfer_server_state {
  pthread_t worker;
  struct sockaddr_in address;
  int server_fd;
  int background;
  int run;
  xfer_function handler;
  void *args;
};

int xfer_client(const char *ipaddr, uint16_t port, xfer_function handler,
                void *args) {
  int ret = -1;
  int sock = 0;
  struct sockaddr_in serv_addr;
  memset(&serv_addr, '0', sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(port);

  // Créer un socket
  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    printf("Socket creation error\n");
    goto out;
  }

  // Conversion text -> ipaddr
  if (inet_pton(AF_INET, ipaddr, &serv_addr.sin_addr) <= 0) {
    printf("Invalid address\n");
    goto out_sock;
  }

  // Établir la connexion
  if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    printf("Connection Failed\n");
    goto out_sock;
  }

  ret = handler(sock, args);

out_sock:
  close(sock);
out:
  return ret;
}

int xfer_server_init(struct xfer_server_state **state, const char *ipaddr,
                     uint16_t port, xfer_function cb, void *args) {
  *state = NULL;
  struct xfer_server_state *tmp = malloc(sizeof(struct xfer_server_state));
  if (!tmp)
    goto out;

  tmp->address.sin_family = AF_INET;
  tmp->address.sin_port = htons(port);

  // Conversion text -> ipaddr
  if (inet_pton(AF_INET, ipaddr, &tmp->address.sin_addr.s_addr) <= 0) {
    printf("Invalid address\n");
    goto out_free;
  }
  tmp->run = 1;
  tmp->handler = cb;
  tmp->args = args;
  *state = tmp;
  return 0;

out_free:
  free(tmp);
out:
  return -1;
}

int xfer_server_once(void *args) {
  int ret = -1;
  struct xfer_server_state *state = (struct xfer_server_state *)args;
  int addrlen = sizeof(state->address);

  if ((state->server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
    perror("socket failed");
    goto err;
  }

  int opt = 1;
  if (setsockopt(state->server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                 &opt, sizeof(opt)) < 0) {
    perror("setsockopt");
    goto err_close_sock;
  }

  if (bind(state->server_fd, (struct sockaddr *)&state->address,
           sizeof(state->address)) < 0) {
    perror("bind");
    goto err_close_sock;
  }

  if (listen(state->server_fd, 10) < 0) {
    perror("listen");
    goto err_close_sock;
  }

  int sockfd = accept(state->server_fd, (struct sockaddr *)&state->address,
                      (socklen_t *)&addrlen);

  if (sockfd < 0) {
    perror("accept");
    goto err_close_sock;
  }

  state->handler(sockfd, state->args);
  close(sockfd);
  ret = 0;

err_close_sock:
  close(state->server_fd);
err:
  return ret;
}

void *server_loop(void *args) {
  struct timeval timeout;
  fd_set readfds;
  struct xfer_server_state *state = args;
  int addrlen = sizeof(state->address);

  FD_ZERO(&readfds);
  while (state->run) {
    timeout.tv_sec = 0;
    timeout.tv_usec = 100000; // 100 ms
    FD_SET(state->server_fd, &readfds);
    int ret = select(state->server_fd + 1, &readfds, NULL, NULL, &timeout);
    if (ret == -1) {
      perror("select error");
      return NULL;
    } else if (ret == 0) {
      // timeout
      continue;
    }

    int sockfd = accept(state->server_fd, (struct sockaddr *)&state->address,
                        (socklen_t *)&addrlen);

    if (sockfd < 0) {
      perror("accept");
      continue;
    }

    state->handler(sockfd, state->args);
    close(sockfd);
  }

  return NULL;
}

int xfer_server_start(struct xfer_server_state *state) {
  if ((state->server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
    perror("socket failed");
    goto err;
  }

  int opt = 1;
  if (setsockopt(state->server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                 &opt, sizeof(opt)) < 0) {
    perror("setsockopt");
    goto err_close_sock;
  }

  if (bind(state->server_fd, (struct sockaddr *)&state->address,
           sizeof(state->address)) < 0) {
    perror("bind");
    goto err_close_sock;
  }

  if (listen(state->server_fd, 10) < 0) {
    perror("listen");
    goto err_close_sock;
  }

  pthread_create(&state->worker, NULL, server_loop, state);
  return 0;

err_close_sock:
  close(state->server_fd);
err:
  return -1;
}

int xfer_server_stop(struct xfer_server_state *state) {
  state->run = 0;

  pthread_join(state->worker, NULL);
  close(state->server_fd);
  free(state);
  return 0;
}
