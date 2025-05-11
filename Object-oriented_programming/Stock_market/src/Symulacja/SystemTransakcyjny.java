package Symulacja;

import Inwestorzy.*;
import Zlecenia.*;
import java.io.*;
import java.util.*;

/**
 * Klasa reprezentujaca system transakcyjny, ktory odpowiada za przeprowadzanie symulacji.
 */

public class SystemTransakcyjny {
    private List<Inwestor> inwestorzy;
    private List<Spolka> spolki;
    // zlecenia kupna w symulacji sa posortowane od tego z najwiekszym priorytetem do tego z najmniejszym
    private Map<Spolka, LinkedList<ZlecenieKupna>> zleceniaKupna;
    // zlecenia sprzedazy w symulacji sa posortowane od tego z najwiekszym priorytetem do tego z najmniejszym
    private Map<Spolka, LinkedList<ZlecenieSprzedazy>> zleceniaSprzedazy;
    private int obecnaTura;
    private int liczbaTur;
    private int niezmiennikGotowki;
    private Map<Spolka, Integer> niezmiennikAkcji;

    public SystemTransakcyjny() {
        inwestorzy = new ArrayList<>();
        spolki = new ArrayList<>();
        zleceniaKupna = new HashMap<>();
        zleceniaSprzedazy = new HashMap<>();
        obecnaTura = 0;
        liczbaTur = 0;
        niezmiennikGotowki = 0;
        niezmiennikAkcji = new HashMap<>();
    }

    /**
     * Metoda w szczegolnosci pozwalajaca inwestorowi poznac liste spolek i dowolna z nich zapytac
     * o cene jej ostatniej transakcji.
     */

    public List<Spolka> getSpolki() {
        return spolki;
    }

    /**
     * Metoda w szczegolnosci pozwalajaca inwestorowi poznac numer aktualnej tury.
     */

    public int getObecnaTura() {
        return obecnaTura;
    }

    public Map<Spolka, Integer> getNiezmiennikAkcji() {
        return niezmiennikAkcji;
    }

    public List<Inwestor> getInwestorzy() {
        return inwestorzy;
    }

    /**
     * Metoda wczytujaca dane do programu, zgodnie z poleceniem.
     */

    public void ReadingInput(String inputFileName) {
        int ileRandomInwestorow = 0;
        int ileSMAInwestorow = 0;
        int poczatkowaKwota = 0;
        Map<Spolka, Integer> poczatkowyPortfel = new HashMap<>();

        try (BufferedReader reader = new BufferedReader(new FileReader(inputFileName))) {
            String line;

            while ((line = reader.readLine()) != null) {
                line = line.trim();

                if (line.startsWith("#") || line.isEmpty())
                    continue;

                if (line.matches("^[RS\\s]+$")) {
                    ileRandomInwestorow = policzLiczbeWystapienZnaku(line, 'R');
                    ileSMAInwestorow = policzLiczbeWystapienZnaku(line, 'S');
                }

                else if (line.matches("^[A-Z]+:\\d+(\\s+[A-Z]+:\\d+)*$")) {
                    String[] parts = line.split("\\s+");

                    for (String part : parts) {
                        String[] akcje = part.split(":");

                        if (akcje[0].length() <= 5)
                            spolki.add(new Spolka(akcje[0], Integer.parseInt(akcje[1])));
                        else
                            throw new IllegalArgumentException("Zbyt dluga nazwa spolki: " + akcje[0]);
                    }
                }

                else if (line.matches("^\\d+\\s+.*")) {
                    String[] parts = line.split("\\s+");

                    poczatkowaKwota = Integer.parseInt(parts[0]);

                    for (int i = 1; i < parts.length; i++) {
                        String[] akcje = parts[i].split(":");

                        Spolka temp = czyAkcjaJestWObrocie(akcje[0]);
                        if (temp != null)
                            poczatkowyPortfel.put(temp, Integer.parseInt(akcje[1]));
                    }
                }

                else
                    throw new IllegalArgumentException("Niepoprawne znaki w pliku");
            }

            reader.close();

            niezmiennikGotowki = poczatkowaKwota * (ileRandomInwestorow + ileSMAInwestorow);
            for (Spolka spolka : spolki) {
                niezmiennikAkcji.put(spolka,
                        poczatkowyPortfel.get(spolka) * (ileRandomInwestorow + ileSMAInwestorow));
            }

            // po wczytaniu nazw spolek oraz gotowki/portfeli dla kazdego inwestora trzeba jeszcze stworzyc
            // obiekty reprezentujace poszczegolnych inwestorow

            for (int i = 0; i < ileRandomInwestorow; ++i) {
                inwestorzy.add(new InwestorRandom(poczatkowaKwota, poczatkowyPortfel));
            }

            for (int i = 0; i < ileSMAInwestorow; ++i) {
                inwestorzy.add(new InwestorSMA(poczatkowaKwota, poczatkowyPortfel));
            }
        }

        catch (IOException e) {
            System.err.println("Blad w czytaniu pliku: " + e.getMessage());
        }

        catch (IllegalArgumentException e) {
            System.err.println("Bledna zawartosc pliku: " + e.getMessage());
        }
    }

