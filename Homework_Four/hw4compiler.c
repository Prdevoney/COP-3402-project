/*
    Patrick DeVoney & Rossella Diorio.
    Homework Assignment Four: Compiler.
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
    whilesym, dosym, callsym, constsym, varsym, procsym, writesym,
    readsym , elsesym_8
} token_type;

typedef struct {
    int kind;      // const = 1, var = 2, proc = 3
    char name[12]; // name up to 11 chars
    int val;       // number (ASCII value)
    int level;     // L level
    int addr;      // M address
    int mark;      // to indicate unavailable or deleted 
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

typedef struct {
    int currCX; 
    char procedureName[12]; 
} calStruct; 

calStruct calArr[50]; 
int calIndexProc = 0; 
int procedureCount = 0; 
int jmpAddress = 0;

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

// keep track of the current level
int currLevel = 0; 

instruction code[CODE_SIZE]; // code array
int cx = 0;   // starting index.
int lexLevel = 0; // current level.

// function declarations for parsecodegen part 
symbol *initSymbolTable (int kind, char *name, int val, int level, int addr, int mark);
int symbolTableCheck(char *name, int typeCheck);
int symSearch (char *name, int thisLevel);
void emit(int op, int l, int m);
void program();
void block ();
void constDeclaration();
int varDeclaration();
void procedure(); 
void statement();
void condition();
void expression();
void term();
void factor();

// ------------- The Lex part of the compiler ------------- 
int main(int argc, char *argv[]){
    // Reserved words (keywords). 
    char * resWords [] = {"odd", "const", "var", "ifel_8", 
                        "procedure", "begin", "end", "if", 
                        "then",  "else_8", "while", 
                        "do", "call", "read", "write"};

    // What the keywords correspond to. 
    int wsym [] = {oddsym, constsym, varsym, ifelsym_8, procsym,
                    beginsym, endsym, ifsym, thensym, 
                    elsesym_8, whilesym, dosym, callsym,
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


    int chcount = 0; 
    int endOfFile = 0; 

    // Print out input file. 
    // **************************************|
    FILE *file = fopen(argv[1], "r");
    char ch;
    if (NULL == file) {
        printf("No input file recieved, please put: 'input.txt' after executable file! \n");
        return 0;
    }
    printf("\nSource Code:\n"); 
    while ((ch = fgetc(file)) != EOF) {
        chcount++;
        printf("%c", ch);
    }

    fclose(file);
    // ****************************************|

    // *******Put input file into array.*******|

    char inputArr[chcount];
    
    FILE *inputFile = fopen(argv[1], "r");
    if (NULL == inputFile) {
        printf("No input file recieved, please put: 'input.txt' after executable file! \n");
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
    // printf("%d", identArr); 

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
        
        printf("i: %d, size: %ld\n", i, sizeof(inputArr));
        printf("----here7   ---\n");
        printf("^^^^^^^^^^^ T O P ^^^^^^^^^\n");
        printf("identArr[0]: %s\n", identArr[0]);
        printf("^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");
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
                printf("----here8   ---\n");
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
                printf("----here9   ---\n");
                while (inputArr[i] == '\0') {
                    /* if the counter is at the end of the input this will
                     imediately take us to the parsecodegen section of the prog. */
                    if (i == chcount-1) {
                        printf("----here10   ---\n");
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
        // printf("tempArr: %s\n", tempArr); 
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
                    
                    // printf("%-12s", tempArr);

                    printf("\tError: Number too long!\n");

                    free(identArr);
                    free(tokenType);
                    exit(1); 
                    i++; 
                    free(tempArr);
                }
                // print digit if meets requirements. 
                if (digitCount == tempArrCount && digitCount <= 5) {
                    printf("======1====== ADDED to ident ====== ---> identArr: %s\n", identArr[identCount]);
                    
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
                    printf("00000000000000000000000here 1.5--\n");
                    identArr = realloc(identArr, sizeof(char*) *identSize);
                    for (int k = identCount; k < identSize; k++)
                        identArr[k] = malloc(sizeof(char) * 12); 
                }
            }
            else {
                //=================== resWords Check ===================
                int keyWordCheck = 0; 
                for (int k = 0; k < norw+1; k++) {
                    if (strcmp(tempArr, resWords[k]) == 0) {
                        printf("went here1\n");
                        // dynamically resize tokenType array if necessary. 
                        if (tokenCount == tokenTypeSize-1) {
                            tokenTypeSize *= 2; 
                            tokenType = realloc(tokenType, sizeof(int) * tokenTypeSize);
                        }
                        printf("^^^^^^^^^^^ MID ^^^^^^^^^\n");
                        printf("identArr[0]: %s\n", identArr[0]);
                        printf("^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");
                        // put the token in the array.
                        
                        tokenType[tokenCount] = wsym[k];
                        if (tokenType[tokenCount] == procsym)
                            procedureCount++; 
                        tokenCount++;
                        printf("^^^^^^^^^^^ MID4 ^^^^^^^^^\n");
                        printf("identArr[0]: %s\n", identArr[0]);
                        printf("^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");
                        keyWordCheck = 1; 
                        printf("^^^^^^^^^^^ MID5 ^^^^^^^^^\n");
                        printf("identArr[0]: %s\n", identArr[0]);
                        printf("^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");
                        free(tempArr);
                        printf("^^^^^^^^^^^ MID2 ^^^^^^^^^\n");
                        printf("identArr[0]: %s\n", identArr[0]);
                        printf("^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");
                    }
                }
                //=================== Identifier Check ===================
                if (keyWordCheck == 0) {
                    printf("^^^^^^^^^^^ crazy ^^^^^^^^^\n");
                        printf("identArr[0]: %s\n", identArr[0]);
                        printf("^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");
                    if (tempArrCount > 11){
                        printf("\tError: Identifier is too long!\n");
                        free(identArr);
                        free(tokenType); 
                        exit(1); 
                    } else {

                        strcpy(identArr[identCount], tempArr); 
                        // print out what is in the idenarra
                        //printf("identArr: %s\n", identArr[identCount]);
                        printf("====2===== ADDED to ident ====== ---> identArr: %s\n", identArr[identCount]);
                        printf("identArr: %s\n", identArr[identCount]);
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
                        printf("00000000111000000000000000here 2.5--\n");
                        identArr = realloc(identArr, sizeof(char*) *identSize);
                        for (int k = identCount; k < identSize; k++)
                            identArr[k] = malloc(sizeof(char) * 12); 
                    }
                    free(tempArr); 
                }
            }
        } 
        else {
            printf("^^^^^^^^^^^ crap ^^^^^^^^^\n");
            printf("identArr[0]: %s\n", identArr[0]);
            printf("^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");
            // ======================= Special Character Check =======================
            if (tempArr[0] == '/' && inputArr[i+1] == '*') {
                i += 2; 
                while (inputArr[i] != '*' || inputArr[i+1] != '/') {
                    i++; 
                }
                printf("i after: %d\n", i);
                printf("i+1 after: %d\n", i+1);
                // print what is in the input arra at that index
                printf("inputArr[i]: %c\n", inputArr[i]);
                printf("inputArr[i+3]: %c\n", inputArr[i+6]);
                // change
                i += 2; 
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
                    printf("----here6---\n");
                } else {
                    printf("Error: Invalid symbol!: %c\n", tempArr[0]);
                    free(identArr);
                    free(tokenType);
                    exit(1); 
                }
            }
            printf("^^^^^^^^^^^ BUTT ^^^^^^^^^\n");
            printf("identArr[0]: %s\n", identArr[0]);
            printf("^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");
            if (tokenCount == tokenTypeSize-1) {
                tokenTypeSize *= 2; 
                tokenType = realloc(tokenType, sizeof(int) * tokenTypeSize);
            }
            free(tempArr); 
        }
        // printf(" tokenType: %d, count: %d\n", tokenType[tokenCount-1], tokenCount-1); 
        i++; 
    }

    parsecode:

    printf("----Why is this bad---\n");
    // print out the first value in the idenarra
    if(identArr[0] == NULL) {
        printf("identArr[0] is NULL\n");
    } else {
        printf("identArr[0] is not NULL\n");
    }
    //print out the address of the first index __valueless
    printf("identArr[0]: %s\n", identArr[0]);
    printf("identArr of the first one: %s\n", identArr[0]);
    // print out all the valuesin the ident array
    
    for (int z = 0; z < identCount; z++) {
        if (z % 4 == 0) {
            printf("\n");
        }
        if(identArr[z] != NULL) {
            printf("identArr[%d]: %s |", z, identArr[z]);
        } else {
            printf("identArr[%d] is NULL\n", z);
        }
    }


    // Call parser codegen function.
    program();
    printf("----here4---\n");

    FILE *outputFile = fopen("elf.txt", "w");

    // -----Print out the parser output-----.

    printf("\nNo errors, program is syntactically correct\n\n");
    printf("Assembly Code:\n\n");
    
    printf("Line    OP    L    M\n");
    for (int i = 0; i < cx; i++) {
        int assemblyCodeNum = code[i].op;
        char* assemblyInsName = "";
        char* oprName = "";
        int oprNum = code[i].m;
        switch (assemblyCodeNum) {
            case 0:
                assemblyInsName = "RTN";
                break;
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
        // print out to elf file
        fprintf(outputFile, "%2d    %2d    %2d\n",code[i].op, code[i].l, code[i].m);        
    }
    
    printf("\n");
    printf("Symbol Table:\n\n");

    printf("Kind | Name        | Value | Level | Address | Mark\n"); 
    for (int i = 0; i < symbolIndex; i++) {
        printf("%4d | %11s | %5d | %5d | %7d | %4d\n", symbolTable[i]->kind, 
                                        symbolTable[i]->name, 
                                        symbolTable[i]->val, 
                                        symbolTable[i]->level, 
                                        symbolTable[i]->addr,
                                        symbolTable[i]->mark);
    }

    // free memory
    for (int i = 0; i < symbolIndex; i++) {
        free(symbolTable[i]);
        symbolTable[i] = NULL; 
    }

    for (int z = 0; z < identSize; z++) {
        free(identArr[z]);
    }

 
    free(identArr);

    identArr = NULL; 

    free(tokenType);

    return 0;
}


