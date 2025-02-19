/**
 * Program przechowujący wartości 26 zmiennych, wypisujący je oraz
 * wykonujący polecenia w języku Pętlik, w zależności od danych wejściowych.
 *
 * Program czyta wiersz z wejścia, a potem go interpretuje, i tak aż
 * do końca danych wejściowych.
 *
 * Autor: Kacper Pasiński, nr 459461
 * Data: 03.01.2024
*/

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

/* Definiowanie stałych podanych dla zadania */

/**
 * Liczba zmiennych, do których program ma dostęp (od a do z)
 */
#define LICZBA_ZMIENNYCH 26

/**
 * Znak świadczący o tym, że po nim znajduje się znak reprezentujący
 * zmienną, której wartość program ma wypisać
 */
#define POLECENIE_WYPISANIA '='

/**
 * Znak świadczący o początku instrukcji Powtarzaj języka Pętlik
 */
#define POWTARZAJ_POCZATEK '('

/**
 * Znak świadczący o końcu instrukcji Powtarzaj języka Pętlik
 */
#define POWTARZAJ_KONIEC ')'

/**
 * Wartość liczbowa odpowiadająca pierwszej literze alfabetu (a)
 */
#define PIERWSZA_ZMIENNA (int)'a'

/**
 * Wartość liczbowa odpowiadająca najmniejszej cyfrze (zero)
 */
#define INT_OF_CHAR_0 (int)'0'

/**
 * Wstępne zdefiniowanie procedury wykonaj_powtarzaj, potrzebnej później
 */

void wykonaj_powtarzaj(char* wiersz, int poczatek, int koniec, int ile_nawiasow, char* zmienne[], int dlugosci_zmiennych[]);

/* Funkcje i procedury pomagające w reprezentacji i operowaniu na zmiennych*/

/**
 * Funkcja zwraca wartość liczbową podanej litery (od 'a' do 'z'), pomniejszoną
 * o wartość liczbową pierwszej litery alfabetu, co w rezultacie zwraca numer
 * danej litery w tablicy, w której przechowywana jest jej wartość
 * (litery w tablicy mają numery od 0 do 25).
 *
 * @param zmienna Znak bedący literą od 'a' do 'z', wskazujący, której
 *                zmiennej należy zmienić wartość
 *
 * @return        Numer podanej zmiennej w tablicy wartości zmiennych
 *
*/

int zmienna_na_numer_elementu_w_tablicy (char zmienna) {
    return (int)zmienna - PIERWSZA_ZMIENNA;
}

/**
 * Procedura wypisuje wartość zmiennej (liczbę zapisaną w tablicy typu char).
 *
 * @param zmienna   Wskaźnik do tablicy ze znakami przedstawiającymi cyfry
 *                  reprezentujące wartość zmiennej
 *
 * @param dlugosc_zmiennej Liczba cyfr składających się na wartość zmiennej
 *
*/

void wypisz_zmienna(char* zmienna, int dlugosc_zmiennej) {
	for (int i = 0; i < dlugosc_zmiennej; ++i){
        printf("%d", zmienna[i] - '0');
	}
	printf("\n");
}

/**
 * Funkcja zamienia cyfrę (typ int) na znak przedstawiający tę cyfrę.
 *
 * @param x     Cyfra w typie int, którą funkcja zamieni na znak
 *
 * @return      Znak odpowiadający danej cyfrze
 *
*/

char int_to_char (int x){
    return (char)(INT_OF_CHAR_0 + x);
}

/* Procedury odpowiadające instrukcjom maszyny*/

/**
 * Procedura inkrementuje wartość zmiennej (liczbę zapisaną w tablicy typu char).
 * Jest to standardowa inkrementacja na liczbach zapisanych w tablicach, szczególny
 * przypadek to, gdy inkrementowana wartość składa się z samych '9' - wtedy alokujemy
 * pamięć na nową tablicę, z 1 dodatkowym elementem.
 *
 * @param zmienna   Wskaźnik do wskaźnika do tablicy ze znakami przedstawiającymi
 *                  cyfry reprezentujące wartość zmiennej
 *
 * @param dlugosc_zmiennej   Wskaźnik do zmiennej przechowującej liczbę cyfr
 *                           składających się na wartość zmiennej
 *
*/

