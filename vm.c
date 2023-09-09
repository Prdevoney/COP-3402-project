#include <stdio.h>


int newFunction(int num); 

int main() {
    printf("Hello, World!\n");
    printf("how do you do"); 
    int num = 5; 
    newFunction(num); 
}

int newFunction(int num){
    printf("%d", num);
}

