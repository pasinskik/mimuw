package Symulacja;

import java.util.*;

/**
 * Klasa reprezentujaca spolke.
 */

public class Spolka {
    private String identyfikator;
    private int cenaOstatniejTransakcji; // cena akcji w ostatniej turze
    private Deque<Integer> dziesiecOstatnichCen; // ceny akcji z ostatnich 10 tur
    private double SMA5; // simple moving average z ostatnich 5 dni
    private double SMA10; // simple moving average z ostatnich 10 dni
    private double poprzednieSMA5; // SMA5 z dnia poprzedniego
    private double poprzednieSMA10; // SMA5 z dnia poprzedniego

    public Spolka(String identyfikator, int cenaOstatniejTransakcji) {
        this.identyfikator = identyfikator;
        this.cenaOstatniejTransakcji = cenaOstatniejTransakcji;
        this.dziesiecOstatnichCen = new LinkedList<>();
        this.SMA5 = 0;
        this.SMA10 = 0;
        this.poprzednieSMA5 = 0;
        this.poprzednieSMA10 = 0;
    }

    public String getIdentyfikator() {
        return identyfikator;
    }

    public int getCenaOstatniejTransakcji() {
        return cenaOstatniejTransakcji;
    }

    public void setCenaOstatniejTransakcji(int cenaOstatniejTransakcji) {
        this.cenaOstatniejTransakcji = cenaOstatniejTransakcji;
    }

    /**
     * Metoda (pod koniec tury) aktualizuje kolejke zawierajaca dziesiec ostatnich cen, poniewaz
     * cena ostatniej transakcji w turze to cena akcji w tej turze.
     */

    public void ustalCeneWTurze() {
        if (dziesiecOstatnichCen.size() >= 10)
            dziesiecOstatnichCen.removeFirst();

        dziesiecOstatnichCen.addLast(cenaOstatniejTransakcji);

        obliczSMA5();
        obliczSMA10();
    }

    /**
     * Metoda oblicza SMA5 na podstawie cen akcji z ostatnich 5 tur.
     */

    private void obliczSMA5() {
        poprzednieSMA5 = SMA5;

        if (dziesiecOstatnichCen.size() >= 5) {
            int suma = 0;

            Iterator<Integer> iterator = dziesiecOstatnichCen.descendingIterator();
            for (int i = 0; i < 5; i++) {
                suma += iterator.next();
            }

            SMA5 = suma / 5.0;
        }

        else
            SMA5 = 0;
    }

    /**
     * Metoda oblicza SMA10 na podstawie cen akcji z ostatnich 10 tur.
     */

    private void obliczSMA10() {
        poprzednieSMA10 = SMA10;

        if (dziesiecOstatnichCen.size() == 10) {
            int suma = 0;

            for (int cena : dziesiecOstatnichCen) {
                suma += cena;
            }

            SMA10 = suma / 10.0;
        }
        else
            SMA10 = 0;
    }

    /**
     * Metoda sprawdza czy SMA5 przecina od dolu SMA10 (co jest warunkiem kupna dla inwestora SMA).
     */

    public boolean czySMA5PrzecinaOdDoluSMA10() {
        return SMA5 > SMA10 && poprzednieSMA5 <= poprzednieSMA10;
    }

    /**
     * Metoda sprawdza czy SMA5 przebija od gory SMA10 (co jest warunkiem sprzedazy dla inwestora SMA).
     */

    public boolean czySMA5PrzebijaOdGorySMA10() {
        return SMA5 < SMA10 && poprzednieSMA5 >= poprzednieSMA10;
    }

    @Override
    public String toString() {
        return identyfikator;
    }
}
