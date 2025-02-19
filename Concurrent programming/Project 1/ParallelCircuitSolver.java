package cp2024.solution;

import cp2024.circuit.*;
import cp2024.demo.*;

import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.atomic.AtomicBoolean;

/*
    Concurrent programming course 2024/2025
    Project 1: concurrent solver of boolean expressions given by a tree.
    Author: Kacper Pasiński
    Date: 30.11.2024
 */

public class ParallelCircuitSolver implements CircuitSolver {
    // Variables shared with ParallelCircuitValue objects created by the solver
    private final ExecutorService executor = Executors.newCachedThreadPool();
    private final AtomicBoolean acceptComputations = new AtomicBoolean(true);

    @Override
    public CircuitValue solve(Circuit c) {
        if (!acceptComputations.get()) {
            return new BrokenCircuitValue();
        }

        return new ParallelCircuitValue(c.getRoot(), executor, acceptComputations);
    }

    @Override
    public void stop() {
        acceptComputations.set(false);
        executor.shutdown();
    }
}
