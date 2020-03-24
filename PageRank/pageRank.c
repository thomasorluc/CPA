/*
Maximilien Danisch
September 2017
http://bit.ly/danisch
maximilien.danisch@gmail.com

Info:
Feel free to use these lines as you wish. This program loads a graph in main memory.

To compile:
"gcc adjlist.c -O9 -o adjlist".

To execute:
"./adjlist edgelist.txt".
"edgelist.txt" should contain the graph: one edge on each line (two unsigned long (nodes' ID)) separated by a space.
The prograph will load the graph in main memory and then terminate.

Note:
If the graph is directed (and weighted) with selfloops and you want to make it undirected unweighted without selfloops, use the following linux command line.
awk '{if ($1<$2) print $1" "$2;else if ($2<$1) print $2" "$1}' net.txt | sort -n -k1,2 -u > net2.txt

Performance:
Up to 200 million edges on my laptop with 8G of RAM: takes more or less 4G of RAM and 30 seconds (I have an SSD hardrive) for 100M edges.
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>//to estimate the runing time
#include <math.h>

#define NLINKS 100000000 //maximum number of edges for memory allocation, will increase if needed

typedef struct {
	unsigned long s;
	unsigned long t;
} edge;

//edge list structure:
typedef struct {
	unsigned long n;//number of nodes
	unsigned long e;//number of edges
	edge *edges;//list of edges
	unsigned long *cd;//cumulative degree cd[0]=0 length=n+1
	unsigned long *adj;//concatenated lists of neighbors of all nodes
} adjlist;

//compute the maximum of three unsigned long
inline unsigned long max3(unsigned long a,unsigned long b,unsigned long c){
	a=(a>b) ? a : b;
	return (a>c) ? a : c;
}

//reading the edgelist from file
adjlist* readedgelist(char* input){
	unsigned long e1=NLINKS;
	FILE *file=fopen(input,"r");

	adjlist *g=malloc(sizeof(adjlist));
	g->n=0;
	g->e=0;
	g->edges=malloc(e1*sizeof(edge));//allocate some RAM to store edges

	while (fscanf(file,"%lu %lu", &(g->edges[g->e].s), &(g->edges[g->e].t))==2) {
		g->n=max3(g->n,g->edges[g->e].s,g->edges[g->e].t);
		if (++(g->e)==e1) {//increase allocated RAM if needed
			e1+=NLINKS;
			g->edges=realloc(g->edges,e1*sizeof(edge));
		}
	}
	fclose(file);

	g->n++;

	g->edges=realloc(g->edges,g->e*sizeof(edge));

	return g;
}

//building the adjacency matrix
void mkadjlist(adjlist* g){
	unsigned long i,u,v;
	unsigned long *d=calloc(g->n,sizeof(unsigned long));

	for (i=0;i<g->e;i++) {
		d[g->edges[i].s]++;
		//d[g->edges[i].t]++;
	}

	g->cd=malloc((g->n+1)*sizeof(unsigned long));
	g->cd[0]=0;
	for (i=1;i<g->n+1;i++) {
		g->cd[i]=g->cd[i-1]+d[i-1];
		d[i-1]=0;
	}

	//g->adj=malloc(2*g->e*sizeof(unsigned long));
	g->adj=malloc(g->e*sizeof(unsigned long));
	
	for (i=0;i<g->e;i++) {
		u=g->edges[i].s;
		v=g->edges[i].t;
		g->adj[ g->cd[u] + d[u]++ ]=v;
		//g->adj[ g->cd[v] + d[v]++ ]=u;
	}

	free(d);
	free(g->edges);
}


//freeing memory
void free_adjlist(adjlist *g){
	//free(g->edges);
	free(g->cd);
	free(g->adj);
	free(g);
}

int main(int argc,char** argv){
	adjlist* g;
	time_t t1,t2;


	printf("Reading edgelist from file %s\n",argv[1]);
	g=readedgelist(argv[1]);

	printf("Number of nodes: %lu\n",g->n);
	printf("Number of edges: %lu\n",g->e);

	printf("Building the adjacency list\n");
	mkadjlist(g);
	printf("Built !\n");
	
	t1=time(NULL);
	for(int exp = 1; exp < 10; exp++){
		long double *p = malloc(g->n*sizeof(long double));
		for(int i = 0; i < g->n; i++){
			p[i] = 1.0 / g->n;
		}
		
		FILE * fp;
		
		int t = 1002;
		double alpha = 0.1;
		
		long double elt = 0;
		int nbIter = 0;
		for(int i = 0; i < t; i++){
			nbIter++;
			long double difference = p[0] - elt;
			if(difference * g->n < 1.0/pow(10,exp) && difference * g->n > - 1.0/pow(10,exp)) break;
			elt = p[0];

			/*if(!((i-1)%1000)){
				printf("%d\n",i);
				char num[6];
				char nom[20] = "resultats/";
				sprintf(num,"%d",i);
				strcat(nom,num);
				fp = fopen(nom,"w");
				for(int j = 0; j < g->n; j++){
					fprintf(fp,"%LF\n",p[j]*g->n);
				}
				fclose(fp);
			}*/
			long double sommeP = 0.0;
			for(int j = 0; j < g->n; j++){
				int degJ = g->cd[j+1] - g->cd[j];
				int dOut;
				if(degJ) dOut = degJ; else dOut = g->n;
				p[j] /= (1.0*dOut);
				p[j] = (1.0 - alpha) * p[j] + alpha * (1.0 / g->n);
				sommeP += p[j];
			}
			long double normalizer = (1.0 - sommeP) / g->n;
			for(int j = 0; j < g->n; j++){
				p[j] += normalizer;
			}
		}
		long double max = 0.0;
		int nMax = -1;
		printf("exp : %d\n",exp);
		for(int i = 0; i < g->n; i++){
			if(p[i] > max){
				max = p[i];
				nMax = i;
				printf("%d %LF %d\n",nMax,max*g->n,nbIter);
			}
		}
		free(p);
	}
	free_adjlist(g);

	t2=time(NULL);

	printf("- Overall time = %ldh%ldm%lds\n",(t2-t1)/3600,((t2-t1)%3600)/60,((t2-t1)%60));

	return 0;
}












