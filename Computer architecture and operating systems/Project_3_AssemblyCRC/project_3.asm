; "Project 3"
; My implementation uses a lookup table-based algorithm,
; allowing data to be processed byte-by-byte rather than bit-by-bit.
; Author: Kacper Pasiński
; Date: 19.06.2024

section .bss
    buffer      resb 65542  ; buffer of max size: data (65536) + size (2) + offset (4)

    polynomial          resq 1      ; the CRC polynomial
    polynomial_degree   resb 1      ; degree of the polynomial

    file_descriptor     resq 1      ; file descriptor
    data_size           resw 1      ; size of the loaded fragment

    lookup          resq 256    ; lookup table used in CRC algorithm
    crc_value       resq 1      ; current CRC value

section .text
    global _start

_start:
    ; Validate arguments.
    mov     rdi, [rsp]      ; number of arguments (including program name)
    cmp     rdi, 3          ; check for exactly 3 arguments
    jne     error

    ; Validate polynomial.
    mov     rcx, [rsp + 24] ; rcx = pointer to CRC polynomial string
    mov     al, [rcx]
    test    al, al          ; check if polynomial string is empty
    jz      error

    xor     rbx, rbx        ; rbx = 0 (accumulates the final polynomial value)
    xor     rax, rax        ; rax = 0 (used for char conversion)
    xor     dl, dl          ; dl = 0 (polynomial degree)

parse_polynomial:
    mov     al, [rcx]       ; read next char from polynomial string
    test    al, al          ; check for null terminator
    jz      calculated_polynomial

    inc     dl              ; increase polynomial degree

    ; Check character value.
    cmp     al, '0'
    je      process_zero
    cmp     al, '1'
    je      process_one
    jmp     error           ; invalid character — not '0' or '1'

process_zero:
    shl     rbx, 1          ; shift left (append '0')
    jmp     advance

process_one:
    shl     rbx, 1          ; shift left
    inc     rbx             ; set LSB to 1 (append '1')

advance:
    inc     rcx             ; move to next character
    jmp     parse_polynomial

calculated_polynomial:
    mov     [polynomial_degree], dl ; store polynomial degree

    ; Align polynomial value to high bits.
    mov     cl, 64
    sub     cl, dl
    shl     rbx, cl         ; shift left (64 - degree)

    mov     [polynomial], rbx   ; save polynomial

open_file:
    mov     rdi, [rsp + 16] ; file name
    mov     rsi, 0          ; read-only mode
    mov     rax, 2          ; sys_open
    syscall

    ; Check if file opened successfully.
    test    rax, rax
    js      error

    mov     [file_descriptor], rax  ; store file descriptor

    ; Create lookup table: compute CRC for each byte value 0–255
    xor     rbx, rbx
    lea     rcx, [lookup]   ; rcx = lookup table address

calculate_lookup:
    mov     rax, rbx        ; rax = current byte value
    shl     rax, 56         ; align to high bits
    mov     rdx, 8          ; process 8 bits per byte

crc_bit_by_bit:
    shl     rax, 1
    jnc     skip_xor            ; if MSB is 0, skip xor
    xor     rax, [polynomial]   ; else xor with polynomial

skip_xor:
    dec     rdx
    jnz     crc_bit_by_bit

    ; Save the computed CRC value into the lookup table at the correct position.
    mov     [rcx + rbx * 8], rax  ; CRC for i is stored in [lookup + i * 8]
    inc     rbx
    cmp     rbx, 256
    jne     calculate_lookup

    ; Parse the file fragments.
parse_fragment:
    ; Read the data block size (2 bytes).
    mov     rdi, [file_descriptor]      ; file descriptor
    mov     rsi, buffer                 ; store the size in buffer
    mov     rdx, 2                      ; number of bytes to read
    mov     rax, 0                      ; sys_read
    syscall

    ; Check for read success.
    test    rax, rax
    js      close_and_error

    movzx   rbx, word [buffer]    ; rbx = size of data fragment
    mov     [data_size], rbx

    ; If there are no bytes to load, go to offset reading.
    test    rbx, rbx
    jz      read_offset

    ; Read the data.
    mov     rdi, [file_descriptor]  ; file descriptor
    add     rsi, 2                  ; advance buffer pointer by 2 bytes
    mov     rdx, rbx                ; number of bytes to read
    mov     rax, 0                  ; sys_read
    syscall

    ; Check for read success.
    test    rax, rax
    js      close_and_error

    ; Compute CRC for the data.
    lea     rdi, [buffer + 2]  ; point to the start of actual data

    ; Thanks to the previously built lookup table, we can now process
    ; the data byte-by-byte using precomputed CRC values for each byte.
