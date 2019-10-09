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

  char maps_path[80];
  char maps_rbuf[256];
  sprintf(maps_path, "/proc/%d/maps", pid);

  FILE* maps = fopen(maps_path, "r");
  perror("fopen maps");

  int unnamed_count = 0;

  while (fgets(maps_rbuf, 256, maps) != NULL) {
    printf("%s", maps_rbuf);

    mapping map = { 0 };

    sscanf(maps_rbuf, "%p-%p %s %d %x:%x %d %s\n", &map.addr_start, &map.addr_stop, map.perms, &map.offset, &map.devmajor, &map.devminor, &map.inode, map.pathname);

    //printf("%p-%p %s %d %x:%x %d %s\n", map.addr_start, map.addr_stop, map.perms, map.offset, map.devmajor, map.devminor, map.inode, map.pathname);

    map.length = (long)map.addr_stop - (long)map.addr_start;

    printf("addr_start %p\n", map.addr_start);
    printf("addr_stop  %p\n", map.addr_stop);
    printf("length     %lx\n", map.length);
    printf("perms      %s\n", map.perms);
    printf("offset     %08d\n", map.offset);
    printf("dev        %02x:%02x\n", map.devmajor, map.devminor);
    printf("inode      %d\n", map.inode);
    printf("pathname   \"%s\"\n", map.pathname);

    if (map.inode == 0) {
      char* dataname = calloc(80, sizeof(char));
      if (strlen(map.pathname) == 0) {
        sprintf(dataname, "unknown%d", unnamed_count);
        unnamed_count++;
      } else {
        sscanf(map.pathname, "[%[a-z]]", dataname);
      }

      printf("dataname %s\n", dataname);
      read_mem(pid, map.addr_start, map.length, dataname);

      free(dataname);
    }

    printf("--------------------------------------------------------------------------------\n");
  }

  fclose(maps);
  perror("fclose maps");
  errno = 0;

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
