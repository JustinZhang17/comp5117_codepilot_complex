#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>
#include"lca.h"
int LCA_simple(int** adj, int h, int i, int j){
	int* i_ancestor_array = malloc(h*sizeof(int));
	int* j_ancestor_array = malloc(h*sizeof(int));
	int i_count = 0;
	int k = i;
	while (k > -1){
		i_ancestor_array[i_count] = k;
		i_count++;
		k = adj[k][0];
	}
	int j_count = 0;
	int m = j;
	while (m > -1){
		j_ancestor_array[j_count] = m;
		j_count++;
		m = adj[m][0];
	}
	int ret = -1;
	int stop = 0;
	for (int k = 0; k < i_count; k++){
		if (!stop){
			for (int m = 0; m < j_count; m++){
				if (i_ancestor_array[k] == j_ancestor_array[m]){
					ret = i_ancestor_array[k];
					stop = 1;
					break;
				}
			}
		}
	}
	free(i_ancestor_array);
	free(j_ancestor_array);
	return ret;
}
int main(){
	int h = 20;
	int m = ((1<<h)-1);
	clock_t start;
	printf("Creating %d-node adjacency list representation of binary tree...", m);
	fflush(stdout);
    start = clock();
	int** a = (int**)calloc(m,sizeof(int*));
	for (int i = 0; i < m; i++){
		a[i] = (int*)calloc(3,sizeof(int));
	}
	a[0][0] = -1;
	for (int i = 1; i < m; i++){
		a[i][0] = (int)ceil((double)i/2) - 1;
	}
	for (int i = 0; i < m; i++){
		if (i < ((m-1)/2)){
			a[i][1] = i+i+1;
			a[i][2] = i+i+1+1;
		}
		else{
			a[i][1] = -1;
			a[i][2] = -1;
		}
	}
	printf("done (%.4fs)\n", ((double)clock()-start)/CLOCKS_PER_SEC);
	printf("Creating RMQ structure for LCA...");
	fflush(stdout);
	printf("done (%.4fs)\n", ((double)clock()-start)/CLOCKS_PER_SEC);
	srand(time(NULL));
	printf("Performing %d queries...\n", m);
	fflush(stdout);
	int k = 0;
	while (k < m) {
		int i = (int)(((double)m/RAND_MAX) * rand());
		int j = (int)(((double)m/RAND_MAX) * rand());
		LCA_simple(a, h, i, j);
		k++;
	}
	printf("done (%.4fs)\n", ((double)clock()-start)/CLOCKS_PER_SEC);
	for (int i = 0; i < m; i++){
		free(a[i]);
	}
	free(a);
}
