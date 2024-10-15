 /*
        .section .rodata

data:   .byte  47, 101, 116,  99
        .byte  47, 111, 115,  45
        .byte 114, 101, 108, 101
        .byte  97, 115, 101,   0
        .byte  73,  83,  69,  76

        .text
        .globl _start
_start:
        mov x?, #-100
        ldr x?, =data
        mov x?, #0
        mov w8, #??
        svc #0

        mov x21, x0

        mov x?, #2
        mov x?, #0
        mov x?, x21
        mov w8, #??
        svc #0

        mov x20, x0

        mov x?, #0
        mov x?, #0
        mov x?, #1
        mov x?, #2
        mov x?, x20
        mov x?, x21
        mov w8, #??
        svc #0

        mov x19, x0

        mov x?, x19
        mov x?, x20
        mov x?, #1
        mov w8, #??
        svc #0

        # Exit
        mov x?, #0
        mov w8, #??
        svc #0

        .end
*/

.section .rodata

data:   .byte  47, 101, 116,  99
        .byte  47, 111, 115,  45
        .byte 114, 101, 108, 101
        .byte  97, 115, 101,   0
        .byte  73,  83,  69,  76

.text
.globl _start
_start:
        # Open File Descriptor
        mov x0, #-100
        ldr x1, =data
        mov x2, #0
        mov w8, #56        # openat syscall number
        svc #0

        mov x21, x0

        # Read File
        mov x0, x21
        mov x1, #0
        mov x2, #2
        mov w8, #63        # read syscall number
        svc #0

        mov x20, x0

        # Memory Mapping
        mov x0, #0
        mov x1, x20
        mov x2, #1
        mov x3, #2
        mov x4, x21
        mov x5, #0
        mov w8, #222       # mmap syscall number
        svc #0

        mov x19, x0

        # Write to Standard Output
        mov x0, #1
        mov x1, x19
        mov x2, x20
        mov w8, #64        # write syscall number
        svc #0

        # Exit
        mov x0, #0
        mov w8, #93        # exit syscall number
        svc #0

.end


/* ************************************************************************* 
 * Explanation of Changes:
 * 
 * 1. Open File Descriptor:
 *  - mov x0, #-100: Set x0 to -100 (AT_FDCWD).
 *  - ldr x1, =data: Load the address of data into x1.
 *  - mov x2, #0: Set x2 to 0 (flags).
 *  - mov w8, #56: Set w8 to 56 (syscall number for openat).
 *  - svc #0: Make the syscall.
 *
 * 2. Read File:
 *  - mov x0, x21: Set x0 to the file descriptor.
 *  - mov x1, #0: Set x1 to 0 (buffer, not specified here).
 *  - mov x2, #2: Set x2 to 2 (number of bytes to read).
 *  - mov w8, #63: Set w8 to 63 (syscall number for read).
 *  - svc #0: Make the syscall.
 * 
 * 3. Memory Mapping:
 *  - mov x0, #0: Set x0 to 0 (address).
 *  - mov x1, x20: Set x1 to the length (number of bytes read).
 *  - mov x2, #1: Set x2 to 1 (PROT_READ).
 *  - mov x3, #2: Set x3 to 2 (MAP_PRIVATE).
 *  - mov x4, x21: Set x4 to the file descriptor.
 *  - mov x5, #0: Set x5 to 0 (offset).
 *  - mov w8, #222: Set w8 to 222 (syscall number for mmap).
 *  - svc #0: Make the syscall.
 * 
 * 4. Write to Standard Output:
 *  - mov x0, #1: Set x0 to 1 (stdout).
 *  - mov x1, x19: Set x1 to the buffer (address of the mapped area).
 *  - mov x2, x20: Set x2 to the number of bytes to write.
 *  - mov w8, #64: Set w8 to 64 (syscall number for write).
 *  - svc #0: Make the syscall.
 * 
 * 5. Exit:
 *  - mov x0, #0: Set x0 to 0 (exit status).
 *  - mov w8, #93: Set w8 to 93 (syscall number for exit).
 *  - svc #0: Make the syscall.
 */