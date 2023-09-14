/* 
    Patrick DeVoney
    Rossella Diorio
    Euripides Montagne
    COP 3402 Fall 2023
    HomeWork 1: Virtual Machine
*/

#include <stdio.h>
#include <stdlib.h>
// figure out more imports??

#define ARRAY_SIZE 512

/*
    You will never be given an input file with more than pas length greater 
    than 150 lines of code
*/

int main (int argc, char *argv[]) {

    FILE *file = fopen(argv[1], "r");
    if (file == NULL) {
        printf("No pas file recieved, please put: 'elf.txt' after executable file! \n"); 
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
    int BP = count * 3;
    int textLength = BP;
    int SP = BP - 1;
    int PC = 0;
    int OP = 0;
    int L = 0;
    int M = 0;
    int arCount = 0; 
    int outerLoop = 0;
    int sec = arCount - 1; 

    // put 0's into the rest of the empty PAS
    for(int r = textLength; r < ARRAY_SIZE; r++)
    {
        pas[r] = 0;
    }

   printf("\t\tPC  BP  SP  stack"); //what if the BP and SP are triple digits?
   printf("\nInitial values:%2d  %3d  %2d\n", PC, BP, SP);

    for (int j = 0; j < textLength; j += 3)
    {
        OP = pas[PC];
        
        //int BP = the index immediately following the M value from the last instruction in the program
        // int IR = idk

        switch (OP) {
            // ==================== LIT ====================
            case 1:
                L = pas[PC+1];
                M = pas[PC+2];
                SP += 1;
                PC += 3;
                pas[SP] = M; // check later to make sure this is after
                
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
                L = pas[PC+1];
                M = pas[PC+2];
                PC += 3;
                switch(M) {
                    case 0:
                        // ========== 2 RTN ==========
                        SP = BP - 1; 
                        BP = pas[SP+2]; 
                        PC = pas[SP+3]; 
                        printf("    RTN %-3d %-3d %-3d %-3d %-3d ", L, M, PC, BP, SP);
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
                    case 1: 
                        // ========== 2 ADD ==========
                        pas[SP-1] = pas[SP-1] + pas[SP]; 
                        SP = SP - 1; 
                        printf("    ADD %-3d %-3d %-3d %-3d %-3d ", L, M, PC, BP, SP);
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
                    case 2: 
                        // ========== 2 SUB ==========
                        pas[SP-1] = pas[SP-1] - pas[SP]; 
                        SP = SP - 1; 
                        printf("    SUB %-3d %-3d %-3d %-3d %-3d ", L, M, PC, BP, SP);
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

                    case 3: 
                        // ========== 2 MUL ==========
                        pas[SP-1] = pas[SP-1] * pas[SP]; 
                        SP = SP - 1; 
                        printf("    MUL %-3d %-3d %-3d %-3d %-3d ", L, M, PC, BP, SP);
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

                    case 4: 
                        // ========== 2 DIV ==========
                        pas[SP-1] = pas[SP-1] / pas[SP]; 
                        SP = SP - 1; 
                        printf("    DIV %-3d %-3d %-3d %-3d %-3d ", L, M, PC, BP, SP);
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

                    case 5: 
                        // ========== 2 EQL ==========
                        if (pas[SP-1] == pas[SP]) {
                            pas[SP-1] = 1; 
                            SP = SP - 1; 
                        }
                        else {
                            pas[SP-1] = 0; 
                            SP = SP - 1; 
                        }
                        printf("    EQL %-3d %-3d %-3d %-3d %-3d ", L, M, PC, BP, SP);
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

                    case 6: 
                        // ========== 2 NEQ ==========
                        if (pas[SP-1] != pas[SP]) {
                            pas[SP-1] = 1; 
                            SP = SP - 1; 
                        }
                        else {
                            pas[SP-1] = 0; 
                            SP = SP - 1; 
                        }
                        printf("    NEQ %-3d %-3d %-3d %-3d %-3d ", L, M, PC, BP, SP);
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

                    case 7: 
                        // ========== 2 LSS ==========
                        if (pas[SP-1] < pas[SP]) {
                            pas[SP-1] = 1; 
                            SP = SP - 1; 
                        }
                        else {
                            pas[SP-1] = 0; 
                            SP = SP - 1; 
                        }
                        printf("    LSS %-3d %-3d %-3d %-3d %-3d ", L, M, PC, BP, SP);
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

                    case 8: 
                        // ========== 2 LEQ ==========
                        if (pas[SP-1] <= pas[SP]) {
                            pas[SP-1] = 1; 
                            SP = SP - 1; 
                        }
                        else {
                            pas[SP-1] = 0; 
                            SP = SP - 1; 
                        }
                        printf("    LEQ %-3d %-3d %-3d %-3d %-3d ", L, M, PC, BP, SP);
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

                    case 9: 
                        // ========== 2 GTR ==========
                        if (pas[SP-1] > pas[SP]) {
                            pas[SP-1] = 1; 
                            SP = SP - 1; 
                        }
                        else {
                            pas[SP-1] = 0; 
                            SP = SP - 1; 
                        }
                        printf("    GTR %-3d %-3d %-3d %-3d %-3d ", L, M, PC, BP, SP);
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

                    case 10: 
                        // ========== 2 GEQ ==========
                        if (pas[SP-1] >= pas[SP]) {
                            pas[SP-1] = 1; 
                            SP = SP - 1; 
                        }
                        else {
                            pas[SP-1] = 0; 
                            SP = SP - 1; 
                        }
                        printf("    GEQ %-3d %-3d %-3d %-3d %-3d ", L, M, PC, BP, SP);
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
                        printf("Not valid OP\n");
                        break;
                }
                break;
            
            // ==================== LOD ====================
            case 3:
                L = pas[PC+1];
                M = pas[PC+2];
                PC += 3;
                SP += 1;
                int arb = BP;
                // base function thing
                while (L > 0) {
                    arb = pas[arb];
                    L--;
                }

                L = pas[PC-2]; 

                pas[SP] = pas[arb + M];
                printf("    LOD %-3d %-3d %-3d %-3d %-3d ", L, M, PC, BP, SP);
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
                L = pas[PC+1];
                M = pas[PC+2];
                PC += 3;
                arb = BP;
                // base function thing
                while (L > 0) {
                    arb = pas[arb];
                    L--;
                }

                L = pas[PC-2]; 

                pas[arb+M] = pas[SP]; 
                SP -= 1; 

                printf("    STO %-3d %-3d %-3d %-3d %-3d ", L, M, PC, BP, SP);
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
                arCount++; 
                L = pas[PC+1];
                M = pas[PC+2];
                PC += 3;
                arb = BP;

                //printf("this is the BPindex: %d",BParray[ARcounter-1] = BP );
                

                // base function thing
                while (L > 0) {
                    arb = pas[arb];
                    L--;
                }
                pas[SP+1] = arb; ///base(BP,L);  // static link (SL)
                pas[SP+2] = BP;          // dynamic link (DL)
                pas[SP+3] = PC;          // return address (RA)
                BP = SP + 1;
                PC = M;
               
                printf("    CAL %-3d %-3d %-3d %-3d %-3d ", L, M, PC, BP, SP);
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
                        // printf("| ");
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
                            // if (arCount != outerLoop)
                                // printf("| ");
                        }
                    }
                    sec--; 
                    outerLoop++; 
                }
                // for (int k = textLength; k < SP + 1; k++)
                // {
                //     //printf("%d ", pas[k]);
                //     if (AR1 == k || AR2 == k){
                //         printf("| ");
                //     }
                //     printf("%d ", pas[k]);

                // }
                printf("\n");
                break;

            // ==================== INC ====================
            case 6:
                L = pas[PC+1];
                M = pas[PC+2];
                SP += M;
                PC += 3;
                printf("    INC %-3d %-3d %-3d %-3d %-3d ", L, M, PC, BP, SP);
                
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
                L = pas[PC+1];
                M = pas[PC+2];
                PC += M;
                // BP and SP stays the same
                printf("    JMP %-3d %-3d %-3d %-3d %-3d ", L, M, PC, BP, SP);

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
                L = pas[PC+1];
                M = pas[PC+2];
                PC += 3;
                // Jump to M if top of stack element is 0
                if(pas[SP] == 0){
                    PC = M;
                    SP -= 1;
                }
                printf("    JPC %-3d %-3d %-3d %-3d %-3d ", L, M, PC, BP, SP);
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
            // ---R TO DO:----> check all cases and make sure they work right later!!!
            case 9:
                // have to put 3 switch statements in here since there are 3 L levels
                L = pas[PC+1];
                M = pas[PC+2];
                PC += 3;
               
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
                    printf("    SYS %-3d %-3d %-3d %-3d %-3d ", L, M, PC, BP, SP);
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
                    return 0;

                }
                printf("    SYS %-3d %-3d %-3d %-3d %-3d ", L, M, PC, BP, SP);
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
                printf("Not valid OP\n");
                break;
        }
    }

    fclose(file);

    return 0;
}


