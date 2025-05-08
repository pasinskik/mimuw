/**
 * "Project 1"
 *
 * This project implements a dynamically loaded C library
 * for modeling boolean circuits made of NAND gates.
 *
 * author:   Kacper Pasiński
 * date:    21/04/2024
 */

 #include <stdio.h>
 #include <stdbool.h>
 #include <stdlib.h>
 #include <errno.h>
 #include <sys/types.h>
 
 /* Define structures needed for the task. */
 
 typedef struct nand nand_t;
 
 /**
  * Structure storing the source of an input signal. If nothing is connected,
  * nand_source points to NULL and is_bool is false.
  *
  * @param   is_bool                 Indicates whether the signal is a boolean.
  * @param   bool_source             Pointer to the boolean signal connected to the input (or NULL).
  * @param   nand_source             Pointer to the gate connected to this input (or NULL).
  * @param   output_gates_position   Position of this gate in the output array of the nand_source.
  *
  */
 
 typedef struct input_source {
     bool is_bool;
     bool *bool_source;
     nand_t *nand_source;
     ssize_t output_gates_position;
 } input_source_t;
 
 /**
  * Structure storing information about the gate connected to the output of a given gate.
  *
  * @param   output_gate         Pointer to the gate connected to this gate's output.
  * @param   inputs_position     Position of this gate in the input array of output_gate.
  *
  */
 
 typedef struct output_gate {
     nand_t *output_gate;
     unsigned inputs_position;
 } output_gate_t;
 
 /**
  * Structure representing a NAND gate.
  *
  * @param   inputs_number           Number of inputs to the gate.
  * @param   inputs                  Array of gate inputs.
  * @param   output_gates_number     Number of gates connected to this gate's output.
  * @param   output_gates            Array of gates connected to this gate's output.
  * @param   visited                 Flag indicating if the gate has been visited.
  * @param   calculated              Flag indicating if the gate's output and critical path are calculated.
  * @param   output                  Output signal of the gate.
  * @param   critical_path           Critical path length of the gate.
  *
  */
 
 typedef struct nand {
     unsigned inputs_number;
     input_source_t *inputs;
     ssize_t output_gates_number;
     output_gate_t *output_gates;
     bool visited;
     bool calculated;
     bool output;
     ssize_t critical_path;
 } nand_t;
 
 /* Basic operations on NAND gates. */
 
 /**
  * Initializes a NAND gate with n inputs and sets all parameters to 0/false/NULL.
  *
  * @param n     Number of inputs to initialize the gate with.
  *
  * @return      Pointer to an initialized gate.
  *
 */
 
 nand_t *nand_new(unsigned n) {
     nand_t *gate = malloc(sizeof(nand_t));
     if (gate == NULL) {
         errno = ENOMEM;
         return NULL;
     }
 
     gate->inputs_number = n;
     gate->output_gates_number = 0;
     gate->output_gates = NULL;
     gate->output = false;
     gate->visited = false;
     gate->calculated = false;
     gate->critical_path = 0;
 
     if (n == 0) {
         gate->inputs = NULL;
         return gate;
     }
 
     gate->inputs = malloc(n * sizeof(input_source_t));
     if (gate->inputs == NULL) {
         free(gate);
         errno = ENOMEM;
         return NULL;
     }
 
     for (unsigned i = 0; i < n; ++i) {
         gate->inputs[i].is_bool = false;
         gate->inputs[i].nand_source = NULL;
         gate->inputs[i].bool_source = NULL;
         gate->inputs[i].output_gates_position = 0;
     }
 
     return gate;
 }
 
 /**
  * Disconnects the k-th input of the gate (after this call,
  * nothing is connected to that input).
  *
  * @param g     Pointer to the gate.
  * @param k     Index of the input to disconnect.
  *
  * @return      0 on success, -1 on failure (e.g., invalid input or gate).
  *
 */
 
 int nand_disconnect_an_input(nand_t *g, unsigned k) {
     if (g == NULL || g->inputs_number <= k) {
         errno = EINVAL;
         return -1;
     }
 
     g->inputs[k].is_bool = false;
     g->inputs[k].nand_source = NULL;
     g->inputs[k].bool_source = NULL;
 
     return 0;
 }
 
 /**
  * Disconnects the k-th output connection of the gate.
  *
  * @param g     Pointer to the gate.
  * @param k     Index of the output connection to disconnect.
  *
  * @return      0 on success, -1 on error (e.g., invalid index).
  *
 */
 
 int nand_disconnect_an_output_gate(nand_t *g, unsigned k) {
     if (g == NULL || g->output_gates_number <= k) {
         errno = EINVAL;
         return -1;
     }
 
     --g->output_gates_number;
 
     // If there was only one output, it is necessary to free the memory.
     if (g->output_gates_number == 0) {
         free(g->output_gates);
         g->output_gates = NULL;
         return 0;
     }
 
     // If the output wasn't the last in the array, it is necessary to fix indexing.
     if (k != g->output_gates_number) {
         g->output_gates[k] = g->output_gates[g->output_gates_number];
         g->output_gates[k].output_gate
         ->inputs[g->output_gates[k].inputs_position].output_gates_position = k;
     }
 
     g->output_gates = realloc(g->output_gates,
                               g->output_gates_number * sizeof(output_gate_t));
     if (g->output_gates == NULL) {
         errno = ENOMEM;
         return -1;
     }
 
     return 0;
 }
 
 /**
  * Disconnects all input and output connections of the gate, then frees its memory.
  * For each input, it disconnects the gate from its source's outputs.
  * For each output, it disconnects the gate from its destination inputs.
  * After this function, the pointer becomes NULL.
  *
  * @param g     Pointer to the gate to delete.
 */
 
 void nand_delete(nand_t *g) {
     if (g == NULL) {
         return;
     }
 
     for (unsigned i = 0; i < g->inputs_number; ++i) {
         if (!g->inputs[i].is_bool && g->inputs[i].nand_source != NULL) {
             if (nand_disconnect_an_output_gate(g->inputs[i].nand_source,
                                                g->inputs[i].output_gates_position) == -1) {
                 errno = ECANCELED;
                 return;
             }
         }
     }
 
     for (ssize_t i = 0; i < g->output_gates_number; ++i) {
         if (g->output_gates[i].output_gate != NULL) {
             if (nand_disconnect_an_input(g->output_gates[i].output_gate,
                                          g->output_gates[i].inputs_position) == -1) {
                 errno = ECANCELED;
                 return;
             }
         }
     }
 
     free(g->inputs);
     free(g->output_gates);
 
     free(g);
 }
 
 /* Functions that connects signals to inputs of a NAND gate. */
 
 /**
  * Connects the output of g_out to the k-th input of g_in.
  * If an input was already connected, it will be disconnected first.
  * This involves modifying g_in's input array and updating g_out's output_gates array.
  *
  * @param g_out     Pointer to the output gate (source).
  * @param g_in      Pointer to the input gate (destination).
  * @param k         Index of the input on g_in to connect to g_out.
  *
  * @return          0 on success, -1 on error (e.g., invalid index or NULL pointers).
  *
 */
 
 int nand_connect_nand(nand_t *g_out, nand_t *g_in, unsigned k) {
     if (g_out == NULL || g_in == NULL || g_in->inputs_number <= k) {
         errno = EINVAL;
         return -1;
     }
 
     if (g_in->inputs[k].is_bool == false) {
         nand_disconnect_an_output_gate(g_in->inputs[k].nand_source,
                                        g_in->inputs[k].output_gates_position);
     }
     nand_disconnect_an_input(g_in, k);
 
     g_in->inputs[k].nand_source = g_out;
     g_in->inputs[k].output_gates_position = g_out->output_gates_number;
 
     ++g_out->output_gates_number;
     g_out->output_gates =
             realloc(g_out->output_gates,
                     g_out->output_gates_number * sizeof(output_gate_t));
     if (g_out->output_gates == NULL) {
         errno = ENOMEM;
         return -1;
     }
     g_out->output_gates[g_out->output_gates_number - 1].output_gate = g_in;
     g_out->output_gates[g_out->output_gates_number - 1].inputs_position = k;
 
     return 0;
 }
 
 /**
  * Connects a boolean signal to the k-th input of gate g.
  * Disconnects any previous connection. The const is deliberately cast away.
  *
  * @param s         Pointer to the boolean signal.
  * @param g         Pointer to the gate whose input is being set.
  * @param k         Index of the input to assign.
  *
  * @return          0 on success, -1 on error.
  *
 */
 
 int nand_connect_signal(bool const *s, nand_t *g, unsigned k) {
     if (g == NULL || s == NULL || k >= g->inputs_number) {
         errno = EINVAL;
         return -1;
     }
 
     if (g->inputs[k].is_bool == false) {
         nand_disconnect_an_output_gate(g->inputs[k].nand_source,
                                        g->inputs[k].output_gates_position);
         nand_disconnect_an_input(g, k);
         g->inputs[k].is_bool = true;
     }
 
     g->inputs[k].bool_source = (bool *) s;
 
     return 0;
 }
 
 /* Functions necessary to perform nand_evaluate. */
 
 /**
  * Resets the 'calculated' flag to false for the given gate and all of its inputs recursively.
  * This is needed after nand_evaluate, since outputs and critical paths may no longer be valid.
  *
  * @param gate     Pointer to the gate.
 */
 
 void remove_calculated(nand_t *gate) {
     if (gate == NULL) {
         return;
     }
 
     gate->calculated = false;
 
     for (unsigned i = 0; i < gate->inputs_number; ++i) {
         if (!gate->inputs[i].is_bool)
             remove_calculated(gate->inputs[i].nand_source);
     }
 
 }
 
 /**
  * Checks for cycles in the input graph of a gate using DFS.
  * Returns true if a cycle is detected, otherwise false.
  *
  * @param gate      Pointer to the gate.
  *
  * @return          true if a cycle is detected; false otherwise.
  *
 */
 
 bool has_cycle(nand_t *gate) {
     if (gate == NULL || gate->visited) {
         return true;
     }
 
     gate->visited = true;
 
     for (unsigned i = 0; i < gate->inputs_number; ++i) {
         if (!gate->inputs[i].is_bool && has_cycle(gate->inputs[i].nand_source)) {
             gate->visited = false;
             return true;
         }
     }
 
     gate->visited = false;
 
     return false;
 }
 
 /**
  * Calculates the output and critical path length of the gate (unless it has already been calculated).
  * Returns -1 on failure or the critical path length on success.
  *
  * @param gate      Pointer to the gate to evaluate.
  *
  * @return          Critical path length on success; -1 on failure.
  *
 */
 
 ssize_t calculate_output_and_critical_path(nand_t *gate) {
 
     if (gate == NULL) {
         errno = EINVAL;
         return -1;
     }
 
     if (gate->calculated)
         return gate->critical_path;
 
     ssize_t max_critical_path = 0;
     ssize_t temp_critical_path;
 
     gate->output = false;
 
     if (gate->inputs_number == 0) {
         return 0;
     }
 
     for (unsigned i = 0; i < gate->inputs_number; ++i) {
         temp_critical_path = 0;
 
         if (gate->inputs[i].is_bool){
             if (!*(gate->inputs[i].bool_source))
                 gate->output = true;
         }
         else {
             temp_critical_path =
                     calculate_output_and_critical_path(gate->inputs[i].nand_source);
 
             if (temp_critical_path == -1) {
                 errno = ECANCELED;
                 return -1;
             }
 
             if (temp_critical_path > max_critical_path)
                 max_critical_path = temp_critical_path;
 
             if (!gate->inputs[i].nand_source->output)
                 gate->output = true;
         }
 
     }
 
     gate->critical_path = 1 + max_critical_path;
     gate->calculated = true;
 
     return gate->critical_path;
 }
 
 /**
  * Evaluates the outputs of all gates and calculates the critical path of the entire circuit.
  * Uses memoization to avoid recalculating outputs.
  * Returns -1 if the circuit contains a cycle or cannot be evaluated.
  *
  * @param gate      Array of pointers to NAND gates.
  * @param s         Output array: each entry stores the output signal for the corresponding gate.
  * @param m         Number of gates in the array.
  *
  * @return          Length of the critical path, or -1 if evaluation fails (e.g., due to cycles).
 */
 
 ssize_t nand_evaluate(nand_t **g, bool *s, size_t m) {
 
     ssize_t max_critical_path = 0;
     ssize_t temp_critical_path;
 
     if (g == NULL || s == NULL || m <= 0){
         errno = EINVAL;
         return -1;
     }
 
     for (unsigned i = 0; i < m; ++i) {
         if (g[i] == NULL) {
             errno = EINVAL;
             return -1;
         }
     }
 
 
     for (unsigned i = 0; i < m; ++i) {
 
         if (has_cycle(g[i])) {
             errno = ECANCELED;
             return -1;
         }
 
         temp_critical_path = calculate_output_and_critical_path(g[i]);
         if (temp_critical_path == -1){
             errno = ECANCELED;
             return -1;
         }
         else {
             if (temp_critical_path > max_critical_path)
                 max_critical_path = temp_critical_path;
         }
         s[i] = g[i]->output;
     }
 
     for (unsigned i = 0; i < m; ++i) {
         remove_calculated(g[i]);
     }
 
     return max_critical_path;
 }
 
 /* Functions to iterate over inputs/outputs of a NAND gate. */
 
 /**
  * Returns the number of inputs of other gates connected to the output of gate g.
  * Returns -1 if the gate pointer is NULL.
  *
  * @param g     Pointer to the gate.
  *
  * @return      Number of outputs connected to this gate, or -1 on error.
 */
 
 ssize_t nand_fan_out(nand_t const *g) {
 
     if (g == NULL) {
         errno = EINVAL;
         return -1;
     }
 
     return g->output_gates_number;
 }
 
 /**
  * Returns a pointer to the boolean signal or gate connected to the k-th input of gate g.
  * Returns NULL if no input is connected or parameters are invalid.
  *
  * @param g     Pointer to the gate.
  * @param k     Index of the input to access.
  *
  * @return      Pointer to the signal or gate connected to input k, or NULL if
  *              none is connected or parameters are invalid.
 */
 
 void* nand_input(nand_t const *g, unsigned k) {
     if (g == NULL || g->inputs_number <= k) {
         errno = EINVAL;
         return NULL;
     }
     if (g->inputs[k].is_bool)
         return g->inputs[k].bool_source;
     else if (g->inputs[k].nand_source == NULL) {
         errno = 0;
         return NULL;
     }
     return g->inputs[k].nand_source;
 }
 
 /**
  * Returns a pointer to the k-th gate connected to the output of gate g.
  * Allows iteration over the output_gates array.
  *
  * @param g     Pointer to the gate.
  * @param k     Index of the output gate to access.
  *
  * @return      Pointer to the output gate at position k, or NULL on error.
 */
 
 nand_t* nand_output(nand_t const *g, ssize_t k) {
     if (g == NULL || g->output_gates_number <= k) {
         errno = EINVAL;
         return NULL;
     }
     return g->output_gates[k].output_gate;
 }