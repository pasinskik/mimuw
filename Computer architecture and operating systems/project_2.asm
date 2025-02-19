; "Zadanie 2" AKSO semestr letni 2023
; Moja implementacja polega na dzieleniu dodatniej liczby przez dodatnią
; oraz zmiany znaku wyniku lub reszty, wtedy kiedy potrzeba.
; Autor: Kacper Pasiński
; Data: 19.05.2024

section .text
global mdiv

mdiv:
    ; Argumenty funkcji:
    ; rdi - int64_t *x
    ; rsi - size_t n
    ; rdx - int64_t y

    xor r8, r8  ; r8 będzie 1, gdy dzielna jest ujemna, 0 wpp.
    xor r9, r9  ; r9 to rejestr pomocniczy w negate_array.
    xor r10, r10 ; r10 będzie 1, gdy dzielnik jest ujemny, 0 wpp.
    mov rcx, rsi

    ; Sprawdzenie czy dzielna (liczba w [rdi + 8*(rsi-1)]) jest ujemna.
    mov rax, [rdi + 8*(rsi-1)]
    test rax, rax    ; Sprawdzenie czy bit znaku jest ustawiony.
    jns check_divisor_negative ; Jeśli nie jest ujemna, to sprawdzamy dzielnik.
    mov r8, 1 ; Jeśli jest ujemna, to przechodzimy do zanegowania dzielnika.
    stc

; Zanegowanie liczby zapisanej w U2 to odwrócenie znaków i dodanie jedynki.
; Przed każdym przejściem do tej funkcji trzeba ustawić CL, wyzerować r9
; oraz przenieść rsi do rcx. Tą częścią kodu negujemy dzielną/wynik.
negate_array:
    mov rax, [rdi + 8*r9]
    not rax
    adc rax, 0
    mov [rdi + 8*r9], rax
    inc r9
    loop negate_array
    cmp r8, 0 ; Jeśli r8 jest zerem, to oznacza, że negujemy końcowy wynik.
    jz end_of_function ; Przechodzimy więc do końca funkcji.
    xor r9, r9 ; Jeśli r8 jest jedynką, to oznacza, że to początek programu.

check_divisor_negative:
    test rdx, rdx ; Sprawdzenie czy bit znaku jest ustawiony.
    jns initialization ; Jeśli dzielnik nie jest ujemny, to idziemy dalej.
    not rdx
    add rdx, 1 ; Jeśli jest ujemny, to go negujemy.
    mov r10, 1 ; Informacja o ujemności dzielnika.

; Inicjalizujemy wartości rejestrów.
initialization:
    mov r9, rsi ; r9 teraz będzie pamiętał liczbę n.
    mov rcx, rdx ; Iloraz zapisujemy w rcx.
    xor rdx, rdx ; Zerujemy rdx i rax, żeby poprawnie wykonywać div.
    xor rax, rax

; Dzielenie kolejnych fragmentów dzielnej przez dzielnik (od końca tablicy x).
divide_loop:
    mov rax, [rdi + 8*(rsi-1)]
    div rcx
    mov [rdi + 8*(rsi-1)], rax
    dec rsi
    cmp rsi, 0 ; Sprawdzanie czy cała dzielna została już podzielona.
    jnz divide_loop

; Korekta znaku wyniku oraz reszty po podzieleniu.
sign_change:
    mov rsi, r9 ; Przywracamy liczbę n do rsi.
    mov r11, r10
    xor r11, r8 ; xor r8 i r10 to informacja o tym, czy wynik jest ujemny.
    cmp r11, 0 ; r11 to 1, jeśli jest ujemny, a 0 wpp.
    jnz negative_result

positive_result:
    cmp r10, 0 ; Jeśli wynik jest dodatni i dzielnik też, to kończymy.
    jz end_of_function
    not rdx ; Jeśli dzielnik jest ujemny, to znaczy, że reszta też.
    add rdx, 1 ; Musimy ją więc zanegować.
    jmp check_overflow ; Sprawdzamy czy nie ma overflow.

negative_result:
    cmp r10, 0 ; Jeśli wynik jest ujemny, a dzielnik nie, to negujemy resztę.
    jnz negate_result ; Jeśli dzielnik jest ujemny, to negujemy tylko wynik.
    not rdx
    add rdx, 1

; Negacja wyniku zapisanego pod wskaźnikiem x.
negate_result:
    xor r8, r8
    xor r9, r9
    mov rcx, rsi
    stc ; Ustawianie rejestrów do poprawnego działania negate_array.
    jmp negate_array

; Sprawdzanie overflow następuje tylko gdy wynik jest pozytywny, ponieważ
; jedyna opcja na overflow to podzielenie -100... przez -1.
; Wtedy ostatni element wyniku jest równy 1000... (0x800...).
check_overflow:
    mov rax, 0x8000000000000000
    cmp qword [rdi + 8*(rsi-1)], rax
    jz overflow

; Jeśli wszystko ok, to kończymy funkcję i zwracamy w rax resztę.
end_of_function:
    mov rax, rdx
    ret

; Obsługa dzielenia przez zero lub overflowu.
overflow:
    xor rdi, rdi
    div rdi ; Wywołujemy błąd taki, jak przy dzieleniu przez zero.