void INC(char** zmienna, int* dlugosc_zmiennej) {

    for (int i = *dlugosc_zmiennej - 1; i >= 0; --i) {
        if ((*zmienna)[i] == '9') {
            (*zmienna)[i] = '0';
        }
        else{
            (*zmienna)[i]++;
            break;
        }
    }

    if ((*zmienna)[0] == '0') {
        ++(*dlugosc_zmiennej);
        char *nowa_zmienna = (char*) calloc((size_t)*dlugosc_zmiennej, sizeof(char));

        nowa_zmienna[0] = '1';
        for (int i = 1; i < *dlugosc_zmiennej; ++i){
            nowa_zmienna[i] = '0';
        }
        free(*zmienna);
        *zmienna = nowa_zmienna;
    }
}

/**
 * Procedura dodaje do wartości zmiennej 0 wartość zmiennej 1, gdy obie liczby
 * są reprezentowane jako tablice typu char. Jest to standardowe dodawanie na
 * liczbach zapisanych w tablicach, z tym, że cyfry są zapisywane w nowej
 * tablicy od końca. Na początku tworzymy tablicę z wynikiem, której długość
 * jest równa liczbie cyfr większej wartości + 1. W wielu przypadkach jest to
 * więcej niż wystarczająco, a skoro kolejne cyfry są zapisywane w nowej tablicy
 * od końca, to jeśli ostatni element po wykonaniu dodawania jest równy '0',
 * wiadomo, że liczba ma długość o 1 mniejszą niż długość zaalokowanej tablicy.
 * Na końcu tablicę odwracamy i zwalniamy starą wartość zmiennej 0.
 *
 * @param zmienna0   Wskaźnik do wskaźnika do tablicy ze znakami przedstawiającymi
 *                   cyfry reprezentujące wartość zmiennej, do której dodajemy
 *
 * @param dlugosc_zmiennej0  Wskaźnik do zmiennej przechowującej liczbę cyfr
 *                           składających się na wartość zmiennej, do której dodajemy
 *
 * @param zmienna1   Wskaźnik do wskaźnika do tablicy ze znakami przedstawiającymi
 *                   cyfry reprezentujące wartość zmiennej, którą dodajemy
 *
 * @param dlugosc_zmiennej1  Wskaźnik do zmiennej przechowującej liczbę cyfr
 *                           składających się na wartość zmiennej, którą dodajemy
 *
*/

void ADD(char** zmienna0, int* dlugosc_zmiennej0, char** zmienna1, int* dlugosc_zmiennej1) {

    int nowa_dlugosc = (*dlugosc_zmiennej0 > *dlugosc_zmiennej1) ? *dlugosc_zmiennej0 : *dlugosc_zmiennej1;
    ++nowa_dlugosc;

    char* nowa_zmienna = (char*)calloc((size_t)nowa_dlugosc, sizeof(char));

    int przeniesienie = 0;
    int i = 0;
    int suma = 0;

    while (i < nowa_dlugosc) {
        int cyfra0 = (i < *dlugosc_zmiennej0) ? (*zmienna0)[*dlugosc_zmiennej0 - 1 - i] - '0' : 0;
        int cyfra1 = (i < *dlugosc_zmiennej1) ? (*zmienna1)[*dlugosc_zmiennej1 - 1 - i] - '0' : 0;

        suma = cyfra0 + cyfra1 + przeniesienie;
        nowa_zmienna[i] = int_to_char(suma % 10);

        przeniesienie = suma / 10;
        ++i;
    }

    if (nowa_zmienna[nowa_dlugosc - 1] == '0')
        --nowa_dlugosc;

    for (int j = 0; j < nowa_dlugosc / 2; ++j) {
        char pom = nowa_zmienna[j];
        nowa_zmienna[j] = nowa_zmienna[nowa_dlugosc - 1 - j];
        nowa_zmienna[nowa_dlugosc - 1 - j] = pom;
    }

    free(*zmienna0);
    *zmienna0 = nowa_zmienna;
    *dlugosc_zmiennej0 = nowa_dlugosc;
}

