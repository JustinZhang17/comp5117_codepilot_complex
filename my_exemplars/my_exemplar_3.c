#include<stdio.h>
#include<stdlib.h>
#include<time.h>
int main(){
    srand(time(NULL));
    int my_rand = rand() % (32);
    if (my_rand < 0){
        printf("my_rand < 0");
        fflush(stdout);
    }
    else if (my_rand >= 32){
        printf("my_rand >= 32");
        fflush(stdout);
    }
    else {
        printf("0 <= my_rand < 32");
        fflush(stdout);
    }
}