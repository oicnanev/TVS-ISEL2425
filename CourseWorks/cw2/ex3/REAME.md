### Explanation of Changes:
1. **a. Increase the resident set (Rss) by about 3MB in the .bss region**:

    ```c
    memset(info, 1, 3 * 1024 * 1024);
    ```

    - This initializes 3MB of the `info` array in the .bss  section, increasing the resident set size.

2. **b. Access 256 bytes of initialized data (.data) with maximum impact in Private Clean pages**:
    
	```c
    for (int i = 0; i < 256; i++) {
        data[i] = i;
    }
    ```
    
    - This accesses 256 bytes of the `data` array, impacting Private Clean pages.

3. **c. Reduce the Pss of non-initialized data (.bss) to around 1.5MB for 30 seconds, while keeping Rss**:
    
	```c
    madvise(info, 14.5 * 1024 * 1024, MADV_DONTNEED);
    sleep(30);
    ```
    
    - This advises the kernel that the specified range of the `info` array can be discarded, reducing the Pss while keeping the Rss.

4. **d. Execute a single operating system function that results in two new regions being added to the existing address space**:
    
	```c
    void *code_region = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    void *data_region = mmap(NULL, 512 * 1024, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    ```
    
    - This maps two new regions: one for code (4KB) and one for data (512KB).

5. **e. Increase Private dirty pages by about 128KB in the region for the data section created in d)**:
    
	```c
    memset(data_region, 1, 128 * 1024);
    ```
    
    - This initializes 128KB of the newly mapped data region, increasing Private dirty pages.

These changes should achieve the desired effects in the process virtual address space, observable in `/proc/<pid>/smaps`.

Source: Conversation with Copilot, 10/15/2024

- [Understanding the Linux /proc/id/maps File](https://www.baeldung.com/linux/proc-id-maps)
- [Chapter 4 Process Address Space - The Linux Kernel Archives](https://www.kernel.org/doc/gorman/html/understand/understand007.html)
- [proc_pid_smaps(5) - Arch manual pages](https://man.archlinux.org/man/proc_pid_smaps.5.en)