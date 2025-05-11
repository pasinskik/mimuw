/**
 * A program that stores the values of 26 variables, prints them,
 * and executes commands written in the Pętlik language based on input data.
 *
 * The program reads one line at a time from the input and interprets it,
 * continuing until the end of input is reached.
 *
 * Author: Kacper Pasiński
 * Date: 03.01.2024
*/

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

/* Definition of constants provided in the task */

/**
 * Number of variables accessible to the program (from 'a' to 'z')
 */
#define NUM_VARIABLES 26

/**
 * Character indicating a command to print a variable's value
 */
#define PRINT_COMMAND '='

/**
 * Character marking the beginning of a "Repeat" instruction in the Pętlik language
 */
#define REPEAT_START '('

/**
 * Character marking the end of a "Repeat" instruction in the Pętlik language
 */
#define REPEAT_END ')'

/**
 * Numeric value corresponding to the first lowercase letter ('a')
 */
#define FIRST_VARIABLE_CHAR (int)'a'

/**
 * Numeric value corresponding to digit zero
 */
#define INT_OF_CHAR_0 (int)'0'

/**
 * Forward declaration of the repeat instruction handler
 */
void execute_repeat(char* line, int start, int end, int nested_count, char* variables[], int variable_lengths[]);

/* Helper functions and procedures for handling variables */

/**
 * Converts a character variable ('a' to 'z') to its index (0 to 25)
 * based on its ASCII value.
 *
 * @param variable Character between 'a' and 'z'
 * @return         Index of the variable in the variable table
 */
int variable_to_index(char variable) {
    return (int)variable - FIRST_VARIABLE_CHAR;
}

/**
 * Prints the value of a variable (represented as a character array of digits).
 *
 * @param variable        Pointer to the array of digit characters
 * @param variable_length Number of digits in the variable's value
 */
void print_variable(char* variable, int variable_length) {
    for (int i = 0; i < variable_length; ++i) {
        printf("%d", variable[i] - '0');
    }
    printf("\n");
}


/**
 * Converts a digit (int) to its character representation.
 *
 * @param x     An integer digit to convert to a character
 * @return      Character representing the given digit
 */
char int_to_char(int x) {
    return (char)(INT_OF_CHAR_0 + x);
}

/* Procedures corresponding to machine instructions */

/**
 * Increments the value of a variable (stored as a char array).
 * This is standard increment logic for digit arrays. A special case
 * is when the value consists only of '9's—in that case, a new array
 * is allocated with one additional digit.
 *
 * @param variable        Pointer to a pointer to the array of digit characters
 *                        representing the variable's value
 * @param variable_length Pointer to the number of digits in the variable
 */
void INC(char** variable, int* variable_length) {
    for (int i = *variable_length - 1; i >= 0; --i) {
        if ((*variable)[i] == '9') {
            (*variable)[i] = '0';
        } else {
            (*variable)[i]++;
            break;
        }
    }

    if ((*variable)[0] == '0') {
        ++(*variable_length);
        char* new_variable = (char*) calloc((size_t)*variable_length, sizeof(char));
        new_variable[0] = '1';
        for (int i = 1; i < *variable_length; ++i) {
            new_variable[i] = '0';
        }
        free(*variable);
        *variable = new_variable;
    }
}

/**
 * Adds the value of variable1 to variable0 (both stored as char arrays).
 * The result is written into variable0. Digits are added in reverse and
 * then reversed back at the end. If the last digit is '0', it's trimmed.
 *
 * @param variable0        Destination variable (will be updated)
 * @param length0          Pointer to the number of digits in variable0
 * @param variable1        Source variable to add
 * @param length1          Pointer to the number of digits in variable1
 */
void ADD(char** variable0, int* length0, char** variable1, int* length1) {
    int result_len = (*length0 > *length1 ? *length0 : *length1) + 1;
    char* result = (char*) calloc((size_t)result_len, sizeof(char));

    int carry = 0;
    int i = 0;
    while (i < result_len) {
        int digit0 = (i < *length0) ? (*variable0)[*length0 - 1 - i] - '0' : 0;
        int digit1 = (i < *length1) ? (*variable1)[*length1 - 1 - i] - '0' : 0;

        int sum = digit0 + digit1 + carry;
        result[i] = int_to_char(sum % 10);
        carry = sum / 10;
        ++i;
    }

    if (result[result_len - 1] == '0') {
        --result_len;
    }

    for (int j = 0; j < result_len / 2; ++j) {
        char temp = result[j];
        result[j] = result[result_len - 1 - j];
        result[result_len - 1 - j] = temp;
    }

    free(*variable0);
    *variable0 = result;
    *length0 = result_len;
}

/**
 * Clears the value of a variable by setting it to a single '0'.
 * The old array is freed and replaced.
 *
 * @param variable        Pointer to the variable array to clear
 * @param variable_length Pointer to the variable's length (set to 1)
 */
void CLR(char** variable, int* variable_length) {
    char* new_variable = (char*) calloc(1, sizeof(char));
    new_variable[0] = '0';
    free(*variable);
    *variable = new_variable;
    *variable_length = 1;
}

/**
 * Performs a jump to a given instruction address (used for loops).
 * It sets the iterator to the desired address.
 *
 * @param i     Pointer to the loop iterator/index
 * @param addr  Address (index in the character array) to jump to
 */
void JMP(int* i, int addr) {
    *i = addr;
}

/**
 * Decrements the value of a variable (stored as a char array).
 * This is standard decrement logic on digit arrays. A special case
 * is when the value is a '1' followed by one or more zeros. Then
 * the resulting array will be shorter, and a new one is allocated.
 * If the variable equals 0, the function jumps to the given address.
 *
 * @param variable          Pointer to a pointer to the array of digit characters
 * @param variable_length   Pointer to the number of digits in the variable
 * @param i                 Pointer to the instruction pointer (iterator)
 * @param address           Target address to jump to if the variable is zero
 */
