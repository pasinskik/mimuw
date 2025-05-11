/**
 * Program that interprets a word in a given L-System.
 *
 * The program reads from input the number of derivation steps, a description
 * of the L-System, and its interpretation rules, and outputs the interpreted
 * word resulting from the derivation in the defined language.
 *
 * Author: Kacper Pasiński
 * Date: 12.12.2023
*/

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

/* Constants defined by the assignment */

/**
 * Maximum number of characters (excluding '\n') that a single line may contain.
 * This value is specified in the project statement.
 *
 * When reading replacement rules (where '\n' doesn't matter), we assume each line
 * has at most 100 characters. When reading interpretation rules, we assume each
 * line has at most 101 characters (including the '\n' character),
 * which is further explained in the respective function comments.
 */
#define MAX_LINE_CHARS 100

/* Helper procedure for dynamic array management */

/**
 * Resizes a character array to a new specified length.
 * Allocates a new array, copies the contents of the old array,
 * frees the old one, and updates the pointer to point to the new array.
 *
 * @param array         Pointer to the pointer to the character array.
 * @param old_length    The current length of the array.
 * @param new_length    The target length to resize the array to.
 */
void resize_array(char** array, int old_length, int new_length) {
    char* new_array = (char*)calloc((size_t)new_length, sizeof(char));

    for (int i = 0; i < old_length; ++i)
        new_array[i] = (*array)[i];

    free(*array);
    *array = new_array;
}

/* Functions for reading data from input */

/**
 * Reads the first non-empty line of input containing the derivation depth.
 * The input is treated as a string of digits, which is then parsed into an integer.
 *
 * @return A non-negative integer representing the derivation depth of the word.
 */
int read_derivation_length(void) {
    char temp_buffer[MAX_LINE_CHARS];
    int i = 0;
    int c = getchar();

    while (c != '\n') {
        temp_buffer[i] = (char)c;
        ++i;
        c = getchar();
    }

    int derivation_length = 0;
    int multiplier = 1;

    while (i > 0) {
        derivation_length += multiplier * (temp_buffer[i - 1] - '0');
        multiplier *= 10;
        --i;
    }

    return derivation_length;
}

/**
 * Reads the axiom (initial string) from the second line of input.
 * Skips the newline character and stores the characters in a fixed-size array.
 *
 * @param axiom             Pointer to the array storing the axiom characters.
 * @param axiom_length      Pointer to an integer where the axiom length will be stored.
 */
void read_axiom(char* axiom, int* axiom_length) {
    int i = 0;
    int c = getchar();

    while (c != '\n') {
        axiom[i] = (char)c;
        ++i;
        c = getchar();
    }

    *axiom_length = i;
}

/**
 * Reads substitution rules from input, one per line (excluding '\n').
 * Stops reading when it encounters an empty line (i.e., only '\n'), 
 * which indicates the beginning of the interpretation section.
 *
 * @param substitution_rules     A 2D array holding substitution rules.
 *                               Each row starts with the symbol to replace,
 *                               followed by the replacement string.
 *
 * @param rule_lengths           An array holding the length of each substitution rule.
 *
 * @param num_rules              Pointer to an integer where the number of rules will be stored.
 */
void read_substitution_rules(char substitution_rules[][MAX_LINE_CHARS],
                            int rule_lengths[], int* num_rules) {
    int row = 0;
    int col = 0;
    int c = getchar();

    while (row < CHAR_MAX) {
        while (c != '\n') {
            substitution_rules[row][col] = (char) c;
            ++col;
            c = getchar();
        }

        if (col != 0) {
            rule_lengths[row] = col;
            ++row;
        } else {
            break;  // Empty line encountered
        }

        col = 0;
        c = getchar();
    }

    *num_rules = row;
}

