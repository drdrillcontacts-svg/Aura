# AURA Windows cross-build: msvcrt sentinel data objects (raw bit patterns).
# 32-bit float layouts; 80-bit x87 extended: [0..8) mantissa LE,
# [8..10) exponent, [10..12) sign byte.
    .section .data, "w"
    .align 4
__DENORMF:
    .long 0x00400000
__INFF:
    .long 0x7F800000
__QNANF:
    .long 0x7FC00000
__SNANF:
    .long 0x7F800001

    .align 16
__DENORML:
    .long 1
    .long 0
    .long 0x00000000
    .align 16
__INFL:
    .long 0
    .long 0
    .long 0x7FFF0000
    .align 16
__QNANL:
    .long 0
    .long 0x80000000
    .long 0x7FF00000
    .align 16
__SNANL:
    .long 1
    .long 0
    .long 0x7FF00000
