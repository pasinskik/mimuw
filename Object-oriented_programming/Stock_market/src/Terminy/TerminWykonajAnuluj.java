package Terminy;

import Zlecenia.Zlecenie;

/**
 * Klasa reprezentujaca termin waznosci wykonaj albo anuluj. Jest to w sumie termin natychmiastowy,
 * ktory dodatkowo musi byc zrealizowany w calosci albo wcale.
 */

public class TerminWykonajAnuluj extends TerminWaznosci {
    @Override
    public boolean czyZleceniePrzeterminowane(Zlecenie zlecenie, int obecnaTura) {
        return zlecenie.getTuraZlozenia() != obecnaTura;
    }

    @Override
    public boolean czyZlecenieWykonajAnuluj() {
        return true;
    }

    @Override
    public String toString() {
        return "WA";
    }
}
