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
    multsym,  slashsym, ifelsym_8, eqsym, neqsym, lessym, leqsym,
    gtrsym, geqsym, lparentsym, rparentsym, commasym, semicolonsym,
    periodsym, becomessym, beginsym, endsym, ifsym, thensym, 
    whilesym, dosym, callsym_8, constsym, varsym, procsym_8, writesym,
    readsym , elsesym_8
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

/* Initialization of global variables: */
// symbol table array
symbol *symbolTable[MAX_SYMBOL_TABLE_SIZE]; 
int symbolIndex = 0; 

// token list 
int *tokenType; 
int tokenIndex = 0; 

// identity array
char **identArr; 
int identIndex = 0; 

instruction code[CODE_SIZE]; // code array
int cx = 0;   // starting index.

// function declarations for parsecodegen part 
symbol *initSymbolTable (int kind, char *name, int val, int level, int addr);
int symbolTableCheck(char *name);
void emit(int op, int l, int m);
void program();
void block ();
void constDeclaration();
int varDeclaration();
void statement();
void condition();
void expression();
void term();
void factor();

// ------------- The Lex part of the compiler ------------- 
int main(int argc, char *argv[]){
    // Reserved words (keywords). 
    char * resWords [] = {"const", "var", "ifel_8", 
                        "procedure_8", "begin", "end", "if", 
                        "then",  "else_8", "while", 
                        "do", "call_8", "read", "write"};

    // What the keywords correspond to. 
    int wsym [] = {constsym, varsym, ifelsym_8, procsym_8,
                    beginsym, endsym, ifsym, thensym, 
                    elsesym_8, whilesym, dosym, callsym_8,
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
    
    while ((ch = fgetc(file)) != EOF) {
        chcount++;
    }

    fclose(file);
    // ****************************************|

    // *******Put input file into array.*******|

    // dynamically allocate memory for the inputArr 
    // char *inputArr = malloc( sizeof(char) * (chcount + 1));
    // char inputArr[chcount+1];
    char inputArr[chcount];
    
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
    while (i < sizeof(inputArr)){
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
                i -= 1; 
                break; 
            }

            // if scaned in keyword, ident, or num follwed by '\0'. 
            if (!isalnum(tempArr[tempArrCount]) && tempArr[tempArrCount] == '\0' && tempArrCount != 0) {
                printf("went over here\n");
                caseCheck = 1; 
                halt = 1; 
                break; 
            }

            // if scanned in keyword, ident, or num follwed by specialChar. 
            else if (!isalnum(tempArr[tempArrCount]) && tempArr[tempArrCount] != '\0' && tempArrCount != 0) {
                 printf("red\n");
                caseCheck = 1; 
                tempArr[tempArrCount] = '\0';
                i--;
                break; 
            }

            // first scan, whitespace, skip 
            else if (tempArr[tempArrCount] == '\0' && tempArrCount == 0) { 
                
                while (inputArr[i] == '\0') {
                    /* if the counter is at the end of the input this will
                     imediately take us to the parsecodegen section of the prog. */
                    if (i == chcount-1) {
                        goto parsecode; // right before: program(); (line 416)
                    }
                    i++; 
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
        printf("Contents of tempArr: %s\n", tempArr); 
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

                    free(identArr);
                    free(tokenType);
                    exit(1); 
                    i++; 
                    free(tempArr);
                }
                // print digit if meets requirements. 
                if (digitCount == tempArrCount && digitCount <= 5) {
                    
                    strcpy(identArr[identCount], tempArr);

                    tokenType[tokenCount] = numbersym; 
                    identCount++; 
                    tokenCount++; 
                    // i++; 
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
                        printf("\tError: Identifier is too long!\n");
                        fprintf(fp,"\tError: Identifier is too long!\n");
                        free(identArr);
                        free(tokenType); 
                        exit(1); 
                    } else {

                        strcpy(identArr[identCount], tempArr); 

                        tokenType[tokenCount] = identsym;
                        tokenCount++;
                        
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
                // change
                i += 1; 
            }
            else if (tempArr[0] == ':' && inputArr[i+1] == '=') {

                tokenType[tokenCount] = becomessym;
                tokenCount++;
                i++;  
            }
            else if (tempArr[0] == '<' && inputArr[i+1] == '=') {

                tokenType[tokenCount] = leqsym;
                tokenCount++;
                i++;
            }
            else if (tempArr[0] == '>' && inputArr[i+1] == '=') {
              
                tokenType[tokenCount] = geqsym;
                tokenCount++;
                i++; 
            }
            else if (tempArr[0] == '<' && inputArr[i+1] == '>') {
                
                tokenType[tokenCount] = neqsym;
                tokenCount++;
                i++; 
            }
            else {
                if (ssym[tempArr[0]] == 4 || ssym[tempArr[0]] == 5 
                    || ssym[tempArr[0]] == 6 || ssym[tempArr[0]] == 7 
                    || ssym[tempArr[0]] == 15 || ssym[tempArr[0]] == 16 
                    || ssym[tempArr[0]] == 9 || ssym[tempArr[0]] == 17 
                    || ssym[tempArr[0]] == 19 || ssym[tempArr[0]] == 11 
                    || ssym[tempArr[0]] == 13 || ssym[tempArr[0]] == 18) {
                    
                    tokenType[tokenCount] = ssym[tempArr[0]];
                    tokenCount++;
                } else {
                    printf("Error: Invalid symbol!\n");
                    fprintf(fp,"Error: Invalid symbol!\n");
                    free(identArr);
                    free(tokenType);
                    exit(1); 
                }
            }
            if (tokenCount == tokenTypeSize-1) {
                tokenTypeSize *= 2; 
                tokenType = realloc(tokenType, sizeof(int) * tokenTypeSize);
            }
            free(tempArr); 
        }
        i++; 
        printf("%d \n", tokenType[tokenCount-1]); 
    }

    parsecode: 
    printf("\ncontents of tokenType array: \n");

    for (int i = 0; i < tokenCount; i++) {
        printf("%d ", tokenType[i]); 
    }
    printf("\n\n"); 

    // Call parser codegen function.
    program();

    printf("\n");
    printf("Line    OP    L    M\n");
    for (int i = 0; i < cx; i++) {
        int assemblyCodeNum = code[i].op;
        char* assemblyInsName;
        char* oprName;
        int oprNum = code[i].m;
        switch (assemblyCodeNum) {
            case 1:
                assemblyInsName = "LIT";
                break;
            case 2:
                assemblyInsName = "OPR";
                break;
            case 3:
                assemblyInsName = "LOD";
                break;
            case 4: 
                assemblyInsName = "STO";
                break;
            case 5:
                assemblyInsName = "CAL";
                break;
            case 6:
                assemblyInsName = "INC";
                break;
            case 7:
                assemblyInsName = "JMP";
                break;
            case 8:
                assemblyInsName = "JPC";
                break;
            case 9:
                assemblyInsName = "SYS";
                break;
        }

        printf(" %2d    %s    %d   %2d\n", i, assemblyInsName, code[i].l, code[i].m);
        
    }


    printf("\n");
  
    printf("Kind | Name        | Value | Level | Address | Mark\n"); 
    for (int i = 0; i < symbolIndex; i++) {
        printf("%4d | %11s | %5d | %5d | %7d |    1\n", symbolTable[i]->kind, 
                                        symbolTable[i]->name, 
                                        symbolTable[i]->val, 
                                        symbolTable[i]->level, 
                                        symbolTable[i]->addr);
    }

    fclose(fp);

    for (int z = 0; z < identCount; z++) {
        free(identArr[z]);
    }

    free(identArr);

    free(tokenType);

    return 0;
}


