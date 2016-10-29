#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INITIAL_MEMORY 30000    /* initial memory, with the data pointer set to the leftmost byte */
#define ERR 1
#define OK 0

/*
* interpret - interprets brainfuck code
* instructions - the brainfuck code to be interpreted
* in - the input string that the interpreter is to be supplied with (it starts reading from in[0])
* out - the output string that the interpreter will output to (it overwrites the string, and starts at the beginning)
* return values - 0 = success, 1 = bad syntax, 2 = memory out of bounds
*/
int interpret(instructions, in, out)
char *instructions;
FILE *in;
FILE *out;
{
    /* verifying matching brackets */
    int brackets = 0;
    size_t b;
    for (b = 0; b < strlen(instructions) && brackets >= 0; b++) {
        if (instructions[b] == '[') {
            brackets++;
        }
        else if (instructions[b] == ']') {
            brackets--;
        }
    }
    if (brackets != 0) {
        return ERR;
    }
    /* initialising variables */
    /*
    * struct cell - doubly linked list node for a byte of memory in the interpreter (double links to be dynamic on both sides)
    * prev - the previous node the the linked list of cells
    * value - the value of the cell, stored in a char with a fixed size of 8 bits
    * next - the next node the the linked list of cells
    */
    struct cell {
        struct cell *prev;
        char value : 8;
        struct cell *next;
    };
    /* program counter */
    size_t pc = 0;
    /* brainfuck data pointer */
    struct cell *dp = calloc(1, sizeof(struct cell));
    if (dp == NULL) return ERR;
    /* initialising memory to INITIAL_MEMORY */
    struct cell *curr = dp;
    size_t dp_addr = 0;
    int i;
    for (i = 0; i < INITIAL_MEMORY - 1; i++) {
        curr->next = calloc(1, sizeof(struct cell));
        if (curr->next == NULL) return ERR;
        curr->next->prev = curr;
        curr = curr->next;
    }
    while (pc < strlen(instructions)) {
        /* fetch and decode */
        char instruction = instructions[pc];
        /* printf("program counter: %d, instruction: %c, address: %d, value: %d\n", pc, instruction, dp_addr, dp->value); */
        /* execute */
		switch (instruction) {
			case '+':
				dp->value++;
				break;
			case '-':
				dp->value--;
				break;
			case '<':
				if (dp->prev == NULL) {
					dp->prev = calloc(1, sizeof(struct cell));
					if (dp->prev == NULL) return ERR;
					dp->prev->next = dp;
				}
				dp = dp->prev;
				dp_addr--;
				break;
			case '>':
				if (dp->next == NULL) {
					dp->next = calloc(1, sizeof(struct cell));
					if (dp->next == NULL) return ERR;
					dp->next->prev = dp;
				}
				dp = dp->next;
				dp_addr++;
				break;
			case ',':
				if (in != NULL && !feof(in)) {
					dp->value = getc(in);
				} else {
					return ERR;
				}
				break;
			case '.':
				if (out != NULL) {
					putc(dp->value, out);
				} else {
					return ERR;
				}
				break;
			case '[':
				if (dp->value == 0) {
					brackets = 0;
					do {
						pc++;
						if (instructions[pc] == '[') {
							brackets++;
						}
						else if (instructions[pc] == ']') {
							brackets--;
						}
					} while (brackets == 0 || instructions[pc] != ']');
				}
				break;
			case ']':
				if (dp->value != 0) {
					brackets = 0;
					do {
						pc--;
						if (instructions[pc] == '[') {
							brackets++;
						}
						else if (instructions[pc] == ']') {
							brackets--;
						}
					} while (brackets == 0 || instructions[pc] != '[');
				}
				break;
		}
        /* increment data pointer */
        pc++;
    }
    /* free the memory allocated to the nodes in the linked list of cells */
    while (dp->prev != NULL) {
        dp = dp->prev;
    }
    while (dp->next != NULL) {
        dp = dp->next;
        free(dp->prev);
    }
    free(dp);
    return OK;
}
