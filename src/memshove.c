#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/uio.h>

int main(int argc, char** argv)
{
  if (argc != 6) {
    printf("Usage: %s <pid> <addr> <offset> <len> <data>\n", argv[0]);
    return 1;
  }

  pid_t pid = 0;
  void* addr = NULL;
  unsigned int offset = 0;
  size_t len = 0;
  unsigned char data[256];

  sscanf(argv[1], "%d", &pid);
  sscanf(argv[2], "%p", &addr);
  sscanf(argv[3], "%x", &offset);
  sscanf(argv[4], "%lu", &len);

  /* parse data from hex */

  if (len > 256 || len < 0) {
    printf("Error: len argument out of bounds\n");
    exit(1);
  }

  for (int i = 0; i < len; i++) {
    sscanf(argv[5] + (i * 2), "%02hhx", data + i);
    printf("data: %x\n", data[i]);
  }

  printf("Writing %lu bytes of data to pid %d at %p+%x\n", len, pid, addr, offset);

  /* Write mem */

  struct iovec local;
  struct iovec remote;

  local.iov_base = data;
  local.iov_len = len;

  remote.iov_base = (char*)addr + offset;
  remote.iov_len = len;

  ssize_t bytesw = process_vm_writev(pid, &local, 1, &remote, 1, 0);
  if (bytesw == -1) {
    perror("Error writing data");
    exit(1);
  }

  printf("Wrote %lu bytes\n", bytesw);

  return 0;
}