// ------------- The parser codegen part of the compiler -------------

// creates a struct for each symbol and then returns it to be stored in the symbolTable array 
symbol *initSymbolTable (int kind, char *name, int val, int level, int addr) {
    symbol *s = malloc(sizeof(symbol));
    s->kind = kind;
    strcpy(s->name, name);
    s->val = val;
    s->level = level;
    s->addr = addr;
    return s;
}

// Determines if an identifier is initialized. If yes return index, else return -1 
int symbolTableCheck(char *name) {
    // loop through symbol table to see if identifier is in array. 
    for(int i = 0; i < symbolIndex; i++) {
        printf("Curr contents of symbolTable: %s\n", symbolTable[i]->name); 
        if (strcmp(symbolTable[i]->name, name) == 0)
            return i; 
    }
    return -1;
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
    emit(JMP, 0, 3);
    block();
    // error 1
    // if the program does not end with a period throw an error 
        printf("%d (last token) line: 534\n", tokenType[tokenIndex]);
    if (tokenType[tokenIndex] != periodsym) {
        printf("Error: Period expected.\n");
        exit(1);
    }
    emit(SYS, 0, 3);
}

void block () {
    constDeclaration();
    int numVars = varDeclaration();
    emit(INC, 0, 3 + numVars);
    statement();
}

