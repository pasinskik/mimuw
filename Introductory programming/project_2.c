/**
 * Program interpretujący słowo w podanym L-systemie.
 *
 * Program czyta z wejścia długość wyprowadzenia, opis L-systemu oraz jego
 * interpretacji i pisze na wyjście interpretację wyprowadzonego słowa
 * należącego do języka definiowanego przez L-system.
 *
 * Autor: Kacper Pasiński, nr 459461
 * Data: 12.12.2023
*/

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

/* Definiowanie stałych podanych dla zadania */

/**
 * Maksymalna liczba znaków (bez '\n'), jakie może mieć wiersz
 * Jest to wartość podana w treści zadania zaliczeniowego.
 * Kiedy program wczytuje reguły zastępowania, w których '\n'
 * "nie ma znaczenia", przyjmuje że każdy wiersz ma co najwyżej 100
 * znaków, a kiedy wczytuje reguły interpretacji, przyjmuje, że każdy
 * wiersz ma co najwyżej 100+1 znaków, co jest szerzej opisane
 * w komentarzach do funkcji wczytujących.
 *
 */
#define LIMIT_ZNAKOW_WIERSZA 100

/* Procedura pomagająca w działaniach na tablicach*/

/**
 * Procedura zmienia rozmiar tablicy na podaną długość. Do tego celu
 * tworzy nową tablicę, do której przepisuje wszystkie wartości poprzedniej
 * tablicy, następnie tę poprzednią zwalnia, a potem wskaźnik do starej tablicy
 * zastępuje wskaźnikiem do nowej tablicy.
 *
 * @param tablica   Wskaźnik do wskaźnika do tablicy, której długość zostaje
 *                  zmieniona.
 *
 * @param stara_dlugosc  Zmienna int przechowująca długość tablicy na początku.
 * @param nowa_dlugosc   Zmienna int przechowująca docelową długość tablicy.
 *
*/

void zmien_rozmiar_tablicy(char** tablica, int stara_dlugosc, int nowa_dlugosc) {

    char* nowa_tablica = (char*)calloc((size_t)nowa_dlugosc, sizeof(char));

    for (int i = 0; i < stara_dlugosc; ++i)
        nowa_tablica[i] = (*tablica)[i];

    free(*tablica);

    *tablica = nowa_tablica;
}

/* Procedury (funkcje) wczytujące dane z wejścia programu*/

/**
 * Funkcja wczytuje pierwszy (zawsze niepusty) wiersz, w którym
 * zapisana jest długość wyprowadzenia. Mając na uwadze, że nie
 * musi to być liczba jednocyfrowa, trzeba wczytać najpierw cały napis
 * do pomocnicznej tablicy (temp_dlugosc_wyprowadzenia), jednocześnie
 * zliczając liczbę elementów tej tablicy (i). Kiedy wiersz się skończy,
 * wiadomo, że na pozycji i-tej w pomocniczej tablicy jest cyfra jedności,
 * na pozycji (i-1)-tej cyfra dziesiątek, itd. Za pomocą zmiennej pomocniczej
 * i algorytmu zamieniającego napis na liczbę funkcja odczytuje długość
 * wyprowadzenia, którą następnie zwraca.
 *
 *
 * @return Liczba nieujemna określająca długość wyprowadzenia słowa.
 *
*/

int wez_dlugosc_wyprowadzenia(void) {

    char temp_dlugosc_wyprowadzenia[LIMIT_ZNAKOW_WIERSZA];
    int i = 0;
    int c = getchar();

    while (c != '\n'){
        temp_dlugosc_wyprowadzenia[i] = (char) c;
        ++i;
        c = getchar();
    }

    int dlugosc_wyprowadzenia = 0;
    int pom = 1;

    while (i > 0){
        dlugosc_wyprowadzenia += (pom * (temp_dlugosc_wyprowadzenia[i-1] - '0'));
        pom *= 10;
        --i;
    }

    return dlugosc_wyprowadzenia;
}

