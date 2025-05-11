package Zlecenia;

import Inwestorzy.*;
import Symulacja.*;

/**
 * Klasa reprezentujaca zlecenie kupna.
 */

public class ZlecenieKupna extends Zlecenie {
    public ZlecenieKupna(Inwestor autorZlecenia, int i, int turaZlozenia,
                         Spolka akcja, int liczbaSztuk, int limitCeny, int termin) {
        super(autorZlecenia, i, turaZlozenia, akcja, liczbaSztuk, limitCeny, termin);
        this.typZlecenia = TypZlecenia.KUPNO;
    }

    @Override
    public String toString() {
        return "KUPNO: " + super.toString();
    }
}
