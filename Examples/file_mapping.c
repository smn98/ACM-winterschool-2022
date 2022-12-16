#include "../lib/util.h"

int main(int argc, char **argv) {

  if(argc !=2)
  {
    printf("usage: %s filename\n", argv[0]);
    exit(0);
  }

  map_handle_t *handle;     // declaring a handle for file mapping
  char *map;

  map = (char *) map_file(argv[1], &handle);  // mapping a file in memory (virtual address space)

  printf("File contents are as follows:\n");
  for(size_t i=0; i< handle->range;i++)
  {
    printf("%c",*(map+i));      // Accessing the file contents
  }

  unmap_file(handle);     // unmapping the file.

  return 0;
}
