#define _GNU_SOURCE
#include "xfer.h"

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>

#include "inf3173/config.h"
#include "utils.h"



int xfer_send_file(int sockfd, void *args) {
  struct xfer_file *ctx = args;
  if (ctx->verbose) {
    printf("xfer_send_file %d %s %d\n", sockfd, ctx->fname, ctx->block_size);
  }
  
  // Ouvrir le fichier
  int fd = open(ctx->fname, O_RDONLY);
  if (fd == -1) {
    perror("Error opening file for reading");
    return errno;
  }

  // Lire la taille du fichier
  off_t filesize = lseek(fd, 0, SEEK_END);
  if (filesize == -1) {
    perror("Error getting file size");
    close(fd);
    return errno;
  }
  lseek(fd, 0, SEEK_SET);

  // Envoyer la taille du fichier
  ssize_t written = write(sockfd, &filesize, sizeof(off_t));
  if (written < sizeof(off_t)) {
    perror("Error sending file size");
    close(fd);
    return errno;
  }

  // Réaliser un mirroir du fichier en mémoire
  void *mapped = mmap(NULL, filesize, PROT_READ, MAP_SHARED, fd, 0);
  if (mapped == MAP_FAILED) {
    perror("Error mapping file into memory");
    close(fd);
    return errno;
  }

  // Boucle de transmission sur le nombre de bloc
  ssize_t sent = 0;
  while (sent < filesize) {
    ssize_t to_send = (filesize - sent > ctx->block_size) ? ctx->block_size : (filesize - sent);
    ssize_t result = write(sockfd, mapped + sent, to_send);
    if (result == -1) {
      perror("Error sending file data");
      munmap(mapped, filesize);
      close(fd);
      return errno;
    }
    sent += result;
  }

  // Libération des ressources
  munmap(mapped, filesize);
  close(fd);

  return 0; // Indiquer un succès
}

int xfer_recv_file(int sockfd, void *args) {
  struct xfer_file *ctx = args;
  if (ctx->verbose) {
    printf("xfer_recv_file %d %s %d\n", sockfd, ctx->fname, ctx->block_size);
  }

  // Recevoir la taille du fichier
  off_t filesize;
  ssize_t read_size = read(sockfd, &filesize, sizeof(off_t));
  if (read_size < sizeof(off_t)) {
    perror("Error reading file size");
    return errno;
  }

  // Ouvrir le fichier
  int fd = open(ctx->fname, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
  if (fd == -1) {
    perror("Error opening file for writing");
    return errno;
  }

  // Définir la taille du fichier de sortie
  if (ftruncate(fd, filesize) == -1) {
    perror("Error setting file size");
    close(fd);
    return errno;
  }

  // Réaliser un mirroir du fichier en mémoire
  void *mapped = mmap(NULL, filesize, PROT_WRITE, MAP_SHARED, fd, 0);
  if (mapped == MAP_FAILED) {
    perror("Error mapping file into memory");
    close(fd);
    return errno;
  }

  // Boucle de réception sur le nombre de bloc
  ssize_t received = 0;
  while (received < filesize) {
    ssize_t to_receive = (filesize - received > ctx->block_size) ? ctx->block_size : (filesize - received);
    ssize_t result = read(sockfd, mapped + received, to_receive);
    if (result == -1) {
      perror("Error reading file data");
      munmap(mapped, filesize);
      close(fd);
      return errno;
    }
    received += result;
  }

  // Libération des ressources
  munmap(mapped, filesize);
  close(fd);

  return 0; // Indiquer un succès
}