// ------------- The parser codegen part of the compiler -------------

// creates a struct for each symbol and then returns it to be stored in the symbolTable array 
symbol *initSymbolTable (int kind, char *name, int val, int level, int addr, int mark) {
    symbol *s = malloc(sizeof(symbol));
    s->kind = kind;
    strcpy(s->name, name);
    s->val = val;
    s->level = level;
    s->addr = addr;
    s->mark = mark;
    return s;
}

// Determines if an identifier is initialized. If yes return index, else return -1 
int symbolTableCheck(char *name, int typeCheck) {

    // ================== DECLARATION CHECK ===============================
    if (typeCheck == 0) {
        // loop through symbol table to see if identifier is in array. 
        // determines if the identifier can be declared again. 
        // all we need to know is if it has the same name as something else and if it is on the same level 
        for (int i = 0; i < symbolIndex; i++) {
            if (strcmp(symbolTable[i]->name, name) == 0 && symbolTable[i]->level == currLevel) {
                // not eligible to be declared
                return i; 
            }
        }
        // eligible to be declared
        return -1;
    }
    // ********************************************************************

    // ================== STATEMENT CHECK =================================
    else {
        // loop through symbol table to see if identifier is in array. 
        // determines if an identifier can be used in statement. 
        // all we need to know is if it has been declared before and if it is available. 
        for (int i = symbolIndex-1; i >= 0; i--) {
            if (strcmp(symbolTable[i]->name, name) == 0 && symbolTable[i]->mark == 0) {
                // eligible to be used in statement 
                return i; 
            }
        }
        // not eleigible to be used in statement. 
        return -1; 
    }
    // ********************************************************************
}

