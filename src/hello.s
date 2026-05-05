    .section .rodata
    .global hello_mid
    .align  2

hello_mid:
    .incbin "../src/hello.mid"
