int *arr1;
int *arr2;

void function1() {
    // You can directly use arr1 and arr2 here.
}

int main() {
    arr1 = (int *)malloc(10 * sizeof(int));
    arr2 = (int *)malloc(10 * sizeof(int));
    //...
    function1();
    //...
    free(arr1);
    free(arr2);
}