/**
 * Procedura wczytuje aksjomat - wszystkie znaki w drugim wierszu,
 * oprócz '\n' (znaku końca linii). Dodatkowo zmienna i zlicza, ile
 * tych znaków zostało wczytanych. Po wczytaniu procedura aktualizuje
 * zmienną dlugosc_aksjomatu, która przechowuje długość wczytanego
 * właśnie aksjomatu.
 *
 * @param aksjomat          Wskaźnik do tablicy, w której przechowywane będą
 *                          znaki składające się na aksjomat. Ta tablica ma
 *                          LIMIT_ZNAKOW_WIERSZA elementów, bo to największa
 *                          liczba znaków, jakie może zawierać aksjomat.
 *
 * @param dlugosc_aksjomatu Wskaźnik do zmiennej typu int, która przechowuje
 *                          długość wczytanego aksjomatu.
 *
*/

void wez_aksjomat(char* aksjomat, int* dlugosc_aksjomatu) {

    int i = 0;
    int c = getchar();

    while (c != '\n'){
        aksjomat[i] = (char) c;
        ++i;
        c = getchar();
    }

    *dlugosc_aksjomatu = i;
}

/**
 * Procedura wczytuje reguły zastępowania - wszystkie znaki w danym wierszu,
 * oprócz '\n' (znaku końca linii). Kiedy taki znak wystąpi, zaczyna wczytywać
 * znaki z kolejnego wiersza, chyba, że pierwszy znak w tym wierszu to '\n', co
 * oznacza, że trzeba przestać wczytywanie, bo dotarliśmy do pustego wiersza
 * przed prologiem. Dodatkowo zmienna i zlicza, ile znaków w wierszu zostało
 * wczytanych i zapisuje tę wartość w tablicy z długościami reguł zastępowania.
 *
 * @param reguly_zastepowania   Wskaźnik do tablicy, w której przechowywane będą
 *                              reguły zastępowania. Ta tablica jest dwuwymiarowa,
 *                              ma CHAR_MAX * LIMIT_ZNAKOW_WIERSZA elementów, bo
 *                              maksymalnie może być tyle reguł, ile różnych znaków
 *                              od 0 do wartości CHAR_MAX, a każda reguła może zawierać
 *                              maksymalnie LIMIT_ZNAKOW_WIERSZA znaków.
 *
 * @param dlugosci_regul_zastepowania Wskaźnik do tablicy, która dla każdej reguły
 *                                    przechowuje jej długość (liczbę znaków jej wiersza).
 *
 * @param ile_regul_zastepowania Wskaźnik do zmiennej typu int, która przechowuje
 *                               liczbę reguł zastępowania w danym L-systemie.
 *
*/

void czytaj_reguly_zastepowania(char reguly_zastepowania[][LIMIT_ZNAKOW_WIERSZA],
                                int dlugosci_regul_zastepowania[], int* ile_regul_zastepowania) {
    int w = 0;
    int i = 0;
    int c = getchar();

    while (w < CHAR_MAX) {
        while (c != '\n') {
            reguly_zastepowania[w][i] = (char) c;
            ++i;
            c = getchar();
        }

        if (i != 0){
            dlugosci_regul_zastepowania[w] = i;
            ++w;
        }
        else
            break;

        i = 0;
        c = getchar();
    }

    *ile_regul_zastepowania = w;
}

