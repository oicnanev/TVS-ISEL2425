### Exercise 2

**a) List and explain the sequence of calls performed by the program in x86-64/prog.s**

----

This assembly program performs a series of system calls to interact with the operating system. Here's a breakdown of what each part of the code does, along with the sequence of system calls:

### Data Section
```assembly
.section .rodata

data:   .byte  47, 101, 116,  99
        .byte  47, 111, 115,  45
        .byte 114, 101, 108, 101
        .byte  97, 115, 101,   0
        .byte  73,  83,  69,  76
```
- This section defines a read-only data segment containing the string "/etc/os-release\0ISEL".

### Text Section
```assembly
.text
.globl _start
_start:
```
- This marks the beginning of the code section and defines the entry point `_start`.

### Sequence of System Calls

1. **Open File Descriptor**
    ```assembly
    movq $-100, %rdi
    leaq data(%rip), %rsi
    xorq %rdx, %rdx
    movq $257, %rax
    syscall
    ```
    - **System Call**: `openat` (syscall number 257)
    - **Arguments**:
        - `rdi = -100`: AT_FDCWD (current working directory)
        - `rsi = data`: Pointer to the file path "/etc/os-release"
        - `rdx = 0`: Flags (read-only)
    - **Purpose**: Opens the file "/etc/os-release" for reading.
    - **Return Value**: File descriptor stored in `rax`, moved to `r15`.

2. **Read File**
    ```assembly
    movq %r15, %rdi
    xorq %rsi, %rsi
    movq $2, %rdx
    movq $8, %rax
    syscall
    ```
    - **System Call**: `read` (syscall number 0)
    - **Arguments**:
        - `rdi = r15`: File descriptor
        - `rsi = 0`: Buffer (not specified here, likely an error)
        - `rdx = 2`: Number of bytes to read
    - **Purpose**: Attempts to read 2 bytes from the file.
    - **Return Value**: Number of bytes read stored in `rax`, moved to `r14`.

3. **Memory Mapping**
    ```assembly
    xorq %rdi, %rdi
    movq %r14, %rsi
    movq $1, %rdx
    movq $2, %r10
    movq %r15, %r8
    xorq %r9, %r9
    movq $9, %rax
    syscall
    ```
    - **System Call**: `mmap` (syscall number 9)
    - **Arguments**:
        - `rdi = 0`: Address (NULL, let the kernel choose)
        - `rsi = r14`: Length (number of bytes read)
        - `rdx = 1`: Protection (PROT_READ)
        - `r10 = 2`: Flags (MAP_PRIVATE)
        - `r8 = r15`: File descriptor
        - `r9 = 0`: Offset
    - **Purpose**: Maps the file into memory.
    - **Return Value**: Address of the mapped area stored in `rax`.

4. **Write to Standard Output**
    ```assembly
    movq $1, %rdi
    movq %rax, %rsi
    movq %r14, %rdx
    movq $1, %rax
    syscall
    ```
    - **System Call**: `write` (syscall number 1)
    - **Arguments**:
        - `rdi = 1`: File descriptor (stdout)
        - `rsi = rax`: Buffer (address of the mapped area)
        - `rdx = r14`: Number of bytes to write
    - **Purpose**: Writes the read bytes to standard output.

5. **Exit**
    ```assembly
    xorq %rdi, %rdi
    movq $231, %rax
    syscall
    ```
    - **System Call**: `exit_group` (syscall number 231)
    - **Arguments**:
        - `rdi = 0`: Exit status
    - **Purpose**: Terminates the program.

### Summary
This program opens the file "/etc/os-release", reads a small portion of it, maps the read content into memory, writes it to standard output, and then exits. The sequence of system calls ensures that the file is accessed, read, and its content is displayed before the program terminates.

Source: Conversation with Copilot, 10/15/2024
(1) System Call in OS (Operating System): What is, Types and Examples - Guru99. https://www.guru99.com/system-call-operating-system.html.
(2) 4.1: Instruction Cycles - Engineering LibreTexts. https://eng.libretexts.org/Courses/Delta_College/Introduction_to_Operating_Systems/04%3A_Computer_Architecture_-_the_CPU/4.01%3A_Instruction_Cycles.
(3) System Calls in Operating System Explained | phoenixNAP KB. https://phoenixnap.com/kb/system-call.
(4) 3.3: System Calls - Engineering LibreTexts. https://eng.libretexts.org/Courses/Delta_College/Introduction_to_Operating_Systems/03%3A_The_Operating_System/3.03%3A_System_Calls.
(5) Assembly - System Calls - Online Tutorials Library. https://www.tutorialspoint.com/assembly_programming/assembly_system_calls.htm.
(6) Assembly System Calls - ref.coddy.tech. https://ref.coddy.tech/assembly/assembly-system-calls.
(7) x86 Assembly NASM System Calls - Cratecode. https://cratecode.com/info/x86-assembly-nasm-system-calls.
(8) 2.4. System Call Interface — Computer Systems Fundamentals - JMU. https://w3.cs.jmu.edu/kirkpams/OpenCSF/Books/csf/html/Syscall.html.

