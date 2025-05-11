package Zlecenia;

import Inwestorzy.*;
import Symulacja.*;
import Terminy.*;

/**
 * Klasa reprezentujaca zlecenie kupna/sprzedazy. Podzial na zlecenie kupna i sprzedazy zrealizowany
 * zostal za pomoca zarowno podklas, jak i enum (zeby ulatwic metody realizowania zlecen).
 */

abstract public class Zlecenie implements Comparable<Zlecenie> {
    public enum TypZlecenia {KUPNO, SPRZEDAZ};
    protected TypZlecenia typZlecenia;
    protected final Inwestor autorZlecenia;
    protected final Spolka akcja;
    protected int liczbaSztuk; // liczba akcji, ktore zlecenie autor chce kupic/sprzedac
    protected final int limitCeny;
    protected final TerminWaznosci terminWaznosci;
    protected final int turaZlozenia; // numer tury, w ktorej zostalo zlozone zlecenie
    protected final int numerWTurze;    // numer oznaczajacy, ktore dane zlecenie bylo
                                        // w kolejnosci skladania zlecen w danej turze

    public Zlecenie(Inwestor autorZlecenia, int numerWTurze, int turaZlozenia, Spolka akcja,
                    int liczbaSztuk, int limitCeny, int termin) {
        this.autorZlecenia = autorZlecenia;
        this.turaZlozenia = turaZlozenia;
        this.numerWTurze = numerWTurze;
        this.akcja = akcja;
        this.liczbaSztuk = liczbaSztuk;
        this.limitCeny = limitCeny;
        this.terminWaznosci = dopasujTerminWaznosci(termin);
    }

    public Spolka getAkcja() {
        return akcja;
    }

    public int getLiczbaSztuk() {
        return liczbaSztuk;
    }

    public void setLiczbaSztuk(int liczbaSztuk) {
        this.liczbaSztuk = liczbaSztuk;
    }

    public int getLimitCeny() {
        return limitCeny;
    }

    public int getTuraZlozenia() {
        return turaZlozenia;
    }

    public TypZlecenia getTypZlecenia() {
        return typZlecenia;
    }

    public Inwestor getAutorZlecenia() {
        return autorZlecenia;
    }

    public int getNumerWTurze() {
        return numerWTurze;
    }

    public void zmniejszLiczbeSztuk(int liczba) {
        this.liczbaSztuk -= liczba;
    }

    /**
     * Metoda sprawdza, czy w danej turze to zlecenie jest juz przeterminowane.
     */

    public boolean czyZleceniePrzeterminowane(int obecnaTura) {
        return terminWaznosci.czyZleceniePrzeterminowane(this, obecnaTura);
    }

    /**
     * Metoda sprawdza, czy to zlecenie jest typu wykonaj albo anuluj.
     */

    public boolean czyZlecenieWykonajAnuluj() {
        return terminWaznosci.czyZlecenieWykonajAnuluj();
    }

    /**
     * Metoda zamienia liczbe na dany termin waznosci; w mojej implementacji sa 4, wiec -2 oznacza termin
     * bez waznosci, -1 wykonaj albo anuluj, 0 termin natychmiastowy, a pozostale liczby
     * to termin do konca tury (dana liczba).
     */

    public TerminWaznosci dopasujTerminWaznosci(int termin) {
        if (termin == -2)
            return new TerminBezWaznosci();

        if (termin == -1)
            return new TerminWykonajAnuluj();

        if (termin == 0)
            return new TerminNatychmiastowe();

        return new TerminDoKoncaTury(termin);
    }

    @Override
    public String toString() {
        return "Zlozone jako " + numerWTurze + " w turze " + turaZlozenia + " z terminem " + terminWaznosci + " " + akcja + " Sztuk: " + liczbaSztuk + " Cena: " + limitCeny;
    }

    /**
     * Metoda porownywania priorytetu zlecenia zgodnie z poleceniem.
     */

    @Override
    public int compareTo(Zlecenie other) {
        if (this.typZlecenia == TypZlecenia.KUPNO) {
            // Kupno: Najwyzsza cena ma priorytet, potem wczesniejsza tura, potem numer w turze
            int porownanie = Integer.compare(other.limitCeny, this.limitCeny);
            
            if (porownanie != 0)
                return porownanie;
            
            porownanie = Integer.compare(this.turaZlozenia, other.turaZlozenia);
            
            if (porownanie != 0)
                return porownanie;
            
            return Integer.compare(this.numerWTurze, other.numerWTurze);
        } else {
            // Sprzedaz: Najnizsza cena ma priorytet, potem wczesniejsza tura, potem numer w turze
            int porownanie = Integer.compare(this.limitCeny, other.limitCeny);
            
            if (porownanie != 0)
                return porownanie;
            
            porownanie = Integer.compare(this.turaZlozenia, other.turaZlozenia);
            
            if (porownanie != 0)
                return porownanie;
            
            return Integer.compare(this.numerWTurze, other.numerWTurze);
        }
    }
}