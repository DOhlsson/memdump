#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/uio.h>

int main(int argc, char** argv)
{
  if (argc != 5) {
    printf("Usage: %s <pid> <addr> <len> <byte>\n", argv[0]);
    return 1;
  }

  pid_t pid = 0;
  void* addr = NULL;
  size_t len = 0;
  unsigned char byte = 0;

  sscanf(argv[1], "%d", &pid);
  sscanf(argv[2], "%p", &addr);
  sscanf(argv[3], "%lu", &len);
  sscanf(argv[4], "%02hhx", &byte);

  printf("%d %p %lu %x\n", pid, addr, len, byte);

  /* Write mem */

  struct iovec local;
  struct iovec remote;

  char* buf = malloc(len * sizeof(char));
  buf[0] = byte;

  local.iov_base = buf;
  local.iov_len = len;

  remote.iov_base = addr;
  remote.iov_len = len;

  ssize_t bytesw = process_vm_writev(pid, &local, 1, &remote, 1, 0);
  perror("vm write");

  printf("Wrote %lu bytes\n", bytesw);

  return 0;
}