// creates the assembly instructions and stores them in the code array
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

// program ::= block "."
void program() {
    printf("\n"); 
    block();
    // error 1
    // if the program does not end with a period throw an error 
    if (tokenType[tokenIndex] != periodsym) {
        printf("Error: program must end with period\n");
        exit(1);
    }
    printf("----here12---\n");
    emit(SYS, 0, 3);
}

// block ::= const-declaration var-declaration statement
void block () {
    int jmpadd = cx; 
    emit(JMP, 0, 0); 
    constDeclaration();
    int numVars = varDeclaration();
    procedure(); 
    code[jmpadd].m = cx * 3; 

    calArr[procedureCount].currCX = cx; 
    procedureCount--; 

    emit(INC, 0, 3 + numVars);
    
    statement();
    emit(OPR, 0, 0); 
}

// constdeclaration ::= [ “const” ident "=" number {"," ident "=" number} ";"]
void constDeclaration() {
    if (tokenType[tokenIndex] == constsym) {
        // checks structure of the const declaration 
        printf("----here14---\n");
        do {
            tokenIndex++;
            // identity check 
            if (tokenType[tokenIndex] != identsym) {
                printf("Error: const declaration must be followed by identifier\n");
                exit(1);
            }
            // has the identifier already been declared? 
            if(symbolTableCheck(identArr[identIndex], 0) != -1) {
                printf("Error: symbol name has already been declared\n");
                exit(1);
            }
            printf("----here15---\n");
            // get the identifier from the identArray 
            char * identName = identArr[identIndex];
            identIndex++; 

            tokenIndex++;
            // "=" check 
            if (tokenType[tokenIndex] != eqsym) {
                printf("Error: constant must be assigned with =\n");
                exit(1);
            }

            tokenIndex++;
            // number check 
            if (tokenType[tokenIndex] != numbersym) {
                printf("Error: constants must be assigned an integer value\n");
                exit(1);
            }
            printf("----here16---\n");
            // if num then we add (kind, name, L, and M) to the symbol table
           printf("identIndex: %d\n", identIndex-1);
            printf("identArr: %s\n", identArr[identIndex-1]);
            // print the index fo the idenarra
            
            int number = atoi(identArr[identIndex]); // Convert string to integer
            identIndex++; 
            printf("----here16.5---\n");
            symbolTable[symbolIndex] = initSymbolTable(1, identName, number, currLevel, 0, 0);
            printf("----here16.6---\n");
            symbolIndex++;
            tokenIndex++;
            // if "," then repeate for next declaration else we break then check for ";"
        } while (tokenType[tokenIndex] == commasym);
        printf ("----here17---\n");
        // check if const declaration is ended with a ";"
        if (tokenType[tokenIndex] != semicolonsym) {
            printf("Error: constant and variable declarations must be followed by a semicolon\n"); 
            exit(1);
        }
        printf("----here18---\n");
        tokenIndex++;
    }
}