/**
 * Funkcja przegląda długości wszystkich wczytanych reguł zastępowania i
 * wybiera tę największą. Nawet jeśli reguł nie ma to funkcja i tak zwraca 1,
 * dlatego, że dzięki wartości tej funkcji program będzie obliczał maksymalną
 * długość słowa (np. jeśli słowo ma w danej chwili długość 2, a najdłuższa
 * reguła ma długość 5 to maksymalnie słowo po następnym przekształceniu może
 * mieć długość 10 (właściwie to 8, ale nie ma potrzeby jeszcze odejmować, bo
 * te kilka elementów nie zrobi różnicy w użyciu pamięci, a pozwoli uniknąć
 * wyjścia poza zakres tablicy).
 * tworzy nową tablicę, do której przepisuje wszystkie wartości poprzedniej
 *
 * @param dlugosci_regul_zastepowania Wskaźnik do tablicy, która dla każdej reguły
 *                                    przechowuje jej długość (liczbę znaków jej wiersza).
 *
 * @param ile_regul_zastepowania    Zmienna typu int, która przechowuje liczbę
 *                                  reguł zastępowania w danym L-systemie.
 *
 * @return Liczba określająca długość najdłuższej reguły zastępowania
 *         lub 1, jeśli nie ma żadnych reguł w podanym L-systemie.
 *
*/

int jaka_najdluzsza_regula_zastepowania (int dlugosci_regul_zastepowania[], int ile_regul_zastepowania){

    int pom = 1;

    for (int i = 0; i < ile_regul_zastepowania; ++i){
        if (dlugosci_regul_zastepowania[i] > pom)
            pom = dlugosci_regul_zastepowania[i];
    }

    return pom;
}

/**
 * Procedura przekształca aksjomat, a potem powstałe z niego słowo tyle razy,
 * ile jest to zapisane w zmiennej dlugosc_wyprowadzenia. Aby to zrobić najpierw
 * wykorzystuje funkcję pomocniczą, aby określić długość najdłuższej reguły
 * zastępowania, co będzie potrzebne, aby poprawnie stworzyć tablicę, w której
 * funkcja będzie zapisywać przekształcone słowo. Ta tablica to slowo_nowe.
 * Dla każdego znaku w przekształcanym słowie (slowo) funkcja sprawdza, czy
 * jest taka reguła zastępowania, którą można zaaplikować na ten znak. Jeśli tak,
 * to właśnie to robi, i zapisuje kolejne elementy nowego słowa w tablicy slowo_nowe
 * Jeśli nie (!czy_jest_na_to_regula), to po prostu przepisuje ten znak bez zmian.
 * Na koniec pętli funkcja aktualizuje długość słowa, pozbywa się starego słowa
 * (free(*slowo)), zastępując je nowym (aktualizując wskaźnik do tablicy slowo).
 *
 * @param dlugosc_wyprowadzenia     Zmienna typu int, która przechowuje długość
 *                                  wyprowadzenia słowa.
 *
 * @param dlugosc_slowa             Wskaźnik do zmiennej typu int, która przechowuje
 *                                  aktualną długość słowa.
 *
 * @param slowo                     Wskaźnik do wskaźnika do tablicy, w której
 *                                  przechowywane jest przekształcane słowo.
 *
 * @param ile_regul_zastepowania    Zmienna typu int, która przechowuje liczbę
 *                                  reguł zastępowania w danym L-systemie.
 *
 * @param dlugosci_regul_zastepowania Wskaźnik do tablicy, która dla każdej reguły
 *                                    przechowuje jej długość (liczbę znaków jej wiersza).
 *
 * @param reguly_zastepowania   Wskaźnik do tablicy, w której przechowywane są
 *                              reguły zastępowania. Ta tablica jest dwuwymiarowa,
 *                              ma CHAR_MAX * LIMIT_ZNAKOW_WIERSZA elementów, bo
 *                              maksymalnie może być tyle reguł, ile różnych znaków
 *                              od 0 do wartości CHAR_MAX, a każda reguła może zawierać
 *                              maksymalnie LIMIT_ZNAKOW_WIERSZA znaków.
 *
*/

