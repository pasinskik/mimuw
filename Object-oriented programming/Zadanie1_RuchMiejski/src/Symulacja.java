import java.util.Objects;
import java.util.Scanner;

/**
 * Klasa pozwala na przeprowadzenie symulacji ruchu tramwajów. Wczytuje wszystkie informacje o tym, jak symulacja
 * ma wyglądać, a następnie ją przeprowadza, zarządzając wszystkimi innymi obiektami w programie. Godzina
 * w symulacji to po prostu liczba minut, które minęły od 6:00 danego dnia (np. 127 reprezentuje 8:07).
 */

public class Symulacja {
    private int liczbaDniSymulacji;
    private int dzieńSymulacji;
    private int pojemnośćPrzystanku;
    private Przystanek[] przystanki;
    private Pasażer[] pasażerowie;
    private int pojemnośćTramwaju;
    private Linia[] linieTramwajowe;
    private KolejkaZdarzeń kolejkaZdarzeń;
    private int[] łączneCzasyCzekaniaNaPrzystankach;
    private int[] łączneLiczbyPrzejazdów;


    public Symulacja() {
        dzieńSymulacji = 1;
    }

    /**
     * Główna metoda klasy, gdyż wykonuje całą symulację. Na początku wczytuje dane, potem je wypisuje, i jeśli
     * liczba dni symulacji jest różna od 0, to metoda przystępuje do jej wykonania. Dla każdego dnia symulacji
     * tworzy nową kolejkę zdarzeń, zleca wykonanie zdarzeń oraz wypisanie ich wszystkich. Pod koniec każdego dnia
     * wypisywane są statystyki dla danego dnia, a na koniec symulacji statystyki ogólne.
     */

    public void wykonajSymulację() {
        wczytajDane();
        wypiszDane();
        if (liczbaDniSymulacji == 0)
            return;

        for (int i = 0; i < liczbaDniSymulacji; ++i) {
            stwórzKolejkęZdarzeńDlaSymulacji();
            wykonajDzieńSymulacji();
            wypiszStatystykiDlaDnia();
            ++dzieńSymulacji;
        }

        wypiszOgólneStatystyki();
    }

    /**
     * Metoda wykonuje dzień symulacji, tzn. liczy "minuty" od 6:00 do 0:00 i jeśli są w kolejce zdarzeń jakieś
     * zdarzenia, które w tej minucie się wykonują, to metoda zleca ich wykonanie. Na koniec dnia metoda zleca
     * powrót pasażerów do domu oraz zliczenie statystyk.
     */

    private void wykonajDzieńSymulacji() {
        int godzina = 0;
        while (godzina != 60*18) {
            if (kolejkaZdarzeń.getRozmiar() > 0 && kolejkaZdarzeń.weźZdarzenie().getGodzina() == godzina) {
                kolejkaZdarzeń.wyjmijZdarzenie().wykonajZdarzenie();
            }
            else
                ++godzina;
        }

        pasażerowieWracająDoDomów();
        zliczStatystyki();
    }

    /**
     * Metoda wczytuje dane z wejścia. Mają one być podane w takiej postaci jak w poleceniu do zadania.
     */

    private void wczytajDane() {
        Scanner sc = new Scanner(System.in);

        liczbaDniSymulacji = sc.nextInt();
        pojemnośćPrzystanku = sc.nextInt();
        int liczbaPrzystanków = sc.nextInt();
        przystanki = new Przystanek[liczbaPrzystanków];
        for (int i = 0; i < liczbaPrzystanków; ++i) {
            przystanki[i] = new Przystanek(sc.next(), pojemnośćPrzystanku);
        }

        int liczbaPasażerów = sc.nextInt();
        pasażerowie = new Pasażer[liczbaPasażerów];
        for (int i = 0; i < liczbaPasażerów; ++i) {
            pasażerowie[i] = new Pasażer(i, przystanki[Losowanie.losuj(0, liczbaPrzystanków)]);
        }

        pojemnośćTramwaju = sc.nextInt();
        int liczbaLiniiTramwajowych = sc.nextInt();
        linieTramwajowe = new Linia[liczbaLiniiTramwajowych];
        Przystanek[] tempPrzystanki = new Przystanek[liczbaPrzystanków];
        int[] tempCzasyDojazdu = new int[liczbaPrzystanków];
        int tempLiczbaTramwajówLinii;
        int tempDługośćTrasy;
        int pojazdyCounter = 0;
        int linieCounter = 0;
        for (int i = 0; i < liczbaLiniiTramwajowych; ++i) {
            tempLiczbaTramwajówLinii = sc.nextInt();
            tempDługośćTrasy = sc.nextInt();

            for (int j = 0; j < tempDługośćTrasy; ++j) {
                String tempNazwaPrzystanku = sc.next();
                for (int k = 0; k < przystanki.length; ++k) {
                    if (Objects.equals(przystanki[k].getNazwaPrzystanku(), tempNazwaPrzystanku))
                        tempPrzystanki[j] = przystanki[k];
                }
                tempCzasyDojazdu[j] = sc.nextInt();
            }

            linieTramwajowe[i] = new Linia(linieCounter, pojazdyCounter, tempLiczbaTramwajówLinii,
                                    tempDługośćTrasy, tempPrzystanki, tempCzasyDojazdu, pojemnośćTramwaju);

            pojazdyCounter += tempLiczbaTramwajówLinii;
            ++linieCounter;
        }

        łączneCzasyCzekaniaNaPrzystankach = new int[liczbaDniSymulacji];
        łączneLiczbyPrzejazdów = new int[liczbaDniSymulacji];
    }

