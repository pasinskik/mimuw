import java.util.Objects;

/**
 * Klasa reprezentująca tramwaj - pojazd, który zgodnie z treścią zadania ma swoją maksymalną pojemność
 * oraz jeździ od przystanku do przystanku.
 */

public class Tramwaj extends Pojazd {
    private int liczbaOsóbWTramwaju;
    private Pasażer[] pasażerowie;
    //Numer przystanku, na którym tramwaj się teraz znajduje (od 0 do n-1, a -1 oraz n oznaczają pobyt na pętli).
    private int nrPrzystanku;

    public Tramwaj(int numerBoczny, Linia linia, int limitOsóbWTramwaju, int nrPrzystanku) {
        super(numerBoczny, linia);
        liczbaOsóbWTramwaju = 0;
        pasażerowie = new Pasażer[limitOsóbWTramwaju];
        this.nrPrzystanku = nrPrzystanku;
    }

    public int getLiczbaOsóbWTramwaju() {
        return liczbaOsóbWTramwaju;
    }

    public Pasażer[] getPasażerowie() {
        return pasażerowie;
    }

    @Override
    public String toString() {
        return "linii " + linia.getNrLinii() + " (nr bocz. " + numerBoczny + ")";
    }

    public void incrementNrPrzystanku() {
        ++nrPrzystanku;
    }
    public void decrementNrPrzystanku() {
        --nrPrzystanku;
    }

    /**
     * Metoda służy do wysadzenia konkretnego pasażera (identyfikowanego numerem w tablicy pasażerów)
     * z tego tramwaju. Dzień oraz godzina potrzebne są, aby wypisać informację o zdarzeniu, co metoda robi na końcu.
     */

    public void wysadźPasażera(int nrPasażera, int dzień, int godzina) {
        System.out.println(dzień + ", " + Symulacja.zformatujGodzinę(godzina) + ": " + pasażerowie[nrPasażera]
                + " wysiadł z tramwaju " + this + " na przystanku " + linia.getPrzystanki()[nrPrzystanku]);

        pasażerowie[nrPasażera].setCzyWTramwaju(false);
        pasażerowie[nrPasażera].incrementLiczbaPrzejazdów();
        pasażerowie[nrPasażera].setGodzinaPójściaNaPrzystanek(godzina);
        linia.getPrzystanki()[nrPrzystanku].dodajPasażera(pasażerowie[nrPasażera]);
        pasażerowie[nrPasażera] = null;
        --liczbaOsóbWTramwaju;
    }

    /**
     * Metoda służy do wpuszczenia konkretnego pasażera z tego tramwaju. Ostatni przystanek w danym kursie jest
     * potrzebny, aby można było określić, w którą stronę trasy w tej chwili jedzie tramwaj i poprawnie wylosować
     * docelowy przystanek dla pasażera. Dzień i godzina potrzebne są, aby prawidłowo wypisać informację o zdarzeniu,
     * co metoda robi, gdy pasażer zostanie już wpuszczony.
     */

    public void wsiadaPasażer(Pasażer pasażer, int dzień, int godzina, Przystanek ostatniPrzystanek) {
        ++liczbaOsóbWTramwaju;
        for (int i = 0; i < pasażerowie.length; ++i) {
            if (pasażerowie[i] == null) {
                pasażerowie[i] = pasażer;
                pasażer.setDocelowyPrzystanek(wylosujPrzystanek(ostatniPrzystanek));
                pasażer.addToCzasCzekania(godzina - pasażer.getGodzinaPójściaNaPrzystanek());
                pasażer.setCzyWTramwaju(true);

                System.out.println(dzień + ", " + Symulacja.zformatujGodzinę(godzina) + ": "
                        + pasażer + " wsiadł do tramwaju " + this + " z zamiarem dojechania do przystanku "
                        + pasażer.getDocelowyPrzystanek());

                return;
            }
        }
    }

    /**
     * Metoda ustawia liczbę osób w tramwaju na 0 oraz resetuje tablicę pasażerów. Jest to potrzebne, aby na koniec
     * dnia "opróżnić tramwaj" (wszyscy idą do domu) i sprawić, że będzie gotowy na kolejny dzień symulacji.
     */

    public void opróżnijPojazd() {
        liczbaOsóbWTramwaju = 0;
        pasażerowie = new Pasażer[pasażerowie.length];
    }

    /**
     * Metoda losuje przystanek wsiadającemu pasażerowi. Potrzebuje wiedzieć o ostatnim przystanku w tym kursie, żeby
     * określić, w którą stronę jedzie tramwaj i jakie przystanki mogą zostać wylosowane.
     */

    public Przystanek wylosujPrzystanek(Przystanek ostatniPrzystanek) {
        if (Objects.equals(ostatniPrzystanek, linia.getPrzystanki()[0]))
            return linia.getPrzystanki()[Losowanie.losuj(0, nrPrzystanku)];
        else
            return linia.getPrzystanki()[Losowanie.losuj(nrPrzystanku + 1, linia.getPrzystanki().length)];
    }

}
