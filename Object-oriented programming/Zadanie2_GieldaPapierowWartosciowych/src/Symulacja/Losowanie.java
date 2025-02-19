package Symulacja;

import java.util.Random;

/**
 * Klasa uzywana do losowania liczb z podanego zakresu tj. [dolna, gorna).
 */

public class Losowanie {
    public static int losuj(int dolna, int gorna) {
        Random random = new Random();
        return random.nextInt(gorna - dolna) + dolna;
    }
}