crc_byte_by_byte:
    test    rbx, rbx
    jz      read_offset     ; if no more bytes to process, go to offset read

    mov     al, [rdi]
    shl     rax, 56     ; move the byte to the highest 8 bits
    xor     rax, [crc_value]    ; prepare for correct xor
    shr     rax, 56
    shl     QWORD [crc_value], 8    ; make space for the next byte
    mov     rax, [lookup + rax * 8] ; fetch the precomputed CRC for the byte
    xor     [crc_value], rax

    inc     rdi     ; move to the next byte
    dec     rbx     ; decrease the number of bytes remaining
    jmp     crc_byte_by_byte

read_offset:
    ; Read the offset (4 bytes).
    mov     rdi, [file_descriptor]        ; file descriptor
    mov     rsi, buffer
    add     rsi, 65538      ; store the offset in [buffer + 65538]
    mov     rdx, 4          ; number of bytes to read
    mov     rax, 0          ; sys_read
    syscall

    ; Check for read success.
    test    rax, rax
    js      close_and_error

    ; Negate the offset value to check whether it's the inverse
    ; of the size of the current fragment (which signals the last fragment).
    mov     eax, dword [buffer + 65538]
    neg     eax

    ; Check whether this is the last fragment.
    mov     esi, [data_size]
    add     esi, 6
    cmp     eax, esi
    je      display_crc      ; if yes, we're done — show result

    ; Move the file pointer by the offset read.
    mov     rdi, [file_descriptor]        ; file descriptor
    mov     esi, [buffer + 65538]
    movsxd  rsi, esi        ; convert offset to 64-bit signed
    mov     rdx, 1          ; SEEK_CUR
    mov     rax, 8          ; sys_lseek
    syscall

    ; Check if offset change succeeded.
    test    rax, rax
    js      close_and_error

    jmp     parse_fragment      ; continue processing the next fragment

display_crc:
    ; Close the file.
    mov     rdi, [file_descriptor]      ; file descriptor
    mov     rax, 3                      ; sys_close
    syscall

    ; Check if file closed successfully.
    test    rax, rax
    js      error

    ; Output the CRC value as binary — use buffer to store characters.
    mov     rcx, 63
    mov     rbx, 0

crc_to_bin:
    ; Convert CRC to a string of '0's and '1's.
    mov     rax, [crc_value]
    shr     rax, cl
    and     rax, 1      ; isolate lowest bit
    add     rax, '0'    ; convert bit to ASCII ('0' or '1')
    mov     [buffer + rbx], al
    inc     rbx
    dec     rcx
    jns     crc_to_bin

    ; Append newline character.
    xor     rax, rax
    mov     al, byte [polynomial_degree]
    mov     [buffer + rax], byte `\n`

    ; Display CRC string on screen.
    mov     rdi, 1          ; stdout
    mov     rsi, buffer     ; CRC string is in buffer
    xor     rdx, rdx
    mov     dl, byte [polynomial_degree]    ; polynomial degree
    inc     rdx                             ; include newline
    mov     rax, 1          ; sys_write
    syscall

    ; Check for write success.
    test    rax, rax
    js      error
    
exit_success:
    ; Exit program successfully.
    mov     rax, 60     ; sys_exit
    xor     rdi, rdi    ; exit code = 0
    syscall

close_and_error:
    ; If an error occurred during processing, close the file.
    mov     rdi, [file_descriptor]      ; file descriptor
    mov     rax, 3                      ; sys_close
    syscall

error:
    ; Exit program with error.
    mov     rax, 60     ; sys_exit
    mov     rdi, 1      ; exit code = 1
    syscall