// var-declaration ::= [ "var" ident {"," ident} ";"]
int varDeclaration() {
    int numVars = 0;
    if (tokenType[tokenIndex] == varsym) {
        do {
            numVars++;
            tokenIndex++;
            // ident check
            if (tokenType[tokenIndex] != identsym) {
                printf("Error: var declaration must be followed by identifier\n");
                exit(1);
            }
            // has the identifier already been declared? 
            if (symbolTableCheck(identArr[identIndex], 0) != -1) {
                printf("Error: This variable has already been declared: %s\ntokenIndex: %d", identArr[identIndex], tokenIndex);
                exit(1);
            }

            // if valid identifier then initialize it in symbolTable 
            // printf("Var Check: %s, %d\n", identArr[identIndex], currLevel); 
            symbolTable[symbolIndex] = initSymbolTable(2, identArr[identIndex], 0, currLevel, 2 + numVars, 0);
            identIndex++; 
            symbolIndex++; 
            tokenIndex++;

            // if "," then continue scaning varDeclaration, else break and check for ";"
        } while (tokenType[tokenIndex] == commasym);

        // check if varDeclaration ends with ";"
        if (tokenType[tokenIndex] != semicolonsym) {
            printf("Error: constant and variable declarations must be followed by a semicolon\n");
            exit(1);
        }
        tokenIndex++;
    }
    // return the number of declared variables
    return numVars;
}

void procedure () {
    printf("----here1---\n");
    while (tokenType[tokenIndex] == procsym) {
        tokenIndex++; 
        if (tokenType[tokenIndex] != identsym) {
            // Error 4
            printf("Error: Procedure must be followed by identifier"); 
            exit(1); 
        }

        strcpy(calArr[calIndexProc].procedureName, identArr[identIndex]); 
        calIndexProc++; 
        // calArr[calIndex].currCX = cx; 
        // calIndex++; 
        jmpAddress++;
        symbolTable[symbolIndex] = initSymbolTable(3, identArr[identIndex], 0, currLevel, jmpAddress, 0);
        symbolIndex++; 
        tokenIndex++; 
        identIndex++; 
        currLevel++; 

        if (tokenType[tokenIndex] != semicolonsym) {
            // Error 6
            printf("Error: Incorrect symbol after procedure declaration\n "); 
            exit(1); 
        }

        tokenIndex++; 
        
        block(); 
        // calArr[calIndex].currCX = cx; 
        // calIndex++; 
        if (tokenType[tokenIndex] != semicolonsym) {
            // Error 17
            printf("Error: Semicolon expected to close procedure"); 
            exit(1); 
        }

        currLevel--; 

        for (int i = 0; i < symbolIndex; i++) {
            if (currLevel < symbolTable[i]->level) {
                symbolTable[i]->mark = 1; 
            }
        }

        tokenIndex++; 
        // emit(OPR, 0, 0); 
    }
}