    /**
     * Metoda inicjalizujaca symulacje na podstawie pliku, a potem ja przeprowadzajaca.
     */

    public void inicjalizacjaSymulacji(String inputFileName, int numberOfTurns) {
        liczbaTur = numberOfTurns;
        ReadingInput(inputFileName);

        for (Spolka spolka: spolki) {
            zleceniaKupna.put(spolka, new LinkedList<>());
            zleceniaSprzedazy.put(spolka, new LinkedList<>());
        }

        przeprowadzSymulacje();
    }

    /**
     * Metoda przeprowadza symulacje. W kazdej turze najpierw usuwa niewazne zlecenia, potem prosi o nowe,
     * nastepnie realizuje zlecenia, aktualizuje dane statystyczne (SMA 5 i SMA10) oraz sprawdza niezmiennik
     * symulacji, zeby zapewnic, ze wszystko dziala prawidlowo.
     */

    private void przeprowadzSymulacje() {
        while (obecnaTura < liczbaTur) {
            usunNiewazneZlecenia();
            poprosOZlecenia();
            realizacjaZlecen();
            zaaktualizujSMA();
            ++obecnaTura;
        }
    }


    /**
     * Metoda wypisuje wynik symulacji (stan konta i portfel kazdego z inwestorow po ostatniej turze).
     */

    public void wypiszWynikSymulacji() {
        for (Inwestor inwestor : inwestorzy) {
            System.out.println(inwestor.toStringOutput());
        }
    }

    /**
     * Metoda realizuje zlecenia. Przyjety sposob to: dla kazdej spolki dla kazdego zlecenia kupna
     * probujemy je zrealizowac zgodnie z warunkami podanymi w poleceniu (zlecenie kupna z najwyzsza
     * cena jest dopasowywane do zlecenia sprzedazy z najnizsza cena itd). Najpierw patrzymy czy
     * realizacja jest mozliwa i jesli tak, to ja wykonujemy. Potem dla kazdego zlecenia sprzedazy
     * patrzymy czy mozna je zrealizowac, po to, aby zrealizowac ewentualne zlecenia wykonaj albo
     * anuluj, ktore nie mogly byc zrealizowane podczas przegladania zlecen kupna.
     */

    private void realizacjaZlecen() {
        for (Spolka spolka : spolki) {
            Iterator<ZlecenieKupna> kupnoIterator = zleceniaKupna.get(spolka).iterator();

            while (kupnoIterator.hasNext()) {
                ZlecenieKupna zlecenieKupna = kupnoIterator.next();

                switch(czyMoznaZrealizowacZlecenieKupna(spolka, zlecenieKupna)) {
                    case -1:
                        kupnoIterator.remove();
                        break;

                    case 1:
                        zrealizujZlecenieKupna(spolka, zlecenieKupna);
                        if (zlecenieKupna.getLiczbaSztuk() == 0)    // jesli zrealizowano cale zlecenie,
                                                                    // mozna je usunac
                            kupnoIterator.remove();
                }
            }

            Iterator<ZlecenieSprzedazy> sprzedazyIterator = zleceniaSprzedazy.get(spolka).iterator();

            while (sprzedazyIterator.hasNext()) {
                ZlecenieSprzedazy zlecenieSprzedazy = sprzedazyIterator.next();

                switch(czyMoznaZrealizowacZlecenieSprzedazy(spolka, zlecenieSprzedazy)) {
                    case -1:
                        sprzedazyIterator.remove();
                        break;

                    case 1:
                        zrealizujZlecenieSprzedazy(spolka, zlecenieSprzedazy);
                        if (zlecenieSprzedazy.getLiczbaSztuk() == 0)    // jesli zrealizowano cale zlecenie,
                                                                        // mozna je usunac
                            sprzedazyIterator.remove();
                }
            }
        }
    }

