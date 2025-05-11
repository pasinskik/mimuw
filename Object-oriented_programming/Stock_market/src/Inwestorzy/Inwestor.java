package Inwestorzy;

import Zlecenia.*;
import Symulacja.*;
import java.util.*;

/**
 * Klasa reprezentujaca inwestora z jego gotowka i zasobami. Poszczegolne typy inwestorow to podklasy.
 */


abstract public class Inwestor {
    protected int gotowka;
    protected final Map<Spolka, Integer> portfel;

    public Inwestor(int gotowka, Map<Spolka, Integer> portfel) {
        this.gotowka = gotowka;
        this.portfel = new HashMap<>(portfel);
    }

    public int getGotowka() {
        return gotowka;
    }

    public Map<Spolka, Integer> getPortfel() {
        return portfel;
    }

    /**
     * Metoda realizuje sprzedaz akcji danej spolki przez inwestora. Odejmuje mu je z portfela oraz
     * dodaje odpowiednia kwote do konta.
     */

    public void sprzedajAkcje(Spolka spolka, int liczbaAkcji, int cena) {
        gotowka += cena;

        Integer obecnaLiczbaAkcji = portfel.get(spolka);
        portfel.put(spolka, obecnaLiczbaAkcji - liczbaAkcji);

        spolka.setCenaOstatniejTransakcji(cena/liczbaAkcji);    // z racji sprzedazy aktualizujemy cene
                                                                // transakcji dla spolki

        // System.out.println("Inwestor sprzedal " + liczbaAkcji
        //                    + " akcji firmy " + spolka + " za " + cena + " zl");
    }

    /**
     * Metoda realizuje kupno akcji danej spolki przez inwestora. Dodaje mu je do portfela oraz
     * odejmuje odpowiednia kwote z konta.
     */

    public void kupAkcje(Spolka spolka, int liczbaAkcji, int cena) {
        gotowka -= cena;

        Integer obecnaLiczbaAkcji = portfel.get(spolka);
        portfel.put(spolka, obecnaLiczbaAkcji + liczbaAkcji);

        // System.out.println("Inwestor kupil " + liczbaAkcji + " akcji firmy "
        //                     + spolka + " za " + cena + " zl");
    }

    /**
     * Metoda sprawdza, czy inwestor ma w swoim portfelu konkretna liczbe akcji danej spolki.
     */

    public boolean czyZdolnySprzedac(Spolka spolka, int liczbaAkcji) {
        if (portfel.get(spolka) < liczbaAkcji || liczbaAkcji == 0)
            return false;
        return true;
    }

    /**
     * Metoda pozwala inwestorowi na zlozenie zlecenia kupna/sprzedazy.
     */

     abstract public Zlecenie zlozZlecenie(SystemTransakcyjny s, int i);

    /**
     * Metoda pozwala na wypisywanie koncowego wyniku programu.
     */

     public String toStringOutput() {
         String output = String.valueOf(gotowka);

         for (Spolka spolka : portfel.keySet()) {
             output += " " + spolka + ":" + portfel.get(spolka);
         }

         return output;
     }

    @Override
    public String toString() {
        return String.valueOf(gotowka);
    }
}
