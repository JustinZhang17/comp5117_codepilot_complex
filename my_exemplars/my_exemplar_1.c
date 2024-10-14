#include<stdio.h>
#include<stdlib.h>
int find_minimum(int i, int j){
    if (i <= j){
        return i;
    }
    return j;
}
int main(){
    int i = 6;
    int j = 2;
    printf("min(%d, %d) = %d\n", i, j, find_minimum(i, j));
}