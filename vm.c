/* 
    Patrick DeVoney
    Rossella Diorio
    Euripides Montagne
    COP 3402 Fall 2023
    HomeWork 1: Virtual Machine
*/

#include <stdio.h>
#include <stdlib.h>

#define ARRAY_SIZE 512

int main (int argc, char *argv[]) {

    FILE *file = fopen(argv[1], "r");
    if (file == NULL) {
        printf("No input file recieved, please put: 'elf.txt' after executable file! \n"); 
        return 0; 
    }

    int pas[ARRAY_SIZE]; 
    int i = 0; 
    int count = 0; 

    // Read in the input from the elf file. 
    while (fscanf(file, "%d %d %d", &pas[i], &pas[i + 1], &pas[i +2]) == 3) {
        i += 3; 
        count += 1; 
    }

    // Variable Party 
    int BP = count * 3;     // base pointer
    int textLength = BP;    // end of the textfile length
    int SP = BP - 1;        // stack pointer
    int PC = 0;             // program counter
    int OP = 0;             // operation code
    int L = 0;              // Lexographical level
    int M = 0;              // different things M
    int arCount = 0;        // Activation Record Counter
    int outerLoop = 0;      // OuterLoop counter
    int sec = arCount - 1;  // Registers Left
    int arb = 0;
    int halt = 1;

    // put 0's into the rest of the empty PAS
    for(int r = textLength; r < ARRAY_SIZE; r++)
    {
        pas[r] = 0;
    }

   printf("\t\tPC  BP  SP  stack"); 
   printf("\nInitial values:%2d  %3d  %2d\n", PC, BP, SP);

    while (halt != 0)
    {
        OP = pas[PC];
        //printf("\n PC before: %d\n", PC);
        switch (OP) {
            // ==================== LIT ====================
            case 1:
                // Update specifically for each instruction
                L = pas[PC+1];
                M = pas[PC+2];
                SP += 1;
                PC += 3;
                pas[SP] = M; 
                
                printf("    LIT %-3d %-3d %-3d %-3d %-3d ", L, M, PC, BP, SP);
                // loop to print out activation records. 
                // copy and pasted this loop into every case. 
                outerLoop = 0;
                sec = arCount - 1; 
                while (outerLoop <= arCount) {
                    int currBPIndex = BP; 
                    int prevBPIndex = pas[BP+1];
                    int kCount = 0; 
                    // determines bounds of the activation record to print. 
                    for (int k = 0; k < sec; k++) {
                        prevBPIndex = pas[prevBPIndex+1]; 
                        currBPIndex = pas[currBPIndex+1]; 
                        kCount++; 
                    }
                    // print the activation record.  
                    if (kCount != 0){
                        for (int y = prevBPIndex; y < currBPIndex; y++) 
                            printf("%d ", pas[y]); 
                        printf("| ");
                    }
                    // print the activation record if we are on the last record to print. 
                    else {
                        if (outerLoop == arCount)
                            for (int y = currBPIndex; y < SP + 1; y++)
                                printf("%d ", pas[y]); 
                        if (sec == 0 && kCount == 0) {
                            for (int y = prevBPIndex; y < currBPIndex; y++) 
                                printf("%d ", pas[y]); 
                            printf("| ");
                        }
                    }
                    sec--; 
                    outerLoop++; 
                }
                printf("\n");
                break;

            // ==================== OPR ====================
            case 2:
                // Updates specifically for OPR instructions
                L = pas[PC+1];
                M = pas[PC+2];
                PC += 3;
                switch(M) {
                    // ========== 2 RTN ==========
                    case 0:
                        // Updates specifically for instruction
                        SP = BP - 1; 
                        BP = pas[SP+2]; 
                        PC = pas[SP+3]; 
                        printf("    RTN %-3d %-3d %-3d %-3d %-3d ", L, M, PC, BP, SP);
                        
                        //print loop for multiple AR
                        outerLoop = 0;
                        sec = arCount - 2; 
                        arCount -= 1; 
                        while (outerLoop <= arCount) {
                            int currBPIndex = BP; 
                            int prevBPIndex = pas[BP+1];
                            int kCount = 0; 
                            for (int k = 0; k < sec; k++) {
                                prevBPIndex = pas[prevBPIndex+1]; 
                                currBPIndex = pas[currBPIndex+1]; 
                                kCount++; 
                            }
                            if (kCount != 0){
                                for (int y = prevBPIndex; y < currBPIndex; y++) 
                                    printf("%d ", pas[y]); 
                                printf("| ");
                            }
                            else {
                                if (outerLoop == arCount)
                                    for (int y = currBPIndex; y < SP + 1; y++)
                                        printf("%d ", pas[y]); 
                                if (sec == 0 && kCount == 0) {
                                    for (int y = prevBPIndex; y < currBPIndex; y++) 
                                        printf("%d ", pas[y]); 
                                    printf("| ");
                                }
                            }
                            sec--; 
                            outerLoop++; 
                        }
                        printf("\n");
                        break; 

                    // ========== 2 ADD ==========
                    case 1: 
                        // Updates specifically for instruction
                        pas[SP-1] = pas[SP-1] + pas[SP]; 
                        SP = SP - 1; 
                        printf("    ADD %-3d %-3d %-3d %-3d %-3d ", L, M, PC, BP, SP);
                        //print loop for multiple AR
                        outerLoop = 0;
                        sec = arCount - 1; 
                        while (outerLoop <= arCount) {
                            int currBPIndex = BP; 
                            int prevBPIndex = pas[BP+1];
                            int kCount = 0; 
                            for (int k = 0; k < sec; k++) {
                                prevBPIndex = pas[prevBPIndex+1]; 
                                currBPIndex = pas[currBPIndex+1]; 
                                kCount++; 
                            }
                            if (kCount != 0){
                                for (int y = prevBPIndex; y < currBPIndex; y++) 
                                    printf("%d ", pas[y]); 
                                printf("| ");
                            }
                            else {
                                if (outerLoop == arCount)
                                    for (int y = currBPIndex; y < SP + 1; y++)
                                        printf("%d ", pas[y]); 
                                if (sec == 0 && kCount == 0) {
                                    for (int y = prevBPIndex; y < currBPIndex; y++) 
                                        printf("%d ", pas[y]); 
                                    printf("| ");
                                }
                            }
                            sec--; 
                            outerLoop++; 
                        }
                        printf("\n");
                        break;

                    // ========== 2 SUB ==========
                    case 2: 
                        // Updates specifically for instruction
                        pas[SP-1] = pas[SP-1] - pas[SP]; 
                        SP = SP - 1; 
                        printf("    SUB %-3d %-3d %-3d %-3d %-3d ", L, M, PC, BP, SP);
                        //print loop for multiple AR
                        outerLoop = 0;
                        sec = arCount - 1; 
                        while (outerLoop <= arCount) {
                            int currBPIndex = BP; 
                            int prevBPIndex = pas[BP+1];
                            int kCount = 0; 
                            for (int k = 0; k < sec; k++) {
                                prevBPIndex = pas[prevBPIndex+1]; 
                                currBPIndex = pas[currBPIndex+1]; 
                                kCount++; 
                            }
                            if (kCount != 0){
                                for (int y = prevBPIndex; y < currBPIndex; y++) 
                                    printf("%d ", pas[y]); 
                                printf("| ");
                            }
                            else {
                                if (outerLoop == arCount)
                                    for (int y = currBPIndex; y < SP + 1; y++)
                                        printf("%d ", pas[y]); 
                                if (sec == 0 && kCount == 0) {
                                    for (int y = prevBPIndex; y < currBPIndex; y++) 
                                        printf("%d ", pas[y]); 
                                    printf("| ");
                                }
                            }
                            sec--; 
                            outerLoop++; 
                        }
                        printf("\n");
                        break;

                    // ========== 2 MUL ==========
                    case 3: 
                        // Updates specifically for instruction
                        pas[SP-1] = pas[SP-1] * pas[SP]; 
                        SP = SP - 1; 
                        printf("    MUL %-3d %-3d %-3d %-3d %-3d ", L, M, PC, BP, SP);
                        //print loop for multiple AR
                        outerLoop = 0;
                        sec = arCount - 1; 
                        while (outerLoop <= arCount) {
                            int currBPIndex = BP; 
                            int prevBPIndex = pas[BP+1];
                            int kCount = 0; 
                            for (int k = 0; k < sec; k++) {
                                prevBPIndex = pas[prevBPIndex+1]; 
                                currBPIndex = pas[currBPIndex+1]; 
                                kCount++; 
                            }
                            if (kCount != 0){
                                for (int y = prevBPIndex; y < currBPIndex; y++) 
                                    printf("%d ", pas[y]); 
                                printf("| ");
                            }
                            else {
                                if (outerLoop == arCount)
                                    for (int y = currBPIndex; y < SP + 1; y++)
                                        printf("%d ", pas[y]); 
                                if (sec == 0 && kCount == 0) {
                                    for (int y = prevBPIndex; y < currBPIndex; y++) 
                                        printf("%d ", pas[y]); 
                                    printf("| ");
                                }
                            }
                            sec--; 
                            outerLoop++; 
                        }
                        printf("\n");
                        break;

                    // ========== 2 DIV ==========
                    case 4: 
                        // Updates specifically for instruction
                        pas[SP-1] = pas[SP-1] / pas[SP]; 
                        SP = SP - 1; 
                        printf("    DIV %-3d %-3d %-3d %-3d %-3d ", L, M, PC, BP, SP);
                        //print loop for multiple AR
                        outerLoop = 0;
                        sec = arCount - 1; 
                        while (outerLoop <= arCount) {
                            int currBPIndex = BP; 
                            int prevBPIndex = pas[BP+1];
                            int kCount = 0; 
                            for (int k = 0; k < sec; k++) {
                                prevBPIndex = pas[prevBPIndex+1]; 
                                currBPIndex = pas[currBPIndex+1]; 
                                kCount++; 
                            }
                            if (kCount != 0){
                                for (int y = prevBPIndex; y < currBPIndex; y++) 
                                    printf("%d ", pas[y]); 
                                printf("| ");
                            }
                            else {
                                if (outerLoop == arCount)
                                    for (int y = currBPIndex; y < SP + 1; y++)
                                        printf("%d ", pas[y]); 
                                if (sec == 0 && kCount == 0) {
                                    for (int y = prevBPIndex; y < currBPIndex; y++) 
                                        printf("%d ", pas[y]); 
                                    printf("| ");
                                }
                            }
                            sec--; 
                            outerLoop++; 
                        }
                        printf("\n");
                        break;

                    // ========== 2 EQL ==========
                    case 5: 
                        // Updates specifically for instruction
                        if (pas[SP-1] == pas[SP]) {
                            pas[SP-1] = 1; 
                            SP = SP - 1; 
                        }
                        else {
                            pas[SP-1] = 0; 
                            SP = SP - 1; 
                        }
                        printf("    EQL %-3d %-3d %-3d %-3d %-3d ", L, M, PC, BP, SP);
                        //print loop for multiple AR
                        outerLoop = 0;
                        sec = arCount - 1; 
                        while (outerLoop <= arCount) {
                            int currBPIndex = BP; 
                            int prevBPIndex = pas[BP+1];
                            int kCount = 0; 
                            for (int k = 0; k < sec; k++) {
                                prevBPIndex = pas[prevBPIndex+1]; 
                                currBPIndex = pas[currBPIndex+1]; 
                                kCount++; 
                            }
                            if (kCount != 0){
                                for (int y = prevBPIndex; y < currBPIndex; y++) 
                                    printf("%d ", pas[y]); 
                                printf("| ");
                            }
                            else {
                                if (outerLoop == arCount)
                                    for (int y = currBPIndex; y < SP + 1; y++)
                                        printf("%d ", pas[y]); 
                                if (sec == 0 && kCount == 0) {
                                    for (int y = prevBPIndex; y < currBPIndex; y++) 
                                        printf("%d ", pas[y]); 
                                    printf("| ");
                                }
                            }
                            sec--; 
                            outerLoop++; 
                        }
                        printf("\n");
                        break;

                    // ========== 2 NEQ ==========
                    case 6: 
                        // Updates specifically for instruction
                        if (pas[SP-1] != pas[SP]) {
                            pas[SP-1] = 1; 
                            SP = SP - 1; 
                        }
                        else {
                            pas[SP-1] = 0; 
                            SP = SP - 1; 
                        }
                        printf("    NEQ %-3d %-3d %-3d %-3d %-3d ", L, M, PC, BP, SP);
                        //print loop for multiple AR
                        outerLoop = 0;
                        sec = arCount - 1; 
                        while (outerLoop <= arCount) {
                            int currBPIndex = BP; 
                            int prevBPIndex = pas[BP+1];
                            int kCount = 0; 
                            for (int k = 0; k < sec; k++) {
                                prevBPIndex = pas[prevBPIndex+1]; 
                                currBPIndex = pas[currBPIndex+1]; 
                                kCount++; 
                            }
                            if (kCount != 0){
                                for (int y = prevBPIndex; y < currBPIndex; y++) 
                                    printf("%d ", pas[y]); 
                                printf("| ");
                            }
                            else {
                                if (outerLoop == arCount)
                                    for (int y = currBPIndex; y < SP + 1; y++)
                                        printf("%d ", pas[y]); 
                                if (sec == 0 && kCount == 0) {
                                    for (int y = prevBPIndex; y < currBPIndex; y++) 
                                        printf("%d ", pas[y]); 
                                    printf("| ");
                                }
                            }
                            sec--; 
                            outerLoop++; 
                        }
                        printf("\n");
                        break;
                        
                    // ========== 2 LSS ==========
                    case 7: 
                        // Updates specifically for instruction
                        if (pas[SP-1] < pas[SP]) {
                            pas[SP-1] = 1; 
                            SP = SP - 1; 
                        }
                        else {
                            pas[SP-1] = 0; 
                            SP = SP - 1; 
                        }
                        printf("    LSS %-3d %-3d %-3d %-3d %-3d ", L, M, PC, BP, SP);
                        //print loop for multiple AR
                        outerLoop = 0;
                        sec = arCount - 1; 
                        while (outerLoop <= arCount) {
                            int currBPIndex = BP; 
                            int prevBPIndex = pas[BP+1];
                            int kCount = 0; 
                            for (int k = 0; k < sec; k++) {
                                prevBPIndex = pas[prevBPIndex+1]; 
                                currBPIndex = pas[currBPIndex+1]; 
                                kCount++; 
                            }
                            if (kCount != 0){
                                for (int y = prevBPIndex; y < currBPIndex; y++) 
                                    printf("%d ", pas[y]); 
                                printf("| ");
                            }
                            else {
                                if (outerLoop == arCount)
                                    for (int y = currBPIndex; y < SP + 1; y++)
                                        printf("%d ", pas[y]); 
                                if (sec == 0 && kCount == 0) {
                                    for (int y = prevBPIndex; y < currBPIndex; y++) 
                                        printf("%d ", pas[y]); 
                                    printf("| ");
                                }
                            }
                            sec--; 
                            outerLoop++; 
                        }
                        printf("\n");
                        break;

                    // ========== 2 LEQ ==========
                    case 8: 
                        // Updates specifically for instruction
                        if (pas[SP-1] <= pas[SP]) {
                            pas[SP-1] = 1; 
                            SP = SP - 1; 
                        }
                        else {
                            pas[SP-1] = 0; 
                            SP = SP - 1; 
                        }
                        printf("    LEQ %-3d %-3d %-3d %-3d %-3d ", L, M, PC, BP, SP);
                        //print loop for multiple AR
                        outerLoop = 0;
                        sec = arCount - 1; 
                        while (outerLoop <= arCount) {
                            int currBPIndex = BP; 
                            int prevBPIndex = pas[BP+1];
                            int kCount = 0; 
                            for (int k = 0; k < sec; k++) {
                                prevBPIndex = pas[prevBPIndex+1]; 
                                currBPIndex = pas[currBPIndex+1]; 
                                kCount++; 
                            }
                            if (kCount != 0){
                                for (int y = prevBPIndex; y < currBPIndex; y++) 
                                    printf("%d ", pas[y]); 
                                printf("| ");
                            }
                            else {
                                if (outerLoop == arCount)
                                    for (int y = currBPIndex; y < SP + 1; y++)
                                        printf("%d ", pas[y]); 
                                if (sec == 0 && kCount == 0) {
                                    for (int y = prevBPIndex; y < currBPIndex; y++) 
                                        printf("%d ", pas[y]); 
                                    printf("| ");
                                }
                            }
                            sec--; 
                            outerLoop++; 
                        }
                        printf("\n");
                        break;

                    // ========== 2 GTR ==========
                    case 9: 
                        // Updates specifically for instruction
                        if (pas[SP-1] > pas[SP]) {
                            pas[SP-1] = 1; 
                            SP = SP - 1; 
                        }
                        else {
                            pas[SP-1] = 0; 
                            SP = SP - 1; 
                        }
                        printf("    GTR %-3d %-3d %-3d %-3d %-3d ", L, M, PC, BP, SP);
                        //print loop for multiple AR
                        outerLoop = 0;
                        sec = arCount - 1; 
                        while (outerLoop <= arCount) {
                            int currBPIndex = BP; 
                            int prevBPIndex = pas[BP+1];
                            int kCount = 0; 
                            for (int k = 0; k < sec; k++) {
                                prevBPIndex = pas[prevBPIndex+1]; 
                                currBPIndex = pas[currBPIndex+1]; 
                                kCount++; 
                            }
                            if (kCount != 0){
                                for (int y = prevBPIndex; y < currBPIndex; y++) 
                                    printf("%d ", pas[y]); 
                                printf("| ");
                            }
                            else {
                                if (outerLoop == arCount)
                                    for (int y = currBPIndex; y < SP + 1; y++)
                                        printf("%d ", pas[y]); 
                                if (sec == 0 && kCount == 0) {
                                    for (int y = prevBPIndex; y < currBPIndex; y++) 
                                        printf("%d ", pas[y]); 
                                    printf("| ");
                                }
                            }
                            sec--; 
                            outerLoop++; 
                        }
                        printf("\n");
                        break;

                    // ========== 2 GEQ ==========
                    case 10:
                        // Updates specifically for instruction
                        if (pas[SP-1] >= pas[SP]) {
                            pas[SP-1] = 1; 
                            SP = SP - 1; 
                        }
                        else {
                            pas[SP-1] = 0; 
                            SP = SP - 1; 
                        }
                        printf("    GEQ %-3d %-3d %-3d %-3d %-3d ", L, M, PC, BP, SP);
                        //print loop for multiple AR
                        outerLoop = 0;
                        sec = arCount - 1; 
                        while (outerLoop <= arCount) {
                            int currBPIndex = BP; 
                            int prevBPIndex = pas[BP+1];
                            int kCount = 0; 
                            for (int k = 0; k < sec; k++) {
                                prevBPIndex = pas[prevBPIndex+1]; 
                                currBPIndex = pas[currBPIndex+1]; 
                                kCount++; 
                            }
                            if (kCount != 0){
                                for (int y = prevBPIndex; y < currBPIndex; y++) 
                                    printf("%d ", pas[y]); 
                                printf("| ");
                            }
                            else {
                                if (outerLoop == arCount)
                                    for (int y = currBPIndex; y < SP + 1; y++)
                                        printf("%d ", pas[y]); 
                                if (sec == 0 && kCount == 0) {
                                    for (int y = prevBPIndex; y < currBPIndex; y++) 
                                        printf("%d ", pas[y]); 
                                    printf("| ");
                                }
                            }
                            sec--; 
                            outerLoop++; 
                        }
                        printf("\n");
                        break;

                    default:
                        printf("Not valid M for OP\n");
                        break;
                }
                break;
            
            // ==================== LOD ====================
            case 3:
                // printf("---%d---\n", pas[SP]); 

                // Updates specifically for instruction
                L = pas[PC+1];
                M = pas[PC+2];
                PC += 3;
                SP += 1;
                arb = BP;

                // printf("\nBefore L: %d  arb: %d", L,arb);
                
                // find AR base L levels down
                while (L > 0) {
                    arb = pas[arb];
                    L--;
                }

                L = pas[PC-2]; 

                // printf("\nAfter L: %d  arb: %d\n",L,arb);

                pas[SP] = pas[arb + M];
                printf("    LOD %-3d %-3d %-3d %-3d %-3d ", L, M, PC, BP, SP);
                // printf(" \n %d \n", pas[SP]); 

                //print loop for multiple AR
                outerLoop = 0;
                sec = arCount - 1; 
                while (outerLoop <= arCount) {
                    int currBPIndex = BP; 
                    int prevBPIndex = pas[BP+1];
                    int kCount = 0; 
                    for (int k = 0; k < sec; k++) {
                        prevBPIndex = pas[prevBPIndex+1]; 
                        currBPIndex = pas[currBPIndex+1]; 
                        kCount++; 
                    }
                    if (kCount != 0){
                        for (int y = prevBPIndex; y < currBPIndex; y++) 
                            printf("%d ", pas[y]); 
                        printf("| ");
                    }
                    else {
                        if (outerLoop == arCount)
                            for (int y = currBPIndex; y < SP + 1; y++)
                                printf("%d ", pas[y]); 
                        if (sec == 0 && kCount == 0) {
                            for (int y = prevBPIndex; y < currBPIndex; y++) 
                                printf("%d ", pas[y]); 
                            printf("| ");
                        }
                    }
                    sec--; 
                    outerLoop++; 
                }
                printf("\n");
                break;

            // ==================== STO ====================
            case 4:
                // Updates specifically for instruction
                L = pas[PC+1];
                M = pas[PC+2];
                PC += 3;
                arb = BP;

                // find AR base L levels down
                while (L > 0) {
                    arb = pas[arb];
                    L--;
                }

                L = pas[PC-2]; 

                pas[arb+M] = pas[SP]; 
                SP -= 1; 

                printf("    STO %-3d %-3d %-3d %-3d %-3d ", L, M, PC, BP, SP);
                //print loop for multiple AR
                outerLoop = 0;
                sec = arCount - 1; 
                while (outerLoop <= arCount) {
                    int currBPIndex = BP; 
                    int prevBPIndex = pas[BP+1];
                    int kCount = 0; 
                    for (int k = 0; k < sec; k++) {
                        prevBPIndex = pas[prevBPIndex+1]; 
                        currBPIndex = pas[currBPIndex+1]; 
                        kCount++; 
                    }
                    if (kCount != 0){
                        for (int y = prevBPIndex; y < currBPIndex; y++) 
                            printf("%d ", pas[y]); 
                        printf("| ");
                    }
                    else {
                        if (outerLoop == arCount) 
                            for (int y = currBPIndex; y < SP + 1; y++)
                                printf("%d ", pas[y]); 
                        if (sec == 0 && kCount == 0) {
                            for (int y = prevBPIndex; y < currBPIndex; y++) 
                                printf("%d ", pas[y]); 
                            printf("| ");
                        }
                    }
                    sec--; 
                    outerLoop++; 
                }
                
                printf("\n"); 
                break;

            // ==================== CAL ====================
            case 5:
                // Updates specifically for instruction
                arCount++; 
                L = pas[PC+1];
                M = pas[PC+2];
                PC += 3;
                arb = BP;

                // find AR base L levels down
                while (L > 0) {
                    arb = pas[arb];
                    L--;
                }

                L = pas[PC-2];
                
                // creating new AR
                pas[SP+1] = arb;         // static link (SL)
                pas[SP+2] = BP;          // dynamic link (DL)
                pas[SP+3] = PC;          // return address (RA)
                BP = SP + 1;
                PC = M;
               
                printf("    CAL %-3d %-3d %-3d %-3d %-3d ", L, M, PC, BP, SP);
                //print loop for multiple AR
                outerLoop = 0;
                sec = arCount - 1; 
                int tempArCount = arCount - 1; 
                while (outerLoop <= tempArCount) {
                    int currBPIndex = BP; 
                    int prevBPIndex = pas[BP+1];
                    int kCount = 0; 
                    for (int k = 0; k < sec; k++) {
                        prevBPIndex = pas[prevBPIndex+1]; 
                        currBPIndex = pas[currBPIndex+1]; 
                        kCount++; 
                    }
                    if (kCount != 0){
                        if (outerLoop != 0)
                            printf("| "); 
                        for (int y = prevBPIndex; y < currBPIndex; y++) 
                            printf("%d ", pas[y]); 
                    }
                    else {
                        if (outerLoop != 0)
                            printf("| "); 
                        if (outerLoop == arCount)
                            for (int y = currBPIndex; y < SP + 1; y++)
                                printf("%d ", pas[y]); 
                        if (sec == 0 && kCount == 0) {
                            for (int y = prevBPIndex; y < currBPIndex; y++) 
                                printf("%d ", pas[y]);
                        }
                    }
                    sec--; 
                    outerLoop++; 
                }
                printf("\n");
                break;

            // ==================== INC ====================
            case 6:
                // Updates specifically for instruction
                L = pas[PC+1];
                M = pas[PC+2];
                SP += M;
                PC += 3;
                printf("    INC %-3d %-3d %-3d %-3d %-3d ", L, M, PC, BP, SP);
                //print loop for multiple AR
                outerLoop = 0;
                sec = arCount - 1; 
                while (outerLoop <= arCount) {
                    int currBPIndex = BP; 
                    int prevBPIndex = pas[BP+1];
                    int kCount = 0; 
                    for (int k = 0; k < sec; k++) {
                        prevBPIndex = pas[prevBPIndex+1]; 
                        currBPIndex = pas[currBPIndex+1]; 
                        kCount++; 
                    }
                    if (kCount != 0){
                        for (int y = prevBPIndex; y < currBPIndex; y++) 
                            printf("%d ", pas[y]); 
                        printf("| ");
                    }
                    else {
                        if (outerLoop == arCount)
                            for (int y = currBPIndex; y < SP + 1; y++)
                                printf("%d ", pas[y]); 
                        if (sec == 0 && kCount == 0) {
                            for (int y = prevBPIndex; y < currBPIndex; y++) 
                                printf("%d ", pas[y]); 
                            printf("| ");
                        }
                    }
                    sec--; 
                    outerLoop++; 
                }

                printf("\n");
                break;

            // ==================== JMP ====================
            case 7:
                // Updates specifically for instruction
                L = pas[PC+1];
                M = pas[PC+2];
                PC += M;
                // BP and SP stays the same
                printf("    JMP %-3d %-3d %-3d %-3d %-3d ", L, M, PC, BP, SP);
                //print loop for multiple AR
                outerLoop = 0;
                sec = arCount - 1; 
                while (outerLoop <= arCount) {
                    int currBPIndex = BP; 
                    int prevBPIndex = pas[BP+1];
                    int kCount = 0; 
                    for (int k = 0; k < sec; k++) {
                        prevBPIndex = pas[prevBPIndex+1]; 
                        currBPIndex = pas[currBPIndex+1]; 
                        kCount++; 
                    }
                    if (kCount != 0){
                        for (int y = prevBPIndex; y < currBPIndex; y++) 
                            printf("%d ", pas[y]); 
                        printf("| ");
                    }
                    else {
                        if (outerLoop == arCount)
                            for (int y = currBPIndex; y < SP + 1; y++)
                                printf("%d ", pas[y]); 
                        if (sec == 0 && kCount == 0) {
                            for (int y = prevBPIndex; y < currBPIndex; y++) 
                                printf("%d ", pas[y]); 
                            printf("| ");
                        }
                    }
                    sec--; 
                    outerLoop++; 
                }
                printf("\n");
                break;

            // ==================== JPC ====================
            case 8:
                // Updates specifically for instruction
                L = pas[PC+1];
                M = pas[PC+2];
                PC += 3;
                
                // Jump to M if top of stack element is 0
                if(pas[SP] == 0){
                    PC = M;
                }
                SP -= 1;

                printf("    JPC %-3d %-3d %-3d %-3d %-3d ", L, M, PC, BP, SP);
                //print loop for multiple AR
                outerLoop = 0;
                sec = arCount - 1; 
                while (outerLoop <= arCount) {
                    int currBPIndex = BP; 
                    int prevBPIndex = pas[BP+1];
                    int kCount = 0; 
                    for (int k = 0; k < sec; k++) {
                        prevBPIndex = pas[prevBPIndex+1]; 
                        currBPIndex = pas[currBPIndex+1]; 
                        kCount++; 
                    }
                    if (kCount != 0){
                        for (int y = prevBPIndex; y < currBPIndex; y++) 
                            printf("%d ", pas[y]); 
                        printf("| ");
                    }
                    else {
                        if (outerLoop == arCount)
                            for (int y = currBPIndex; y < SP + 1; y++)
                                printf("%d ", pas[y]); 
                        if (sec == 0 && kCount == 0) {
                            for (int y = prevBPIndex; y < currBPIndex; y++) 
                                printf("%d ", pas[y]); 
                            printf("| ");
                        }
                    }
                    sec--; 
                    outerLoop++; 
                }
                printf("\n");
                break;
                
            // ==================== SYS ====================
            case 9:
                // Updates specifically for instruction
                L = pas[PC+1];
                M = pas[PC+2];
                PC += 3;
                // 3 switch statements in here since there are 3 L levels for SYS
                switch (M)
                {
                    case 1:
                    printf("    Output result is: %d \n", pas[SP]);
                    SP -= 1;
                    break;

                    case 2:
                    printf("Please Enter an Integer: ");
                    SP += 1;
                    scanf("%d", &pas[SP]);
                    break;

                    case 3:
                    //printf("    SYS %-3d %-3d %-3d %-3d %-3d ", L, M, PC, BP, SP);
                    // Prints out the stack for case 3 in SYS
                    // outerLoop = 0;
                    // sec = arCount - 1; 
                    // while (outerLoop <= arCount) {
                    //     int currBPIndex = BP; 
                    //     int prevBPIndex = pas[BP+1];
                    //     int kCount = 0; 
                    //     for (int k = 0; k < sec; k++) {
                    //         prevBPIndex = pas[prevBPIndex+1]; 
                    //         currBPIndex = pas[currBPIndex+1]; 
                    //         kCount++; 
                    //     }
                    //     if (kCount != 0){
                    //         for (int y = prevBPIndex; y < currBPIndex; y++) 
                    //             printf("%d ", pas[y]); 
                    //         printf("| ");
                    //     }
                    //     else {
                    //         if (outerLoop == arCount)
                    //             for (int y = currBPIndex; y < SP + 1; y++)
                    //                 printf("%d ", pas[y]); 
                    //         if (sec == 0 && kCount == 0) {
                    //             for (int y = prevBPIndex; y < currBPIndex; y++) 
                    //                 printf("%d ", pas[y]); 
                    //             printf("| ");
                    //         }
                    //     }
                    //     sec--; 
                    //     outerLoop++; 
                    // }
                    // printf("\n");
                    halt = 0;
                    break;
                    //return 0;

                }
                printf("    SYS %-3d %-3d %-3d %-3d %-3d ", L, M, PC, BP, SP);
                //print loop for multiple AR
                outerLoop = 0;
                sec = arCount - 1; 
                while (outerLoop <= arCount) {
                    int currBPIndex = BP; 
                    int prevBPIndex = pas[BP+1];
                    int kCount = 0; 
                    for (int k = 0; k < sec; k++) {
                        prevBPIndex = pas[prevBPIndex+1]; 
                        currBPIndex = pas[currBPIndex+1]; 
                        kCount++; 
                    }
                    if (kCount != 0){
                        for (int y = prevBPIndex; y < currBPIndex; y++) 
                            printf("%d ", pas[y]); 
                        printf("| ");
                    }
                    else {
                        if (outerLoop == arCount)
                            for (int y = currBPIndex; y < SP + 1; y++)
                                printf("%d ", pas[y]); 
                        if (sec == 0 && kCount == 0) {
                            for (int y = prevBPIndex; y < currBPIndex; y++) 
                                printf("%d ", pas[y]); 
                            printf("| ");
                        }
                    }
                    sec--; 
                    outerLoop++; 
                }
                printf("\n");
                break;
                
            // 
            default:
                printf("Not valid OP\n");
                break;
        }
        //printf("\n PC after: %d\n j = %d\n", PC, j);
    }

    fclose(file);
    return 0;
}
