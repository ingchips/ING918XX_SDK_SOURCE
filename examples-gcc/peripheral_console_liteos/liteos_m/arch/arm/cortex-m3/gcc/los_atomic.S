.global ArchAtomicRead
.global ArchAtomicSet
.global ArchAtomicAdd
.global ArchAtomicSub
.global ArchAtomicXchg32bits
.global ArchAtomicCmpXchg32bits

.syntax unified
.thumb

ArchAtomicRead:
    ldr r1, [r0]
    mov r0, r1
    bx lr

ArchAtomicSet:
    ldr r2, [r0]
    str r1, [r0]
    cmp r2, #0
    bne ArchAtomicSet
    bx lr

ArchAtomicAdd:
    ldr r2, [r0]
    add r2, r2, r1
    str r2, [r0]
    cmp r2, #0
    bne ArchAtomicAdd
    mov r0, r2
    bx lr

ArchAtomicSub:
    ldr r2, [r0]
    sub r2, r2, r1
    str r2, [r0]
    cmp r2, #0
    bne ArchAtomicSub
    mov r0, r2
    bx lr

ArchAtomicXchg32bits:
    ldr r2, [r0]
    str r1, [r0]
    cmp r2, #0
    bne ArchAtomicXchg32bits
    mov r0, r2
    bx lr

ArchAtomicCmpXchg32bits:
    ldr r3, [r0]
    cmp r3, r2
    bne end
    str r1, [r0]
    cmp r1, #0
    bne ArchAtomicCmpXchg32bits
end:
    bx lr
