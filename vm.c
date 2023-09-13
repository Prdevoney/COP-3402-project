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
    You will never be given an input file with more than text length greater 
    than 150 lines of code
*/


int main(int argc, char *argv[]) {

    FILE *file = fopen(argv[1], "r");
    if (file == NULL) {
        printf("No text file recieved, please put: 'elf.txt' after executable file! \n"); 
        return 0; 
    }

    int text[ARRAY_SIZE]; 
    int i = 0; 
    int count = 0; 

    // Read in the input from the elf file. 
    while (fscanf(file, "%d %d %d", &text[i], &text[i + 1], &text[i +2]) == 3) {
        i += 3; 
        count += 1; 
    }

    int BP = count * 3;
    int textLength = BP;
    int SP = BP - 1;
    int PC = 0;
    int OP = 0;
    int L = 0;
    int M = 0;
    int AR1 = 0; //Activation Record between level 1 and level 2
    int AR2 = 0; //Activation Record between level 2 and level 3

    // put 0's into the rest of the empty PAS
    for(int r = textLength; r < ARRAY_SIZE; r++)
    {
        text[r] = 0;
    }

    /*
    4 spaces before JMP
    L is nomral spacing
    M in &4d spacing

    \t\tPC BP SP
    "Initial values: %2d %2d %2d"
    [4 spaces] + "JMP %d %4d"
    */

   printf("\t\tPC  BP  SP  stack"); //what if the BP and SP are triple digits?
   printf("\nInitial values:%2d  %3d  %2d\n", PC, BP, SP);

    for (int j = 0; j < textLength; j += 3)
    {
        OP = text[PC];
        
        //int BP = the index immediately following the M value from the last instruction in the program
        // int IR = idk

        switch (OP) {
            // ==================== LIT ====================
            case 1:
            L = text[PC+1];
            M = text[PC+2];
            SP += 1;
            //BP += 1;
            PC += 3;
            text[SP] = M; // check later to make sure this is after
            
            printf("    LIT %-3d %-3d %-3d %-3d %-3d ", L, M, PC, BP, SP);
            /*  -> printing out the stack below:
                -- have to figure out how to print the different AR records
                and keep the | symbol to seperate the different levels
                -- maybe able to print this all out in a function since we have to do it every time??
                -- maybe if there is an AR extra variable that points to the beginning of each level 
                so we know to print | between each level
            */
            // this spacing is still wrong
            for (int k = textLength; k < SP + 1; k++)
            {
                if (AR1 == k || AR2 == k){
                    printf("| ");
                }
                printf("%d ", text[k]);
            }
            printf("\n");
            break;

            // ==================== OPR ====================
            case 2:
            printf("    OPR\n");
            //M value
            break;
            
            // ==================== LOD ====================
            case 3:
            L = text[PC+1];
            M = text[PC+2];
            PC += 3;
            printf("    LOD\n");
            break;

            // ==================== STO ====================
            case 4:
            L = text[PC+1];
            M = text[PC+2];
            PC += 3;
            printf("    STO\n");
            break;

            // ==================== CAL ====================
            case 5:
            L = text[PC+1];
            M = text[PC+2];
            PC += 3;
            int arb = BP;
            // base function thing
            while (L > 0) {
                arb = text[arb];
                L--;
            }
            text[SP+1] = arb; ///base(BP,L);  // static link (SL)

            text[SP+2] = BP;          // dynamic link (DL)
            text[SP+3] = PC;          // return address (RA)
            BP = SP + 1;
            PC = M;
            //SP stays the same;
            if (AR1 > 0)
            {
                AR2 = SP + 1;
            } else {
                AR1 = SP + 1;
            }
            printf("    CAL %-3d %-3d %-3d %-3d %-3d ", L, M, PC, BP, SP);
            for (int k = textLength; k < SP + 1; k++)
            {
                printf("%d ", text[k]);
                if (AR1 == k || AR2 == k){
                    printf("| ");
                }
            }
            printf("\n");
            break;

            // ==================== INC ====================
            case 6:
            L = text[PC+1];
            M = text[PC+2];
            SP += M;
            PC += 3;
            printf("    INC %-3d %-3d %-3d %-3d %-3d ", L, M, PC, BP, SP);

            for (int k = textLength; k < SP + 1; k++)
            {
                if (AR1 == k || AR2 == k){
                    printf("| ");
                }
                printf("%d ", text[k]);
            }
            printf("\n");
            break;

            // ==================== JMP ====================
            case 7:
            L = text[PC+1];
            M = text[PC+2];
            PC += M;
            // BP and SP stays the same
            printf("    JMP %-3d %-3d %-3d %-3d %-3d ", L, M, PC, BP, SP);

            for (int k = textLength; k < SP +1 ; k++)
            {
                if (AR1 == k || AR2 == k){
                    printf("| ");
                }
                printf("%d ", text[k]);
            }
            printf("\n");
            break;

            // ==================== JCP ====================
            case 8:
            L = text[PC+1];
            M = text[PC+2];
            PC += 3;
            printf("    JCP\n");
            break;

            // ==================== SYS ====================
            // ---R TO DO:----> check all cases and make sure they work right later!!!
            case 9:
            // have to put 3 switch statements in here since there are 3 L levels
            L = text[PC+1];
            M = text[PC+2];
            PC += 3;
            //printf("\n this is L: %d\n", L );

            //printf("\n this is M: %d\n", M );
            switch (M)
            {
                case 1:
                printf("%d", text[SP]);
                SP -= 1;
                break;

                case 2:
                printf("Please Enter an Integer: ");
                SP += 1;
                scanf("%d", &text[SP]);
                break;

                case 3:
                printf("End of Program");
                return 0;
                // break?

            }
            printf("    SYS %-3d %-3d %-3d %-3d %-3d ", L, M, PC, BP, SP);
            for (int k = textLength; k < (SP + 1); k++)
            {
                if (AR1 == k || AR2 == k){
                    printf("| ");
                }
                printf("%d ", text[k]);
            }
            printf("\n");
            break;

            case 10:
            L = text[PC+1];
            M = text[PC+2];
            PC += 3;
            printf("    LIT\n");
            break;

            default:
            printf("Not valid OP\n");
            break;
        }
    }

    fclose(file);

    return 0;

}