// constdeclaration ::= [ “const” ident "=" number {"," ident "=" number} ";"]
void constDeclaration() {
    if (tokenType[tokenIndex] == constsym) {
        // checks structure of the const declaration 
        do {
            printf("%d (token) line: 554\n", tokenType[tokenIndex]); 
            tokenIndex++;
            // identity check 
            if (tokenType[tokenIndex] != identsym) {
                printf("Error: const, var, and read must be followed by identifier.\n");
                exit(1);
            }
            // has the identifier already been declared? 
            if(symbolTableCheck(identArr[identIndex]) != -1) {
                printf("Error: This variable has already been declared.\n");
                exit(1);
            }
            // get the identifier from the identArray 
            char * identName = identArr[identIndex];
            identIndex++; 
            printf("%d (token) line: 569\n", tokenType[tokenIndex]); 

            tokenIndex++;
            // "=" check 
            if (tokenType[tokenIndex] != eqsym) {
<<<<<<< HEAD
                printf("Error: constant must be followed by =.\n");
=======
                printf("Error: constant must be assigned with =.\n");
>>>>>>> 9cad54799e6956df015cd465d65ef8b716a07f9d
                exit(1);
            }
            printf("%d (token) line: 577\n", tokenType[tokenIndex]); 

            tokenIndex++;
            // number check 
            if (tokenType[tokenIndex] != numbersym) {
                printf("Error: constants must be assigned an integer value.\n");
                exit(1);
            }
            
            // if num then we add (kind, name, L, and M) to the symbol table
            int number = atoi(identArr[identIndex]); // Convert string to integer
            identIndex++; 
            symbolTable[symbolIndex] = initSymbolTable(1, identName, number, 0, 0);
            symbolIndex++;
            printf("%d (token) line: 590\n", tokenType[tokenIndex]); 

            tokenIndex++;
            // if "," then repeate for next declaration else we break then check for ";"
        } while (tokenType[tokenIndex] == commasym);

        // check if const declaration is ended with a ";"
        if (tokenType[tokenIndex] != semicolonsym) {
            printf("Error: constant and variable declarations must be followed by a semicolon.\n"); 
            exit(1);
        }
            printf("%d (token) line: 601\n", tokenType[tokenIndex]); 

        tokenIndex++;
    }
}

// var-declaration ::= [ "var" ident {"," ident} ";"]
int varDeclaration() {
    int numVars = 0;
    if (tokenType[tokenIndex] == varsym) {
        do {
            numVars++;
            printf("%d (token) line: 613\n", tokenType[tokenIndex]); 

            tokenIndex++;
            // ident check
            if (tokenType[tokenIndex] != identsym) {
                printf("Error: const, var, and read must be followed by identifier.\n");
                exit(1);
            }
            // has the identifier already been declared? 
            if (symbolTableCheck(identArr[identIndex]) != -1) {
                printf("Error: This variable has already been declared.\n");
                exit(1);
            }

            // if valid identifier then initialize it in symbolTable 
            symbolTable[symbolIndex] = initSymbolTable(2, identArr[identIndex], 0, 0, 2 + numVars);
            identIndex++; 
            symbolIndex++; 
            printf("%d (token) line: 631\n", tokenType[tokenIndex]); 

            tokenIndex++;

            // if "," then continue scaning varDeclaration, else break and check for ";"
        } while (tokenType[tokenIndex] == commasym);

        // check if varDeclaration ends with ";"
        if (tokenType[tokenIndex] != semicolonsym) {
            printf("Error: Semicolon or comma missing.\n");
            exit(1);
        }
            printf("%d (token) line: 643\n", tokenType[tokenIndex]); 

        tokenIndex++;
    }
    // return the number of declared variables
    return numVars;
}

