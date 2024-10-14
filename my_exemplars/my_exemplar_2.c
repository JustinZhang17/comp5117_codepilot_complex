#include<stdio.h>
#include<stdlib.h>
#include<time.h>
void simple_recursion(int my_rand){
    if (my_rand > 1){
        simple_recursion(my_rand/2);
    }
    else {
        return;
    }
}
int main(){
    srand(time(NULL));
    int my_rand = rand();
    if ((my_rand % 2) == 0){
        simple_recursion(my_rand);
    }
    else {
        simple_recursion(my_rand-1);
    }
}