    /**
     * Metoda realizuje zlecenie kupna. Patrzymy na zlecenia sprzedazy zgodnie z ich priorytetem i
     * umozliwiamy dokonanie transakcji wtedy, gdy mozna to zrobic. Przed wywolaniem tej metody
     * zostalo sprawdzone, czy da sie zrealizowac dane zlecenie (w tym czy jego autor jest w stanie
     * za nie zaplacic oraz czy sprzedajacy maja wystarczajaco akcji), wiec wszystko zrealizuje sie poprawnie.
     */

    private void zrealizujZlecenieKupna(Spolka spolka, ZlecenieKupna zlecenieKupna) {
        Iterator<ZlecenieSprzedazy> sprzedazIterator = zleceniaSprzedazy.get(spolka).iterator();

        int akcjePozostaleDoKupienia = zlecenieKupna.getLiczbaSztuk();
        int lacznaKwota = 0;
        int tempKwota;

        while (sprzedazIterator.hasNext()) {
            ZlecenieSprzedazy zlecenieSprzedazy = sprzedazIterator.next();

            // jesli zlecenie wykonano do konca lub dalej nie bedzie juz mozliwych dopasowan, to przerywamy
            if (zlecenieSprzedazy.getLimitCeny() > zlecenieKupna.getLimitCeny() || akcjePozostaleDoKupienia == 0)
                break;

            int cenaTransakcji = obliczCeneTransakcji(zlecenieSprzedazy, zlecenieKupna);

            // jesli dane zlecenie sprzedazy bedzie zrealizowane w calosci to trzeba je usunac
            if (zlecenieSprzedazy.getLiczbaSztuk() <= akcjePozostaleDoKupienia) {
                akcjePozostaleDoKupienia -= zlecenieSprzedazy.getLiczbaSztuk();
                tempKwota = cenaTransakcji * zlecenieSprzedazy.getLiczbaSztuk();
                lacznaKwota += tempKwota;

                zlecenieSprzedazy.getAutorZlecenia().
                        sprzedajAkcje(spolka, zlecenieSprzedazy.getLiczbaSztuk(), tempKwota);

                sprzedazIterator.remove();
            }

            // w innym przypadku zlecenie sprzedazy nie bedzie realizowane w calosci, wiec go nie usuwamy
            else {
                // jesli zlecenie sprzedazy jest typu wykonaj albo anuluj, to jego czesc nie moze byc teraz wykonana
                // bo nie wiadomo, czy reszta tez pozniej zostanie wykonana
                if (zlecenieSprzedazy.czyZlecenieWykonajAnuluj())
                    continue;

                tempKwota = cenaTransakcji*akcjePozostaleDoKupienia;
                lacznaKwota += tempKwota;

                zlecenieSprzedazy.getAutorZlecenia().sprzedajAkcje(spolka, akcjePozostaleDoKupienia, tempKwota);

                zlecenieSprzedazy.zmniejszLiczbeSztuk(akcjePozostaleDoKupienia);
                akcjePozostaleDoKupienia = 0;
            }
        }

        //dodajemy akcje do portfela kupujacego i dostosowujemy dane przetwarzanego zlecenia
        zlecenieKupna.getAutorZlecenia().kupAkcje(spolka,
                zlecenieKupna.getLiczbaSztuk() - akcjePozostaleDoKupienia, lacznaKwota);
        zlecenieKupna.setLiczbaSztuk(akcjePozostaleDoKupienia);
    }

    /**
     * Metoda sprawdza czy mozna zrealizowac zlecenie kupna. Patrzymy na zlecenia sprzedazy
     * zgodnie z ich priorytetem i sprawdzamy jak duzo akcji da sie kupic.
     */