    /**
     * Metoda tworzy kolejkę zdarzeń na dany dzień symulacji. W tej kolejce znajdować się będą tylko przyjazdy
     * tramwajów na przystanek oraz pójścia na przystanek pasażerów. Te drugie zdarzenia są tworzone na początku
     * dla każdego pasażera (każdy ma już bowiem przydzielony swój przystanek i godzinę pójścia na niego). Przyjazdy
     * tramwajów są tworzone w następujący sposób: Dla każdej linii dla pierwszej połowy pojazdów dla każdego pojazdu
     * po kolei metoda tworzy wszystkie kółka, które ten pojazd zacznie od 6:00 do 23:00, najpierw puszczając pojazd
     * z lewego końca trasy do prawego, i po postoju na pętli, z powrotem z prawego do lewego. Dla drugiej połowy
     * pojazdów robi się to samo, ale puszczając pojazdy z prawego końca trasy do lewego.
     */

    private void stwórzKolejkęZdarzeńDlaSymulacji() {
        kolejkaZdarzeń = new KolejkaZdarzeń();

        for (int i = 0; i < pasażerowie.length; ++i) {
            Zdarzenie temp = new PójścieNaPrzystanek(dzieńSymulacji, pasażerowie[i].getGodzinaPójściaNaPrzystanek(),
                                                    pasażerowie[i], pasażerowie[i].getNajbliższyPrzystanek());
            kolejkaZdarzeń.dodajZdarzenie(temp);
        }

        for (int i = 0; i < linieTramwajowe.length; ++i) {
            int godzinaPierwszegoWyjazdu = 0;
            int j = 0;
            while (j < Math.ceil((double) linieTramwajowe[i].getLiczbaPojazdówNaLinii() / 2)) {
                int tempGodzina = godzinaPierwszegoWyjazdu;
                while (tempGodzina <= 60*17) {
                    for (int k = 0; k < linieTramwajowe[i].getPrzystanki().length; ++k) {
                        Zdarzenie temp = new PrzyjechanieTramwaju(dzieńSymulacji, tempGodzina,
                                linieTramwajowe[i].getPojazdy()[j], linieTramwajowe[i].getPrzystanki()[k],
                                linieTramwajowe[i].getPrzystanki()[linieTramwajowe[i].getPrzystanki().length - 1]);
                        kolejkaZdarzeń.dodajZdarzenie(temp);
                        tempGodzina += linieTramwajowe[i].getCzasyPrzejazdu()[k];
                    }

                    for (int k = linieTramwajowe[i].getPrzystanki().length - 1; k >= 0; --k) {
                        Zdarzenie temp = new PrzyjechanieTramwaju(dzieńSymulacji, tempGodzina,
                                            linieTramwajowe[i].getPojazdy()[j], linieTramwajowe[i].getPrzystanki()[k],
                                            linieTramwajowe[i].getPrzystanki()[0]);
                        kolejkaZdarzeń.dodajZdarzenie(temp);

                        if (k != 0)
                            tempGodzina += linieTramwajowe[i].getCzasyPrzejazdu()[k-1];
                        else
                            tempGodzina += linieTramwajowe[i].getCzasyPrzejazdu()[linieTramwajowe[i]
                                    .getPrzystanki().length - 1];
                    }
                }

                godzinaPierwszegoWyjazdu += linieTramwajowe[i].getCoIleRanoWyjeżdża();
                ++j;
            }

            godzinaPierwszegoWyjazdu = 0;
            while (j < linieTramwajowe[i].getLiczbaPojazdówNaLinii()) {
                int tempGodzina = godzinaPierwszegoWyjazdu;
                while (tempGodzina <=  60*17) {
                    for (int k = linieTramwajowe[i].getPrzystanki().length - 1; k >= 0; --k) {
                        Zdarzenie temp = new PrzyjechanieTramwaju(dzieńSymulacji, tempGodzina,
                                        linieTramwajowe[i].getPojazdy()[j], linieTramwajowe[i].getPrzystanki()[k],
                                        linieTramwajowe[i].getPrzystanki()[0]);
                        kolejkaZdarzeń.dodajZdarzenie(temp);

                        if (k != 0)
                            tempGodzina += linieTramwajowe[i].getCzasyPrzejazdu()[k-1];
                        else
                            tempGodzina += linieTramwajowe[i].getCzasyPrzejazdu()[linieTramwajowe[i]
                                    .getPrzystanki().length - 1];
                    }

                    for (int k = 0; k < linieTramwajowe[i].getPrzystanki().length; ++k) {
                        Zdarzenie temp = new PrzyjechanieTramwaju(dzieńSymulacji, tempGodzina,
                                linieTramwajowe[i].getPojazdy()[j], linieTramwajowe[i].getPrzystanki()[k],
                                linieTramwajowe[i].getPrzystanki()[linieTramwajowe[i].getPrzystanki().length - 1]);
                        kolejkaZdarzeń.dodajZdarzenie(temp);
                        tempGodzina += linieTramwajowe[i].getCzasyPrzejazdu()[k];
                    }
                }

                godzinaPierwszegoWyjazdu += linieTramwajowe[i].getCoIleRanoWyjeżdża();
                ++j;
            }
        }
    }

