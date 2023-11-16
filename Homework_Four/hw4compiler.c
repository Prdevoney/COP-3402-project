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

// function declarations for parsecodegen part 
symbol *initSymbolTable (int kind, char *name, int val, int level, int addr, int mark);
int symbolTableCheck(char *name, int typeCheck);
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
        printf("tempArr: %s\n", tempArr); 
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
                for (int k = 0; k < norw+1; k++) {
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
        printf(" tokenType: %d, count: %d\n", tokenType[tokenCount-1], tokenCount-1); 
        i++; 
    }

    parsecode: 

    // Call parser codegen function.
    program();

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
        for (int i = 0; i < symbolIndex; i++) {
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
    emit(JMP, 0, 3);
    block();
    // error 1
    // if the program does not end with a period throw an error 
    if (tokenType[tokenIndex] != periodsym) {
        printf("Error: program must end with period\n");
        exit(1);
    }
    emit(SYS, 0, 3);
}

// block ::= const-declaration var-declaration statement
void block () {
    constDeclaration();
    int numVars = varDeclaration();
    procedure(); 
    emit(INC, 0, 3 + numVars);
    statement();
}

// constdeclaration ::= [ “const” ident "=" number {"," ident "=" number} ";"]

void constDeclaration() {
    if (tokenType[tokenIndex] == constsym) {
        // checks structure of the const declaration 
        do {
            tokenIndex++;
            // identity check 
            if (tokenType[tokenIndex] != identsym) {
                printf("Error: const, var, and read must be followed by identifier\n");
                exit(1);
            }
            // has the identifier already been declared? 
            if(symbolTableCheck(identArr[identIndex], 0) != -1) {
                printf("Error: symbol name has already been declared\n");
                exit(1);
            }
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
            // if num then we add (kind, name, L, and M) to the symbol table
            int number = atoi(identArr[identIndex]); // Convert string to integer
            identIndex++; 
            symbolTable[symbolIndex] = initSymbolTable(1, identName, number, currLevel, 0, 0);
            symbolIndex++;
            tokenIndex++;
            // if "," then repeate for next declaration else we break then check for ";"
        } while (tokenType[tokenIndex] == commasym);

        // check if const declaration is ended with a ";"
        if (tokenType[tokenIndex] != semicolonsym) {
            printf("Error: constant and variable declarations must be followed by a semicolon\n"); 
            exit(1);
        }
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
                printf("Error: const, var, and read must be followed by identifier\n");
                exit(1);
            }
            // has the identifier already been declared? 
            if (symbolTableCheck(identArr[identIndex], 0) != -1) {
                printf("Error: This variable has already been declared: %s\ntokenIndex: %d", identArr[identIndex], tokenIndex);
                exit(1);
            }

            // if valid identifier then initialize it in symbolTable 
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
    while (tokenType[tokenIndex] == procsym) {
        currLevel++; 
        tokenIndex++; 
        if (tokenType[tokenIndex] != identsym) {
            printf("Error: procedure error 1"); 
            exit(1); 
        }
        symbolTable[symbolIndex] = initSymbolTable(3, identArr[identIndex], 0, currLevel, 0, 0);
        // printf("%d, %s, %d, %d, %d, %d\n", symbolTable[symbolIndex]->kind, symbolTable[symbolIndex]->name, symbolTable[symbolIndex]->val, symbolTable[symbolIndex]->level, symbolTable[symbolIndex]->addr, symbolTable[symbolIndex]->mark);
        symbolIndex++; 
        tokenIndex++; 
        identIndex++; 

        if (tokenType[tokenIndex] != semicolonsym) {
            printf("Error: procedure error 2"); 
            printf("--%d--\n", tokenIndex); 
            exit(1); 
        }
        tokenIndex++; 
        block(); 
        if (tokenType[tokenIndex] != semicolonsym) {
            printf("Error: procedure error 3 "); 
            printf("--%d--\n", tokenIndex); 
            exit(1); 
        }
        /* 
        go through the symbolTable when you leave a procedure. (go down a level) 
        if a variable was declared in that procedure it will have the same level 
        as that procedure; so, decrement curr level because you left the procedure 
        then see what variables in the symbol table were declared in that procedure 
        by checking their level and if it is greater than the currLevel, then set the 
        mark to 1, because it is no longer available. 

        We just need to find out where to actually put this logic; because, I don't
        know where we exit the procedure. It looks like there are two locations 
        that check for a semicolon in the procedure function. 
        */
            if (currLevel <= symbolTable[i]-> level)
                symbolTable[i]->mark = 1; 
        

        currLevel--; 
        tokenIndex++; 
    }
}

/* 
    statement ::= [ ident ":=" expression | "begin" statement {";" statement} "end" | 
    "if" condition "then" statement | "while" condition "do" statement | "read" ident | 
    "write" ident ] 
 */
void statement() {
    int symIdx, jpcIdx, loopIdx;
    // if identifier 
    if (tokenType[tokenIndex] == identsym) {
        // check to see if in symbolTable 
        symIdx = symbolTableCheck(identArr[identIndex], 1);
        printf("\nidentIndex: %d,\nidentArr: %s,\nsymIdx: %d,\ntokenIndex: %d\n", identIndex, identArr[identIndex], symIdx, tokenIndex); 
        identIndex++; 
        // not in symbolTable
        if (symIdx == -1) {
            printf("Error1: Undeclared identifier: %s\ntokenIndex: %d\n", identArr[identIndex-1], tokenIndex);
            exit(1);
        }
        // not a variable
        if(symbolTable[symIdx]->kind != 2) {
            printf("\nToken Type: %d,\n", tokenType[tokenIndex]);
            printf("name: %s, \nlevel: %d, \nkind: %d, \ntokenIndex: %d\n", symbolTable[symIdx]->name, symbolTable[symIdx]->level, symbolTable[symIdx]->kind, tokenIndex); 
            printf("Error: only variable values may be altered\n");
            exit(1);
        }
        tokenIndex++;
        // if not ":="
        if (tokenType[tokenIndex] != becomessym) {
            printf("Error: assignment statements must use :=\n");
            exit(1);
        }

        tokenIndex++;
        expression();
        emit(STO, currLevel, symbolTable[symIdx]->addr);
        return;
    }
    //======================================= THIS IS NEW =========================
    // if "call"
    if (tokenType[tokenIndex] == callsym){
        tokenIndex++;
        if (tokenType[tokenIndex] != identsym) {
            printf("Error: call must be followed by an identifier\n");
            exit(1);
        }
        symIdx = symbolTableCheck(identArr[identIndex], 1);

        identIndex++; 

        if (symIdx == -1) {
            printf("Error1.1: Undeclared identifier: %s\ntokenIndex: %d\n", identArr[identIndex-1], tokenIndex);
            exit(1);
        }
        tokenIndex++;
        emit(CAL, currLevel, symbolTable[symIdx]->addr); 

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
        emit(STO, currLevel, symbolTable[symIdx]->addr);
        return;
    }
    // if "write"
    if (tokenType[tokenIndex] == writesym) {
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
            printf("Error3: Undeclared identifier: %s\n tokenIndex: %d\n", identArr[identIndex-1], tokenIndex);
            exit(1);
        }
        // const or var
        if (symbolTable[symIdx]->kind == 1) {
            emit(LIT, 0, symbolTable[symIdx]->val);
        } else if(symbolTable[symIdx]->kind == 2) {
            emit(LOD, currLevel, symbolTable[symIdx]->addr);
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
