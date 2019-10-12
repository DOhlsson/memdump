#define _GNU_SOURCE
#include "memlib.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/uio.h>
#include <unistd.h>

int read_mem(pid_t pid, void* addr, long length, char* dataname);

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

      printf("dataname %s\n", dataname);
      read_mem(pid, map->addr_start, map->length, dataname);

      free(dataname);
    }

    map = map->next;
  }

  return 0;
}

int read_mem(pid_t pid, void* addr, long length, char* dataname)
{
  printf("reading mem %p %ld %s\n", addr, length, dataname);

  char filename[80];

  strcpy(filename, "dump.");
  strcat(filename, dataname);

  struct iovec local[1];
  struct iovec remote[1];

  char* buf = malloc(length * sizeof(char));

  local[0].iov_base = buf;
  local[0].iov_len = length;

  remote[0].iov_base = addr;
  remote[0].iov_len = length;

  process_vm_readv(pid, local, 1, remote, 1, 0);
  perror("vm read");
  errno = 0;

  FILE* fp = fopen(filename, "w");
  perror("fopen");
  errno = 0;

  fwrite(buf, sizeof(char), length, fp);
  perror("fwrite");
  errno = 0;

  fclose(fp);
  perror("fclose");
  errno = 0;

  free(buf);

  return 0;
}
