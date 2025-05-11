package Terminy;

import Zlecenia.Zlecenie;

/**
 * Klasa reprezentujaca termin waznosci zlecenia. Posiada jedna metode, pozwalajaca sprawdzac, czy zlecenie
 * jest juz przeterminowane i trzeba je usunac.
 */

public abstract class TerminWaznosci {
    abstract public boolean czyZleceniePrzeterminowane(Zlecenie zlecenie, int obecnaTura);
    abstract public boolean czyZlecenieWykonajAnuluj();
}

