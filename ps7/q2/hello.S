.text
.global _start

_start:
    mov $4, %eax # write syscall
    mov $1, %ebx # stdout file descriptior
    lea hello, %ecx # load address of string
    mov $14, %edx # length of hello world
    int $0x80 # bang

    mov %eax, %ebx # status
    mov $1, %eax # exit syscall 
    int $0x80 # boom

.data
hello:
    .string "Hello, world!\n" # pseudo opcode for sotring

