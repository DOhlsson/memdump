#define _GNU_SOURCE
#include "memlib.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/uio.h>
#include <unistd.h>

int read_mem(pid_t pid, void* addr, size_t length, char* dataname);

int main(int argc, char** argv)
{
  if (argc != 2) {
    printf("Usage: %s <pid>\n", argv[0]);
    return 1;
  }

  pid_t pid = atoi(argv[1]);

  mapping* map = mapping_getall(pid);

  int unnamed_count = 0;

  while (map != NULL) {
    if (map->inode == 0) {
      char* dataname = calloc(80, sizeof(char));
      if (strlen(map->pathname) == 0) {
        sprintf(dataname, "unknown%d", unnamed_count);
        unnamed_count++;
      } else {
        sscanf(map->pathname, "[%[a-z]]", dataname);
      }

      read_mem(pid, map->addr_start, map->length, dataname);

      free(dataname);
    }

    map = map->next;
  }

  return 0;
}

int read_mem(pid_t pid, void* addr, size_t length, char* dataname)
{
  printf("reading mem %p %ld %s\n", addr, length, dataname);

  char filename[80];

  strcpy(filename, "dump.");
  strcat(filename, dataname);

  struct iovec local;
  struct iovec remote;

  char* buf = malloc(length * sizeof(char));

  local.iov_base = buf;
  local.iov_len = length;

  remote.iov_base = addr;
  remote.iov_len = length;

  ssize_t vm_bytes_read = process_vm_readv(pid, &local, 1, &remote, 1, 0);
  if (vm_bytes_read == -1) {
    perror("vm read");
    return -1;
  }

  FILE* fp = fopen(filename, "w");
  if (fp == NULL) {
    perror("fopen");
    return -1;
  }

  size_t file_bytes_written = fwrite(buf, sizeof(char), length, fp);
  if (file_bytes_written == 0) {
    perror("fwrite");
    return -1;
  }

  fclose(fp);
  free(buf);

  return 0;
}
