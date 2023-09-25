/*
    Patrick DeVoney & Rossella Diorio.
    Homework Assignment Two: Lexical Analyzer.
    Prof: Euripides Montage.
    COP 3402, Fall 2023.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

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
    ssym['+'] = plussym; 
    ssym['-'] = minussym; 
    ssym['*'] = multsym; 
    ssym['/'] = slashsym; 
    ssym['('] = lparentsym; 
    ssym[')'] = rparentsym; 
    ssym['='] = eqsym; 
    ssym[','] = commasym; 
    ssym['.'] = periodsym; 
    ssym['<'] = lessym; 
    // ssym['<='] = leqsym; 
    ssym['>'] = gtrsym; 
    // ssym['>='] = geqsym; 
    ssym[';'] = semicolonsym; 
    ssym[':'] = becomessym; 


    int chcount = 0;

    // Print out input file. 
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
    printf("\n\n"); 
    fclose(file);
    // ****************************************|

    // *******Put input file into array.*******|
    char inputArr[chcount+1];
    FILE *inputFile = fopen("input.txt", "r");
    if (NULL == inputFile) {
        printf("The file cannot be opened\n");
        return 0; // or do you put exit 1; ??
    }
    int i = 0; 
    while(!feof(inputFile)) {
        char c = fgetc(inputFile); 
        if (isspace(c)) {
            inputArr[i] = '\0'; 
        } else {
            inputArr[i] = c; 
        }
        i++;
    }
    inputArr[i] = '\0'; 
    fclose(inputFile);
    // ****************************************|

    // Print out lexeme table titles. 
    printf("Lexeme Table: \n\n lexeme \t token type\n");
    
    // dynamically allocate memory for tokenType array
    int tokenTypeSize = 50; 
    int *tokenType = malloc(sizeof(int) * tokenTypeSize);
    int tokenCount = 0; 

    // dynamically allocate memory for identifier array
    int identSize = 50; 
    char *identArr = malloc(sizeof(char) * identSize);
    int identCount = 0;

    // ******* loops though inputArr ***********|
    i = 0; 
    while (i < sizeof(inputArr)){

        int tempArrSize = 12; 
        char *tempArr = malloc(sizeof(int) * tempArrSize);
        int tempArrCount = 0; 

        // 1 for keyword, identifier, or num .
        // 2 for special character. 
        int caseCheck = 0; 

        int halt = 0;
        // ================ all cases for looping trough inputArr ================
        do {
            // scan in to tempArr. 
            tempArr[tempArrCount] = inputArr[i]; 
            
            // if scaned in keyword, ident, or num follwed by '\0'. 
            if (tempArr[tempArrCount] == '\0' && tempArrCount != 0) {
                caseCheck = 1; 
                halt = 1; 
            }
            // if scanned in keyword, ident, or num follwed by specialChar. 
            else if (!isalnum(tempArr[tempArrCount]) && tempArr[tempArrCount] != '\0' && tempArrCount != 0) {
                caseCheck = 1; 
                tempArr[tempArrCount] = '\0';
                break; 
            }
            // first scan, whitespace, skip 
            else if (tempArr[tempArrCount] == '\0' && tempArrCount == 0) {
                tempArr[tempArrCount] = inputArr[i+1]; 
                i++; 
            }
            // first scan, specialChar
            else if (tempArr[tempArrCount] != '\0' && !isalnum(tempArr[tempArrCount]) && tempArrCount == 0) {
                caseCheck = 2; 
                i++; 
                break; 
            }

            // if input is too long resize. 
            if (tempArrCount == tempArrSize - 1) {
                tempArrSize *= 2; 
                tempArr = realloc(tempArr, tempArrSize * sizeof(int));
            }

            i++;
            tempArrCount++; 
        } while (halt == 0); 
        tempArr[tempArrCount+1] = '\0'; 


        // =============== Find out what is in tempArr ===============

        if (caseCheck == 1) {
            int digitCount = 0; 
            // determine if it is digit. 
            int u = 0; 
            while (isdigit(tempArr[u])) {
                digitCount++; 
                u++; 
            }
            // print error, if digit too long. 
            if (digitCount == tempArrCount && digitCount > 5) {
                    for (int k = 0; k < digitCount; k++){
                        printf("%d", tempArr[k] - 48); 
                    }
                    printf("\tError number too long!\n"); 
            }
            // print digit if meets requirements. 
            if (digitCount == tempArrCount && digitCount <= 5) {
                for (int k = 0; k < digitCount; k++){
                        printf("%d", tempArr[k] - 48); 
                }
                tokenType[tokenCount] = numbersym; 
                printf("\t%d\n", tokenType[tokenCount]);
                tokenCount++; 
            }

            // compare tempArr to reserved words
            for (int k = 0; k < norw; k++) {
                if (strcmp(tempArr, resWords[k]) == 0) {
                    // print out lexem and token 
                    for (int l = 0; tempArr[l] != '\0'; l++) {
                        printf("%c", tempArr[l]);
                    }
                    printf("\t%d\n", wsym[k]);

                    // dynamically resize tokenType array if necessary. 
                    if (tokenCount == tokenTypeSize-1) {
                        tokenTypeSize *= 2; 
                        tokenType = realloc(tokenType, sizeof(int) * tokenTypeSize);
                    }
                    // put the token in the array. 
                    tokenType[i] = wsym[k];
                    tokenCount++;
                }
            }

            // compare tempArr to identifiers
            for (int k = 0; k < identCount; k++) {
                if (tempArrCount > 11){
                    for (int l = 0; tempArr[l] != '\0'; l++) {
                        printf("%c", tempArr[l]);
                    }
                    printf("\tError idenfitier is too long!\n"); 
                }
                else if (strcmp(tempArr, &identArr[k]) == 0) {
                    // print out lexem and token 
                    for (int l = 0; tempArr[l] != '\0'; l++) {
                        printf("%c", tempArr[l]);
                    }

                    printf("\t%d\n", identsym);

                    // dynamically resize tokenType array if necessary. 
                    if (tokenCount == tokenTypeSize-1) {
                        tokenTypeSize *= 2; 
                        tokenType = realloc(tokenType, sizeof(int) * tokenTypeSize);
                    }
                    tokenType[i] = identsym;
                    tokenCount++;
                }
            }
        }
        else {

        }

        i++; 
        free(tempArr); 
    }
    return 0;
}
