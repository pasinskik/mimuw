package Terminy;

import Zlecenia.Zlecenie;

/**
 * Klasa reprezentujaca termin waznosci natychmiastowy.
 */

public class TerminNatychmiastowe extends TerminWaznosci {
    @Override
    public boolean czyZleceniePrzeterminowane(Zlecenie zlecenie, int obecnaTura) {
        return zlecenie.getTuraZlozenia() != obecnaTura;
    }

    @Override
    public boolean czyZlecenieWykonajAnuluj() {
        return false;
    }

    @Override
    public String toString() {
        return "N";
    }
}
