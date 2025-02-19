; "Zadanie 3" AKSO semestr letni 2023
; Moja implementacja polega na użyciu algorytmu wykorzystującego lookup table,
; dzięki czemu dane mogą być przetwarzane bajtami, a nie bitami.
; Autor: Kacper Pasiński
; Data: 19.06.2024

section .bss
    buffer      resb 65542  ; bufor o maksymalnym rozmiarze danych jednego
                            ; fragmentu (65536) + rozmiar (2) + przesunięcie (4)

    polynomial          resq 1      ; wielomian
    polynomial_degree   resb 1      ; stopień wielomianu

    file_descriptor     resq 1      ; deskryptor pliku
    data_size           resw 1      ; rozmiar wczytywanego fragmentu

    lookup          resq 256    ; lookup table potrzebne do algorytmu CRC
    crc_value       resq 1      ; CRC

section .text
    global _start

_start:
    ; Walidacja argumentów.
    mov     rdi, [rsp]      ; liczba argumentów (w tym nazwa programu)
    cmp     rdi, 3          ; sprawdzenie, czy są 3 argumenty
    jne     error

    ; Walidacja wielomianu.
    mov     rcx, [rsp + 24] ; rcx = wskaźnik do wielomianu CRC
    mov     al, [rcx]
    test    al, al          ; sprawdzenie, czy wielomian jest stały (pusty)
    jz      error

    xor     rbx, rbx        ; rbx = 0 (będziemy tu zapisywać wynikowy wielomian)
    xor     rax, rax        ; rax = 0 (używane do konwersji znaków)
    xor     dl, dl          ; dl = 0 (stopień wielomianu)

parse_polynomial:
    mov     al, [rcx]       ; pobranie następnego znaku wielomianu
    test    al, al          ; sprawdzenie, czy to koniec ciągu (null terminator)
    jz      calculated_polynomial

    inc     dl              ; zwiększenie stopnia wielomianu

    ; Sprawdzenie wartości znaku.
    cmp     al, '0'
    je      process_zero
    cmp     al, '1'
    je      process_one
    jmp     error           ; błędny znak - różny od '0' i '1'

process_zero:
    shl     rbx, 1      ; przesunięcie wyniku w lewo o 1 bit (dodanie '0')
    jmp     advance

process_one:
    shl     rbx, 1      ; przesunięcie wyniku w lewo o 1 bit
    inc     rbx         ; ustawienie 1 na najmłodszym bicie (dodanie '1')

advance:
    inc     rcx         ; przesunięcie wskaźnika do następnego znaku
    jmp     parse_polynomial

calculated_polynomial:
    mov     [polynomial_degree], dl ; zapisanie stopnia wielomianu

    ; Przesunięcie wyniku na odpowiednią pozycję.
    mov     cl, 64
    sub     cl, dl      ; obliczenie liczby bitów do przesunięcia (64 - stopień)
    shl     rbx, cl     ; przesunięcie wyniku w lewo o odpowiednią liczbę bitów

    mov     [polynomial], rbx   ; zapisanie wielomianu do zmiennej

open_file:
    mov     rdi, [rsp + 16] ; nazwa pliku
    mov     rsi, 0          ; tryb otwarcia - do odczytu
    mov     rax, 2          ; sys_open
    syscall

    ; Sprawdzenie poprawności otwarcia pliku.
    test    rax, rax
    js      error

    mov     [file_descriptor], rax  ; zachowanie deskryptora pliku

    ; Tworzymy lookup table, czyli obliczamy CRC dla każdej możliwej wartości
    ; od 0 do 255 bit po bicie.
    xor     rbx, rbx
    lea     rcx, [lookup]   ; rcx będzie przechowywało adres tablicy lookup

calculate_lookup:
    mov     rax, rbx ; rax = rbx (początkowa wartość CRC)
    shl     rax, 56  ; przesunięcie wartości CRC do najwyższych bitów
    mov     rdx, 8   ; pętla wykona się 8 razy (będziemy iterować przez każdy bit)

crc_bit_by_bit:
    shl     rax, 1
    jnc     skip_xor            ; jeśli najwyższy bit jest '0' to pomijamy xor
    xor     rax, [polynomial]   ; jeśli najwyższy bit jest '1' to wykonujemy xor

skip_xor:
    dec     rdx
    jnz     crc_bit_by_bit

    ; Zapisanie obliczonej wartości CRC do tablicy lookup w odpowiednim miejscu.
    mov     [rcx + rbx * 8], rax  ; CRC dla i znajduje się w [lookup + i * 8]
    inc     rbx
    cmp     rbx, 256
    jne     calculate_lookup

    ; Parsowanie fragmentów pliku.