/**
 * Procedura ustawia wartość zmiennej na jednoelementową tablicę, gdzie jedynym
 * znakiem jest '0' (wyzerowuje wartość zmiennej). Robi to poprzez zaalokowanie
 * nowej tablicy i zwolnienie starej.
 *
 * @param zmienna   Wskaźnik do wskaźnika do tablicy ze znakami przedstawiającymi
 *                  cyfry reprezentujące wartość zmiennej
 *
 * @param dlugosc_zmiennej   Wskaźnik do zmiennej przechowującej liczbę cyfr
 *                           składających się na wartość zmiennej
 *
*/

void CLR (char** zmienna, int* dlugosc_zmiennej) {
    char* nowa_zmienna = (char*)calloc(1, sizeof(char));
    nowa_zmienna[0] = '0';
    free(*zmienna);
    *zmienna = nowa_zmienna;
    *dlugosc_zmiennej = 1;
}

/**
 * Procedura służy do przeskoczenia do instrukcji o podanym adresie, czyli w praktyce
 * np. do początku pętli. Robi to zamieniając wartość iteratora idącego po znakach
 * instrukcji na numer elementu, pod którym znajduje się początek instrukcji, do której
 * chcemy się przenieść (adres).
 *
 * @param i   Wskaźnik do zmiennej typu int, która w praktyce jest iteratorem idącym
 *            po znakach wczytanego wiersza lub jęgo mniejszej części
 *
 * @param adres   Zmienna typu int, która stanie się nową wartością iteratora
 *
*/

void JMP (int* i, int adres) {
    *i = adres;
}

/**
 * Procedura dekrementuje wartość zmiennej (liczbę zapisaną w tablicy typu char).
 * Jest to standardowa dekrementacja na liczbach zapisanych w tablicach, szczególny
 * przypadek, to gdy inkrementowana wartość składa się z jedynki i n > 0 zer - wtedy
 * alokujemy pamięć na nową tablicę, która ma mniej elementów, a poprzednią zwalniamy.
 * Gdy początkowa wartość zmiennej jest równa 0, to nie dekrementujemy jej, tylko
 * skaczemy do instrukcji o podanym adresie, czyli wywołujemy funkcję JMP.
 *
 * @param zmienna   Wskaźnik do wskaźnika do tablicy ze znakami przedstawiającymi
 *                  cyfry reprezentujące wartość zmiennej
 *
 * @param dlugosc_zmiennej   Wskaźnik do zmiennej przechowującej liczbę cyfr
 *                           składających się na wartość zmiennej
 *
 * @param i   Wskaźnik do zmiennej typu int, która w praktyce jest iteratorem idącym
 *            po znakach wczytanego wiersza lub jęgo mniejszej części
 *
 * @param adres   Zmienna typu int, która stanie się nową wartością iteratora
 *
*/

void DJZ (char** zmienna, int* dlugosc_zmiennej, int* i, int adres) {
    if (*dlugosc_zmiennej == 1 && (*zmienna)[0] == '0'){
        JMP(i, adres);
        return;
    }

    for (int j = *dlugosc_zmiennej - 1; j >= 0; --j) {
        if ((*zmienna)[j] == '0') {
            (*zmienna)[j] = '9';
        }
        else{
            (*zmienna)[j]--;
            break;
        }
    }

    if ((*zmienna)[0] == '0' && *dlugosc_zmiennej > 1) {
        --(*dlugosc_zmiennej);
        char *nowa_zmienna = (char*) calloc((size_t)*dlugosc_zmiennej, sizeof(char));

        for (int j = 0; j < *dlugosc_zmiennej; ++j){
            nowa_zmienna[j] = '9';
        }
        free(*zmienna);
        *zmienna = nowa_zmienna;
    }

}

