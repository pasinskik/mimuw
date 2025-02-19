package Terminy;

import Zlecenia.Zlecenie;

/**
 * Klasa reprezentujaca termin "bez waznosci".
 */

public class TerminBezWaznosci extends TerminWaznosci {
    @Override
    public boolean czyZleceniePrzeterminowane(Zlecenie zlecenie, int obecnaTura) {
        return false;
    }

    @Override
    public boolean czyZlecenieWykonajAnuluj() {
        return false;
    }

    @Override
    public String toString() {
        return "BW";
    }
}