void przeksztalcamy_slowo(int dlugosc_wyprowadzenia, int* dlugosc_slowa, char** slowo,
                            int ile_regul_zastepowania, int dlugosci_regul_zastepowania[],
                            char reguly_zastepowania[][LIMIT_ZNAKOW_WIERSZA]){

    int najdluzsza_regula_zastepowania = jaka_najdluzsza_regula_zastepowania(dlugosci_regul_zastepowania,
                                                                             ile_regul_zastepowania);

	for (int i = 0; i < dlugosc_wyprowadzenia; ++i){
	    int max_dlugosc_slowa = *dlugosc_slowa * najdluzsza_regula_zastepowania;
	    char* slowo_nowe = (char*)calloc((size_t)max_dlugosc_slowa, sizeof(char));
	    int j_stare = 0;
	    int j_nowe = 0;

	    while (j_stare < *dlugosc_slowa){
	        char temp_char = (*slowo)[j_stare];
	        int czy_jest_na_to_regula = 0;

	        for (int k = 0; k < ile_regul_zastepowania; ++k){
	            if (temp_char == reguly_zastepowania[k][0]){
	                for (int l = 1; l < dlugosci_regul_zastepowania[k]; ++l){
	                    slowo_nowe[j_nowe] = reguly_zastepowania[k][l];
	                    ++j_nowe;
	                }
	                czy_jest_na_to_regula = 1;
	                break;
	            }
	        }
            if (!czy_jest_na_to_regula) {
                slowo_nowe[j_nowe] = temp_char;
                ++j_nowe;
            }
            ++j_stare;
	    }

	    *dlugosc_slowa = j_nowe;

    	free(*slowo);
    	*slowo = slowo_nowe;
	}
}

/**
 * Procedura wczytuje prolog i epilog - wszystkie znaki do napotkania dwóch
 * znaków '\n' występujących po sobie. Kiedy tak się stanie, przestaje
 * wczytywać, bo to znaczy, że prolog (epilog) się skończył. Z racji tego,
 * że nie wiadomo, ile znaków ma prolog (epilog), a może mieć "bardzo dużo",
 * procedura używa funkcji zmien_rozmiar_tablicy, kiedy pozostały jeszcze
 * znaki do wczytania, a miejsce w tablicy, w której prolog (epilog) jest
 * zapisywany już się skończyło. Dodatkowo zmienna i zlicza ile znaków
 * wczytano, a procedura na końcu zapisuje tę wartość w zmiennej przechowującej
 * długość prologu (epilogu). Na końcu sprawdzamy też warunek (i != 0), bo jeśli
 * tak, to znaczy, że prolog (epilog) nie był pusty, a więc sprawdzając, czy
 * to już jego koniec (czy po '\n' jest '\n') pominęliśmy wpisanie jednego
 * znaku '\n', który jest elementem prologu (epilogu), a więc dodajemy go, aby
 * program działał poprawnie.
 *
 * @param arr       Wskaźnik do wskaźnika do tablicy, w której zapisany będzie
 *                  prolog lub epilog. Ta tablica może zmieniać swoją długość
 *                  w trakcie działania programu, a jej aktualny rozmiar
 *                  przechowuje zmienna rozmiar_tablicy.
 *
 *
 * @param dlugosc_logu      Wskaźnik do zmiennej typu int, która przechowuje
 *                          liczbę znaków prologu lub epilogu.
 *
 * @param rozmiar_tablicy   Zmienna int przechowująca aktualny rozmiar tablicy,
                            do której wczytujemy prolog lub epilog.
 *
*/

void czytaj_log(char** arr, int* dlugosc_logu, int rozmiar_tablicy) {

    int i = 0;
    int c = getchar();

    while (c != EOF) {

        if (i == rozmiar_tablicy){
            zmien_rozmiar_tablicy(arr, rozmiar_tablicy, 2 * rozmiar_tablicy);
            rozmiar_tablicy *= 2;
        }

        if (c == '\n' && i != 0){
            c = getchar();
            if (c == '\n' || c == EOF)
                break;
            else{
                (*arr)[i] = '\n';
                ++i;
            }
        }
        else if (c == '\n' && i == 0)
            break;
        else{
            (*arr)[i] = (char) c;
            ++i;

            c = getchar();
        }
    }

    if (i != 0){
        if (i == rozmiar_tablicy)
            zmien_rozmiar_tablicy(arr, rozmiar_tablicy, 2 * rozmiar_tablicy);
        (*arr)[i] = '\n';
        ++i;
    }

    *dlugosc_logu = i;
}

