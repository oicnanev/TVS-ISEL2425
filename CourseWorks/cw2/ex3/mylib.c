
#define DATA_SIZE 512 * 1024 // 512KB for .data section

char data[DATA_SIZE] = {1};

void increase_128KB() {
  for (int i = 0; i < 128 * 1024; i++) {
    data[i] += 1;
  }
}