void statement() {
    int symIdx, jpcIdx, loopIdx;
    // if identifier 
    if (tokenType[tokenIndex] == identsym) {
        // check to see if in symbolTable 
        symIdx = symbolTableCheck(identArr[identIndex]);
        identArr++; 
        // not in symbolTable
        if (symIdx == -1) {
<<<<<<< HEAD
            printf("Error: 1 Undeclared identifier: %s\n", identArr[identIndex-1]);
=======

            printf("1 Error: Undeclared identifier: %s\n", identArr[identIndex-1]);
>>>>>>> 9cad54799e6956df015cd465d65ef8b716a07f9d
            exit(1);
        }
        // not a variable
        if(symbolTable[symIdx]->kind != 2) {
            printf("Error: only variables can be altered.\n");
            exit(1);
        }
            printf("%d (token) line: 668\n", tokenType[tokenIndex]); 

        tokenIndex++;
        // if not ":="
        if (tokenType[tokenIndex] != becomessym) {
            printf("Error: assignment statements must use :=.\n");
            exit(1);
        }
            printf("%d (token) line: 676\n", tokenType[tokenIndex]); 

        tokenIndex++;
        expression();
        emit(STO, 0, symbolTable[symIdx]->addr);
        return;
    }
    // if "begin" 
    if (tokenType[tokenIndex] == beginsym) {
        do { 
            printf("%d (token) line: 686\n", tokenType[tokenIndex]); 

            tokenIndex++;
            statement();
        } while (tokenType[tokenIndex] == semicolonsym);
        // if not "end"
        if (tokenType[tokenIndex] != endsym) {
            printf("Error: begin must be followed by end.\n");
            exit(1);
        }
            printf("%d (token) line: 696\n", tokenType[tokenIndex]); 

        tokenIndex++;
        return;
    }
    // if "if"
    if (tokenType[tokenIndex] == ifsym) {
            printf("%d (token) line: 703\n", tokenType[tokenIndex]); 

        tokenIndex++;
        condition();
        jpcIdx = cx;
        emit(JPC, 0,  0); // placeholder for M
        if (tokenType[tokenIndex] != thensym) {
            printf("Error: then expected.\n");
            exit(1);
        }
            printf("%d (token) line: 713\n", tokenType[tokenIndex]); 

        tokenIndex++;
        statement();
        code[jpcIdx].m = cx;
        return;
    }
    // if "while" 
    if (tokenType[tokenIndex] == whilesym) {
        loopIdx = cx;
            printf("%d (token) line 723\n", tokenType[tokenIndex]); 

        tokenIndex++;
        condition();
        // if not "do"
        if (tokenType[tokenIndex] != dosym) {
            printf("Error: while must be follwed by do.\n");
            exit(1);
        }
            printf("%d (token) line: 732\n", tokenType[tokenIndex]); 

        tokenIndex++;
        jpcIdx = cx;
        emit(JPC, 0, jpcIdx);
        statement();
        emit(JMP, 0, loopIdx);
        code[jpcIdx].m = cx;
        return;
    }
    // if "read"
    if (tokenType[tokenIndex] == readsym) {
            printf("%d (token) line: 744\n", tokenType[tokenIndex]); 

        tokenIndex++;
        if (tokenType[tokenIndex] != identsym) {
            printf("Error: Assignment to constant or procedure is not allowed.\n");
            exit(1);
        }
        // check to see if identifier is in array 
        symIdx = symbolTableCheck(identArr[identIndex]);
        if (symIdx == -1) {
            printf("2 Error: Undeclared identifier: %s\n", identArr[identIndex-1]);
            exit(1);
        }
        if (symbolTable[symIdx]->kind != 2) {
            printf("Error: Assignment to constant or procedure is not allowed.\n");
            exit(1);
        }
            printf("%d (token) line: 761\n", tokenType[tokenIndex]); 

        tokenIndex++;
        emit(SYS, 0, 2); //read
        emit(STO, 0, symbolTable[symIdx]->addr);
        return;
    }
    // if "write"
    if (tokenType[tokenIndex] == writesym) {
            printf("%d (token) line: 770\n", tokenType[tokenIndex]); 

        tokenIndex++;
        expression();
        emit(SYS, 0, 1); //write
        return;
    }
}

