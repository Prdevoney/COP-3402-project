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

    int array[ARRAY_SIZE]; 
    int i = 0; 

    // read in the input from the elf file. 
    while (fscanf(file, "%d %d %d", &array[i], &array[i + 1], &array[i +2]) == 3) {
        i += 3; 
    }
    
    // like ^^ that better probably ^ 
    // links below
    // -->  https://users.cs.utah.edu/~germain/PPS/Topics/C_Language/file_IO.html 
    // --> https://www.opentechguides.com/how-to/article/c/45/c-read-file.html 
    
    // Each PAS is always initialized to zero
    // create the PAS to 0's ? somehow 
    // make lots of loop de loops to read in the text files
    // then execute the statements with if statements cus he keeps saying not to do the instructions
    // with functions which is confusing me
    // so that probably means lots of if statements 
    // the variables below have to be global
    // maybe switch statements can help??
    // we can probably do helper functions for the dumb printing formating
    // I have no idea if he gave input or output files he probably did I just haven't read those yet
    // I shall sleep now...

    int BP = count * 3;
    int SP = BP - 1;
    int PC = 0;

    /* 
        The values of SP and BP will be set up dynamically 
        once the program had been uploaded.
    */

    //int BP = the index immediately following the M value from the last instruction in the program

   
    // int IR = idk

    // printing out form array:
    printf(text[0]);

    fclose(inputFile);
}


