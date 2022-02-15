#include <unistd.h>

int main(int argc, char const *argv[]) {

  write(1, "hello ", 6);
  write(1, "world\n", 6);
  write(1, "hello ", 6);
  write(1, "world\n", 6);

  return 0;
}