/* Procedura wczytująca wejście*/

/**
 * Procedura wczytuje wiersz - wszystkie znaki w nim zawarte, dopóki
 * nie natrafi na '\n' (znak końca linii). Dodatkowo zmienna i zlicza, ile
 * tych znaków zostało wczytanych. Po wczytaniu procedura aktualizuje
 * zmienną dlugosc_wiersza, która przechowuje długość wczytanego właśnie wiersza.
 * Jeśli natrafimy nie na '\n', ale na EOF to oprócz końca czytania aktualizujemy
 * zmienną czy_koniec, żeby pętla w int main() mogła zakończyć wczytywanie.
 * Każdy kolejny wiersz jest zapisywany w tej samej zmiennej, dlatego w tej
 * procedurze posługujemy się pomocniczą tablicą, żeby później podmienić ją
 * z tablicą zawierającą poprzedni wiersz oraz usunąć tą drugą.
 *
 * @param wiersz     Wskaźnik do wskaźnika do tablicy, w której przechowywane
 *                   są znaki wiersza.
 *
 * @param dlugosc_wiersza   Wskaźnik do zmiennej typu int, która przechowuje
 *                          długość wczytanego wiersza.
 *
 * @param dlugosc_wiersza   Wskaźnik do zmiennej typu int, która przechowuje
 *                          wartość 0 lub 1, w zależności od tego czy nastąpił
 *                          już koniec pliku, czy nie.
 *
*/

void czytaj_wiersz (char** wiersz, int* dlugosc_wiersza, int* czy_koniec) {

    char *pom = NULL;
    int rozmiar_tablicy = 0;
    int i = 0;
    int c = getchar();

    while (c != '\n' && c != EOF){

        if (i == rozmiar_tablicy){
            if (rozmiar_tablicy < (INT_MAX - 2 ) / 3 * 2){
                rozmiar_tablicy = rozmiar_tablicy / 2 * 3 + 2;
            }
            else{
                rozmiar_tablicy = INT_MAX;
            }
            pom = realloc(pom, (size_t) rozmiar_tablicy * sizeof *pom);
        }

        pom[i] = (char) c;
        ++i;
        c = getchar();
    }

    if (c == EOF){
        *czy_koniec = 1;
    }

    free(*wiersz);
    *wiersz = pom;
    *dlugosc_wiersza = i;
}

/* Procedury wykonujące instrukcje w języku Pętlik*/

/**
 * Procedura wykonuje program w języku Pętlik.
 * To znaczy, wykonuje instrukcje wypisania wartości zmiennej oraz instrukcje
 * niezagnieżdzone w instrukcji Powtarzaj. Aby odnaleźć instrukcje Powtarzaj
 * a następnie przekazać ją do void wykonaj_powtarzaj, która rozpatrzy ją oddzielnie,
 * procedura liczbę lewych nawiasów, prawych nawiasów oraz ich różnice. Jeśli
 * różnica wynosi 0, to wiadomo, że instrukcja Powtarzaj się właśnie zakończyła,
 * więc procedura przekazuje ją całą (tzn. przekazuje wiersz i adres jej początku
 * oraz adres końca) do procedury wykonaj_powtarzaj. Przy pierwszej operacji na
 * wierszu procedura rozpatruje cały wiersz (poczatek - 0, koniec - jego dlugosc - 1),
 * a potem już tylko kawałki (wykonaj_powtarzaj będzie odwoływało się z powrotem
 * do tej funkcji, tak aby cały program w języku Pętlik mógł się wykonać poprawnie).
 *
 * @param wiersz     Wskaźnik do tablicy, w której przechowywane są znaki wiersza.
 *
 * @param poczatek   Liczba reprezentująca adres (numer elementu w tablicy) początku
 *                   wykonywanego fragmentu wiersza.
 *
 * @param koniec     Liczba reprezentująca adres (numer elementu w tablicy) końca
 *                   wykonywanego fragmentu wiersza.
 *
 * @param zmienne  Wskaźnik do wskaźnika do tablicy, która przechowuje wartości
 *                 wszystkich zmiennych (0-25, 'a' - 'z'), do których program ma dostęp.
 *
 * @param dlugosci_zmiennych Wskaźnik do zmiennej typu int, która przechowuje liczby
 *                           cyfr wartości wszystkich zmiennych (0-25, 'a' - 'z').
 *
*/

