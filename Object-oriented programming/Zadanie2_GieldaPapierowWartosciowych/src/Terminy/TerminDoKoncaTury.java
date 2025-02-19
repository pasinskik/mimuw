package Terminy;

import Zlecenia.Zlecenie;

/**
 * Klasa reprezentujaca termin waznosci do konca danej tury.
 */

public class TerminDoKoncaTury extends TerminWaznosci {
    private final int turaKoncowa;

    public TerminDoKoncaTury(int endTurn) {
        this.turaKoncowa = endTurn;
    }

    @Override
    public boolean czyZleceniePrzeterminowane(Zlecenie zlecenie, int obecnaTura) {
        return obecnaTura > turaKoncowa;
    }

    @Override
    public boolean czyZlecenieWykonajAnuluj() {
        return false;
    }

    @Override
    public String toString() {
        return "DT" + turaKoncowa;
    }
}
