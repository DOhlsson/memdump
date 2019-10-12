#include "memlib.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

mapping* mapping_getall(pid_t pid)
{
  mapping* first_mapping = NULL;
  char maps_path[80];
  char maps_rbuf[256];

  sprintf(maps_path, "/proc/%d/maps", pid);

  FILE* maps = fopen(maps_path, "r");
  if (maps == NULL) {
    char errmsg[80] = "fopen ";
    strcat(errmsg, maps_path);
    perror(errmsg);
    exit(1);
  }

  mapping* prev_mapping = NULL;

  while (fgets(maps_rbuf, 256, maps) != NULL) {
    mapping* map = calloc(1, sizeof(mapping));

    if (first_mapping == NULL) {
      first_mapping = map;
    } else {
      map->prev = prev_mapping;
      prev_mapping->next = map;
    }

    sscanf(maps_rbuf, "%p-%p %s %d %x:%x %d %s\n", &map->addr_start, &map->addr_stop, map->perms, &map->offset, &map->devmajor, &map->devminor, &map->inode, map->pathname);

    map->length = (long)map->addr_stop - (long)map->addr_start;

    mapping_print(map);

    prev_mapping = map;
  }

  fclose(maps);

  return first_mapping;
}

int mapping_print(mapping* map)
{
  printf("addr_start %p\n", map->addr_start);
  printf("addr_stop  %p\n", map->addr_stop);
  printf("length     %lx\n", map->length);
  printf("perms      %s\n", map->perms);
  printf("offset     %08d\n", map->offset);
  printf("dev        %02x:%02x\n", map->devmajor, map->devminor);
  printf("inode      %d\n", map->inode);
  printf("pathname   \"%s\"\n", map->pathname);
  printf("--------------------------------------------------------------------------------\n");

  return 0;
}