void wykonaj_program (char* wiersz, int poczatek, int koniec, char* zmienne[], int dlugosci_zmiennych[]) {

    if (koniec == 0)
        return;

    if (wiersz[0] == POLECENIE_WYPISANIA){
        int numer_w_tablicy = zmienna_na_numer_elementu_w_tablicy(wiersz[1]);
        wypisz_zmienna(zmienne[numer_w_tablicy], dlugosci_zmiennych[numer_w_tablicy]);
        return;
    }

    int i = poczatek;
    int ile_lewych_nawiasow = 0;
    int ile_prawych_nawiasow = 0;
    int o_ile_wiecej_lewych = 0;
    int adres_zewnetrznego_lewego_nawiasu = poczatek;
    int adres_zewnetrznego_prawego_nawiasu = poczatek - 1;

    while (i < koniec){
        char znak = wiersz[i];
        if (znak == POWTARZAJ_POCZATEK){
            ++ile_lewych_nawiasow;
            ++o_ile_wiecej_lewych;
            if (o_ile_wiecej_lewych == 1)
                adres_zewnetrznego_lewego_nawiasu = i;
        }
        else if (znak == POWTARZAJ_KONIEC){
            ++ile_prawych_nawiasow;
            --o_ile_wiecej_lewych;
            if (o_ile_wiecej_lewych == 0)
                adres_zewnetrznego_prawego_nawiasu = i;
        }

        if (o_ile_wiecej_lewych == 0){
            if (i > adres_zewnetrznego_prawego_nawiasu){
                int numer_w_tablicy = zmienna_na_numer_elementu_w_tablicy(wiersz[i]);
                INC(&zmienne[numer_w_tablicy], &dlugosci_zmiennych[numer_w_tablicy]);
            }
            else{
                wykonaj_powtarzaj(wiersz, adres_zewnetrznego_lewego_nawiasu, adres_zewnetrznego_prawego_nawiasu, ile_lewych_nawiasow - 1, zmienne, dlugosci_zmiennych);
                ile_lewych_nawiasow = 0;
                ile_prawych_nawiasow = 0;
            }
        }
        ++i;
    }
}

