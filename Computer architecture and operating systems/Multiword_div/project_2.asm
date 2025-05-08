; "Project 2"
; My implementation performs division of a positive number by another positive number,
; and then adjusts the sign of the result or the remainder, if needed.
; Author: Kacper Pasiński
; Date: 19.05.2024

section .text
global mdiv

mdiv:
    ; Function arguments:
    ; rdi - int64_t *x   (pointer to dividend array)
    ; rsi - size_t n     (length of the array)
    ; rdx - int64_t y    (divisor)

    xor r8, r8   ; r8 will be 1 if the dividend is negative, 0 otherwise
    xor r9, r9   ; r9 is a helper register used in negate_array
    xor r10, r10 ; r10 will be 1 if the divisor is negative, 0 otherwise
    mov rcx, rsi

    ; Check if the dividend (last element at [rdi + 8*(rsi-1)]) is negative
    mov rax, [rdi + 8*(rsi-1)]
    test rax, rax    ; Check if the sign bit is set
    jns check_divisor_negative ; If not negative, check the divisor
    mov r8, 1        ; If negative, we’ll negate the dividend
    stc              ; Set carry flag

; Negating a number in two’s complement means inverting the bits and adding 1
; Before calling this section, make sure to set CL, zero r9,
; and copy rsi into rcx. This part is used to negate the dividend/result.
negate_array:
    mov rax, [rdi + 8*r9]
    not rax
    adc rax, 0
    mov [rdi + 8*r9], rax
    inc r9
    loop negate_array
    cmp r8, 0     ; If r8 is zero, we're negating the final result
    jz end_of_function ; In that case, go to function exit
    xor r9, r9    ; If r8 is 1, we're still in the beginning phase

check_divisor_negative:
    test rdx, rdx ; Check if the divisor is negative
    jns initialization ; If not negative, continue
    not rdx
    add rdx, 1    ; Negate the divisor
    mov r10, 1    ; Mark that the divisor was negative

; Initialize register values
initialization:
    mov r9, rsi   ; Save n in r9
    mov rcx, rdx  ; Copy the divisor to rcx
    xor rdx, rdx  ; Clear rdx and rax to prepare for division
    xor rax, rax

; Divide each chunk of the dividend array by the divisor (starting from the end)
divide_loop:
    mov rax, [rdi + 8*(rsi-1)]
    div rcx
    mov [rdi + 8*(rsi-1)], rax
    dec rsi
    cmp rsi, 0
    jnz divide_loop

; Adjust the sign of the result and remainder after division
sign_change:
    mov rsi, r9     ; Restore original value of n
    mov r11, r10
    xor r11, r8     ; XOR of r8 and r10 tells us if result should be negative
    cmp r11, 0
    jnz negative_result

positive_result:
    cmp r10, 0      ; If result is positive and divisor was too, we’re done
    jz end_of_function
    not rdx         ; If divisor was negative, negate the remainder
    add rdx, 1
    jmp check_overflow

negative_result:
    cmp r10, 0      ; If result is negative and divisor wasn’t, negate remainder
    jnz negate_result ; If divisor is negative, only result is negated
    not rdx
    add rdx, 1

; Negate the result stored in memory at pointer x
negate_result:
    xor r8, r8
    xor r9, r9
    mov rcx, rsi
    stc              ; Setup for correct carry flag usage in negate_array
    jmp negate_array

; Overflow detection is only necessary if the result is positive,
; since the only overflow case is dividing -INT64_MIN by -1
; This would result in INT64_MIN (0x8000000000000000)
check_overflow:
    mov rax, 0x8000000000000000
    cmp qword [rdi + 8*(rsi-1)], rax
    jz overflow

; If no issues, return remainder in rax and exit
end_of_function:
    mov rax, rdx
    ret

; Handle division by zero or overflow
overflow:
    xor rdi, rdi
    div rdi     ; Trigger division by zero error