    private int czyMoznaZrealizowacZlecenieKupna(Spolka spolka, ZlecenieKupna zlecenieKupna) {
        Iterator<ZlecenieSprzedazy> sprzedazIterator = zleceniaSprzedazy.get(spolka).iterator();

        // patrzac na to, czy da sie zrealizowac zamowienie musimy wiedziec, ktory inwestor ile akcji ma
        // sprzedac, aby moc wykryc, kiedy te akcje beda przekraczaly jego zasob portfela w chwili transakcji
        Map<Inwestor, Integer> zobowiazaniaInwestorow = new HashMap<>();

        int akcjePozostaleDoKupienia = zlecenieKupna.getLiczbaSztuk();
        int lacznaKwota = 0;

        while (sprzedazIterator.hasNext()) {
            ZlecenieSprzedazy zlecenieSprzedazy = sprzedazIterator.next();

            // jesli zlecenie wykonano do konca lub dalej nie bedzie juz mozliwych dopasowan, to przerywamy
            if (zlecenieSprzedazy.getLimitCeny() > zlecenieKupna.getLimitCeny() || akcjePozostaleDoKupienia == 0)
                break;

            int cenaTransakcji = obliczCeneTransakcji(zlecenieSprzedazy, zlecenieKupna);
            Inwestor inwestor = zlecenieSprzedazy.getAutorZlecenia();
            int akcjeTegoZlecenia;

            // ustalmy, czy realizujemy cale zlecenie sprzedazy czy tylko kawalek
            if (zlecenieSprzedazy.getLiczbaSztuk() <= akcjePozostaleDoKupienia)
                akcjeTegoZlecenia = zlecenieSprzedazy.getLiczbaSztuk();
            else
                akcjeTegoZlecenia = akcjePozostaleDoKupienia;

            int ileOdTegoInwestora = zobowiazaniaInwestorow.getOrDefault(inwestor, 0) + akcjeTegoZlecenia;

            // jesli inwestor nie jest w stanie zrealizowac tego zlecenia, to zgodnie z poleceniem - usuwamy je
            if (!zlecenieSprzedazy.getAutorZlecenia().czyZdolnySprzedac(spolka, ileOdTegoInwestora)) {
                sprzedazIterator.remove();
                continue;
            }

            // jesli jest to zlecenie wykonaj albo anuluj niewykonywane w calosci, to musimy je pominajac,
            // bo nie da sie w tej chwili zagwarantowac ze to zlecenie zostanie w tej turze wykonane w calosci
            if (zlecenieSprzedazy.getLiczbaSztuk() > akcjePozostaleDoKupienia
                    && zlecenieSprzedazy.czyZlecenieWykonajAnuluj())
                continue;

            lacznaKwota += cenaTransakcji * akcjeTegoZlecenia;
            akcjePozostaleDoKupienia -= akcjeTegoZlecenia;
            zobowiazaniaInwestorow.put(inwestor, ileOdTegoInwestora);
        }

        // jesli zlecenie jest typu wykonaj albo anuluj, to musi byc zrealizowane w calosci
        if (zlecenieKupna.czyZlecenieWykonajAnuluj() && akcjePozostaleDoKupienia != 0)
            return -1;

        // jesli nie da sie ze zleceniem kupna nic polaczyc, to nie da sie go teraz zrealizowac
        if (lacznaKwota == 0)
            return 0;

        // jesli kupujacy ma wystarczajaco gotowki, to da sie zlecenie zrealizowac
        if (zlecenieKupna.getAutorZlecenia().getGotowka() > lacznaKwota)
            return 1;

        // a jesli nie ma wystarczajaco pieniedzy lub jesli zlecenie jest typu wykonaj albo anuluj,
        // a nie jest mozliwe do zrealizowania w calosci, to zlecenie bedzie usuniete z systemu
        return -1;
    }

    /**
     * Metoda realizuje zlecenie sprzedazy. Patrzymy na zlecenia kupna zgodnie z ich priorytetem i
     * umozliwiamy dokonanie transakcji wtedy, gdy mozna to zrobic. Przed wywolaniem tej metody
     * zostalo sprawdzone, czy da sie zrealizowac dane zlecenie (w tym czy jego autor ma w portfelu
     * odpowiednia liczbe akcji oraz czy kupujacy maja wystarczajaco pieniedzy), wiec wszystko
     * zrealizuje sie poprawnie.
     */

