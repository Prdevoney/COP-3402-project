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
        OP = text[j];
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
            
            printf("    LIT %d %4d %3d %3d %3d", L, M, PC, BP, SP);
            /*  -> printing out the stack below:
                -- have to figure out how to print the different AR records
                and keep the | symbol to seperate the different levels
                -- maybe able to print this all out in a function since we have to do it every time??
                -- maybe if there is an AR extra variable that points to the beginning of each level 
                so we know to print | between each level
            */
            // this spacing is still wrong
            for (int k = textLength; k < SP; k++)
            {
                printf("%3d ", text[k]);
            }
            printf("\n");
            break;

            case 2:
            printf("    OPR\n");
            //M value
            break;
            
            case 3:
            L = text[j+1];
            M = text[j+2];
            printf("    LOD\n");
            break;

            case 4:
            L = text[j+1];
            M = text[j+2];
            printf("    STO\n");
            break;

            case 6:
            L = text[j+1];
            M = text[j+2];
            printf("    INC\n");
            break;

            // ==== JMP ====
            case 7:
            L = text[j+1];
            M = text[j+2];
            PC = M;
            // BP and SP stays the same
            printf("    JMP %d %4d %3d %3d %3d", L, M, PC, BP, SP);
              for (int k = textLength; k < SP; k++)
            {
                printf(" %3d", text[k]);
            }
            printf("\n");
            break;

            case 8:
            L = text[j+1];
            M = text[j+2];
            printf("    JCP\n");
            break;

            case 9:
            // have to put 3 switch statements in here since there are 3 L levels
            L = text[j+1];
            M = text[j+2];
            printf("    SYS\n");
            break;

            case 10:
            L = text[j+1];
            M = text[j+2];
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


