import java.util.Objects;

/**
 * Klasa reprezentująca przyjazd tramwaju. Kiedy tramwaj przyjeżdża, pociąga to za sobą różne inne zdarzenia,
 * takie jak wysadzenie pasażera, ale uznałem za zbędne implementowanie ich w oddzielnych klasach, ponieważ
 * nie mogą one istnieć "samodzielnie", są nieodłącznie związane z przyjazdem tramwaju na konkretny przystanek.
 */

public class PrzyjechanieTramwaju extends Zdarzenie {
    private final Tramwaj pojazd;
    private final Przystanek przystanek;
    //Ostatni przystanek (lewy lub prawy koniec trasy), do którego tramwaj zmierza, aby tam odbyć postój na pętli.
    private final Przystanek ostatniPrzystanek;

    public PrzyjechanieTramwaju(int dzień, int godzina, Tramwaj pojazd, Przystanek przystanek,
                                Przystanek ostatniPrzystanek) {
        super(dzień, godzina);
        this.pojazd = pojazd;
        this.przystanek = przystanek;
        this.ostatniPrzystanek = ostatniPrzystanek;
    }

    @Override
    public String toString() {
        return dzień + ", " + Symulacja.zformatujGodzinę(godzina) + ": Tramwaj "
                +  pojazd + " przyjechał na " + przystanek;
    }

    /**
     * Metoda wypisuje informację o zdarzeniu, a potem po prostu je "wykonuje". Zmienia numer przystanku pojazdu, który
     * podjechał na kolejny przystanek (dekrementując lub inkrementując, w zależności, w którą stronę jedzie tramwaj).
     * Następnie pasażerowie są wysadzani oraz umożliwia się im wejście, ale tylko wtedy, gdy tramwaj nie udaje się
     * na pobyt na pętli przed ponownym przyjazdem na przystanek.
     */

    @Override
    public void wykonajZdarzenie(){
        System.out.println(this);

        if (Objects.equals(ostatniPrzystanek, pojazd.getLinia().getPrzystanki()[0]))
            pojazd.decrementNrPrzystanku();
        else
            pojazd.incrementNrPrzystanku();

        wysadźPasażerów();

        if (Objects.equals(ostatniPrzystanek, przystanek)) {
            if (Objects.equals(ostatniPrzystanek, pojazd.getLinia().getPrzystanki()[0]))
                pojazd.decrementNrPrzystanku();
            else
                pojazd.incrementNrPrzystanku();
            return;
        }

        wsiadająPasażerowie();
    }

    /**
     * Metoda wysadza pasażerów, jeśli jacyś w tramwaju są. Aby lepiej zasymulować losowość, na początku losowany
     * jest indeks, od którego zaczyna się przeglądanie tablicy pasażerów tramwaju, żeby ich wysadzić. W innym
     * przypadku mogłoby się zdarzyć, że wysiadałyby zawsze tylko osoby będące na początku tablicy, a pozostałe nigdy
     * by takiej szansy nie dostały.
     */

    private void wysadźPasażerów() {
        if (pojazd.getLiczbaOsóbWTramwaju() == 0)
            return;

        int i = 0;
        int temp = Losowanie.losuj(0, pojazd.getPasażerowie().length);

        while (pojazd.getLiczbaOsóbWTramwaju() > 0 && i < pojazd.getPasażerowie().length
                && przystanek.getLiczbaOsóbNaPrzystanku() < przystanek.getPasażerowie().length) {
            Pasażer tempPasażer = pojazd.getPasażerowie()[(i + temp) % pojazd.getPasażerowie().length];
            if (tempPasażer != null && Objects.equals(przystanek, tempPasażer.getDocelowyPrzystanek()))
                pojazd.wysadźPasażera((i + temp) % pojazd.getPasażerowie().length, dzień, godzina);
            ++i;
        }
    }

    /**
     * Metoda pozwala wsiąść pasażerom, jeśli jacyś na przystanku są. Aby lepiej zasymulować losowość, na początku
     * losowany jest indeks, od którego zaczyna się przeglądanie tablicy pasażerów na przystanku. W innym przypadku
     * mogłoby się zdarzyć, że wsiadałyby zawsze tylko osoby będące na początku tablicy, a pozostałe nigdy by takiej
     * szansy nie dostały.
     */

    private void wsiadająPasażerowie() {
        if (przystanek.getLiczbaOsóbNaPrzystanku() == 0)
            return;

        int i = 0;
        int temp = Losowanie.losuj(0, przystanek.getPasażerowie().length);

        while (przystanek.getLiczbaOsóbNaPrzystanku() > 0 && i < przystanek.getPasażerowie().length
                && pojazd.getLiczbaOsóbWTramwaju() < pojazd.getPasażerowie().length) {
            Pasażer tempPasażer = przystanek.getPasażerowie()[(i + temp) % przystanek.getPasażerowie().length];
            if (tempPasażer != null) {
                pojazd.wsiadaPasażer(tempPasażer, dzień, godzina, ostatniPrzystanek);
                przystanek.opuszczaPasażer((i + temp) % przystanek.getPasażerowie().length);
            }
            ++i;
        }
    }
}