/**
 * Finds the maximum length among all substitution rules.
 * If no rules exist, returns 1 (used as a safe default for allocation).
 *
 * @param rule_lengths     An array of rule lengths.
 * @param num_rules        Number of substitution rules present.
 *
 * @return The maximum rule length, or 1 if there are no rules.
 */
int max_substitution_rule_length(int rule_lengths[], int num_rules) {
    int max = 1;

    for (int i = 0; i < num_rules; ++i) {
        if (rule_lengths[i] > max)
            max = rule_lengths[i];
    }

    return max;
}

/**
 * Rewrites the axiom string by applying substitution rules repeatedly.
 * The word is rewritten as many times as specified by derivation_depth.
 * 
 * For each character in the word:
 * - If a substitution rule exists, replace it with its corresponding rule.
 * - If not, copy the character unchanged.
 *
 * @param derivation_depth        Number of times to apply substitution rules.
 * @param word_length             Pointer to current word length.
 * @param word                    Pointer to the current word (modified in-place).
 * @param num_rules               Number of substitution rules.
 * @param rule_lengths            Lengths of substitution rules.
 * @param substitution_rules      The array of substitution rules.
 */
void apply_substitution_rules(int derivation_depth, int* word_length, char** word,
                              int num_rules, int rule_lengths[],
                              char substitution_rules[][MAX_LINE_CHARS]) {

    int max_rule_len = max_substitution_rule_length(rule_lengths, num_rules);

    for (int step = 0; step < derivation_depth; ++step) {
        int max_word_len = (*word_length) * max_rule_len;
        char* new_word = (char*)calloc((size_t)max_word_len, sizeof(char));
        int old_index = 0;
        int new_index = 0;

        while (old_index < *word_length) {
            char current_char = (*word)[old_index];
            int matched_rule = 0;

            for (int k = 0; k < num_rules; ++k) {
                if (current_char == substitution_rules[k][0]) {
                    for (int l = 1; l < rule_lengths[k]; ++l) {
                        new_word[new_index++] = substitution_rules[k][l];
                    }
                    matched_rule = 1;
                    break;
                }
            }

            if (!matched_rule) {
                new_word[new_index++] = current_char;
            }

            ++old_index;
        }

        *word_length = new_index;
        free(*word);
        *word = new_word;
    }
}

/**
 * Reads either the prologue or epilogue from input.
 * Reading continues until two consecutive newline characters are found.
 * Since the length of the input is unknown and potentially large,
 * the function resizes the array dynamically using `zmien_rozmiar_tablicy`
 * when more space is needed. At the end, the total number of characters read
 * (including newlines) is stored.
 *
 * If the log was not empty, a final newline is added (which might have been skipped).
 *
 * @param arr              Pointer to the character array pointer (prologue or epilogue).
 * @param log_length       Pointer to an integer where the number of characters read will be stored.
 * @param array_capacity   Initial size of the array (will be resized as needed).
 */
void read_log(char** arr, int* log_length, int array_capacity) {
    int i = 0;
    int c = getchar();

    while (c != EOF) {
        if (i == array_capacity) {
            zmien_rozmiar_tablicy(arr, array_capacity, 2 * array_capacity);
            array_capacity *= 2;
        }

        if (c == '\n' && i != 0) {
            c = getchar();
            if (c == '\n' || c == EOF)
                break;
            else {
                (*arr)[i++] = '\n';
            }
        }
        else if (c == '\n' && i == 0) {
            break;
        }
        else {
            (*arr)[i++] = (char)c;
            c = getchar();
        }
    }

    if (i != 0) {
        if (i == array_capacity)
            zmien_rozmiar_tablicy(arr, array_capacity, 2 * array_capacity);
        (*arr)[i++] = '\n';
    }

    *log_length = i;
}

/**
 * Reads interpretation rules from input, line by line, including newline characters.
 * Each rule maps a symbol to a text line to be printed during interpretation.
 * Stops reading when it encounters a blank line (i.e., a line with only '\n').
 *
 * @param interpretation_rules     2D array storing each interpretation rule.
 *                                 Each row starts with a symbol followed by its interpretation text.
 *
 * @param rule_lengths             Array storing the length of each rule (including newline).
 *
 * @param num_rules                Pointer to an integer where the number of rules will be stored.
 */