void DJZ(char** variable, int* variable_length, int* i, int address) {
    if (*variable_length == 1 && (*variable)[0] == '0') {
        JMP(i, address);
        return;
    }

    for (int j = *variable_length - 1; j >= 0; --j) {
        if ((*variable)[j] == '0') {
            (*variable)[j] = '9';
        } else {
            (*variable)[j]--;
            break;
        }
    }

    if ((*variable)[0] == '0' && *variable_length > 1) {
        --(*variable_length);
        char* new_variable = (char*) calloc((size_t)*variable_length, sizeof(char));
        for (int j = 0; j < *variable_length; ++j) {
            new_variable[j] = '9';
        }
        free(*variable);
        *variable = new_variable;
    }
}

/**
 * Reads a line from standard input until a newline or EOF.
 * The result is stored in a dynamically allocated array.
 * Updates line length and EOF status.
 *
 * @param line              Pointer to the line array to be replaced
 * @param line_length       Pointer to the resulting line's length
 * @param eof_reached       Pointer to a flag indicating whether EOF has been reached
 */
void read_line(char** line, int* line_length, int* eof_reached) {
    char* temp = NULL;
    int buffer_size = 0;
    int i = 0;
    int c = getchar();

    while (c != '\n' && c != EOF) {
        if (i == buffer_size) {
            if (buffer_size < (INT_MAX - 2) / 3 * 2) {
                buffer_size = buffer_size / 2 * 3 + 2;
            } else {
                buffer_size = INT_MAX;
            }
            temp = realloc(temp, (size_t) buffer_size * sizeof *temp);
        }

        temp[i] = (char) c;
        ++i;
        c = getchar();
    }

    if (c == EOF) {
        *eof_reached = 1;
    }

    free(*line);
    *line = temp;
    *line_length = i;
}

/**
 * Executes a program written in the Pętlik language.
 * Handles increment and non-nested repeat instructions.
 * Repeat blocks are detected by matching parentheses,
 * then delegated to the `execute_repeat` function.
 *
 * @param line              Pointer to the input program line
 * @param start             Index of the start of the current block
 * @param end               Index of the end of the current block
 * @param variables         Array of variable value arrays (a–z)
 * @param variable_lengths  Array of lengths for each variable's value
 */
void execute_program(char* line, int start, int end,
                     char* variables[], int variable_lengths[]) {
    if (end == 0)
        return;

    if (line[0] == PRINT_COMMAND) {
        int idx = char_to_var_index(line[1]);
        print_variable(variables[idx], variable_lengths[idx]);
        return;
    }

    int i = start;
    int left_parens = 0, right_parens = 0, unmatched = 0;
    int outer_left = start, outer_right = start - 1;

    while (i < end) {
        char ch = line[i];
        if (ch == REPEAT_START) {
            ++left_parens;
            ++unmatched;
            if (unmatched == 1)
                outer_left = i;
        } else if (ch == REPEAT_END) {
            ++right_parens;
            --unmatched;
            if (unmatched == 0)
                outer_right = i;
        }

        if (unmatched == 0) {
            if (i > outer_right) {
                int idx = char_to_var_index(line[i]);
                INC(&variables[idx], &variable_lengths[idx]);
            } else {
                execute_repeat(line, outer_left, outer_right,
                               left_parens - 1, variables, variable_lengths);
                left_parens = right_parens = 0;
            }
        }
        ++i;
    }
}

/**
 * Executes a Repeat instruction in the Pętlik language.
 * If paren_count == 0, applies optimized ADD/CLR sequence;
 * otherwise simulates the loop with DJZ, recursive execute_program, and JMP.
 *
 * @param line              Pointer to the input program line
 * @param start             Index of '('
 * @param end               Index of ')'
 * @param paren_count       Number of inner parentheses (excluding outer)
 * @param variables         Array of variable value arrays (a–z)
 * @param variable_lengths  Array of lengths for each variable's value
 */
void execute_repeat(char* line, int start, int end, int paren_count,
                    char* variables[], int variable_lengths[]) {
    int i = start + 1;
    int var0 = char_to_var_index(line[i]);
    ++i;

    if (paren_count == 0) {
        while (i < end) {
            int varN = char_to_var_index(line[i]);
            ADD(&variables[varN], &variable_lengths[varN],
                &variables[var0], &variable_lengths[var0]);
            ++i;
        }
        CLR(&variables[var0], &variable_lengths[var0]);
        return;
    }

    while (i < end) {
        DJZ(&variables[var0], &variable_lengths[var0], &i, end);
        execute_program(line, i, end, variables, variable_lengths);
        if (i < end)
            JMP(&i, i);
    }
}

/**
 * Entry point:
 * - Initializes 26 variables (a–z) as "0"
 * - Reads lines until EOF
 * - Executes each line as a Pętlik program or print command
 * - Frees all allocated memory before exit
 */
int main() {
    char* variables[NUM_VARIABLES];
    int variable_lengths[NUM_VARIABLES];

    // Initialize each variable to "0"
    for (int v = 0; v < NUM_VARIABLES; ++v) {
        variables[v] = calloc(1, sizeof(char));
        variables[v][0] = '0';
        variable_lengths[v] = 1;
    }

    char* line = NULL;
    int line_length;
    int eof_flag = 0;

    while (!eof_flag) {
        read_line(&line, &line_length, &eof_flag);
        execute_program(line, 0, line_length, variables, variable_lengths);
    }

    // Free variable storage
    for (int v = 0; v < NUM_VARIABLES; ++v) {
        free(variables[v]);
    }
    free(line);

    return 0;
}