void condition() {
    if (tokenType[tokenIndex] == oddsym) {
            printf("%d (token) line: 781\n", tokenType[tokenIndex]); 

        tokenIndex++;
        expression();
        emit(OPR, 0, 11);
    } else {
        expression();
        if (tokenType[tokenIndex] != eqsym && tokenType[tokenIndex] != neqsym && 
        tokenType[tokenIndex] != lessym && tokenType[tokenIndex] != leqsym && 
        tokenType[tokenIndex] != gtrsym && tokenType[tokenIndex] != geqsym) {
            printf("Error: condition must contain comparision operator.\n");
            exit(1);
        }
        int relOp = tokenType[tokenIndex];
            printf("%d (token) line: 795\n", tokenType[tokenIndex]); 

        tokenIndex++;
        expression();
        switch (relOp) {
            case eqsym:
                emit(OPR, 0, 5);
                break;
            case neqsym:
                emit(OPR, 0, 6);
                break;
            case lessym:
                emit(OPR, 0, 7);
                break;
            case leqsym:
                emit(OPR, 0, 8);
                break;
            case gtrsym:
                emit(OPR, 0, 9);
                break;
            case geqsym:
                emit(OPR, 0, 10);
                break;
        }
    }
}

void expression() {
    if (tokenType[tokenIndex] == minussym) {
            printf("%d (token) line: 824\n", tokenType[tokenIndex]); 

        tokenIndex++;
        term();
        //emit(OPR, 0, 1); what is NEG in the pseudocode??????=======
        while (tokenType[tokenIndex] == plussym || tokenType[tokenIndex] == minussym) {
            int addOp = tokenType[tokenIndex];
            // tokenIndex++;
           // term();
            if (addOp == plussym) {
            printf("%d (token) line: 834\n", tokenType[tokenIndex]); 

                tokenIndex++;
                term();
                emit(OPR, 0, 1);
            } else {
            printf("%d (token) line: 840\n", tokenType[tokenIndex]); 

                tokenIndex++;
                term();
                emit(OPR, 0, 2);
            }
        }
    } else {
        if (tokenType[tokenIndex] == plussym) {
            printf("%d (token) line: 849\n", tokenType[tokenIndex]); 

            tokenIndex++;
        }
        term();
        while (tokenType[tokenIndex] == plussym || tokenType[tokenIndex] == minussym) {
            int addOp = tokenType[tokenIndex];
            // tokenIndex++;
            // term();
            if (addOp == plussym) {
            printf("%d (token) line: 859\n", tokenType[tokenIndex]); 

                tokenIndex++;
                term();
                emit(OPR, 0, 1);
            } else {
            printf("%d (token) line: 865\n", tokenType[tokenIndex]); 

                tokenIndex++;
                term();
                emit(OPR, 0, 2);
            }
        }
    }
}

void term() {
    factor();
    while (tokenType[tokenIndex] == multsym || tokenType[tokenIndex] == slashsym) {
        int multOp = tokenType[tokenIndex];
        // tokenIndex++;
        // factor();
        if (multOp == multsym) {
            printf("%d (token) line: 882\n", tokenType[tokenIndex]); 

            tokenIndex++;
            factor();
            emit(OPR, 0, 3);
        } else {
            printf("%d (token) line: 888\n", tokenType[tokenIndex]); 

            tokenIndex++;
            factor();
            emit(OPR, 0, 4);
        }
    }
}

void factor() {
    if (tokenType[tokenIndex] == identsym) {
        int symIdx = symbolTableCheck(identArr[identIndex]);
        identIndex++; 
        if (symIdx == -1) {
            printf("3 Error: Undeclared identifier: %s\n", identArr[identIndex-1]);
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
            printf("%d (token) line: 913\n", tokenType[tokenIndex]); 

        tokenIndex++;
    }
    else if (tokenType[tokenIndex] == numbersym) {
        emit(LIT, 0, atoi(identArr[identIndex]));
            printf("%d (token) line: 919\n", tokenType[tokenIndex]); 
        // potentially may have to get rid of: identIndex++; 
        identIndex++; 
        tokenIndex++;
    }
    else if (tokenType[tokenIndex] == lparentsym) {
            printf("%d (token) line: 924\n", tokenType[tokenIndex]); 

        tokenIndex++;
        expression();
        if (tokenType[tokenIndex] != rparentsym) {
            printf("Error: right parenthesis must follow left parenthesis.\n");
            exit(1);
        }
            printf("%d (token) line: 932\n", tokenType[tokenIndex]); 

        tokenIndex++;
    }
    else {
        printf("Error: arithmetic equations must contain operands, parentheses, and numbers, or symbols.\n");
        exit(1);
    }
}

