#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>
#include"rmq.h"
int RMQ_simple(struct rmq_struct* s, int i , int j){
	if (i < 0 || j >= (s->n) || j < i) return -1;
	int min = i;
	for (int k = i; k <= j; k++){
		if (s->d[k] < s->d[min]) min = k;
	}
	return min;
}
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
int main(){
	int k = 1000;
	int l = 0;
	struct rmq_struct s1;
	clock_t start;
	double elapsed;
	printf("Creating input %d-element input array...", k);
	fflush(stdout);
	start = clock();
	srand(time(NULL));
	s1.n = k;
	s1.d = (int*)malloc(k * sizeof(int));
	s1.d[0] = 0;
	for (int i = 1; i < k; i++){
		if ((rand() % 2) == 0) s1.d[i] = (s1.d[i-1]) + 1;
		else s1.d[i] = (s1.d[i-1]) - 1;
	}
	printf("done (%.4fs)\n", ((double)clock()-start)/CLOCKS_PER_SEC);
	printf("Creating RMQ structure...\n");
	fflush(stdout);
	RMQ_init(&s1);
	printf("done (%.4fs)\n", ((double)clock()-start)/CLOCKS_PER_SEC);
	printf("Block size = %d\n", s1.b);
	printf("Performing %d queries...\n", k);
	fflush(stdout);
	while (l < k) {
		int j = (int)(((double)k/RAND_MAX) * rand());
		int i = (int)(((double)(j)/RAND_MAX) * rand());
		if (RMQ_query(&s1, i, j) != RMQ_simple(&s1, i, j)){
			printf("ERROR\n");
			printf("RMQ_query = %d, RMQ_simple = %d\n", RMQ_query(&s1, i, j), RMQ_simple(&s1, i, j));
			printf("i = %d, j = %d\n", i, j);
			break;
		}
		RMQ_query(&s1, i, j);
		l++;
	}
	printf("done (%.4fs)\n", ((double)clock()-start)/CLOCKS_PER_SEC);
	RMQ_free(&s1);
}