    private void zrealizujZlecenieSprzedazy(Spolka spolka, ZlecenieSprzedazy zlecenieSprzedazy) {
        Iterator<ZlecenieKupna> kupnaIterator = zleceniaKupna.get(spolka).iterator();

        int akcjePozostaleDoSprzedazy = zlecenieSprzedazy.getLiczbaSztuk();
        int lacznaKwota = 0;
        int tempKwota;

        while (kupnaIterator.hasNext()) {
            ZlecenieKupna zlecenieKupna = kupnaIterator.next();

            // jesli zlecenie wykonano do konca lub dalej nie bedzie juz mozliwych dopasowan, to przerywamy
            if (zlecenieSprzedazy.getLimitCeny() > zlecenieKupna.getLimitCeny() || akcjePozostaleDoSprzedazy == 0)
                break;

            int cenaTransakcji = obliczCeneTransakcji(zlecenieSprzedazy, zlecenieKupna);

            // jesli dane zlecenie kupna bedzie zrealizowane w calosci to trzeba je usunac
            if (zlecenieKupna.getLiczbaSztuk() <= akcjePozostaleDoSprzedazy) {
                akcjePozostaleDoSprzedazy -= zlecenieKupna.getLiczbaSztuk();
                tempKwota = cenaTransakcji * zlecenieKupna.getLiczbaSztuk();
                lacznaKwota += tempKwota;

                zlecenieKupna.getAutorZlecenia().kupAkcje(spolka, zlecenieKupna.getLiczbaSztuk(), tempKwota);
                kupnaIterator.remove();
            }

            // w innym przypadku wykonujemy tylko czesc zlecenia
            else {
                // jesli zlecenie kupna jest typu wykonaj albo anuluj, to jego czesc nie moze byc teraz wykonana
                // bo nie wiadomo, czy reszta tez pozniej zostanie wykonana
                if (zlecenieKupna.czyZlecenieWykonajAnuluj())
                    continue;

                tempKwota = cenaTransakcji * akcjePozostaleDoSprzedazy;
                lacznaKwota += tempKwota;

                zlecenieKupna.getAutorZlecenia().kupAkcje(spolka, akcjePozostaleDoSprzedazy, tempKwota);
                zlecenieKupna.zmniejszLiczbeSztuk(akcjePozostaleDoSprzedazy);

                akcjePozostaleDoSprzedazy = 0;
            }
        }

        //odejmujemy akcje z portfela sprzedajacego i dostosowujemy dane przetwarzanego zlecenia
        zlecenieSprzedazy.getAutorZlecenia().sprzedajAkcje(spolka,
                zlecenieSprzedazy.getLiczbaSztuk() - akcjePozostaleDoSprzedazy, lacznaKwota);
        zlecenieSprzedazy.setLiczbaSztuk(akcjePozostaleDoSprzedazy);
    }

    /**
     * Metoda sprawdza czy mozna zrealizowac zlecenie sprzedazy. Patrzymy na zlecenia kupna
     * zgodnie z ich priorytetem i sprawdzamy jak duzo akcji da sie sprzedac.
     */

    private int czyMoznaZrealizowacZlecenieSprzedazy(Spolka spolka, ZlecenieSprzedazy zlecenieSprzedazy) {
        Iterator<ZlecenieKupna> kupnaIterator = zleceniaKupna.get(spolka).iterator();

        // patrzac na to, czy da sie zrealizowac zamowienie musimy wiedziec, ktory inwestor ile ma zaplacic,
        // aby moc wykryc, kiedy laczna kwota bedzie przekraczala stan jego konta
        Map<Inwestor, Integer> zobowiazaniaInwestorow = new HashMap<>();

        int akcjePozostaleDoSprzedazy = zlecenieSprzedazy.getLiczbaSztuk();
        int lacznaKwota = 0;

        while (kupnaIterator.hasNext()) {
            ZlecenieKupna zlecenieKupna = kupnaIterator.next();

            // jesli zlecenie wykonano do konca lub dalej nie bedzie juz mozliwych dopasowan, to przerywamy
            if (zlecenieSprzedazy.getLimitCeny() > zlecenieKupna.getLimitCeny() || akcjePozostaleDoSprzedazy == 0)
                break;

            int cenaTransakcji = obliczCeneTransakcji(zlecenieSprzedazy, zlecenieKupna);
            Inwestor inwestor = zlecenieKupna.getAutorZlecenia();
            int akcjeTegoZlecenia;

            // ustalmy, czy realizujemy cale zlecenie sprzedazy czy tylko kawalek
            if (zlecenieKupna.getLiczbaSztuk() <= akcjePozostaleDoSprzedazy)
                akcjeTegoZlecenia = zlecenieKupna.getLiczbaSztuk();
            else
                akcjeTegoZlecenia = akcjePozostaleDoSprzedazy;

            int cenaTegoZlecenia = akcjeTegoZlecenia * cenaTransakcji;
            int ileOdTegoInwestora = zobowiazaniaInwestorow.getOrDefault(inwestor, 0) + cenaTegoZlecenia;

            // jesli inwestor nie jest w stanie zrealizowac tego zlecenia, to zgodnie z poleceniem - usuwamy je
            if (inwestor.getGotowka() < cenaTegoZlecenia) {
                kupnaIterator.remove();
                continue;
            }

            // jesli zlecenie kupna jest typu wykonaj albo anuluj, to jego czesc nie moze byc teraz wykonana
            // bo nie wiadomo, czy reszta tez pozniej zostanie wykonana
            if (zlecenieKupna.getLiczbaSztuk() > akcjePozostaleDoSprzedazy
                    && zlecenieKupna.czyZlecenieWykonajAnuluj())
                continue;

            lacznaKwota += cenaTegoZlecenia;
            akcjePozostaleDoSprzedazy -= akcjeTegoZlecenia;
            zobowiazaniaInwestorow.put(inwestor, ileOdTegoInwestora);
        }

        // jesli zlecenie jest typu wykonaj albo anuluj, to musi byc zrealizowane w calosci
        if (zlecenieSprzedazy.czyZlecenieWykonajAnuluj() && akcjePozostaleDoSprzedazy != 0)
            return -1;

        // jesli nie da sie ze zleceniem sprzedazy nic polaczyc, to nie da sie go teraz zrealizowac
        if (lacznaKwota == 0)
            return 0;

        // jesli sprzedajacy jest zdolny zrealizowac zamowienie, to jest ono realizowalne
        if (zlecenieSprzedazy.getAutorZlecenia().
                czyZdolnySprzedac(spolka, zlecenieSprzedazy.getLiczbaSztuk() - akcjePozostaleDoSprzedazy))
            return 1;

        // jesli nie, to zlecenie bedzie usuniete z systemu
        return -1;
    }

