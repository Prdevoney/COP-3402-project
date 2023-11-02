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
#define CODE_SIZE 150  // Max code length.
# define MAX_SYMBOL_TABLE_SIZE 500

// Internal representation of PL/0 symbols.
typedef enum { 
    oddsym = 1, identsym, numbersym, plussym, minussym,
    multsym,  slashsym, ifelsym, eqsym, neqsym, lessym, leqsym,
    gtrsym, geqsym, lparentsym, rparentsym, commasym, semicolonsym,
    periodsym, becomessym, beginsym, endsym, ifsym, thensym, 
    whilesym, dosym, callsym, constsym, varsym, procsym, writesym,
    readsym , elsesym
} token_type;

typedef struct {
    int kind;      // const = 1, var = 2, proc = 3
    char name[12]; // name up to 11 chars
    int val;       // number (ASCII value)
    int level;     // L level
    int addr;      // M address
} symbol;

// Key for OP instructions generation.
typedef enum { 
    LIT = 1, OPR, LOD, STO, CAL, INC, JMP, JPC, SYS
} OP_code;

typedef struct { 
    int op; // opcode
    int l;  // L level
    int m;  // M address
} instruction;

// Initialization of global variables: 
int *tokenType; // token list 
char **identArr; // identity array
symbol *symbolTable[MAX_SYMBOL_TABLE_SIZE]; // symbol table array
instruction code[CODE_SIZE]; // code array
int cx = 0;   // starting index.
int tokenIndex = 0; 
int symbolIndex = 0; 


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
    while ((ch = fgetc(file)) != EOF) {
        chcount++;
        printf("%c", ch);
        fprintf(fp, "%c", ch);
    }
    printf("\n\n");
    fprintf(fp,"\n\n"); 
    fclose(file);
    // ****************************************|

    // *******Put input file into array.*******|

    // dynamically allocate memory for the inputArr 
    char * inputArr = malloc( sizeof(char) * (chcount+1));

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
    // Global Array 
    tokenType = malloc(sizeof(int) * tokenTypeSize);
    int tokenCount = 0; 

    // dynamically allocate memory for identifier array
    int identSize = 100; 
    // Global Array 
    identArr = malloc(sizeof(char*) * identSize);
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
            
            // first scan, is digit 
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

    // Call parser codegen function.
    // do we need to do anything with tokenCount???
    program();

    
    for (int z = 0; z < identCount; z++) {
        free(identArr[z]);
    }

    free(identArr);

    free(tokenType);

    // free memory for the inputArr
    free(inputArr); 

    return 0;
}



// ------------- The parser codegen part of the compiler ------------------------


symbol *initSymbolTable (int kind, char *name, int val, int level, int addr) {
    symbol *s = malloc(sizeof(symbol));
    s->kind = kind;
    strcpy(s->name, name);
    s->val = val;
    s->level = level;
    s->addr = addr;
    return s;
    
}

void emit(int op, int l, int m) {
    if (cx > CODE_SIZE) {
        printf("Error: Code too long!\n");
        exit(1);
    } else {
        code[cx].op = op;
        code[cx].l = l;
        code[cx].m = m;
        cx++;
    }
}

void program() {
    block();
    if (tokenType[tokenIndex] != periodsym) {
        printf("Error: Period expected.\n");
        exit(1);
    }
    emit(SYS, 0, 3);
}

void block () {
    constDeclaration(tokenType, tokenIndex);
    int numVars = varDeclaration(tokenType, tokenIndex);
    char INC[] = "INC";
    emit(INC, 0, 3 + numVars);
    statement();
}

