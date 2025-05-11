package Inwestorzy;

import Zlecenia.*;
import Symulacja.*;

import java.util.*;

/**
 * Klasa reprezentujaca inwestora dzialajacego losowo.
 */

public class InwestorRandom extends Inwestor {
    public InwestorRandom(int gotowka, Map<Spolka, Integer> portfel) {
        super(gotowka, portfel);
    }

    /**
     * Metoda sklada zlecenie - losowo, albo kupno, albo sprzedaz.
     */

    @Override
    public Zlecenie zlozZlecenie(SystemTransakcyjny s, int i) {
        if (Losowanie.losuj(0, 2) == 0)
            return zlozZlecenieKupna(s, i);
        return zlozZlecenieSprzedazy(s, i);
    }

    /**
     * Metoda sklada losowe zlecenie kupna. Limit ceny jest rozny o max 10 od ostatniej ceny transakcji, ale
     * zawsze jest dodatni. Liczba akcji to od 1 do poczatkowej liczby w portfelu inwestora, zeby zapobiec
     * wykupowaniu prawie wszystkich akcji przez jednego inwestora.
     */

    public ZlecenieKupna zlozZlecenieKupna(SystemTransakcyjny s, int i) {
        Spolka spolka = s.getSpolki().get(Losowanie.losuj(0, s.getSpolki().size()));

        int limitCeny = Losowanie.losuj(gwarancjaDodatniejCeny(spolka), spolka.getCenaOstatniejTransakcji() + 11);
        int liczbaAkcji = Losowanie.losuj(1,
                s.getNiezmiennikAkcji().get(spolka)/s.getInwestorzy().size() + 1);
        if (gotowka < liczbaAkcji*limitCeny)
            return null;    // jesli inwestor nie ma pieniedzy na zrealizowanie swojego zlecenia,
                            // jest ono uniewazniane

        int termin = Losowanie.losuj(0, 4) - 2;
        if (termin == 1)
            termin = Losowanie.losuj(s.getObecnaTura(), s.getObecnaTura() + 100);

        return new ZlecenieKupna(this, i, s.getObecnaTura(), spolka, liczbaAkcji, limitCeny, termin);
    }

    /**
     * Metoda sklada losowe zlecenie sprzedazy. Limit ceny jest rozny o max 10 od ostatniej ceny transakcji, ale
     * zawsze jest dodatni. Liczba akcji to losowa liczba, ale taka, ktora pozwala w danej chwili na realizacje
     * zlecenia (nie wiecej niz ktos ma w portfelu).
     */

    public ZlecenieSprzedazy zlozZlecenieSprzedazy(SystemTransakcyjny s, int i) {
        Spolka spolka = s.getSpolki().get(Losowanie.losuj(0, s.getSpolki().size()));
        if (portfel.get(spolka) == 0)
            return null;

        int liczbaAkcji = Losowanie.losuj(1, portfel.get(spolka) + 1);
        int limitCeny = Losowanie.losuj(gwarancjaDodatniejCeny(spolka), spolka.getCenaOstatniejTransakcji() + 11);

        int termin = Losowanie.losuj(0, 4) - 2;
        if (termin == 1)
            termin = Losowanie.losuj(s.getObecnaTura(), s.getObecnaTura() + 100);

        return new ZlecenieSprzedazy(this, i, s.getObecnaTura(), spolka, liczbaAkcji, limitCeny, termin);
    }

    /**
     * Metoda dba, zeby limit ceny w zleceniach byl dodatni, nawet jesli
     * ostatnia cena transakcji - 10 to liczba ujemna.
     */

    private int gwarancjaDodatniejCeny(Spolka spolka) {
        if (spolka.getCenaOstatniejTransakcji() <= 10)
            return 1;

        return spolka.getCenaOstatniejTransakcji() - 10;
    }

    @Override
    public String toString() {
        return "Random " + super.toString();
    }
}
