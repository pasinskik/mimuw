package Inwestorzy;

import Zlecenia.*;
import Symulacja.*;

import java.util.Map;

/**
 * Klasa reprezentujaca inwestora dzialajacego analizujac SMA.
 */

public class InwestorSMA extends Inwestor {
    public InwestorSMA(int gotowka, Map<Spolka, Integer> portfel) {
        super(gotowka, portfel);
    }

    /**
     * Metoda sklada zlecenie. Przeglada spolki i sklada zlecenie dla pierwszej z nich, dla ktorej
     * zaleznosci miedzy SMA5 i SMA10 wskazuja na sygnal kupna lub sprzedazy.
     */

    @Override
    public Zlecenie zlozZlecenie(SystemTransakcyjny s, int i) {
        for (Spolka spolka : s.getSpolki()) {
            if (spolka.czySMA5PrzecinaOdDoluSMA10())
                return zlozZlecenieKupna(spolka, s, i);

            else if (spolka.czySMA5PrzebijaOdGorySMA10())
                return zlozZlecenieSprzedazy(spolka, s, i);
        }

        return null;
    }

    /**
     * Metoda sklada zlecenie kupna. Dzieje sie tak, gdy dla danej spolki SMA5 przecina od dolu SMA10.
     * Wtedy inwestor chce po cenie ostatniej transakcji kupic jakas liczbe akcji (losowa, bo
     * nie zostalo to sprecyzowane w poleceniu).
     */

    public ZlecenieKupna zlozZlecenieKupna(Spolka spolka, SystemTransakcyjny s, int i) {
        int limitCeny = spolka.getCenaOstatniejTransakcji();
        int liczbaAkcji = Losowanie.losuj(1,
                s.getNiezmiennikAkcji().get(spolka)/s.getInwestorzy().size() + 1);
        if (gotowka < liczbaAkcji*limitCeny)
            return null;    // jesli inwestor nie ma pieniedzy na zrealizowanie swojego zlecenia,
                            // jest ono uniewazniane

        int termin = 0; // zlecenie takie jest zawsze z terminem natychmiastowym

        return new ZlecenieKupna(this, i, s.getObecnaTura(), spolka, liczbaAkcji, limitCeny, termin);
    }

    /**
     * Metoda sklada zlecenie kupna. Dzieje sie tak, gdy dla danej spolki SMA5 przebija od gory SMA10.
     * Wtedy inwestor chce po cenie ostatniej transakcji sprzedaz jakas liczbe akcji (losowa, bo
     * nie zostalo to sprecyzowane w poleceniu).
     */

    public ZlecenieSprzedazy zlozZlecenieSprzedazy(Spolka spolka, SystemTransakcyjny s, int i) {
        if (portfel.get(spolka) == 0)
            return null;

        int liczbaAkcji = Losowanie.losuj(0, portfel.get(spolka) + 1);
        int limitCeny = spolka.getCenaOstatniejTransakcji();

        int termin = 0; // zlecenie takie jest zawsze z terminem natychmiastowym

        return new ZlecenieSprzedazy(this, i, s.getObecnaTura(), spolka, liczbaAkcji, limitCeny, termin);
    }

    @Override
    public String toString() {
        return "SMA " + super.toString();
    }
}