void constDeclaration() {
    if (tokenType[tokenIndex] == constsym) {
        do {
            tokenIndex++;
            if (tokenType[tokenIndex] != identsym) {
                printf("Error: const, var, procedure must be followed by identifier.\n");
                exit(1);
            }
            if(SYMBOLTABLECHECK(tokenType[tokenIndex]) != -1) {
                printf("Error: This variable has already been declared.\n");
                exit(1);
            }
            char * identName = tokenType[tokenIndex];
            tokenIndex++;
            if (tokenType[tokenIndex] != eqsym) {
                printf("Error: Identifier must be followed by =.\n");
                exit(1);
            }
            tokenIndex++;
            if (tokenType[tokenIndex] != numbersym) {
                printf("Error: = must be followed by a number.\n");
                exit(1);
            }
            // this bit is extra idk if it is needed
            if (tokenType[tokenIndex] == numbersym) {
                tokenIndex++;
                int number = atoi(tokenType[tokenIndex]); // Convert string to integer
                symbolTable[symbolIndex] = initSymbolTable(1, identName, number, 0, 0);
                tokenIndex++;
                // to here just get rid of the if else and keep the line in the else statement
            } else {
                symbolTable[symbolIndex] = initSymbolTable(1, identName, tokenType[tokenIndex], 0, 0);
                // make sure the tokenType[tokenIndex] doesnt need to be cased to an int
            }
            tokenIndex++;
        } while (tokenType[tokenIndex] == commasym);
        if (tokenType[tokenIndex] != semicolonsym) {
            printf("Error: Semicolon or comma missing.\n");
            exit(1);
        }
        tokenIndex++;
    }
}

int varDeclaration() {
    int numVars = 0;
    if (tokenType[tokenIndex] == varsym) {
        do {
            numVars++;
            tokenIndex++;
            if (tokenType[tokenIndex] != identsym) {
                printf("Error: const, var, procedure must be followed by identifier.\n");
                exit(1);
            }
            if (SYMBOLTABLECHECK(tokenType[tokenIndex]) != -1) {
                printf("Error: This variable has already been declared.\n");
                exit(1);
            }
            initSymbolTable(2, tokenType[tokenIndex], 0, 0, 2 + numVars);
            tokenIndex++;
        } while (tokenType[tokenIndex] == commasym);
        if (tokenType[tokenIndex] != semicolonsym) {
            printf("Error: Semicolon or comma missing.\n");
            exit(1);
        }
        tokenIndex++;
    }
    return numVars;
}

void statement() {
    int symIdx, jpcIdx, loopIdx;
    if (tokenType[tokenIndex] == identsym) {
        symIdx = SYMBOLTABLECHECK(tokenType[tokenIndex]);
        if (symIdx == -1) {
            printf("Error: Undeclared variable.\n");
            exit(1);
        }
        if(symbolTable[symIdx]->kind != 2) {
            printf("Error: Not a var.\n");
            exit(1);
        }
        tokenIndex++;
        if (tokenType[tokenIndex] != becomessym) {
            printf("Error: Assignment operator expected.\n");
            exit(1);
        }
        tokenIndex++;
        expression();
        emit(STO, 0, symbolTable[symIdx]->addr);
        return 0;
    }
    if (tokenType[tokenIndex] == beginsym) {
        do { 
            tokenIndex++;
            statement();
        } while (tokenType[tokenIndex] == semicolonsym);
        if (tokenType[tokenIndex] != endsym) {
            printf("Error: Semicolon or } expected.\n");
            exit(1);
        }
        tokenIndex++;
        return 0;
    }
    if (tokenType[tokenIndex] == ifsym) {
        tokenIndex++;
        condition();
        jpcIdx = cx;
        emit(JPC, 0, 0);
        if (tokenType[tokenIndex] != thensym) {
            printf("Error: then expected.\n");
            exit(1);
        }
        tokenIndex++;
        statement();
        code[jpcIdx].m = cx;
        return 0;
    }
    if (tokenType[tokenIndex] == whilesym) {
        loopIdx = cx;
        tokenIndex++;
        condition();
        if (tokenType[tokenIndex] != dosym) {
            printf("Error: do expected.\n");
            exit(1);
        }
        tokenIndex++;
        jpcIdx = cx;
        emit(JPC, 0, 0);
        statement();
        emit(JMP, 0, loopIdx);
        code[jpcIdx].m = cx;
        return 0;
    }
    if (tokenType[tokenIndex] == readsym) {
        tokenIndex++;
        if (tokenType[tokenIndex] != identsym) {
            printf("Error: Assignment to constant or procedure is not allowed.\n");
            exit(1);
        }
        symIdx = SYMBOLTABLECHECK(tokenType[tokenIndex]);
        if (symIdx == -1) {
            printf("Error: Undeclared variable.\n");
            exit(1);
        }
        if (symbolTable[symIdx]->kind != 2) {
            printf("Error: Assignment to constant or procedure is not allowed.\n");
            exit(1);
        }
        tokenIndex++;
        emit(SYS, 0, 2); //read
        emit(STO, 0, symbolTable[symIdx]->addr);
        return 0;
    }
    if (tokenType[tokenIndex] == writesym) {
        tokenIndex++;
        expression();
        emit(SYS, 0, 1); //write
        return 0;
    }
}