/* 
    statement ::= [ ident ":=" expression | "begin" statement {";" statement} "end" | 
    "if" condition "then" statement | "while" condition "do" statement | "read" ident | 
    "write" ident ] 
 */
void statement() {
    int symIdx, jpcIdx, loopIdx, levelsDown;
    // if identifier 
    if (tokenType[tokenIndex] == identsym) {
        // check to see if in symbolTable 
        symIdx = symbolTableCheck(identArr[identIndex], 1);
        // printf("\nidentIndex: %d,\nidentArr: %s,\nsymIdx: %d,\ntokenIndex: %d\n", identIndex, identArr[identIndex], symIdx, tokenIndex); 
        identIndex++; 
        // not in symbolTable
        // printf("\nToken: %d   ", tokenType[tokenIndex-1]);
        if (symIdx == -1) {
            printf("Error: Undeclared identifier: %s\ntokenIndex: %d\n", identArr[identIndex-1], tokenIndex);
            exit(1);
        }
        // not a variable
        printf("symbolTable[symIdx]->kind = %s", symbolTable[symIdx]->name);
        if(symbolTable[symIdx]->kind != 2) {
            // Error 12
            printf("Error: Assignment to constant or procedure is not allowed\n");
            exit(1);
        }
        tokenIndex++;
        // if not ":="
        if (tokenType[tokenIndex] != becomessym) {
            printf("Error: assignment statements must use :=\n");
            exit(1);
        }
        printf("----here2---\n");
        tokenIndex++;
        expression();
        levelsDown = currLevel - symbolTable[symIdx]->level;

        emit(STO, levelsDown, symbolTable[symIdx]->addr);
        return;
    }
    //======================================= THIS IS NEW =========================
    // if "call"
    if (tokenType[tokenIndex] == callsym){
        tokenIndex++;
        if (tokenType[tokenIndex] != identsym) {
            // Error 14
            printf("Error: call must be followed by an identifier\n");
            exit(1);
        }
        symIdx = symbolTableCheck(identArr[identIndex], 1);

        if (symIdx == -1) {
            printf("Error: Undeclared identifier: %s\ntokenIndex: %d\n", identArr[identIndex], tokenIndex);
            exit(1);
        }

        if (symbolTable[symIdx]->kind != 3) {
            // Error 15
            printf("Error: call must be followed by a valid procedure name, not a const or var\n");
            exit(1);
        }

        int calAdress = 0; 
        for (int i = 0; i < calIndexProc; i++) {
            if (strcmp(symbolTable[symIdx]->name, calArr[i].procedureName) == 0) {
                printf("\nCAL: \ncalArr[%d].procedureName: %s\ncalArr[%d].currCX: %d\n", i, calArr[i].procedureName, i, calArr[i].currCX); 
                calAdress = calArr[i+1].currCX; 
                break; 
            }
        }
        printf("calAdress: %d\n", calAdress); 

        identIndex++; 
        tokenIndex++;
        levelsDown = currLevel - symbolTable[symIdx]->level;

        emit(CAL, levelsDown, symbolTable[symIdx]->addr * 3); 

        return;
    }
    // ====================================== END NEW =============================
    // if "begin" 
    if (tokenType[tokenIndex] == beginsym) {
        do { 
            tokenIndex++;
            statement();
        } while (tokenType[tokenIndex] == semicolonsym);
        // if not "end"
        if (tokenType[tokenIndex] != endsym) {
            printf("Error: begin must be followed by end\n");
            exit(1);
        }
        tokenIndex++;
        return;
    }
    // if "if"
    if (tokenType[tokenIndex] == ifsym) {
        tokenIndex++;
        condition();
        jpcIdx = cx;
        emit(JPC, 0,  0); // placeholder for M
        if (tokenType[tokenIndex] != thensym) {
            printf("Error: if must be followed by then\n");
            exit(1);
        }
        tokenIndex++;
        statement();
        code[jpcIdx].m = cx * 3;
        return;
    }
    // if "while" 
    if (tokenType[tokenIndex] == whilesym) {
        loopIdx = cx;
        tokenIndex++;
        condition();
        // if not "do"
        if (tokenType[tokenIndex] != dosym) {
            printf("Error: while must be follwed by do\n");
            exit(1);
        }
        tokenIndex++;
        jpcIdx = cx;
        emit(JPC, 0, jpcIdx * 3);
        statement();
        
        emit(JMP, 0, loopIdx * 3);
        code[jpcIdx].m = cx * 3;
        return;
    }
    // if "read"
    if (tokenType[tokenIndex] == readsym) {
        tokenIndex++;
        if (tokenType[tokenIndex] != identsym) {
            printf("Error: input must be an identifier\n");
            exit(1);
        }
        // check to see if identifier is in array 
        symIdx = symbolTableCheck(identArr[identIndex], 1);
        identIndex++; 
        if (symIdx == -1) {
                printf("Error2: Undeclared identifier: %s\n tokenIndex: %d\n", identArr[identIndex-1], tokenIndex);
            exit(1);
        }
        if (symbolTable[symIdx]->kind != 2) {
            printf("Error: not a variable\n");
            exit(1);
        }
        tokenIndex++;
        emit(SYS, 0, 2); //read
        printf("#2 %s, %d, %d\n", symbolTable[symIdx]->name, symbolTable[symIdx]->level, symbolTable[symIdx]->addr);


        levelsDown = currLevel - symbolTable[symIdx]->level; 
        emit(STO, levelsDown, symbolTable[symIdx]->addr);
        return;
    }
    // if "write"
    if (tokenType[tokenIndex] == writesym) {
        printf("----here3---\n");
        tokenIndex++;
        expression();
        emit(SYS, 0, 1); //write
        return;
    }
}

