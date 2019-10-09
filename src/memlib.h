#ifndef MEMLIB_H
#define MEMLIB_H

typedef struct mapping mapping;

struct mapping {
  mapping* next;
  mapping* prev;
  void* addr_start;
  void* addr_stop;
  long length;
  char perms[5];
  int offset;
  unsigned int devmajor;
  unsigned int devminor;
  int inode;
  char pathname[80];
};

// struct maps
// get maps
// get mem
// write mem

#endif // MEMLIB_H
