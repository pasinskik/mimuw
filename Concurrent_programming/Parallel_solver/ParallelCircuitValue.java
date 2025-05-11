package cp2024.solution;

import cp2024.circuit.*;

import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.*;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.function.Predicate;

public class ParallelCircuitValue implements CircuitValue {
    private final Future<Boolean> computation;
    private final ExecutorService executor;
    private final AtomicBoolean acceptComputations;

    @Override
    public boolean getValue() throws InterruptedException {
        try {
            return computation.get();
        } catch (InterruptedException | ExecutionException e) {
            throw new InterruptedException("Computation interrupted.");
        }
    }

    public ParallelCircuitValue(CircuitNode root, ExecutorService executor,
                                AtomicBoolean acceptComputations) {
        this.acceptComputations = acceptComputations;
        this.executor = executor;
        computation = executor.submit(() -> evaluateNode(root));
    }

    /*
    The method for computing the value of a CircuitNode is initially called for
    the root and then for subsequent nodes (though usually not by the same thread).
    */
    private boolean evaluateNode(CircuitNode n) throws InterruptedException {
        if (!acceptComputations.get()) {
            throw new InterruptedException("Computation interrupted.");
        }

        if (n.getType() == NodeType.LEAF) {
            return ((LeafNode) n).getValue(); // Thrown exception is propagated further.
        }

        CircuitNode[] args = n.getArgs(); // Thrown exception is propagated further.

        return switch (n.getType()) {
            case IF -> evaluateIF(args);
            case AND -> evaluateAND(args);
            case OR -> evaluateOR(args);
            case GT -> evaluateGT(args, ((ThresholdNode) n).getThreshold());
            case LT -> evaluateLT(args, ((ThresholdNode) n).getThreshold());
            case NOT -> evaluateNOT(args);
            default -> throw new RuntimeException("Illegal type " + n.getType() + ".");
        };
    }

    /*
    The evaluation of a NOT expression.
    */
    private boolean evaluateNOT(CircuitNode[] args) throws InterruptedException {
        if (args.length > 1) {
            throw new InterruptedException("Incorrect data. Computation interrupted.");
        }

        return !evaluateNode(args[0]);
    }

    /*
    The evaluation of an IF expression will be performed concurrently to allow for 
    potentially early termination. Unfortunately, computing the condition first is 
    not always the best approach. For example, if the second and third arguments 
    are the same, the condition’s value no longer matters.
    */
    private boolean evaluateIF(CircuitNode[] args) throws InterruptedException {
        if (args.length != 3) {
            throw new InterruptedException("Incorrect data. Computation interrupted.");
        }

        CompletionService<Boolean> completionService = new ExecutorCompletionService<>(executor);
        Future<Boolean> conditionFuture = completionService.submit(() -> evaluateNode(args[0]));
        Future<Boolean> trueBranchFuture = completionService.submit(() -> evaluateNode(args[1]));
        Future<Boolean> falseBranchFuture = completionService.submit(() -> evaluateNode(args[2]));

        try {
            // We want to terminate evaluation based on whichever values compute the fastest.
            for (int i = 0; i < 3; i++) {
                Future<Boolean> completed = completionService.take();
                // If the condition has already been computed, we can cancel the evaluation of one of the branches.
                if (completed == conditionFuture) {
                    boolean conditionResult = conditionFuture.get();
                    if (conditionResult) {
                        falseBranchFuture.cancel(true);
                        return trueBranchFuture.get();
                    } else {
                        trueBranchFuture.cancel(true);
                        return falseBranchFuture.get();
                    }
                } else {
                    // Either trueBranchFuture or falseBranchFuture has been computed.
                    boolean branchResult = completed.get();
                    if (trueBranchFuture.isDone() && falseBranchFuture.isDone()) {
                        // If both have been computed (but the condition has not yet been),
                        // and they are equal, we can cancel the condition evaluation
                        // since it will not affect the final result of the IF expression.
                        if (branchResult == falseBranchFuture.get()) {
                            conditionFuture.cancel(true);
                            return branchResult;
                        }
                    }
                }
            }
        } catch (CancellationException | ExecutionException e) {
            throw new InterruptedException("Computation interrupted.");
        }
        return false;  // Default value (though it should never be reached).
    }

    /*
    The methods computing the values of AND, OR, LT, and GT work in a very similar way.
    Each of these expressions has a known default value - e.g., for AND, it is true,
    and the computation involves checking whether any argument is false.
    In each of these expressions, we sequentially check the values of the arguments.
    With each argument, we verify whether the "termination condition" 
    (for instance, for AND, the condition is finding a false value) has been met.
    If, after evaluating all arguments, the condition has not been met, 
    we return the default value. However, if at any point the condition is met, 
    the expression does not retain its default value, and we return the opposite value.
    */
    private boolean evaluateWithLogic(CircuitNode[] args, Predicate<Boolean> stopCondition,
                                      boolean defaultValue) throws InterruptedException {
        // The computation of argument values (child nodes) is performed concurrently 
        // (a new thread for each child node).
        CompletionService<Boolean> completionService = new ExecutorCompletionService<>(executor);
        List<Future<Boolean>> futures = new ArrayList<>();

        for (CircuitNode childNode : args) {
            futures.add(completionService.submit(() -> evaluateNode(childNode)));
        }

        try {
            for (int i = 0; i < args.length; i++) {
                // CompletionService is used to prioritize checking the arguments that 
                // are computed the fastest. This allows us to quickly cancel the remaining 
                // computations if the termination condition has been met.
                Future<Boolean> completedFuture = completionService.take();
                boolean result = completedFuture.get();

                // If the termination condition is true, all remaining computations are canceled.
                if (stopCondition.test(result)) {
                    futures.forEach(future -> future.cancel(true));
                    return !defaultValue;
                }
            }
        } catch (CancellationException | ExecutionException e) {
            throw new InterruptedException("Computation interrupted.");
        }

        return defaultValue;
    }

    /*
    For AND, the default value is true, and the termination condition is finding a false.
    */
    private boolean evaluateAND(CircuitNode[] args) throws InterruptedException {
        return evaluateWithLogic(args, result -> !result, true);
    }

    /*
    For OR, it is the opposite - the default value is false, and the termination 
    condition is finding a true.
    */
    private boolean evaluateOR(CircuitNode[] args) throws InterruptedException {
        return evaluateWithLogic(args, result -> result, false);
    }

    /*
    For GT (greather than), the default value is false, unless more than a threshold
    number of true values are found. In this method, I used an atomic variable to count
    the number of true values.
    */
    private boolean evaluateGT(CircuitNode[] args, int threshold) throws InterruptedException {
        AtomicInteger trueCount = new AtomicInteger(0);

        return evaluateWithLogic(args, result -> {
            if (result) trueCount.incrementAndGet();
            return trueCount.get() > threshold;
        }, false);
    }

    /*
    For LT (less than), the default value is true, unless a threshold number of true values
    are found. In this method, I used an atomic variable to count the number of true values.
    */
    private boolean evaluateLT(CircuitNode[] args, int threshold) throws InterruptedException {
        AtomicInteger trueCount = new AtomicInteger(0);

        return evaluateWithLogic(args, result -> {
            if (result) trueCount.incrementAndGet();
            return trueCount.get() >= threshold;
        }, true);
    }

}
