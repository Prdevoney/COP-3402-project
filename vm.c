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

#define ARRAY_SIZE 512;

/*
    You will never be given an input file with more than text length greater 
    than 150 lines of code
*/

int main(int argc, char *argv[]) {

    FILE *file = fopen(argv[1], "r");
    if (file == NULL) {
        printf("no file!\n"); 
        return; 
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

    int OP = __; 

    //int BP = the index immediately following the M value from the last instruction in the program
    // int IR = idk

    switch (OP) {
        case 1:
        printf("    LIT");
        SP = SP -1;
        BP + 1;
        printf(SP);
        break;

        switch (OP) {
            case 1:
            printf("    LIT");
            L = text[j+1];
            M = text[j+2];
            SP -= 1;
            BP += 1;
            PC += 3;
            printf(SP);
            break;

            case 2:
            printf("    OPR");
            //M value
            break;
            
            case 3:
            printf("    LIT");
            break;

            case 4:
            printf("    LIT");
            break;

        case 6:
        printf("    LIT");
        break;

        case 7:
        printf("    LIT");
        break;

        case 8:
        printf("    LIT");
        break;

        case 9:
        printf("    LIT");
        break;

        case 10:
        printf("    LIT");
        break;


        }
    }

    // printing out form text:
    printf(text[0]);

    fclose(inputFile);
}


