package Main;
import Symulacja.*;

/**
 * "Zadanie nr 2"
 *
 * Program sluzy do symulacji gieldy papierow wartosciowych. Wystarczy uruchomic go
 * z poprawnymi danymi na wejsciu.
 *
 * autor:   Kacper Pasinski
 * data:    13/06/2024
 */

public class GPWSimulation {
    public static void main(String[] args) {
        if (args.length != 2) {
            System.out.println("Uzycie programu: java Main.GPWSimulation <plik> <liczba tur>");
            return;
        }

        String inputPlik = args[0];
        int liczbaTur = Integer.parseInt(args[1]);

        SystemTransakcyjny s = new SystemTransakcyjny();
        s.inicjalizacjaSymulacji(inputPlik, liczbaTur);
        s.wypiszWynikSymulacji();
    }
}