/**
 * Procedura wczytuje reguły interpretacji - wszystkie znaki w danym wierszu,
 * włącznie z '\n' (znakiem końca linii). Kiedy taki znak wystąpi, zaczyna wczytywać
 * znaki z kolejnego wiersza, chyba, że pierwszy znak w tym wierszu to '\n', co
 * oznacza, że trzeba przestać wczytywanie, bo dotarliśmy do pustego wiersza
 * przed epilogiem. Dodatkowo zmienna i zlicza, ile znaków w wierszu zostało
 * wczytanych i zapisuje tę wartość w tablicy z długościami reguł interpretacji.
 *
 * @param reguly_interpretacji  Wskaźnik do tablicy, w której przechowywane będą
 *                              reguły interpretacji. Ta tablica jest dwuwymiarowa,
 *                              ma CHAR_MAX * LIMIT_ZNAKOW_WIERSZA+1 elementów, bo
 *                              maksymalnie może być tyle reguł, ile różnych znaków
 *                              od 0 do wartości CHAR_MAX, a każda reguła może zawierać
 *                              maksymalnie LIMIT_ZNAKOW_WIERSZA+1 znaków (wczytujemy
 *                              również '\n' na końcu wiersza).
 *
 * @param dlugosci_regul_interpretacji Wskaźnik do tablicy, która dla każdej reguły
 *                                     przechowuje jej długość (liczbę znaków jej wiersza).
 *
 * @param ile_regul_interpretacji Wskaźnik do zmiennej typu int, która przechowuje
 *                                liczbę reguł interpretacji w danym L-systemie.
 *
*/

void czytaj_reguly_interpretacji(char reguly_interpretacji[][LIMIT_ZNAKOW_WIERSZA+1],
                                int dlugosci_regul_interpretacji[], int* ile_regul_interpretacji) {
    int w = 0;
    int i = 0;
    int c = getchar();

    while (w < CHAR_MAX) {
        while (c != '\n') {
            reguly_interpretacji[w][i] = (char) c;
            ++i;
            c = getchar();
        }

        reguly_interpretacji[w][i] = (char) c;
        ++i;

        if (i != 1){
            dlugosci_regul_interpretacji[w] = i;
            ++w;
        }
        else
            break;

        i = 0;
        c = getchar();
    }

    *ile_regul_interpretacji = w;
}

/* Procedura wypisująca końcowe słowo, po wszystkich przekształceniach */

/**
 * Procedura działa podobnie do tej, która przekształcała słowo, jednak tutaj
 * nie robi tego w pętli, a więc może po prostu wypisywać kolejne elementy.
 * Dla każdego znaku w przekształcanym słowie (slowo) funkcja sprawdza, czy
 * jest taka reguła interpretacji, którą można zaaplikować na ten znak. Jeśli tak,
 * to właśnie to robi, i wypisuje kolejne elementy, a jeśli nie - pomija ten znak.
 *
 *
 * @param dlugosc_slowa             Wskaźnik do zmiennej typu int, która przechowuje
 *                                  aktualną długość słowa.
 *
 * @param slowo                     Wskaźnik do wskaźnika do tablicy, w której
 *                                  przechowywane jest przekształcane słowo.
 *
 * @param ile_regul_interpretacji Wskaźnik do zmiennej typu int, która przechowuje
 *                                liczbę reguł interpretacji w danym L-systemie.
 *
 * @param dlugosci_regul_interpretacji Wskaźnik do tablicy, która dla każdej reguły
 *                                     przechowuje jej długość (liczbę znaków jej wiersza).
 *
 *
 * @param reguly_interpretacji  Wskaźnik do tablicy, w której przechowywane są
 *                              reguły interpretacji. Ta tablica jest dwuwymiarowa,
 *                              ma CHAR_MAX * LIMIT_ZNAKOW_WIERSZA+1 elementów, bo
 *                              maksymalnie może być tyle reguł, ile różnych znaków
 *                              od 0 do wartości CHAR_MAX, a każda reguła może zawierać
 *                              maksymalnie LIMIT_ZNAKOW_WIERSZA+1 znaków (wczytujemy
 *                              również '\n' na końcu wiersza).
 *
*/

