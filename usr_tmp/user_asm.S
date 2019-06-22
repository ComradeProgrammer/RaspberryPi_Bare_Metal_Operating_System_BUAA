.global _start
.section ".text.boot"
.extern user_main
_start:
    bl user_main