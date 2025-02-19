package Zlecenia;

import Inwestorzy.*;
import Symulacja.*;

/**
 * Klasa reprezentujaca zlecenie sprzedazy.
 */

public class ZlecenieSprzedazy extends Zlecenie {
    public ZlecenieSprzedazy(Inwestor autorZlecenia, int i, int turaZlozenia, Spolka akcja, int liczbaSztuk, int limitCeny, int termin) {
        super(autorZlecenia, i, turaZlozenia, akcja, liczbaSztuk, limitCeny, termin);
        this.typZlecenia = TypZlecenia.SPRZEDAZ;
    }

    @Override
    public String toString() {
        return "SPRZEDAZ: " + super.toString();
    }

}