void pisz_koncowe_slowo(int* dlugosc_slowa, char** slowo, int ile_regul_interpretacji, int dlugosci_regul_interpretacji[],
                            char reguly_interpretacji[][LIMIT_ZNAKOW_WIERSZA+1]){
    int i = 0;

    while (i < *dlugosc_slowa){
        char temp_char = (*slowo)[i];

        for (int j = 0; j < ile_regul_interpretacji; ++j){
            if (temp_char == reguly_interpretacji[j][0]){
                for (int k = 1; k < dlugosci_regul_interpretacji[j]; ++k){
                    printf("%c", reguly_interpretacji[j][k]);
                }
                break;
            }
        }
        ++i;
    }
}

/**
 * Tworzy wszystkie zmienne, do których wczytane zostaną dane z wejścia, a także
 * kilka zmiennych pomocnicznych, przydatnych w powyższych funkcjach. Po wczytaniu
 * wszystkiego wypisuje prolog, potem końcowe słowo uruchamiając odpowiednią funkcję,
 * a na końcu wypisuje epilog. Następnie zwalnia wszystkie zalokowane tablice.
*/

int main(){

	int dlugosc_wyprowadzenia = wez_dlugosc_wyprowadzenia();

	int dlugosc_slowa;
    char* slowo = (char*)calloc(LIMIT_ZNAKOW_WIERSZA, sizeof(char));
    wez_aksjomat(slowo, &dlugosc_slowa);

    int ile_regul_zastepowania;
    int dlugosci_regul_zastepowania[CHAR_MAX];
	char reguly_zastepowania[CHAR_MAX][LIMIT_ZNAKOW_WIERSZA];
	czytaj_reguly_zastepowania(reguly_zastepowania, dlugosci_regul_zastepowania, &ile_regul_zastepowania);

	przeksztalcamy_slowo(dlugosc_wyprowadzenia, &dlugosc_slowa, &slowo, ile_regul_zastepowania,
	                        dlugosci_regul_zastepowania, reguly_zastepowania);

	int dlugosc_prologu;
    char* prolog = (char*)calloc(LIMIT_ZNAKOW_WIERSZA, sizeof(char));
    czytaj_log(&prolog, &dlugosc_prologu, LIMIT_ZNAKOW_WIERSZA);

    int ile_regul_interpretacji;
    int dlugosci_regul_interpretacji[CHAR_MAX];
	char reguly_interpretacji[CHAR_MAX][LIMIT_ZNAKOW_WIERSZA+1];
	czytaj_reguly_interpretacji(reguly_interpretacji, dlugosci_regul_interpretacji, &ile_regul_interpretacji);

	int dlugosc_epilogu;
    char* epilog = (char*)calloc(LIMIT_ZNAKOW_WIERSZA, sizeof(char));
    czytaj_log(&epilog, &dlugosc_epilogu, LIMIT_ZNAKOW_WIERSZA);

	for (int i = 0; i < dlugosc_prologu; ++i)
	    printf("%c", prolog[i]);

	pisz_koncowe_slowo(&dlugosc_slowa, &slowo, ile_regul_interpretacji, dlugosci_regul_interpretacji, reguly_interpretacji);

	for (int i = 0; i < dlugosc_epilogu; ++i)
	    printf("%c", epilog[i]);

	free(slowo);
	free(prolog);
	free(epilog);

	return 0;
}