void condition() {
    if (tokenType[tokenIndex] == oddsym) {
        tokenIndex++;
        expression();
        emit(OPR, 0, 11);
    } else {
        expression();
        if (tokenType[tokenIndex] != eqsym && tokenType[tokenIndex] != neqsym && 
        tokenType[tokenIndex] != lessym && tokenType[tokenIndex] != leqsym && 
        tokenType[tokenIndex] != gtrsym && tokenType[tokenIndex] != geqsym) {
            printf("Error: Relational operator expected.\n");
            exit(1);
        }
        int relOp = tokenType[tokenIndex];
        tokenIndex++;
        expression();
        switch (relOp) {
            case eqsym:
                emit(OPR, 0, 8);
                break;
            case neqsym:
                emit(OPR, 0, 9);
                break;
            case lessym:
                emit(OPR, 0, 10);
                break;
            case leqsym:
                emit(OPR, 0, 11);
                break;
            case gtrsym:
                emit(OPR, 0, 12);
                break;
            case geqsym:
                emit(OPR, 0, 13);
                break;
        }
    }
}

void expression() {
    if (tokenType[tokenIndex] == minussym) {
        tokenIndex++;
        term();
        emit(OPR, 0, 1);
        while (tokenType[tokenIndex] == plussym || tokenType[tokenIndex] == minussym) {
            int addOp = tokenType[tokenIndex];
            tokenIndex++;
            term();
            if (addOp == plussym) {
                emit(OPR, 0, 2);
            } else {
                emit(OPR, 0, 3);
            }
        }
    } else {
        if (tokenType[tokenIndex] == plussym) {
            tokenIndex++;
        }
        term();
        while (tokenType[tokenIndex] == plussym || tokenType[tokenIndex] == minussym) {
            int addOp = tokenType[tokenIndex];
            tokenIndex++;
            term();
            if (addOp == plussym) {
                emit(OPR, 0, 2);
            } else {
                emit(OPR, 0, 3);
            }
        }
    }
}

void term() {
    factor();
    while (tokenType[tokenIndex] == multsym || tokenType[tokenIndex] == slashsym) {
        int multOp = tokenType[tokenIndex];
        tokenIndex++;
        factor();
        if (multOp == multsym) {
            emit(OPR, 0, 4);
        } else {
            emit(OPR, 0, 5);
        }
    }
}

void factor() {
    if (tokenType[tokenIndex] == identsym) {
        int symIdx = SYMBOLTABLECHECK(tokenType[tokenIndex]);
        if (symIdx == -1) {
            printf("Error: Undeclared variable.\n");
            exit(1);
        }
        if (symbolTable[symIdx]->kind == 1) {
            emit(LIT, 0, symbolTable[symIdx]->val);
        } else if (symbolTable[symIdx]->kind == 2) {
            emit(LOD, 0, symbolTable[symIdx]->addr);
        } else {
            printf("Error: Expression must not contain a procedure identifier.\n");
            exit(1);
        }
        tokenIndex++;
    }
    else if (tokenType[tokenIndex] == numbersym) {
        emit(LIT, 0, tokenType[tokenIndex]);
        tokenIndex++;
    }
    else if (tokenType[tokenIndex] == lparentsym) {
        tokenIndex++;
        expression();
        if (tokenType[tokenIndex] != rparentsym) {
            printf("Error: Right parenthesis missing.\n");
            exit(1);
        }
        tokenIndex++;
    }
    else {
        printf("Error: The preceding factor cannot begin with this symbol.\n");
        exit(1);
    }
}