parse_fragment:
    ; Czytanie rozmiaru danych (2 bajty).
    mov     rdi, [file_descriptor]      ; deskryptor pliku
    mov     rsi, buffer                 ; zapisujemy rozmiar w buffer
    mov     rdx, 2                      ; liczba bajtów do odczytu
    mov     rax, 0                      ; sys_read
    syscall

    ; Sprawdzenie poprawności odczytu.
    test    rax, rax
    js      close_and_error

    movzx   rbx, word [buffer]    ; rbx = rozmiar danych we fragmencie
    mov     [data_size], rbx

    ; Jeśli nie ma bajtów do wczytania - przejdź do czytania przesunięcia.
    test    rbx, rbx
    jz      read_offset

    ; Wczytywanie danych.
    mov     rdi, [file_descriptor]  ; deskryptor pliku
    add     rsi, 2                  ; przesunięcie wskaźnika bufora o 2 bajty
    mov     rdx, rbx                ; liczba bajtów do odczytu
    mov     rax, 0                  ; sys_read
    syscall

    ; Sprawdzenie poprawności odczytu.
    test    rax, rax
    js      close_and_error

    ; Obliczanie CRC dla danych.
    lea     rdi, [buffer + 2]  ; ustawienie wskaźnika na początek danych

    ; Dzięki wcześniej stworzonej lookup table możemy teraz przetwarzać bajt
    ; po bajcie, gdyż mamy już obliczone wartości CRC dla każdego możliwego
    ; bajtu i są one łatwo dostępne.
crc_byte_by_byte:
    test    rbx, rbx
    jz      read_offset     ; jeśli nie ma więcej bajtów danych - kończymy

    mov     al, [rdi]
    shl     rax, 56     ; przetwarzany bajt musi znaleźć się w najwyższych bitach
    xor     rax, [crc_value]    ; aby prawidłowo wykonać xor
    shr     rax, 56
    shl     QWORD [crc_value], 8    ; przygotowujemy miejsce dla kolejnego bajtu
    mov     rax, [lookup + rax * 8] ; znajdujemy obliczony już CRC dla bajtu
    xor     [crc_value], rax

    inc     rdi     ; przejście do następnego bajtu danych
    dec     rbx     ; zmniejszenie liczby bajtów pozostałych do przetworzenia
    jmp     crc_byte_by_byte

read_offset:
    ; Czytanie przesunięcia (4 bajty).
    mov     rdi, [file_descriptor]        ; deskryptor pliku
    mov     rsi, buffer
    add     rsi, 65538      ; zapisujemy przesunięcie w [buffer + 65538]
    mov     rdx, 4          ; liczba bajtów do odczytu
    mov     rax, 0          ; sys_read
    syscall

    ; Sprawdzenie poprawności odczytu.
    test    rax, rax
    js      close_and_error

    ; Wartość przesunięcia negujemy aby sprawdzić, czy jest to liczba przeciwna
    ; do rozmiaru całego obecnego fragmentu (wtedy jest to ostatni fragment).
    mov     eax, dword [buffer + 65538]
    neg     eax

    ; Sprawdzenie, czy to ostatni fragment.
    mov     esi, [data_size]
    add     esi, 6
    cmp     eax, esi
    je      display_crc      ; jeśli jest to koniec - pozostało wypisać wynik

    ; Przesunięcie wskaźnika pliku o wczytane przesunięcie.
    mov     rdi, [file_descriptor]        ; deskryptor pliku
    mov     esi, [buffer + 65538]
    movsxd  rsi, esi        ; wartość przesunięcia
    mov     rdx, 1          ; SEEK_CUR
    mov     rax, 8          ; sys_lseek
    syscall

    ; Sprawdzenie poprawności zmiany offsetu.
    test    rax, rax
    js      close_and_error

    jmp     parse_fragment      ; jeśli wszystko ok, to przetwarzamy plik dalej

display_crc:
    ; Zamknięcie pliku.
    mov     rdi, [file_descriptor]      ; deskryptor pliku
    mov     rax, 3                      ; sys_close
    syscall

    ; Sprawdzenie poprawności zamknięcia pliku.
    test    rax, rax
    js      error

    ; Wypisanie wyniku - wartości CRC, do tego posłuży buffer.
    mov     rcx, 63
    mov     rbx, 0

crc_to_bin:
    ; Pobieranie wartości CRC i przekształcenie na ciąg '0' i '1'.
    mov     rax, [crc_value]
    shr     rax, cl
    and     rax, 1      ; wyodrębniamy najmłodszy bit
    add     rax, '0'    ; przekształcamy bit na znak ASCII ('0' lub '1')
    mov     [buffer + rbx], al
    inc     rbx
    dec     rcx
    jns     crc_to_bin

    ; Dodanie znaku nowej linii na koniec.
    xor     rax, rax
    mov     al, byte [polynomial_degree]
    mov     [buffer + rax], byte `\n`

    ; Wyświetlenie przekształconej wartości CRC na ekranie.
    mov     rdi, 1          ; std_out
    mov     rsi, buffer     ; CRC jest zapisane w buforze
    xor     rdx, rdx
    mov     dl, byte [polynomial_degree]    ; stopień wielomianu
    inc     rdx                             ; zwiększamy, żeby wypisać też '\n'
    mov     rax, 1          ; sys_write
    syscall

    ; Sprawdzenie poprawności wypisania CRC.
    test    rax, rax
    js      error
    
exit_success:
    ; Zakończenie programu.
    mov     rax, 60     ; sys_exit
    xor     rdi, rdi    ; kod zakończenia = 0
    syscall

close_and_error:
    ; Jeśli błąd pojawił się podczas przetwarzania, to plik trzeba zamknąć.
    mov     rdi, [file_descriptor]      ; deskryptor pliku
    mov     rax, 3                      ; sys_close
    syscall

error:
    ; Zakończenie działania programu z błędem.
    mov     rax, 60     ; sys_exit
    mov     rdi, 1      ; kod zakończenia = 1
    syscall