.section .text
.global main

main:
    movl $4, %eax
    movl $1, %ebx
    movl $msg, %ecx
    movl $14, %edx #输入13，都没有办法换行
    int $0x80

    movl $1, %eax
    xorl %ebx, %ebx
    int $0x80

.section .data
msg:
    .string "Hello, World!\n"