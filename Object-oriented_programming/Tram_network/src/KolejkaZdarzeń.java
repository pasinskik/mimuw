/**
 * Klasa reprezentuje kolejkę zdarzeń za pomocą linked list. Jest to zwykła kolejka, po prostu przechowująca obiekty
 * klasy Zdarzenie. Wszystkie zaimplementowane metody są standardowe dla każdej kolejki (push, pop, peek), z tą
 * tylko różnicą, że zdarzenia w kolejce są posortowane godzinowo, a więc dodanie elementu do kolejki nie dodaje
 * go na koniec linked listy, a po prostu w odpowiednim miejscu, tak, żeby zawsze elementy były posortowane.
 */

public class KolejkaZdarzeń {
    private KolejkaZdarzeńNode pierwszeZdarzenie;
    private int rozmiar;

    private static class KolejkaZdarzeńNode {
        Zdarzenie zdarzenie;
        KolejkaZdarzeńNode następne;

        KolejkaZdarzeńNode(Zdarzenie zdarzenie) {
            this.zdarzenie = zdarzenie;
            this.następne = null;
        }
    }

    public KolejkaZdarzeń() {
        pierwszeZdarzenie = null;
        rozmiar = 0;
    }

    public int getRozmiar() {
        return rozmiar;
    }

    public void dodajZdarzenie(Zdarzenie zdarzenie) {
        ++rozmiar;
        KolejkaZdarzeńNode nowy = new KolejkaZdarzeńNode(zdarzenie);

        if (pierwszeZdarzenie == null) {
            pierwszeZdarzenie = nowy;
            return;
        }

        KolejkaZdarzeńNode temp = pierwszeZdarzenie;
        if (pierwszeZdarzenie.zdarzenie.getGodzina() > zdarzenie.getGodzina()) {
            nowy.następne = pierwszeZdarzenie;
            pierwszeZdarzenie = nowy;
            return;
        }

        while (temp.następne != null && temp.następne.zdarzenie.getGodzina() <= zdarzenie.getGodzina()) {
            temp = temp.następne;
        }

        nowy.następne = temp.następne;
        temp.następne = nowy;
    }

    public Zdarzenie wyjmijZdarzenie() {
        assert pierwszeZdarzenie != null : "Próba pobrania zdarzenia z pustej kolejki";

        Zdarzenie temp = pierwszeZdarzenie.zdarzenie;
        pierwszeZdarzenie = pierwszeZdarzenie.następne;
        --rozmiar;
        return temp;
    }

    public Zdarzenie weźZdarzenie() {
        if (pierwszeZdarzenie == null)
            return null;
        else
            return pierwszeZdarzenie.zdarzenie;
    }
}
