    .section .rodata
    .global hello_mid
    .align  2

hello_mid:
    .incbin "../../shared/mid/hello.mid"
