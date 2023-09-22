/*
    Patrick DeVoney & Rossella Diorio.
    Homework Assignment Two: Lexical Analyzer.
    Prof: Euripides Montage.
    COP 3402, Fall 2023.
*/

#include <stdio.h>
#include <stdlib.h> 

typedef enum { 
    skipsym = 1, identsym, numbersym, plussym, minussym,
    multsym,  slashsym, ifelsym, eqsym, neqsym, lessym, leqsym,
    gtrsym, geqsym, lparentsym, rparentsym, commasym, semicolonsym,
    periodsym, becomessym, beginsym, endsym, ifsym, thensym, 
    whilesym, dosym, callsym, constsym, varsym, procsym, writesym,
    readsym , elsesym
} token_type;


int main(){
    // reserved words (keywords). 
    char * resWords [] = {"const", "var", "procedure", 
                        "call", "begin", "end", "if", 
                        "then", "ifel", "else", "while", 
                        "do", "read", "write"};
    // what the keywords correspond to. 
    int wsym [] = {constsym, varsym, procsym, callsym, 
                    beginsym, endsym, ifsym, thensym, 
                    ifelsym, elsesym, whilesym, dosym, 
                    readsym, writesym};
    // array for special characters. 
    int ssym [256];
    
    // loops over all input. 
    FILE *file = fopen("input.txt", "r");
    char ch;
 
    if (NULL == file) {
        printf("file can't be opened \n");
        return 0;
    }
 
    printf("Source Code:\n");
 
    while (!feof(file)) {
        ch = fgetc(file);
        printf("%c", ch);
    }
    fclose(file);
    return 0;
    
}
