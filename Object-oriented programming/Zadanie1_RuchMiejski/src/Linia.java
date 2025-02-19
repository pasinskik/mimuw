/**
 * Klasa reprezentująca linię tramwajową.
 */

public class Linia {
    private final int nrLinii;
    private final int liczbaPojazdówNaLinii;
    private final Tramwaj[] pojazdy;
    private final Przystanek[] przystanki;
    private final int[] czasyPrzejazdu;
    private final int coIleRanoWyjeżdża;

    /**
     * Konstruktor przyjmuje informację o liczbie pojazdów na linii, ale tworzy te pojazdy sam. Z tej racji potrzebna
     * jest mu informacja o tym, jakie są dozwolone numery boczne, bo pierwszy tworzony pojazd dostaje najmniejszy
     * dozwolony numer (pierwszyNrBoczny), a kolejne mają te numery coraz większe. Dodatkowo pierwsza połowa
     * pojazdów jako numer przystanku ustawiona ma -1, co oznacza, że wyjeżdżają z lewego końca trasy, a druga połowa
     * z kolei wyjeżdża z końca prawego (jeśli pojazdów jest nieparzyście, to o jeden więcej wyjeżdża z lewego końca).
     * Ponadto konstruktor oblicza, co ile mają wyjeżdżać rano tramwaje na tej linii.
     */

    public Linia(int nrLinii, int pierwszyNrBoczny, int liczbaPojazdówNaLinii, int liczbaPrzystanków,
                 Przystanek[] przystanki, int[] czasyPrzejazdu, int pojemnośćTramwaju) {
        this.nrLinii = nrLinii;
        this.liczbaPojazdówNaLinii = liczbaPojazdówNaLinii;

        this.przystanki = new Przystanek[liczbaPrzystanków];
        this.czasyPrzejazdu = new int[liczbaPrzystanków];
        for (int i = 0; i < liczbaPrzystanków; ++i) {
            this.przystanki[i] = przystanki[i];
            this.czasyPrzejazdu[i] = czasyPrzejazdu[i];
        }

        pojazdy = new Tramwaj[liczbaPojazdówNaLinii];
        int j = 0;
        while (j < Math.ceil((double) liczbaPojazdówNaLinii / 2)){
            pojazdy[j] = new Tramwaj(pierwszyNrBoczny + j, this, pojemnośćTramwaju, -1);
            ++j;
        }
        while (j < liczbaPojazdówNaLinii) {
            pojazdy[j] = new Tramwaj(pierwszyNrBoczny + j, this, pojemnośćTramwaju, liczbaPrzystanków);
            ++j;
        }

        int temp = 0;
        for (int i = 0; i < czasyPrzejazdu.length; ++i) {
            temp += 2 * czasyPrzejazdu[i];
        }
        temp /= liczbaPojazdówNaLinii;
        coIleRanoWyjeżdża = temp;
    }

    public int getNrLinii() {
        return nrLinii;
    }

    public int getLiczbaPojazdówNaLinii() {
        return liczbaPojazdówNaLinii;
    }

    public Tramwaj[] getPojazdy() {
        return pojazdy;
    }

    public Przystanek[] getPrzystanki() {
        return przystanki;
    }

    public int[] getCzasyPrzejazdu() {
        return czasyPrzejazdu;
    }

    public int getCoIleRanoWyjeżdża() {
        return coIleRanoWyjeżdża;
    }

    @Override
    public String toString() {
        return "nr " + nrLinii + ", liczba pojazdów: " + liczbaPojazdówNaLinii + ". Trasa to: " + trasaToString();
    }

    private String trasaToString() {
        String temp = "";
        for (int i = 0; i < przystanki.length; ++i) {
            temp += przystanki[i].getNazwaPrzystanku();
            if (i != przystanki.length - 1)
                temp += " ";
        }
        return temp;
    }
}
