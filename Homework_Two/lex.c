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

int main(int argc, char *argv[]){
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
    ssym['>'] = gtrsym; 
    ssym[';'] = semicolonsym; 
    ssym[':'] = becomessym; 


    FILE *fp = fopen("out.txt","w");

    int chcount = 0;
    int endOfFile = 0;

    // Print out input file. 
    // **************************************|
    FILE *file = fopen(argv[1], "r");
    char ch;
    if (NULL == file) {
        printf("No input file recieved, please put: 'input.txt' after executable file! \n");
        fprintf(fp,"No input file recieved, please put: 'input.txt' after executable file! \n");

        return 0;
    }
    printf("Source Code:\n");
    fprintf(fp,"Source Code:\n");
    while (!feof(file)) {
        chcount++;
        ch = fgetc(file);
        printf("%c", ch);
        fprintf(fp,"%c", ch);
    }
    printf("\n\n");
    fprintf(fp,"\n\n"); 
    fclose(file);
    // ****************************************|

    // *******Put input file into array.*******|
    char inputArr[chcount+1];
    FILE *inputFile = fopen(argv[1], "r");
    if (NULL == inputFile) {
        printf("No input file recieved, please put: 'input.txt' after executable file! \n");
        fprintf(fp,"No input file recieved, please put: 'input.txt' after executable file! \n");
        return 0; 
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
    printf("Lexeme Table: \n\nlexeme          token type\n");
    fprintf(fp,"Lexeme Table: \n\nlexeme          token type\n");
    
    // dynamically allocate memory for tokenType array
    int tokenTypeSize = 50; 
    int *tokenType = malloc(sizeof(int) * tokenTypeSize);
    int tokenCount = 0; 

    // dynamically allocate memory for identifier array
    int identSize = 100; 
    char **identArr = malloc(sizeof(char*) * identSize);
    int identCount = 0;
    for (int k = 0; k < identSize; k++) {
        identArr[k] = malloc(sizeof(char) * 12); 
    }

    // *********** loops though inputArr ***********|
    i = 0; 
    while (i < sizeof(inputArr) - 2){
        int tempArrSize = 12; 
        char *tempArr = malloc(sizeof(char) * tempArrSize);
        int tempArrCount = 0; 

        // 1 for keyword, identifier, or num .
        // 2 for special character. 
        int caseCheck = 0; 

        int halt = 0;
        // ================ All cases for looping trough inputArr ================
        do {
            // scan in to tempArr. 
            tempArr[tempArrCount] = inputArr[i];
            
            if(isdigit(tempArr[tempArrCount]) && tempArrCount == 0) {
                caseCheck = 1;
                i++;
                tempArrCount++;
                while(isdigit(inputArr[i])) {
                    tempArr[tempArrCount] = inputArr[i];
                    i++;
                    tempArrCount++;
                }
                tempArr[tempArrCount] = '\0';
                i -= 2; 
                break; 
            }
            
            // if scaned in keyword, ident, or num follwed by '\0'. 
            if (!isalnum(tempArr[tempArrCount]) && tempArr[tempArrCount] == '\0' && tempArrCount != 0) {
                caseCheck = 1; 
                halt = 1; 
                break; 
            }

            // if scanned in keyword, ident, or num follwed by specialChar. 
            else if (!isalnum(tempArr[tempArrCount]) && tempArr[tempArrCount] != '\0' && tempArrCount != 0) {
                caseCheck = 1; 
                tempArr[tempArrCount] = '\0';
                i--;
                break; 
            }
            // first scan, whitespace, skip 
            else if (tempArr[tempArrCount] == '\0' && tempArrCount == 0) { 
                
                while (inputArr[i] == '\0') {
                    i++; 
                    if (i == chcount - 1)
                        endOfFile = 1;
                }
                tempArr[tempArrCount] = inputArr[i];
                
            }
            // first scan, specialChar
            if (tempArr[tempArrCount] != '\0' && !isalnum(tempArr[tempArrCount]) && tempArrCount == 0) {
                caseCheck = 2; 
                break; 
            }

            // if input is too long resize. 
            if (tempArrCount == tempArrSize - 1) {
                tempArrSize *= 2; 
                tempArr = realloc(tempArr, tempArrSize * sizeof(char));
            }

            i++;
            tempArrCount++; 
        } while (halt == 0); 
       
        tempArr[tempArrCount+1] = '\0'; 
        // =============== Find out what is in tempArr ===============
        if (caseCheck == 1) {
            //=================== Digit Check ===================
            int digitCount = 0; 
            // determine if it is digit. 
            int u = 0; 
            while (isdigit(tempArr[u])) {
                digitCount++; 
                u++; 
            }
            if (digitCount > 0) {
                // digit too long. 
                if (digitCount > 5) {
                    
                    printf("%-12s", tempArr);
                    fprintf(fp, "%-12s", tempArr); 

                    printf("\tError: Number too long!\n");
                    fprintf(fp,"\tError: Number too long!\n"); 
                    i++; 
                    free(tempArr);
                }
                // print digit if meets requirements. 
                if (digitCount == tempArrCount && digitCount <= 5) {
                    
                    strcpy(identArr[identCount], tempArr);
                    fprintf(fp, "%-12s", identArr[identCount]); 
                    printf("%-12s", identArr[identCount]); 

                    tokenType[tokenCount] = numbersym; 
                    printf("\t%d\n", tokenType[tokenCount]);
                    fprintf(fp,"\t%d\n", tokenType[tokenCount]);

                    identCount++; 
                    tokenCount++; 
                    i++; 
                    free(tempArr);
                }
                // dynamically realocate tokenType & identArr 
                if (tokenCount == tokenTypeSize-1) {
                    tokenTypeSize *= 2; 
                    tokenType = realloc(tokenType, sizeof(int) * tokenTypeSize);
                }
                if (identCount == identSize-1) {
                    identSize *= 2;
                    identArr = realloc(identArr, sizeof(char*) *identSize);
                    for (int k = identCount; k < identSize; k++)
                        identArr[k] = malloc(sizeof(char) * 12); 
                }
            }
            else {
                //=================== resWords Check ===================
                int keyWordCheck = 0; 
                for (int k = 0; k < norw; k++) {
                    if (strcmp(tempArr, resWords[k]) == 0) {
                        
                        printf("%-12s", tempArr); 
                        fprintf(fp, "%-12s", tempArr); 
                        printf("\t%d\n", wsym[k]);
                        fprintf(fp,"\t%d\n", wsym[k]);

                        // dynamically resize tokenType array if necessary. 
                        if (tokenCount == tokenTypeSize-1) {
                            tokenTypeSize *= 2; 
                            tokenType = realloc(tokenType, sizeof(int) * tokenTypeSize);
                        }
                        // put the token in the array. 
                        tokenType[tokenCount] = wsym[k];
                        tokenCount++;
                        keyWordCheck = 1; 
                        free(tempArr);
                    }
                }
                //=================== Identifier Check ===================
                if (keyWordCheck == 0) {
                    int len = 0;

                    if (tempArrCount > 11){
                        
                        printf("%-12s", tempArr); 
                        fprintf(fp, "%-12s", tempArr); 

                        printf("\tError: Identifier is too long!\n");
                        fprintf(fp,"\tError: Identifier is too long!\n"); 
                    } else {

                        strcpy(identArr[identCount], tempArr); 

                        tokenType[tokenCount] = identsym;
                        tokenCount++;
                        
                        printf("%-12s", identArr[identCount]);
                        printf("\t%d\n", identsym);

                        fprintf(fp,"%-12s", identArr[identCount]);
                        fprintf(fp,"\t%d\n", identsym);

                        identCount++;
                    }
                    
                    // dynamically resize tokenType array if necessary. 
                    if (tokenCount == tokenTypeSize-1) {
                        tokenTypeSize *= 2; 
                        tokenType = realloc(tokenType, sizeof(int) * tokenTypeSize);
                    }
                    if (identCount == identSize-1) {
                        identSize *= 2;
                        identArr = realloc(identArr, sizeof(char*) *identSize);
                        for (int k = identCount; k < identSize; k++)
                            identArr[k] = malloc(sizeof(char) * 12); 
                    }
                    free(tempArr); 
                }
            }
        } 
        else {
            // ======================= Special Character Check =======================
            if (tempArr[0] == '/' && inputArr[i+1] == '*') {
                i += 2; 
                while (inputArr[i] != '*' && inputArr[i+1] != '/') {
                    i++; 
                }
                i += 1; 
            }
            else if (tempArr[0] == ':' && inputArr[i+1] == '=') {
                printf(":=\t");
                printf("%10d\n", becomessym);

                fprintf(fp,":=\t");
                fprintf(fp,"%10d\n", becomessym);

                tokenType[tokenCount] = becomessym;
                tokenCount++;
                i++;  
            }
            else if (tempArr[0] == '<' && inputArr[i+1] == '=') {
                printf("<=\t");
                printf("%10d\n", leqsym);

                fprintf(fp,"<=\t");
                fprintf(fp,"%10d\n", leqsym);

                tokenType[tokenCount] = leqsym;
                tokenCount++;
                i++;
            }
            else if (tempArr[0] == '>' && inputArr[i+1] == '=') {
                printf(">=\t");
                printf("%10d\n", geqsym);

                fprintf(fp,">=\t");
                fprintf(fp,"%10d\n", geqsym);

                tokenType[tokenCount] = geqsym;
                tokenCount++;
                i++; 
            }
            else if (tempArr[0] == '<' && inputArr[i+1] == '>') {
                printf("<>\t");
                printf("%10d\n", neqsym);

                fprintf(fp,"<>\t");
                fprintf(fp,"%10d\n", neqsym);

                tokenType[tokenCount] = neqsym;
                tokenCount++;
                i++; 
            }
            else {
                if (!endOfFile) {
                    printf("%-12c\t", tempArr[0]); 
                    fprintf(fp,"%-12c\t", tempArr[0]);
                    if (ssym[tempArr[0]] == 4 || ssym[tempArr[0]] == 5 
                        || ssym[tempArr[0]] == 6 || ssym[tempArr[0]] == 7 
                        || ssym[tempArr[0]] == 15 || ssym[tempArr[0]] == 16 
                        || ssym[tempArr[0]] == 9 || ssym[tempArr[0]] == 17 
                        || ssym[tempArr[0]] == 19 || ssym[tempArr[0]] == 11 
                        || ssym[tempArr[0]] == 13 || ssym[tempArr[0]] == 18) {
                        printf("%d\n", ssym[tempArr[0]]);
                        fprintf(fp,"%d\n", ssym[tempArr[0]]);
                        tokenType[tokenCount] = ssym[tempArr[0]];
                        tokenCount++;
                    } else {
                        printf("Error: Invalid symbol!\n");
                        fprintf(fp,"Error: Invalid symbol!\n");
                    }
                }
            }
            if (tokenCount == tokenTypeSize-1) {
                tokenTypeSize *= 2; 
                tokenType = realloc(tokenType, sizeof(int) * tokenTypeSize);
            }
            free(tempArr); 
        }
        
        i++; 
    }
    
    int tempIdentIndex = 0;

    printf("\nToken List:\n");
    fprintf(fp,"\nToken List:\n");

    for (int tempTokenIndex = 0; tempTokenIndex < tokenCount; tempTokenIndex++) {

        printf("%d ",tokenType[tempTokenIndex]);
        fprintf(fp, "%d ",tokenType[tempTokenIndex]);

        if((tokenType[tempTokenIndex] == 2) || tokenType[tempTokenIndex] == 3) {
            printf("%s ", identArr[tempIdentIndex]);
            fprintf(fp,"%s ", identArr[tempIdentIndex]);
            tempIdentIndex++;
        }
    }

    printf("\n");
    fprintf(fp,"\n");

    fclose(fp);

    for (int z = 0; z < identCount; z++) {
        free(identArr[z]);
    }

    free(identArr);

    free(tokenType);

    return 0;
}