    /**
     * Metoda oblicza cene transakcji na podstawie wytycznych zadania (to znaczy w zaleznosci od tego,
     * ktore zlecenie zostalo zlozone wczesniej).
     */

    private int obliczCeneTransakcji(ZlecenieSprzedazy zlecenieSprzedazy, ZlecenieKupna zlecenieKupna) {
        if (zlecenieKupna.getTuraZlozenia() < zlecenieSprzedazy.getTuraZlozenia())
            return zlecenieKupna.getLimitCeny();

        else if (zlecenieKupna.getTuraZlozenia() == zlecenieSprzedazy.getTuraZlozenia()
                && (zlecenieKupna.getNumerWTurze() < zlecenieSprzedazy.getNumerWTurze()))
            return zlecenieKupna.getLimitCeny();

        return zlecenieSprzedazy.getLimitCeny();
    }

    /**
     * Metoda sprawdza kazde zlecenie w danej turze i sprawdza czy jest wazne. Jesli nie - usuwa je.
     */

    private void usunNiewazneZlecenia() {
        for (Spolka spolka : spolki) {
            Iterator<ZlecenieKupna> kupnoIterator = zleceniaKupna.get(spolka).iterator();

            while (kupnoIterator.hasNext()) {
                ZlecenieKupna zlecenie = kupnoIterator.next();

                if (zlecenie.czyZleceniePrzeterminowane(obecnaTura))
                    kupnoIterator.remove();
            }

            Iterator<ZlecenieSprzedazy> sprzedazIterator = zleceniaSprzedazy.get(spolka).iterator();

            while (sprzedazIterator.hasNext()) {
                ZlecenieSprzedazy zlecenie = sprzedazIterator.next();

                if (zlecenie.czyZleceniePrzeterminowane(obecnaTura))
                    sprzedazIterator.remove();
            }
        }
    }


    /**
     * Metoda odpytuje kazdego inwestora o zlecenie, ktore chce zlozyc.
     */

    private void poprosOZlecenia() {
        int i = 0;  // iterator jest informacja o tym, ktore w kolejnosci skladania jest
                    // konkretne zlecenie w danej turze (tj. zostalo zlozone jako i-te)

        for (Inwestor inwestor : inwestorzy) {
            ++i;

            Zlecenie zlecenie = inwestor.zlozZlecenie(this, i);

            // jesli zlecenie == null, to inwestor nie zlozyl zlecenia lub zlozyl zlecenie niepoprawne
            if (zlecenie == null)
                continue;

            if (zlecenie.getTypZlecenia() == Zlecenie.TypZlecenia.KUPNO)
                dodajZlecenieKupna((ZlecenieKupna) zlecenie);
            else
                dodajZlecenieSprzedazy((ZlecenieSprzedazy) zlecenie);
        }
    }

