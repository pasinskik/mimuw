/**
 * Klasa reprezentująca przystanek, na który przyjeżdżają pojazdy. Przystanek ma nazwę oraz maksymalna liczbę
 * osób, które mogą się na nim znaleźć w jednej chwili.
 */

public class Przystanek {
    private final String nazwaPrzystanku;
    private int liczbaOsóbNaPrzystanku;
    private Pasażer[] pasażerowie;

    public Przystanek(String nazwaPrzystanku, int limitOsóbNaPrzystanku) {
        this.nazwaPrzystanku = nazwaPrzystanku;
        liczbaOsóbNaPrzystanku = 0;
        pasażerowie = new Pasażer[limitOsóbNaPrzystanku];
    }

    public String getNazwaPrzystanku() {
        return nazwaPrzystanku;
    }

    public int getLiczbaOsóbNaPrzystanku() {
        return liczbaOsóbNaPrzystanku;
    }


    public Pasażer[] getPasażerowie() {
        return pasażerowie;
    }

    @Override
    public String toString() {
        return nazwaPrzystanku;
    }

    /**
     * Metoda dodaje pasażera do tablicy pasażerów (jeśli jest jakieś wolne miejsce).
     */

    public void dodajPasażera(Pasażer pasażer) {
        for (int i = 0; i < pasażerowie.length; ++i) {
            if (pasażerowie[i] == null) {
                pasażerowie[i] = pasażer;
                pasażer.setCzyPodróżujeTegoDnia(true);
                ++liczbaOsóbNaPrzystanku;
                return;
            }
        }
    }

    /**
     * Metoda usuwa pasażera z tablicy pasażerów.
     */

    public void opuszczaPasażer(int nrPasażera) {
        --liczbaOsóbNaPrzystanku;
        pasażerowie[nrPasażera] = null;
    }

    /**
     * Metoda ustawia liczbę osób na przystanku na 0 oraz resetuje tablicę pasażerów. Jest to potrzebne, aby na koniec
     * dnia "opróżnić przystanek" (wszyscy idą do domu) i sprawić, że będzie gotowy na kolejny dzień symulacji.
     */

    public void opróżnijPrzystanek() {
        liczbaOsóbNaPrzystanku = 0;
        pasażerowie = new Pasażer[pasażerowie.length];
    }
}
