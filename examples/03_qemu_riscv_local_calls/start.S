# Simple C runtime startup bootstrap
# Two primary functions:
# - Stack allocation and initializing stack pointer
# - Jumping to main

.section .text._start
.global _start
_start:
    la sp, __stack_top      # Load the stack pointer
    add s0, sp, zero        # Set the frame pointer
    call main               # Run main entry point - no argc
loop:	j loop              # Spin forever in case main returns