    /**
     * Metoda dodaje zlecenie kupna do listy zlecen kupna (sa one ulozone zgodnie z priorytetem: cena,
     * termin zlozenia; na poczatku jest najwazniejsze zlecenie).
     */

    private void dodajZlecenieKupna(ZlecenieKupna zlecenie) {
        LinkedList<ZlecenieKupna> listaKupna = zleceniaKupna.get(zlecenie.getAkcja());

        int i = 0;
        for (ZlecenieKupna z : listaKupna) {
            if (z.compareTo(zlecenie) > 0)
                break;

            ++i;
        }

        listaKupna.add(i, zlecenie);
    }

    /**
     * Metoda dodaje zlecenie sprzedazy do listy zlecen sprzedazy (sa one ulozone zgodnie z
     * priorytetem: cena, termin zlozenia; na poczatku jest najwazniejsze zlecenie).
     */

    private void dodajZlecenieSprzedazy(ZlecenieSprzedazy zlecenie) {
        LinkedList<ZlecenieSprzedazy> listaSprzedazy = zleceniaSprzedazy.get(zlecenie.getAkcja());

        int i = 0;

        for (ZlecenieSprzedazy z : listaSprzedazy) {
            if (z.compareTo(zlecenie) > 0)
                break;

            ++i;
        }

        listaSprzedazy.add(i, zlecenie);
    }

    /**
     * Metoda sprawdza czy wszyscy inwestorzy maja nieujemna liczbe akcji/gotowke. Tzn. sprawdzamy, czy
     * przypadkiem nie ma nigdzie liczby ujemnej (tak byc nie moze).
     */

    public boolean sprawdzCzyWszyscyMajaNieujemnaGotowkeAkcje() {
        for (Inwestor inwestor : inwestorzy) {
            if (inwestor.getGotowka() < 0)
                return false;

            for (Spolka spolka : spolki) {
                if (inwestor.getPortfel().get(spolka) < 0)
                    return false;
            }
        }

        return true;
    }

    /**
     * Metoda sprawdza niezmiennik symulacji (laczna liczba gotowki oraz poszczegolnych akcji taka sama
     * jak w pliku wejsciowym).
     */

    public boolean sprawdzNiezmiennikSymulacji() {
        if (policzIleJestCalejGotowki() != niezmiennikGotowki)
            return false;

        for (Spolka spolka : spolki) {
            if (policzIleJestDlaSpolkiAkcji(spolka) != niezmiennikAkcji.get(spolka))
                return false;
        }

        return true;
    }

    /**
     * Metoda zlicza laczna gotowke, ktora posiadaja wszyscy inwestorowie.
     */

    private int policzIleJestCalejGotowki() {
        int suma = 0;

        for (Inwestor inwestor : inwestorzy) {
            suma += inwestor.getGotowka();
        }

        return suma;
    }

    /**
     * Metoda zlicza laczna liczbe akcji danej spolki, ktore posiadaja inwestorowie.
     */

    private int policzIleJestDlaSpolkiAkcji(Spolka spolka) {
        int suma = 0;
        for (Inwestor inwestor : inwestorzy) {
            suma += inwestor.getPortfel().get(spolka);
        }
        return suma;
    }

    /**
     * Metoda dla kazdej spolki aktualizuje wartosci SMA5 oraz SMA10 (pod koniec tury).
     */

    private void zaaktualizujSMA() {
        for (Spolka spolka : spolki) {
            spolka.ustalCeneWTurze();
        }
    }

    /**
     * Metoda sprawdza czy dana spolka jest juz na liscie spolek w tej symulacji (czy jest w obrocie).
     */

    private Spolka czyAkcjaJestWObrocie(String nazwaAkcji) {
        for (Spolka spolka : spolki) {
            if (spolka.getIdentyfikator().equals(nazwaAkcji))
                return spolka;
        }

        return null;
    }

    /**
     * Metoda liczy liczbe wystapien znaku w napisie.
     */

    private static int policzLiczbeWystapienZnaku(String string, char znak) {
        int wynik = 0;

        for (int i = 0; i < string.length(); i++) {
            char c = string.charAt(i);
            if (c == znak)
                ++wynik;
        }

        return wynik;
    }
}
