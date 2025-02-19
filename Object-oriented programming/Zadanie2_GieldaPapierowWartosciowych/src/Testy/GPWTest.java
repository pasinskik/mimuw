package Testy;

import Symulacja.SystemTransakcyjny;
import org.junit.Test;
import org.junit.After;

import static org.junit.Assert.assertTrue;

public class GPWTest {

    // Test 1 - przykladowe dane z polecenia
    @Test
    public void testScenariusza1() {
        System.out.println("Test 1: ");
        SystemTransakcyjny s = new SystemTransakcyjny();
        s.inicjalizacjaSymulacji("src/Testy/scenariusz1.txt", 10000);
        s.wypiszWynikSymulacji();
        assertTrue(s.sprawdzNiezmiennikSymulacji());
        assertTrue(s.sprawdzCzyWszyscyMajaNieujemnaGotowkeAkcje());
    }

    // Test 2 - wiecej inwestorow SMA, 4 spolki
    @Test
    public void testScenariusza2() {
        System.out.println("Test 2: ");
        SystemTransakcyjny s = new SystemTransakcyjny();
        s.inicjalizacjaSymulacji("src/Testy/scenariusz2.txt", 10000);
        s.wypiszWynikSymulacji();
        assertTrue(s.sprawdzNiezmiennikSymulacji());
        assertTrue(s.sprawdzCzyWszyscyMajaNieujemnaGotowkeAkcje());
    }

    // Test 3 - sami random inwestorzy
    @Test
    public void testScenariusza3() {
        System.out.println("Test 3: ");
        SystemTransakcyjny s = new SystemTransakcyjny();
        s.inicjalizacjaSymulacji("src/Testy/scenariusz3.txt", 10000);
        s.wypiszWynikSymulacji();
        assertTrue(s.sprawdzNiezmiennikSymulacji());
        assertTrue(s.sprawdzCzyWszyscyMajaNieujemnaGotowkeAkcje());
    }

    // Test 4 - drogie spolki
    @Test
    public void testScenariusza4() {
        System.out.println("Test 4: ");
        SystemTransakcyjny s = new SystemTransakcyjny();
        s.inicjalizacjaSymulacji("src/Testy/scenariusz4.txt", 10000);
        s.wypiszWynikSymulacji();
        assertTrue(s.sprawdzNiezmiennikSymulacji());
        assertTrue(s.sprawdzCzyWszyscyMajaNieujemnaGotowkeAkcje());
    }

    // Test 5 - tanie spolki
    @Test
    public void testScenariusza5() {
        System.out.println("Test 5: ");
        SystemTransakcyjny s = new SystemTransakcyjny();
        s.inicjalizacjaSymulacji("src/Testy/scenariusz5.txt", 10000);
        s.wypiszWynikSymulacji();
        assertTrue(s.sprawdzNiezmiennikSymulacji());
        assertTrue(s.sprawdzCzyWszyscyMajaNieujemnaGotowkeAkcje());
    }

    // Test 6 - jedna spolka
    @Test
    public void testScenariusza6() {
        System.out.println("Test 6: ");
        SystemTransakcyjny s = new SystemTransakcyjny();
        s.inicjalizacjaSymulacji("src/Testy/scenariusz6.txt", 5000);
        s.wypiszWynikSymulacji();
        assertTrue(s.sprawdzNiezmiennikSymulacji());
        assertTrue(s.sprawdzCzyWszyscyMajaNieujemnaGotowkeAkcje());
    }

    // Test 7 - jeden inwestor
    @Test
    public void testScenariusza7() {
        System.out.println("Test 7: ");
        SystemTransakcyjny s = new SystemTransakcyjny();
        s.inicjalizacjaSymulacji("src/Testy/scenariusz7.txt", 5000);
        s.wypiszWynikSymulacji();
        assertTrue(s.sprawdzNiezmiennikSymulacji());
        assertTrue(s.sprawdzCzyWszyscyMajaNieujemnaGotowkeAkcje());
    }

    @After
    public void nowaLinia() {
        System.out.println();
    }

}