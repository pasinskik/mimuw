/**
 * Klasa reprezentująca zdarzenie, które będzie częścią kolejki zdarzeń, pozwalającej na stworzenie symulacji
 * jazdy tramwajów. Zdarzenie składa się z dnia i godziny. Konkretne typy zdarzeń są opisane w podklasach.
 * Dnie numerowane są od 1, a godzina to liczba minut, które minęły od 6:00 danego dnia. Na przykład 127 to 8:07.
 */

abstract public class Zdarzenie {
    protected final int dzień;
    protected final int godzina;

    public Zdarzenie (int dzień, int godzina) {
        this.dzień = dzień;
        this.godzina = godzina;
    }

    public int getGodzina() {
        return godzina;
    }

    abstract public void wykonajZdarzenie();
}
