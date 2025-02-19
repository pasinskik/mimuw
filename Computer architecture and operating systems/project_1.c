/**
 * "Zadanie 1"
 *
 * Program jest implementacją dynamicznie ładowanej biblioteki
 * obsługującej kombinacyjne układy boolowskie złożone z bramek NAND.
 *
 * autor:   Kacper Pasiński
 * data:    21/04/2024
 */

 #include <stdio.h>
 #include <stdbool.h>
 #include <stdlib.h>
 #include <errno.h>
 #include <sys/types.h>
 
 /* Definiowanie struktur potrzebnych do zadania. */
 
 typedef struct nand nand_t;
 
 /**
  * Struktura przechowująca zródło sygnału wejścia. Jeśli nic nie jest podłączone,
  * to nand_source wskazuje na NULL, a is_bool ma wartość false.
  *
  * @param   is_bool                 Zmienna określająca czy sygnał jest boolem.
  * @param   bool_source             Wskaźnik do sygnału boolowskiego, który jest
  *                                  podłączony do wejścia (lub NULL).
  * @param   nand_source             Wskaźnik do bramki, która jest podłączona do
  *                                  tego wejścia (lub NULL).
  * @param   output_gates_position   Pozycja, na której ta bramka znajduje się
  *                                  w tablicy wyjść bramki nand_source.
  *
  */
 
 typedef struct input_source {
     bool is_bool;
     bool *bool_source;
     nand_t *nand_source;
     ssize_t output_gates_position;
 } input_source_t;
 
 /**
  * Struktura przechowująca informacje o bramce, do której podłączone jest wyjście danej bramki.
  *
  * @param   output_gate         Wskaźnik do bramki, do której jest podłączone
  *                              wyjście danej bramki.
  * @param   inputs_position     Pozycja, na której ta bramka znajduje się
  *                              w tablicy wejść bramki output_gate.
  *
  */
 
 typedef struct output_gate {
     nand_t *output_gate;
     unsigned inputs_position;
 } output_gate_t;
 
 /**
  * Struktura reprezentująca bramkę NAND.
  *
  * @param   inputs_number           Liczba wejść bramki.
  * @param   inputs                  Tablica wejść bramki.
  * @param   output_gates_number     Liczba bramek, do których podłączone jest
  *                                  wyjście tej bramki.
  * @param   output_gates            Tablica bramek, do których podłączone jest
  *                                  wyjście tej bramki.
  * @param   visited                 Informacja, czy bramka została już odwiedzona
  *                                  (potrzebna do późniejszej funkcji).
  * @param   calculated              Informacja, czy bramka została już obliczona
  *                                  i jej output oraz ścieżka krytyczna są poprawne.
  * @param   output                  Sygnał wyjściowy bramki.
  * @param   critical_path           Wartość scieżki krytycznej bramki.
  *
  */
 
 typedef struct nand {
     unsigned inputs_number;
     input_source_t *inputs;
     ssize_t output_gates_number;
     output_gate_t *output_gates;
     bool visited;
     bool calculated;
     bool output;
     ssize_t critical_path;
 } nand_t;
 
 /* Funkcje wykonujące podstawowe operacje na bramkach NAND. */
 
 /**
  * Funkcja inicjalizuje bramkę NAND o n wejściach i ustawia wartości wszystkich
  * parametrów na 0/false/NULL.
  *
  * @param n     Liczba wejść inicjalizowanej bramki.
  *
  * @return      Wskaźnik do zainicjowanej bramki.
  *
 */
 
 nand_t *nand_new(unsigned n) {
     nand_t *gate = malloc(sizeof(nand_t));
     if (gate == NULL) {
         errno = ENOMEM;
         return NULL;
     }
 
     gate->inputs_number = n;
     gate->output_gates_number = 0;
     gate->output_gates = NULL;
     gate->output = false;
     gate->visited = false;
     gate->calculated = false;
     gate->critical_path = 0;
 
     if (n == 0) {
         gate->inputs = NULL;
         return gate;
     }
 
     gate->inputs = malloc(n * sizeof(input_source_t));
     if (gate->inputs == NULL) {
         free(gate);
         errno = ENOMEM;
         return NULL;
     }
 
     for (unsigned i = 0; i < n; ++i) {
         gate->inputs[i].is_bool = false;
         gate->inputs[i].nand_source = NULL;
         gate->inputs[i].bool_source = NULL;
         gate->inputs[i].output_gates_position = 0;
     }
 
     return gate;
 }
 
 /**
  * Funkcja odłącza wejście numer k w bramce (po jej wywołaniu na wejście
  * nic nie jest podłączone).
  *
  * @param g     Wskaźnik do bramki.
  * @param k     Numer odłączanego wejścia.
  *
  * @return      Informacja zwrotna o (nie)powodzeniu funkcji.
  *
 */
 
 int nand_disconnect_an_input(nand_t *g, unsigned k) {
     if (g == NULL || g->inputs_number <= k) {
         errno = EINVAL;
         return -1;
     }
 
     g->inputs[k].is_bool = false;
     g->inputs[k].nand_source = NULL;
     g->inputs[k].bool_source = NULL;
 
     return 0;
 }
 
 /**
  * Funkcja odłącza wyjście numer k w bramce.
  *
  * @param g     Wskaźnik do bramki.
  * @param k     Numer odłączanego wyjścia.
  *
  * @return      Informacja zwrotna o (nie)powodzeniu funkcji.
  *
 */
 
 int nand_disconnect_an_output_gate(nand_t *g, unsigned k) {
     if (g == NULL || g->output_gates_number <= k) {
         errno = EINVAL;
         return -1;
     }
 
     --g->output_gates_number;
 
     //Jeśli odłączane wyjście było jedynym, trzeba zwolnić pamięć po output_gates
     if (g->output_gates_number == 0) {
         free(g->output_gates);
         g->output_gates = NULL;
         return 0;
     }
 
     //Jeśli odłączane wyjście nie było na ostatniej pozycji, trzeba poprawić indeksy
     if (k != g->output_gates_number) {
         g->output_gates[k] = g->output_gates[g->output_gates_number];
         g->output_gates[k].output_gate
         ->inputs[g->output_gates[k].inputs_position].output_gates_position = k;
     }
 
     g->output_gates = realloc(g->output_gates,
                               g->output_gates_number * sizeof(output_gate_t));
     if (g->output_gates == NULL) {
         errno = ENOMEM;
         return -1;
     }
 
     return 0;
 }
 
 /**
  * Funkcja odłącza sygnały wejściowe i wyjściowe danej bramki, a następnie
  * zwalnia po niej pamięć (usuwa ją). Dla każdego wejścia funkcja odłącza
  * wyjście na którym znajduje się usuwana bramka i na odwrót.
  * Po jej wykonaniu wskaźnik staje się nullem.
  *
  * @param g     Wskaźnik do bramki.
  *
  * @return      Informacja zwrotna o (nie)powodzeniu funkcji.
  *
 */
 
 void nand_delete(nand_t *g) {
     if (g == NULL) {
         return;
     }
 
     for (unsigned i = 0; i < g->inputs_number; ++i) {
         if (!g->inputs[i].is_bool && g->inputs[i].nand_source != NULL) {
             if (nand_disconnect_an_output_gate(g->inputs[i].nand_source,
                                                g->inputs[i].output_gates_position) == -1) {
                 errno = ECANCELED;
                 return;
             }
         }
     }
 
     for (ssize_t i = 0; i < g->output_gates_number; ++i) {
         if (g->output_gates[i].output_gate != NULL) {
             if (nand_disconnect_an_input(g->output_gates[i].output_gate,
                                          g->output_gates[i].inputs_position) == -1) {
                 errno = ECANCELED;
                 return;
             }
         }
     }
 
     free(g->inputs);
     free(g->output_gates);
 
     free(g);
 }
 
 /* Funkcje podłączające sygnały do wejść bramki NAND. */
 
 /**
  * Funkcja podłącza wyjście bramki g_out do wejścia k bramki g_in, ewentualnie
  * odłączając od tego wejścia sygnał, który był do niego dotychczas podłączony.
  * Podłączanie nowego wyjścia to w g_in podmiana k-tego wejścia na nowe,
  * a w g_out powiększenie pamięci zaalokowanej na output_gates
  * i dodanie nowego elementu do tej tablicy.
  *
  * @param g_out     Wskaźnik do bramki, której wyjście podłącza się na wejście g_in.
  * @param g_in      Wskaźnik do bramki, na której wejście podłącza się wyjście g_out.
  * @param k         Numer wejścia, na które podłącza się wyjście g_out.
  *
  * @return          Informacja zwrotna o (nie)powodzeniu funkcji.
  *
 */
 
 int nand_connect_nand(nand_t *g_out, nand_t *g_in, unsigned k) {
     if (g_out == NULL || g_in == NULL || g_in->inputs_number <= k) {
         errno = EINVAL;
         return -1;
     }
 
     if (g_in->inputs[k].is_bool == false) {
         nand_disconnect_an_output_gate(g_in->inputs[k].nand_source,
                                        g_in->inputs[k].output_gates_position);
     }
     nand_disconnect_an_input(g_in, k);
 
     g_in->inputs[k].nand_source = g_out;
     g_in->inputs[k].output_gates_position = g_out->output_gates_number;
 
     ++g_out->output_gates_number;
     g_out->output_gates =
             realloc(g_out->output_gates,
                     g_out->output_gates_number * sizeof(output_gate_t));
     if (g_out->output_gates == NULL) {
         errno = ENOMEM;
         return -1;
     }
     g_out->output_gates[g_out->output_gates_number - 1].output_gate = g_in;
     g_out->output_gates[g_out->output_gates_number - 1].inputs_position = k;
 
     return 0;
 }
 
 /**
  * Funkcja podłącza sygnał boolowski do wejścia k bramki g_in, ewentualnie
  * odłączając od tego wejścia sygnał, który był do niego dotychczas podłączony.
  * Podłączając castuje bool const * na bool *, świadomie usuwając const.
  *
  * @param s         Wskaźnik do sygnału boolowskiego podłączanego na wejście g.
  * @param g         Wskaźnik do bramki, na której wejście podłącza się sygnał s.
  * @param k         Numer wejścia, na które podłącza się sygnał.
  *
  * @return          Informacja zwrotna o (nie)powodzeniu funkcji.
  *
 */
 
 int nand_connect_signal(bool const *s, nand_t *g, unsigned k) {
     if (g == NULL || s == NULL || k >= g->inputs_number) {
         errno = EINVAL;
         return -1;
     }
 
     if (g->inputs[k].is_bool == false) {
         nand_disconnect_an_output_gate(g->inputs[k].nand_source,
                                        g->inputs[k].output_gates_position);
         nand_disconnect_an_input(g, k);
         g->inputs[k].is_bool = true;
     }
 
     g->inputs[k].bool_source = (bool *) s;
 
     return 0;
 }
 
 /* Funkcje potrzebne do poprawnego wykonania nand_evaluate. */
 
 /**
  * Procedura ustawia flagi calculated na false w danej bramce oraz wszystkich
  * jej wejściach. Jest to niezbędne, gdyż po zakończeniu funkcji nand_evaluate
  * nie będzie gwarancji poprawności outputów czy ścieżek krytycznych tych bramek.
  *
  * @param gate     Wskaźnik do bramki.
 
  *
 */
 
 void remove_calculated(nand_t *gate) {
     if (gate == NULL) {
         return;
     }
 
     gate->calculated = false;
 
     for (unsigned i = 0; i < gate->inputs_number; ++i) {
         if (!gate->inputs[i].is_bool)
             remove_calculated(gate->inputs[i].nand_source);
     }
 
 }
 
 /**
  * Funkcja sprawdza czy w wejściach danej bramki istnieje cykl (dla pewnej
  * bramki jej wejście zależy od wyjścia). Funkcja używa algorytmu DFS.
  *
  * @param gate      Wskaźnik do bramki.
  *
  * @return          True, jeśli cykl jest albo false, jeśli takiego nie ma.
  *
 */
 
 bool has_cycle(nand_t *gate) {
     if (gate == NULL || gate->visited) {
         return true;
     }
 
     gate->visited = true;
 
     for (unsigned i = 0; i < gate->inputs_number; ++i) {
         if (!gate->inputs[i].is_bool && has_cycle(gate->inputs[i].nand_source)) {
             gate->visited = false;
             return true;
         }
     }
 
     gate->visited = false;
 
     return false;
 }
 
 /**
  * Funkcja oblicza output bramki NAND i jej ścieżkę krytyczną
  * (chyba że ta bramka już została obliczona, czyli gate->calculated == true).
  *
  * @param gate      Wskaźnik do bramki.
  *
  * @return          Informacja zwrotna o (nie)powodzeniu funkcji.
  *
 */
 
 ssize_t calculate_output_and_critical_path(nand_t *gate) {
 
     if (gate == NULL) {
         errno = EINVAL;
         return -1;
     }
 
     if (gate->calculated)
         return gate->critical_path;
 
     ssize_t max_critical_path = 0;
     ssize_t temp_critical_path;
 
     gate->output = false;
 
     if (gate->inputs_number == 0) {
         return 0;
     }
 
     for (unsigned i = 0; i < gate->inputs_number; ++i) {
         temp_critical_path = 0;
 
         if (gate->inputs[i].is_bool){
             if (!*(gate->inputs[i].bool_source))
                 gate->output = true;
         }
         else {
             temp_critical_path =
                     calculate_output_and_critical_path(gate->inputs[i].nand_source);
 
             if (temp_critical_path == -1) {
                 errno = ECANCELED;
                 return -1;
             }
 
             if (temp_critical_path > max_critical_path)
                 max_critical_path = temp_critical_path;
 
             if (!gate->inputs[i].nand_source->output)
                 gate->output = true;
         }
 
     }
 
     gate->critical_path = 1 + max_critical_path;
     gate->calculated = true;
 
     return gate->critical_path;
 }
 
 /**
  * Funkcja wyznacza wartości sygnałów na wyjściach podanych bramek i oblicza
  * długość ścieżki krytycznej. Aby zoptymalizować ten algorytm, funkcja używa
  * flagi calculated w strukturze NAND, aby nie liczyć wartości dla tych samych
  * bramek kilka razy. Gdy w bramkach istnieje cykl, ścieżka krytyczna nie jest
  * możliwa do obliczenia, więc funkcja sprawdza istnienie cyklu zawczasu.
  *
  * @param gate      Tablica wskaźników do bramek.
  * @param s         Tablica sygnałów boolowskich,sygnał na i-tej pozycji
  *                  odpowiada sygnałowi wyjściowemu i-tej bramki.
  * @param m         Liczba bramek.
  *
  * @return          Ścieżka krytyczna układu bramek
  *                  albo -1, gdy ścieżki nie da się policzyć.
  *
 */
 
 ssize_t nand_evaluate(nand_t **g, bool *s, size_t m) {
 
     ssize_t max_critical_path = 0;
     ssize_t temp_critical_path;
 
     if (g == NULL || s == NULL || m <= 0){
         errno = EINVAL;
         return -1;
     }
 
     for (unsigned i = 0; i < m; ++i) {
         if (g[i] == NULL) {
             errno = EINVAL;
             return -1;
         }
     }
 
 
     for (unsigned i = 0; i < m; ++i) {
 
         if (has_cycle(g[i])) {
             errno = ECANCELED;
             return -1;
         }
 
         temp_critical_path = calculate_output_and_critical_path(g[i]);
         if (temp_critical_path == -1){
             errno = ECANCELED;
             return -1;
         }
         else {
             if (temp_critical_path > max_critical_path)
                 max_critical_path = temp_critical_path;
         }
         s[i] = g[i]->output;
     }
 
     for (unsigned i = 0; i < m; ++i) {
         remove_calculated(g[i]);
     }
 
     return max_critical_path;
 }
 
 /* Funkcje pozwalające na iteracje po wejściach/wyjściach bramki NAND. */
 
 /**
  * Funkcja wyznacza liczbę wejść bramek podłączonych do wyjścia danej bramki.
  *
  * @param g     Wskaźnik do bramki.
  *
  * @return      Liczba wejść bramek podłączonych do wyjścia danej bramki
  *              albo -1, gdy wskaźnik jest nullem.
  *
 */
 
 ssize_t nand_fan_out(nand_t const *g) {
 
     if (g == NULL) {
         errno = EINVAL;
         return -1;
     }
 
     return g->output_gates_number;
 }
 
 /**
  * Funkcja zwraca wskaźnik do sygnału boolowskiego lub bramki podłączonej
  * do danego wejścia danej bramki.
  *
  * @param g     Wskaźnik do bramki.
  * @param k     Numer wejścia, do którego jest podłączony zwracany sygnał.
  *
  * @return      Sygnał podłączony do wejścia k bramki g albo NULL, jeśli nic
  *              nie jest podłączone do tego wejścia lub parametry są niepoprawne.
  *
 */
 
 void* nand_input(nand_t const *g, unsigned k) {
     if (g == NULL || g->inputs_number <= k) {
         errno = EINVAL;
         return NULL;
     }
     if (g->inputs[k].is_bool)
         return g->inputs[k].bool_source;
     else if (g->inputs[k].nand_source == NULL) {
         errno = 0;
         return NULL;
     }
     return g->inputs[k].nand_source;
 }
 
 /**
  * Funkcja zwraca wskaźnik do konkretnej w kolejności bramki, do której
  * podłączone jest wyjście danej bramki. Pozwala to na iterowanie po output_gates.
  *
  * @param g     Wskaźnik do bramki.
  * @param k     Numer bramki w tablicy output_gates bramki g.
  *
  * @return      Wskaźnik do k-tej w kolejności bramki, do której podłączone
  *              jest wyjście g.
  *
 */
 
 nand_t* nand_output(nand_t const *g, ssize_t k) {
     if (g == NULL || g->output_gates_number <= k) {
         errno = EINVAL;
         return NULL;
     }
     return g->output_gates[k].output_gate;
 }