// condition ::= "odd" expression | expression rel-op expression
void condition() {
    // if "odd"
    if (tokenType[tokenIndex] == oddsym) {
        tokenIndex++;
        expression();
        emit(OPR, 0, 11);
    } else {
        expression();
        // if not a relational operator
        if (tokenType[tokenIndex] != eqsym && tokenType[tokenIndex] != neqsym && 
        tokenType[tokenIndex] != lessym && tokenType[tokenIndex] != leqsym && 
        tokenType[tokenIndex] != gtrsym && tokenType[tokenIndex] != geqsym) {
            printf("Error: condition must contain comparision operator\n");
            exit(1);
        }
        // relational operator
        int relOp = tokenType[tokenIndex];
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

// expression ::=  term { ("+"|"-") term}..
void expression() {
    term();
    while (tokenType[tokenIndex] == plussym || tokenType[tokenIndex] == minussym) {
        int addOp = tokenType[tokenIndex];
        if (addOp == plussym) {
            tokenIndex++;
            term();
            emit(OPR, 0, 1);
        } else {
            tokenIndex++;
            term();
            emit(OPR, 0, 2);
        }
    }
}

void term() {
    factor();
    // if "*" or "/"
    while (tokenType[tokenIndex] == multsym || tokenType[tokenIndex] == slashsym) {
        int multOp = tokenType[tokenIndex];
        if (multOp == multsym) {
            tokenIndex++;
            factor();
            emit(OPR, 0, 3);
        } else {
            tokenIndex++;
            factor();
            emit(OPR, 0, 4);
        }
    }
}

void factor() {
    // if identifier
    if (tokenType[tokenIndex] == identsym) {
        int symIdx = symbolTableCheck(identArr[identIndex], 1);
        identIndex++; 
        if (symIdx == -1) {
            printf("Error: undeclared identifier: %s\n tokenIndex: %d\n", identArr[identIndex-1], tokenIndex);
            exit(1);
        }
        // error 21
        if (symbolTable[symIdx]->kind == 3) {
            printf("Error: expression must not contain a procedure identifier\n");
            exit(1);
        }
        // const or var
        if (symbolTable[symIdx]->kind == 1) {
            emit(LIT, 0, symbolTable[symIdx]->val);
        } else if(symbolTable[symIdx]->kind == 2) {
            int levelsDown = currLevel - symbolTable[symIdx]->level;
            emit(LOD, levelsDown, symbolTable[symIdx]->addr);
        }
        tokenIndex++;
    }
    // if number
    else if (tokenType[tokenIndex] == numbersym) {
        emit(LIT, 0, atoi(identArr[identIndex]));
        identIndex++; 
        tokenIndex++;
    }
    //  if "("
    else if (tokenType[tokenIndex] == lparentsym) {
        tokenIndex++;
        expression();
        // if not ")"
        if (tokenType[tokenIndex] != rparentsym) {
            printf("Error: right parenthesis must follow left parenthesis\n");
            exit(1);
        }
        tokenIndex++;
    }
    else {
        printf("Error: arithmetic equations must contain operands, parentheses, and numbers, or symbols\n");
        exit(1);
    }
}