void read_interpretation_rules(char interpretation_rules[][MAX_LINE_CHARS + 1],
                               int rule_lengths[], int* num_rules) {
    int row = 0;
    int col = 0;
    int c = getchar();

    while (row < CHAR_MAX) {
        while (c != '\n') {
            interpretation_rules[row][col++] = (char)c;
            c = getchar();
        }

        interpretation_rules[row][col++] = (char)c;  // include the newline

        if (col != 1) {
            rule_lengths[row] = col;
            ++row;
        } else {
            break;  // blank line encountered
        }

        col = 0;
        c = getchar();
    }

    *num_rules = row;
}

/* Function that prints the final world */

/**
 * Outputs the final interpreted word using the interpretation rules.
 * For each character in the generated word:
 * - If there is a matching interpretation rule, prints the mapped string.
 * - If not, the character is skipped.
 *
 * @param word_length               Pointer to the length of the word.
 * @param word                      Pointer to the interpreted word array.
 * @param num_rules                 Number of interpretation rules.
 * @param rule_lengths              Array of interpretation rule lengths.
 * @param interpretation_rules      2D array of interpretation rules.
 */
void write_final_word(int* word_length, char** word,
                      int num_rules, int rule_lengths[],
                      char interpretation_rules[][MAX_LINE_CHARS + 1]) {
    int i = 0;

    while (i < *word_length) {
        char current_char = (*word)[i];

        for (int j = 0; j < num_rules; ++j) {
            if (current_char == interpretation_rules[j][0]) {
                for (int k = 1; k < rule_lengths[j]; ++k) {
                    printf("%c", interpretation_rules[j][k]);
                }
                break;
            }
        }

        ++i;
    }
}

int main() {
    // Read the derivation length
    int derivation_length = read_derivation_length();

    // Read the axiom
    int word_length;
    char* word = (char*)calloc(MAX_LINE_CHARS, sizeof(char));
    read_axiom(word, &word_length);

    // Read replacement rules
    int num_replacement_rules;
    int replacement_rule_lengths[CHAR_MAX];
    char replacement_rules[CHAR_MAX][MAX_LINE_CHARS];
    read_replacement_rules(replacement_rules, replacement_rule_lengths, &num_replacement_rules);

    // Apply the rules over the derivation length
    apply_replacement_rules(derivation_length, &word_length, &word,
                            num_replacement_rules, replacement_rule_lengths, replacement_rules);

    // Read prologue
    int prologue_length;
    char* prologue = (char*)calloc(MAX_LINE_CHARS, sizeof(char));
    read_text_block(&prologue, &prologue_length, MAX_LINE_CHARS);

    // Read interpretation rules
    int num_interpretation_rules;
    int interpretation_rule_lengths[CHAR_MAX];
    char interpretation_rules[CHAR_MAX][MAX_LINE_CHARS + 1];
    read_interpretation_rules(interpretation_rules, interpretation_rule_lengths, &num_interpretation_rules);

    // Read epilogue
    int epilogue_length;
    char* epilogue = (char*)calloc(MAX_LINE_CHARS, sizeof(char));
    read_text_block(&epilogue, &epilogue_length, MAX_LINE_CHARS);

    // Print prologue
    for (int i = 0; i < prologue_length; ++i)
        printf("%c", prologue[i]);

    // Print the interpreted final word
    write_interpreted_word(&word_length, &word, num_interpretation_rules,
                           interpretation_rule_lengths, interpretation_rules);

    // Print epilogue
    for (int i = 0; i < epilogue_length; ++i)
        printf("%c", epilogue[i]);

    // Clean up
    free(word);
    free(prologue);
    free(epilogue);

    return 0;
}