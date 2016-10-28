#include <stdio.h>
#include <stdlib.h>
#include "interpreter.h"

int main(argc, argv)
int argc;
char **argv;
{
    char *input = "Hello World!";
    FILE *inStream = tmpfile();
    if (inStream == NULL) return ERR;
    fprintf(inStream, "%s", input);
    rewind(inStream);
    FILE *outStream = tmpfile();
    char *code = ",.,.,.";
    if (!interpret(code, inStream, stdout)) {
        rewind(outStream);
        while (!feof(outStream)) {
            putchar(getc(outStream));
        }
    }
    fclose(outStream);
    fclose(inStream);
    return OK;
}
