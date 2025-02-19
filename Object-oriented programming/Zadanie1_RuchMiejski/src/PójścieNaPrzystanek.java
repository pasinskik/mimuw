/**
 * Klasa reprezentująca przyjście pasażera na przystanek, w którego pobliżu mieszka. To zdarzenie dla każdego pasażera
 * jest tylko jedno na dzień - chodzi o pójście na przystanek po raz pierwszy jakiegoś dnia, a nie o po prostu
 * znalezienie się na przystanku, np. po wyjściu z tramwaju.
 */

public class PójścieNaPrzystanek extends Zdarzenie {
    private final Pasażer pasażer;
    private final Przystanek przystanek;

    public PójścieNaPrzystanek(int dzień, int godzina, Pasażer pasażer, Przystanek przystanek) {
        super(dzień, godzina);
        this.pasażer = pasażer;
        this.przystanek = przystanek;
    }

    @Override
    public String toString() {
        return dzień + ", " + Symulacja.zformatujGodzinę(godzina) + ": "
                + pasażer + " przyszedł na przystanek " + przystanek;
    }

    /**
     * Metoda znajduje dla pasażera miejsce na przystanku i dodaje go, do tablicy pasażerów, albo, gdy miejsc nie ma,
     * odsyła go do domu.
     */

    @Override
    public void wykonajZdarzenie() {
        if (przystanek.getLiczbaOsóbNaPrzystanku() == przystanek.getPasażerowie().length) {
            pasażer.setCzyPodróżujeTegoDnia(false);
            return;
        }

        System.out.println(this);
        przystanek.dodajPasażera(pasażer);
    }
}
