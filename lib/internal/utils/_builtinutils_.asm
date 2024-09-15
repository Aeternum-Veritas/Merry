;; Some quick and useful procedures 

proc __builtin_quick_save
proc __builtin_quick_restore
proc __builtin_align_value
proc __builtin_exit

__builtin_quick_save
    pusha
    ret

__builtin_quick_restore
    popa
    ret

;; ARG: Ma = The value to align
;; RETURNS: The closest higher multiple of 8 to the value
__builtin_align_value
    push Mb
    add Ma, 7
    movl Mb, 0xFFFFFFFFFFFFFFF8
    and Ma, Mb
    pop Mb
    ret

;; ARGS: Ma = RET value
;; RETURNS: Doesn't return at all
__builtin_exit
    intr _M_EXIT_

depends _builtinintr_.asm