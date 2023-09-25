/*
    Patrick DeVoney & Rossella Diorio.
    Homework Assignment Two: Lexical Analyzer.
    Prof: Euripides Montage.
    COP 3402, Fall 2023.
*/

#include <stdio.h>
#include <stdlib.h>

#define norw 14     // Number of reserved words. 
#define imax 32767  // Max integer val. 
#define cmax 11     // Max number of chars for indents. 
#define strmax 256  // Max n



// Internal representation of PL/0 symbols.
typedef enum { 
    skipsym = 1, identsym, numbersym, plussym, minussym,
    multsym,  slashsym, ifelsym, eqsym, neqsym, lessym, leqsym,
    gtrsym, geqsym, lparentsym, rparentsym, commasym, semicolonsym,
    periodsym, becomessym, beginsym, endsym, ifsym, thensym, 
    whilesym, dosym, callsym, constsym, varsym, procsym, writesym,
    readsym , elsesym
} token_type;

int main(){
    // Reserved words (keywords). 
    char * resWords [] = {"const", "var", "procedure", 
                        "call", "begin", "end", "if", 
                        "then", "ifel", "else", "while", 
                        "do", "read", "write"};

    // What the keywords correspond to. 
    int wsym [] = {constsym, varsym, procsym, callsym, 
                    beginsym, endsym, ifsym, thensym, 
                    ifelsym, elsesym, whilesym, dosym, 
                    readsym, writesym};

    // Array for special characters. 
    int ssym [256];

    int chcount = 0;

    // Open once section
    // **************************************|
    FILE *file = fopen("input.txt", "r");
    char ch;
    if (NULL == file) {
        printf("The file cannot be opened\n");
        return 0;
    }
    printf("Source Code:\n");
    while (!feof(file)) {
        chcount++;
        ch = fgetc(file);
        printf("%c", ch);
    }
    fclose(file);
    // ****************************************|

    char inputArr[chcount];

    FILE *inputFile = fopen("input.txt", "r");
    if (NULL == inputFile) {
        printf("The file cannot be opened\n");
        return 0; // or do you put exit 1; ??
    }
    
    int i = 0; //index of inputArr
    

    while(!feof(inputFile)) {
        fscanf("%c", &inputArr[i]);  // or was this temp array?
    }

    i = 0; 
    while (i < sizeof(inputArr)){
        if (inputArr[i] == 'c'/*white space or special character*/ )
        {
            i++;
            continue;
        } 
        else {
            /* 
                convert from starting index to one before white space/special character index
                into a string Compare to each resWord

                these variables have to be made

            */  
            int len = curIndex - startIndex;
            char tempCharArr[len];
            tempCharArr[len] = '/0';
            
            String tempStr[len]
            tempStr = strcpy(tempStr, tempCharArr);

            // shouldn't this be in a loop to cmpare to all reswords
            int save = 0;
            for (int j = 0; j < sizeof(resWords); j++) {
                // compare each word to the temp str
                if(strcmp(tempStr, resWords[j]) == 0) {
                    save = j;
                }
            }

            switch(save) {
                case 0:
                // const
                break;

            }

            // example kinda
            if(strcmp(tempStr, "write") == 0) {
                // do the write stuff
            }
           

           
        }
    }
    
    // Arrays needed:
    /*
        - token list array
        - identifier array
        - token type array
    */

    switch(tokenNum) {
        // ========== identifier ==========
        case 2:
            break;
        // ========== ifel ==========
        case 8:
            break;
        // ========== begin ==========
        case 21:
            break;
        // ========== end ==========
        case 22:
            break;
        // ========== if ==========
        case 23:
            break;
        // ========== then ==========
        case 24:
            break;
        // ========== while ==========
        case 25:
            break;
        // ========== do ==========
        case 26:
            break;
        // ========== call ==========
        case 27:
            break;
        // ========== const ==========
        case 28:
            break;
        // ========== var ==========
        case 29:
            break;
        // ========== procedure ==========
        case 30:
            break;
        // ========== write ==========
        case 31:
            break;
        // ========== read ==========
        case 32:
            break;
        // ========== else ==========
        case 33:
            break;
    }

    return 0;
}
