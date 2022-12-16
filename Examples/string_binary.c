#include "../lib/util.h"

int main(int argc, char **argv) {

  char buf[100] = "Hello World..!";
  char *binary, *str;

  binary = string_to_binary(buf);    // Converting string message into binary data stream

  printf("Hello World..! in binary :\n");
  printf("%s\n",binary);

  printf("Now Converting from binary to string\n");
  printf("Converted message is : ");

  str = binary_to_string(binary);  // Converting binary data stream into string.
  printf("%s\n", str);

  free(binary);
  free(str);

  return 0;
}
