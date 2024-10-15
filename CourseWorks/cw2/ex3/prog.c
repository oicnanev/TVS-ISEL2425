#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>
#include <stdlib.h>

#define DATA_SIZE 16*1024*1024

char info[DATA_SIZE];
char data[DATA_SIZE] = {1};

int main() {
    printf("PID: %u\n", getpid());

    const long PAGE_SIZE = sysconf(_SC_PAGE_SIZE);
    printf("PAGE_SIZE: %ld\n", PAGE_SIZE);

    printf("#1 (press ENTER to continue)"); getchar();

    // a. Increase the resident set (Rss) by about 3MB in the .bss region.
    memset(info, 1, 3 * 1024 * 1024);

    printf("#2 (press ENTER to continue)"); getchar();

    // b. Access 256 bytes of initialized data (.data) with maximum impact in Private Clean pages.
    for (int i = 0; i < 256; i++) {
        data[i] = i;
    }

    printf("#3 (press ENTER to continue)"); getchar();

    // c. Reduce the Pss of non-initialized data (.bss) to around 1.5MB for 30 seconds, while keeping Rss.
    madvise(info, 14.5 * 1024 * 1024, MADV_DONTNEED);
    sleep(30);

    printf("#4 (press ENTER to continue)"); getchar();

    // d. Execute a single operating system function that results in two new regions being added to the
    //    existing address space, one with code (~ 4KB) and one with data (~ 512KB).
    void *code_region = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    void *data_region = mmap(NULL, 512 * 1024, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    printf("#5 (press ENTER to continue)"); getchar();

    // e. Increase Private dirty pages by about 128KB in the region for the data section created in d).
    memset(data_region, 1, 128 * 1024);

    printf("END (press ENTER to continue)"); getchar();

    return 0;
}
