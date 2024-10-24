#include <dlfcn.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

#define DATA_SIZE 16 * 1024 * 1024
#define DATA_3MB 3 * 1024 * 1026

char info[DATA_SIZE];
char data[DATA_SIZE] = {1};

int main() {
  printf("PID: %u\n", getpid());

  const long PAGE_SIZE = sysconf(_SC_PAGE_SIZE);
  printf("PAGE_SIZE: %ld\n", PAGE_SIZE);

  printf("#1 a. Increase the resident set (Rss) by about 3MB in the .bss "
         "region. (press ENTER to continue)");
  getchar();

  // a Increase the resident set (Rss) by about 3MB in the .bss region.
  for (int i = 0; i < DATA_3MB; i++) {
    info[i] = 1;
  };

  printf("#2 b. Access 256 bytes of initialized data (.data) with maximum "
         "impact in Private Clean pages. (press ENTER to continue)");
  getchar();
  

  // b. Access 256 bytes of initialized data (.data) with maximum impact in
  // Private Clean pages.
  int acumulator = 0;
  for (int i = 0; i <= 256 * 4096; i += 4096) {
    acumulator += data[i];
  };

  printf("#3 c. Reduce the Pss of non-initialized data (.bss) to around 1.5MB "
         "for 30 seconds, while keeping Rss. (press ENTER to continue)");
  getchar();

  pid_t pid = fork();

  if (pid == 0) {

    sleep(30);
    return 0;

  } else {
    int status;
    waitpid(pid, &status, 0);
  }
  
  // c. Reduce the Pss of non-initialized data (.bss) to around 1.5MB for 30
  // seconds, while keeping Rss.
  // We start another process of this program in another terminal?

  printf("#4 d. Execute a single operating system function that results in two "
         "new regions being added to the existing address space, one with code "
         "(~ 4KB) and one with data (~ 512KB). (press ENTER to continue)");
  getchar();

  // d d. Execute a single operating system function that results in two new
  // regions being added to the
  // existing address space, one with code (~ 4KB) and one with data (~ 512KB).

  // Carregar a biblioteca dinâmica
  void *handle = dlopen("./mylib.so", RTLD_LAZY);
  if (!handle) {
    printf("Erro ao carregar biblioteca: %s\n", dlerror());
    return 1;
  }

  printf("#5 e. Increase Private dirty pages by about 128KB in the region for "
         "the data section created in d)(press ENTER to continue)");
  getchar();

  // e e. Increase Private dirty pages by about 128KB in the region for the data
  // section created in d)
  void (*increase_128KB)(void) = dlsym(handle, "increase_128KB");
  const char *dlsym_error = dlerror();
  if (dlsym_error) {
    printf("Error finding the function: %s\n", dlsym_error);
    dlclose(handle);
    return 1;
  }
  increase_128KB();

  printf("END (press ENTER to continue)");
  getchar();

  dlclose(handle); // close dynamic library

  return 0;
}
