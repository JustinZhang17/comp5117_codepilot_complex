#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>
#include"lca.h"
void RMQ_init(struct rmq_struct* s){
	if ((s->n) == 1) return;
	s->b = 2*((8*sizeof(s->n))-__builtin_clz((s->n)-1))/3;
	s->min_array = (int*)malloc((((s->n)+(s->b)-1)/(s->b)) * sizeof(int));
	s->index_array = (int*)malloc((((s->n)+(s->b)-1)/(s->b)) * sizeof(int));
	int min, min_index;
	for (int k = 0; k < (s->n); k += (s->b)){
		min = s->d[k];
		min_index = 0;
		for (int l = k; (l < (k+(s->b))) && (l < (s->n)); l++){
			if (s->d[l] < min){
				min = s->d[l];
				min_index = l-k;
			}
		}
		s->min_array[k/s->b] = min;
		s->index_array[k/s->b] = min_index;
	}
	s->st = (int**)calloc((((s->n)+(s->b)-1)/(s->b)),sizeof(int*));
	for (int i = 0; i < (((s->n)+(s->b)-1)/(s->b)); i++){
		s->st[i] = (int*)calloc(ceil(log2(((s->n)+(s->b)-1)/(s->b))),sizeof(int));
	}
	for (int i = 0; i < (((s->n)+(s->b)-1)/(s->b)); i++){
		s->st[i][0] = i;
	}
	for (int j = 1; j < ceil(log2(((s->n)+(s->b)-1)/(s->b))); j++){
		for (int i = 0; (i+(1<<j)) <= (((s->n)+(s->b)-1)/(s->b)); i++){
			if (s->min_array[s->st[i][j-1]] <= s->min_array[s->st[i+(1<<(j-1))][j-1]]) s->st[i][j] = s->st[i][j-1];
			else s->st[i][j] = s->st[i+(1<<(j-1))][j-1];
		}
	}
	s->t = (int*)malloc((((((s->b)*((s->b)-1))/2)+(s->b))*(1<<((s->b)-1)))*sizeof(int));
	for (int k = 0; k < (((((s->b)*((s->b)-1))/2)+(s->b))*(1<<((s->b)-1))); k += ((((s->b)*((s->b)-1))/2)+(s->b))){
		s->t[k] = -1;
	}
	int z;
	int offset;
	int offset2;
	s->signatures = (int*)malloc((((s->n)+(s->b)-1)/(s->b)) * sizeof(int));
	for (int j = 0; j < (((s->n)+(s->b)-1)/(s->b)); j++){
		z = 0;
		for (int l = 0; l < ((s->b)-1); l++){
			if (((((s->b)*j)+l) < (s->n)) && ((((s->b)*j)+l+1) < (s->n)) && (s->d[((s->b)*j)+l] < s->d[((s->b)*j)+l+1])) z += (1<<((s->b)-l-2));
		}
		s->signatures[j] = z;
		if (s->t[((((s->b)*((s->b)-1))/2)+(s->b))*(s->signatures[j])] == -1){
			offset = (s->b)+1;
			offset2 = 0;
			for (int k = 0; k < ((((s->b)*((s->b)-1))/2)+(s->b)) && offset > -1; k += offset){
				s->t[((((s->b)*((s->b)-1))/2)+(s->b))*(s->signatures[j])+k] = (s->b)-offset+1;
				for (int m = 1; m < offset-1; m++){
					if (s->d[((s->b)*j)+(s->t[((((s->b)*((s->b)-1))/2)+(s->b))*(s->signatures[j])+k+m-1])] <= s->d[((s->b)*j)+m+offset2]) s->t[((((s->b)*((s->b)-1))/2)+(s->b))*(s->signatures[j])+k+m] = s->t[((((s->b)*((s->b)-1))/2)+(s->b))*(s->signatures[j])+k+m-1];
					else s->t[((((s->b)*((s->b)-1))/2)+(s->b))*(s->signatures[j])+k+m] = m+offset2;
				}
				offset--;
				offset2++;
			}
		}
	}
}
int RMQ_query(struct rmq_struct* s, int i, int j){
	if (i < 0 || j >= (s->n) || j < i) return -1;
	if ((s->n) == 1) return 0;
	int y = ((s->b)*(i-((i/(s->b))*(s->b))))-(((i-((i/(s->b))*(s->b)))*((i-((i/(s->b))*(s->b)))-1))/2);
	if ((j/(s->b)) == (i/(s->b))) return ((s->b)*(i/(s->b))) + s->t[(s->signatures[(i/(s->b))]*((((s->b)*((s->b)-1))/2)+(s->b)))+y+((j-((j/(s->b))*(s->b)))-(i-((i/(s->b))*(s->b))))];
	int suffix_min = ((s->b)*(i/(s->b))) + s->t[(s->signatures[(i/(s->b))]*((((s->b)*((s->b)-1))/2)+(s->b)))+y+((s->b)-(i-((i/(s->b))*(s->b)))-1)];
	int prefix_min = ((s->b)*(j/(s->b))) + s->t[(s->signatures[(j/(s->b))]*((((s->b)*((s->b)-1))/2)+(s->b)))+(j-((j/(s->b))*(s->b)))];
	if (((j/(s->b))-1)-((i/(s->b))+1) < 0) return (s->d[suffix_min] <= s->d[prefix_min]) ? suffix_min : prefix_min;
	int range_min, k;
	(((j/(s->b))-1)-((i/(s->b))+1) == 0) ? (k = 0) : (k = floor(log2(((j/(s->b))-1)-((i/(s->b))+1))));
	if (s->min_array[s->st[(i/(s->b))+1][k]] <= s->min_array[s->st[(j/(s->b))-1-(1<<k)+1][k]]) range_min = ((s->st[(i/(s->b))+1][k])*(s->b))+(s->index_array[s->st[(i/(s->b))+1][k]]);
	else range_min = ((s->st[(j/(s->b))-1-(1<<k)+1][k])*(s->b))+(s->index_array[s->st[(j/(s->b))-1-(1<<k)+1][k]]);
	if (s->d[suffix_min] <= s->d[range_min]) return (s->d[suffix_min] <= s->d[prefix_min]) ? suffix_min : prefix_min;
	else return (s->d[range_min] <= s->d[prefix_min]) ? range_min : prefix_min;
}
void RMQ_free(struct rmq_struct* s){
	free(s->d);
	if ((s->n) == 1) return;
	free(s->min_array);
	free(s->index_array);
	for (int i = 0; i < (((s->n)+(s->b)-1)/(s->b)); i++){
		free(s->st[i]);
	}
	free(s->st);
	free(s->t);
	free(s->signatures);
}
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
void LCA_init(struct rmq_struct* s, int** adj, int n){
    if (n == 1) return;
    s->n = n;
    s->d = (int*)malloc(((2*(s->n))-1)*sizeof(int));
    s->e = (int*)malloc(((2*(s->n))-1)*sizeof(int));
    s->r = (int*)malloc((s->n)*sizeof(int));
    for (int w = 0; w < (s->n); w++){
        s->r[w] = -1;
    }
    int u = 0;
    int p = -1;
    int k = 1;
    s->d[0] = 0;
    while(u > -1){
        s->e[k-1] = u;
        if (s->r[u] == -1) s->r[u] = (k-1);
        if (p == adj[u][0]){
            p = u;
            if (adj[u][1] > -1){
                u = adj[u][1];
                if (k < ((2*(s->n))-1)) s->d[k] = s->d[k-1]+1;
            }
            else if (adj[u][2] > -1){
                u = adj[u][2];
                if (k < ((2*(s->n))-1)) s->d[k] = s->d[k-1]+1;
            }
            else{
                u = adj[u][0];
                if (k < ((2*(s->n))-1)) s->d[k] = s->d[k-1]-1;
            }
        }
        else if (p == adj[u][1]){
            p = u;
            if (adj[u][2] > -1){
                u = adj[u][2];
                if (k < ((2*(s->n))-1)) s->d[k] = s->d[k-1]+1;
            }
            else {
                u = adj[u][0];
                if (k < ((2*(s->n))-1)) s->d[k] = s->d[k-1]-1;
            }
        }
        else{
            p = u;
            u = adj[u][0];
            if (k < ((2*(s->n))-1)) s->d[k] = s->d[k-1]-1;
        }
        k++;
    }
    RMQ_init(s);
}
int LCA_query(struct rmq_struct* s, int i, int j){
    if (s->r[i] > s->r[j]) return s->e[RMQ_query(s, s->r[j], s->r[i])];
    return s->e[RMQ_query(s, s->r[i], s->r[j])];
}
void LCA_free(struct rmq_struct* s){
    if ((s->n) == 1) return;
    free(s->e);
    free(s->r);
    RMQ_free(s);
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
	struct rmq_struct rs;
	printf("Creating RMQ structure for LCA...");
	fflush(stdout);
	LCA_init(&rs, a, ((2*(m))-1));
	printf("done (%.4fs)\n", ((double)clock()-start)/CLOCKS_PER_SEC);
	srand(time(NULL));
	printf("Performing %d queries...\n", m);
	fflush(stdout);
	int k = 0;
	while (k < m) {
		int i = (int)(((double)m/RAND_MAX) * rand());
		int j = (int)(((double)m/RAND_MAX) * rand());
		if (LCA_query(&rs, i, j) != LCA_simple(a, h, i, j)){
			printf("ERROR\n");
			printf("LCA_query = %d, LCA_simple = %d\n", LCA_query(&rs, i, j), LCA_simple(a, h, i, j));
			printf("i = %d, j = %d\n", i, j);
			break;
		}
		LCA_query(&rs, i, j);
		k++;
	}
	printf("done (%.4fs)\n", ((double)clock()-start)/CLOCKS_PER_SEC);
	LCA_free(&rs);
	for (int i = 0; i < m; i++){
		free(a[i]);
	}
	free(a);
}
