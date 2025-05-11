/**
 * Klasa reprezentująca pasażera. Ma on swój numer oraz przystanek, w którego pobliżu mieszka. Jego pozostałe
 * atrybuty są potrzebne do symulacji oraz często modyfikowane.
 */

public class Pasażer {
    private final int numerPasażera;
    private final Przystanek najbliższyPrzystanek;
    private int godzinaPójściaNaPrzystanek;
    private boolean czyPodróżujeTegoDnia;
    private boolean czyWTramwaju;
    //Przystanek, na którym pasażer chce wysiąść, jeśli jest w tej chwili w tramwaju (czyWTramwaju).
    private Przystanek docelowyPrzystanek;
    //Łączny czas czekania na przystankach jednego dnia.
    private int czasCzekania;
    //Łączna liczba przejazdów jednego dnia.
    private int liczbaPrzejazdów;

    public Pasażer(int numerPasażera, Przystanek najbliższyPrzystanek) {
        this.numerPasażera = numerPasażera;
        this.najbliższyPrzystanek = najbliższyPrzystanek;
        przygotujSięNaNowyDzień();
    }

    public Przystanek getNajbliższyPrzystanek() {
        return najbliższyPrzystanek;
    }

    public int getGodzinaPójściaNaPrzystanek() {
        return godzinaPójściaNaPrzystanek;
    }

    public void setGodzinaPójściaNaPrzystanek(int godzinaPójściaNaPrzystanek) {
        this.godzinaPójściaNaPrzystanek = godzinaPójściaNaPrzystanek;
    }

    public boolean isCzyPodróżujeTegoDnia() {
        return czyPodróżujeTegoDnia;
    }

    public void setCzyPodróżujeTegoDnia(boolean czyPodróżujeTegoDnia) {
        this.czyPodróżujeTegoDnia = czyPodróżujeTegoDnia;
    }

    public boolean isCzyWTramwaju() {
        return czyWTramwaju;
    }

    public void setCzyWTramwaju(boolean czyWTramwaju) {
        this.czyWTramwaju = czyWTramwaju;
    }

    public Przystanek getDocelowyPrzystanek() {
        return docelowyPrzystanek;
    }

    public void setDocelowyPrzystanek(Przystanek docelowyPrzystanek) {
        this.docelowyPrzystanek = docelowyPrzystanek;
    }

    public int getCzasCzekania() {
        return czasCzekania;
    }

    public int getLiczbaPrzejazdów() {
        return liczbaPrzejazdów;
    }

    @Override
    public String toString() {
        return "Pasażer " + numerPasażera;
    }

    public void incrementLiczbaPrzejazdów() {
        ++liczbaPrzejazdów;
    }

    public void addToCzasCzekania(int value) {
        czasCzekania += value;
    }

    /**
     * Metoda ustawia wartości atrybutów pasażera tak, aby można było rozpocząć dzień symulacji. W szczególności
     * losowana jest mu godzina pójścia na przystanek (między 6:00 a 12:00).
     */

    public void przygotujSięNaNowyDzień() {
        godzinaPójściaNaPrzystanek = Losowanie.losuj(1, 60 * 6);
        liczbaPrzejazdów = 0;
        czasCzekania = 0;
        czyWTramwaju = false;
    }

}