    /**
     * Metoda symuluje powrót pasażerów do domu, tzn. usuwa wszystkich z tramwajów oraz przystanków, a tym, którzy
     * o godzinie 0:00 znajdują się jeszcze na przystankach, dodaje do ich czasu czekania odpowiednią liczbę minut.
     */

    private void pasażerowieWracająDoDomów() {
        for (int i = 0; i < pasażerowie.length; ++i) {
            if (pasażerowie[i].isCzyPodróżujeTegoDnia() && !pasażerowie[i].isCzyWTramwaju()) {
                pasażerowie[i].addToCzasCzekania(18*60 - pasażerowie[i].getGodzinaPójściaNaPrzystanek());
            }
        }

        for (int i = 0; i < linieTramwajowe.length; ++i) {
            for (int j = 0; j < linieTramwajowe[i].getLiczbaPojazdówNaLinii(); ++j) {
                linieTramwajowe[i].getPojazdy()[j].opróżnijPojazd();
            }
        }

        for (int i = 0; i < przystanki.length; ++i) {
            przystanki[i].opróżnijPrzystanek();
        }
    }

    /**
     * Metoda zlicza statystyki po skończonym dniu symulacji.
     */

    private void zliczStatystyki() {
        for (int i = 0; i < pasażerowie.length; ++i) {
            łączneLiczbyPrzejazdów[dzieńSymulacji - 1] += pasażerowie[i].getLiczbaPrzejazdów();
            łączneCzasyCzekaniaNaPrzystankach[dzieńSymulacji - 1] += pasażerowie[i].getCzasCzekania();
            pasażerowie[i].przygotujSięNaNowyDzień();
        }
    }

    /**
     * Metoda zlicza wszystkie przejazdy z wszystkich dni symulacji.
     */

    private int policzWszystkiePrzejazdy() {
        int wynik = 0;
        for (int i = 0; i < łączneLiczbyPrzejazdów.length; ++i) {
            wynik += łączneLiczbyPrzejazdów[i];
        }
        return wynik;
    }

    /**
     * Metoda oblicza średni czas oczekiwania pasażera na przystanku na przejazd.
     */

    private float obliczŚredniCzasCzekania() {
        if (policzWszystkiePrzejazdy() == 0)
            return 0;

        int temp = 0;
        for (int i = 0; i < łączneCzasyCzekaniaNaPrzystankach.length; ++i) {
            temp += łączneCzasyCzekaniaNaPrzystankach[i];
        }
        return (float) (temp) / policzWszystkiePrzejazdy();
    }

    /**
     * Metoda wypisuje wczytane dane.
     */

    private void wypiszDane() {
        System.out.println("Wczytano następujące dane:");
        System.out.println("Liczba dni symulacji: " + liczbaDniSymulacji);
        System.out.println("Pojemność przystanku: " + pojemnośćPrzystanku);
        System.out.println(przystanki.length + " przystanki o nazwach:");
        for (int i = 0; i < przystanki.length; ++i) {
            System.out.println(przystanki[i]);
        }
        System.out.println("Liczba pasażerów: " + pasażerowie.length);
        System.out.println("Pojemność tramwaju: " + pojemnośćTramwaju);
        System.out.println(linieTramwajowe.length + " linii tramwajowych:");
        for (int i = 0; i < linieTramwajowe.length; ++i) {
            System.out.println(linieTramwajowe[i]);
        }
    }

    /**
     * Metoda wypisuje ogólne statystyki na koniec symulacji.
     */

    private void wypiszOgólneStatystyki() {
        System.out.println("We wszystkich dniach symulacji odbyto łącznie " + policzWszystkiePrzejazdy()
                + " przejazdów, a średni czas czekania na przystanku to " + obliczŚredniCzasCzekania() +  " minut.");
    }

    /**
     * Metoda wypisuje statystyki z jednego dnia symulacji.
     */

    private void wypiszStatystykiDlaDnia() {
        System.out.println("W dniu " + dzieńSymulacji + " odbyto łącznie " + łączneLiczbyPrzejazdów[dzieńSymulacji - 1]
                + " przejazdów, a łączny czas oczekiwania na przystankach to "
                + łączneCzasyCzekaniaNaPrzystankach[dzieńSymulacji - 1] + " minut.");
    }

    /**
     * Metoda zlicza statystyki po skończonym dniu symulacji.
     */

    public static String zformatujGodzinę (int godzina) {
        return (6 + godzina / 60) + ":" + (godzina % 60 < 10 ? "0" : "") + (godzina % 60);
    }
}
