/* 
    Patrick DeVoney
    Rossella Diorio
    Euripides Montagne
    COP 3402 Fall 2023
    HomeWork 1: Virtual Machine
*/

#include <stdio.h>
#include <stdlib.h>

ARRAY_SIZE 512;

/*
    You will never be given an input file with more than text length greater 
    than 150 lines of code
*/

int main(int argc char *argv[]) {

    FILE *inputFile = fopen(argv[1], "r");
    // use buffer thingy?? or simple fscanf stuff!!! :)
    fscanf(file, "%d %d", &variable, &variable);
    
    // like ^^ that better probably ^ 
    // links below
    // -->  https://users.cs.utah.edu/~germain/PPS/Topics/C_Language/file_IO.html 
    // --> https://www.opentechguides.com/how-to/article/c/45/c-read-file.html 
    char buff[buffersize?];
    fgets(buff, buff size-1, file);
    /*  First you must load
        the input program into the PAS. 
    */ 

    // Each PAS is always initialized to zero

    /* 
        The values of SP and BP will be set up dynamically 
        once the program had been uploaded.
    */

    //int BP = the index immediately following the M value from the last instruction in the program

    int text [ARRAY_SIZE];

    int SP = BP - 1;
    int PC = 0;
    // int IR = idk

    // printing out form array:
    printf(text[0]);

    fclose(inputFile);
}


