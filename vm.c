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
        printf("no file!\n"); 
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
    int OP = 1;
    int L = 0;
    int M = 0;

    /*
    4 spaces before JMP
    L is nomral spacing
    M in &4d spacing

    \t\tPC BP SP
    "Initial values: %2d %2d %2d"
    [4 spaces] + "JMP %d %4d"
    */

   printf("\t\tPC\tBP\tSP\tstack");
   printf("\nInitial values: %2d %2d %2d\n", PC, BP, SP);

    for (int j = 0; j < textLength; j + 3)
    {

    //int BP = the index immediately following the M value from the last instruction in the program
    // int IR = idk
    
        switch (OP) {
            case 1:
            L = text[j+1];
            M = text[j+2];
            SP += 1;
            BP += 1;
            PC += 3;
            text[SP] = M; // check later to make sure this is after
            
            printf("    LIT %d %4d %2d %2d %2d", L, M, PC, BP, SP);
            /*  -> printing out the stack below:
                -- have to figure out how to print the different AR records
                and keep the | symbol to seperate the different levels
                -- maybe able to print this all out in a function since we have to do it every time??
                -- maybe if there is an AR extra variable that points to the beginning of each level 
                so we know to print | between each level
            */
            for (int k = textLength; k < SP; k++)
            {
                printf("%2d", text[k]);
            }
            break;

            case 2:
            printf("    OPR");
            //M value
            break;
            
            case 3:
            L = text[j+1];
            M = text[j+2];
            printf("    LOD");
            break;

            case 4:
            L = text[j+1];
            M = text[j+2];
            printf("    STO");
            break;

            case 6:
            L = text[j+1];
            M = text[j+2];
            printf("    INC");
            break;

            case 7:
            L = text[j+1];
            M = text[j+2];
            PC = M;
            // BP and SP stays the same
            printf("    JMP %d %4d %2d %2d %2d", L, M, PC, BP, SP);
            break;

            case 8:
            L = text[j+1];
            M = text[j+2];
            printf("    JCP");
            break;

            case 9:
            // have to put 3 switch statements in here since there are 3 L levels
            L = text[j+1];
            M = text[j+2];
            printf("    SYS");
            break;

            case 10:
            L = text[j+1];
            M = text[j+2];
            printf("    LIT");
            break;

            default:
            printf("Not valid OP");
            break;
        }
    }

    fclose(file);

    return 0;

}


