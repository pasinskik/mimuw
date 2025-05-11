/**
 * Klasa reprezentująca pojazd, posiadający numer boczny oraz będący przypisany do jakiejś linii.
 */

public class Pojazd {
    protected final int numerBoczny;
    protected final Linia linia;

    public Pojazd(int numerBoczny, Linia linia) {
        this.numerBoczny = numerBoczny;
        this.linia = linia;
    }

    public Linia getLinia() {
        return linia;
    }

    @Override
    public String toString() {
        return "linii " + linia.getNrLinii() + " (nr bocz. " + numerBoczny + ")";
    }

}