/**
 * Procedura wykonuje instrukcję Powtarzaj w języku Pętlik.
 * Zgodnie ze specyfikają programu wiadomo, że w tablicy zawierającej znaki wiersza
 * na numerze adres + 1 znajduje się najbardziej znacząca zmienna w instrukcji Powtarzaj.
 * Procedura zapamiętuje ją na początku jako Zmienna 0. Najpierw rozpatrujemy, czy można
 * kod zoptymalizować, a jako że procedura dostaje w parametrze liczbę nawiasów nie licząc
 * zewnętrznych nawiasów, to jeśli ta liczba jest równa 0, to wiadomo, że można wykonać
 * zoptymalizowany kod, bo żadne nawiasy w instrukcji nie są zagnieżdzone. Jeśli natomiast
 * jakieś są zagnieżdzone, to wykonujemy "pętlę", tzn. najpierw dekrementujemy zmienną 0
 * (lub jeśli jest ona równa 0, to skaczemy do adresu końca Powtarzaj, czyli funkcja się
 * kończy), potem wykonujemy to, co jest zagnieżdzone w rozpatrywanej instrukcji Powtarzaj,
 * czyli przekazujemy tę część do funkcji wykonaj_program, aby każda instrukcja programu
 * Pętlik mogła się poprawnie wykonać (zauważmy, że wykonaj_powtarzaj wykonuje tylko
 * te instrukcje, a wykonaj_program - pozostałe). Obie funkcje odwołują się więc do siebie,
 * aż wykonana zostanie najbardziej zagnieżdzona instrukcja, działa to podobnie do rekurencji.
 * Kiedy wszystko zostanie wykonane i powrócimy do zewnętrznej instrukcji Powtarzaj to z
 * powrotem wracamy do jej początku, za pomocą procedury JMP.
 *
 * @param wiersz     Wskaźnik do tablicy, w której przechowywane są znaki wiersza.
 *
 * @param poczatek   Liczba reprezentująca adres (numer elementu w tablicy) początku
 *                   wykonywanego fragmentu wiersza.
 *
 * @param koniec     Liczba reprezentująca adres (numer elementu w tablicy) końca
 *                   wykonywanego fragmentu wiersza.
 *
 * @param ile_nawiasow Liczba nawiasów w rozpatrywanej instrukcji Powtarzaj (bez
 *                     tych zewnętrznych, czyli bez znaków o "adresie" początek i koniec.
 *
 * @param zmienne  Wskaźnik do wskaźnika do tablicy, która przechowuje wartości
 *                 wszystkich zmiennych (0-25, 'a' - 'z'), do których program ma dostęp.
 *
 * @param dlugosci_zmiennych Wskaźnik do zmiennej typu int, która przechowuje liczby
 *                           cyfr wartości wszystkich zmiennych (0-25, 'a' - 'z').
 *
*/

void wykonaj_powtarzaj (char* wiersz, int poczatek, int koniec, int ile_nawiasow, char* zmienne[], int dlugosci_zmiennych[]) {
    int i = poczatek + 1;
    int Zmienna0 = zmienna_na_numer_elementu_w_tablicy(wiersz[i]);
    ++i;

    if (ile_nawiasow == 0){
        while (i < koniec){
            int ZmiennaN = zmienna_na_numer_elementu_w_tablicy(wiersz[i]);
            ADD(&zmienne[ZmiennaN], &dlugosci_zmiennych[ZmiennaN], &zmienne[Zmienna0], &dlugosci_zmiennych[Zmienna0]);
            ++i;
        }
        CLR(&zmienne[Zmienna0], &dlugosci_zmiennych[Zmienna0]);
        return;
    }

    while (i < koniec){
        DJZ(&zmienne[Zmienna0], &dlugosci_zmiennych[Zmienna0], &i, koniec);
        wykonaj_program(wiersz, i, koniec, zmienne, dlugosci_zmiennych);
        if (i < koniec)
            JMP(&i, i);
    }

}

/**
 * Tworzy wszystkie zmienne i ustawia je na '0', a także tablicę z ich długościami,
 * oczywiście na początku są tam same jedynki. Potem wczytujemy wiersz po wierszu
 * i go wykonujemy, a na końcu zwalniamy pamięć po zmiennych.
*/

int main(){

	char* zmienne[LICZBA_ZMIENNYCH];

	for (int i = 0; i < LICZBA_ZMIENNYCH; ++i){
	    zmienne[i] = (char*) calloc(1, sizeof(char));
	    zmienne[i][0] = '0';
	}

	int dlugosci_zmiennych[LICZBA_ZMIENNYCH];

	for (int i = 0; i < LICZBA_ZMIENNYCH; ++i){
        dlugosci_zmiennych[i] = 1;
	}

    int dlugosc_wiersza;
	char* wiersz = NULL;
	int czy_koniec = 0;

	while (czy_koniec == 0){
        czytaj_wiersz(&wiersz, &dlugosc_wiersza, &czy_koniec);
        wykonaj_program(wiersz, 0, dlugosc_wiersza, zmienne, dlugosci_zmiennych);
	}

    for (int i = 0; i < LICZBA_ZMIENNYCH; ++i){
        free(zmienne[i]);
    }


	free(wiersz);

	return 0;
}
