import java.util.Random;

/**
 * Klasa używana do losowania liczb z podanego zakresu.
 */

public class Losowanie {
    public static int losuj(int dolna, int górna) {
        Random random = new Random();
        return random.nextInt(górna - dolna) + dolna;
    }
}