/*
Maximilien Danisch
October 2017
http://bit.ly/danisch
maximilien.danisch@gmail.com

Info:
Feel free to use these lines as you wish. This program computes an approximation (lower bound) to the diameter of the graph using few BFS such as described in https://arxiv.org/abs/0904.2728. The graph is assumed to be undirected.

To compile:
"gcc diameter.c -O9 -o diameter".

To execute:
"./diameter edgelist.txt".
"edgelist.txt" should contain the graph: one edge on each line (two unsigned long (nodes' ID)) separated by a space.
The approximation (lower bound) to diameter will be printed in the terminal.
type "ctrl C" to stop the program.

Note:
If the graph is directed (and weighted) with selfloops and you want to make it undirected unweighted without selfloops, use the following linux command line.
awk '{if ($1<$2) print $1" "$2;else if ($2<$1) print $2" "$1}' net.txt | sort -n -k1,2 -u > net2.txt

Performance:
Up to 500 million edges on my laptop with 8G of RAM.
*/

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>//to estimate the runing time

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
	adjlist *g=malloc(sizeof(adjlist));
	FILE *file;

	g->n=0;
	g->e=0;
	file=fopen(input,"r");
	g->edges=malloc(e1*sizeof(edge));
	while (fscanf(file,"%lu %lu", &(g->edges[g->e].s), &(g->edges[g->e].t))==2) {
		g->n=max3(g->n,g->edges[g->e].s,g->edges[g->e].t);
		if (g->e++==e1) {
			e1+=NLINKS;
			g->edges=realloc(g->edges,e1*sizeof(edge));
		}
	}
	fclose(file);
	g->n++;

	g->edges=realloc(g->edges,g->e*sizeof(edge));

	return g;
}

//building the adjacency list
void mkadjlist(adjlist* g){
	unsigned long i,u,v;
	unsigned long *d=calloc(g->n,sizeof(unsigned long));

	for (i=0;i<g->e;i++) {
		d[g->edges[i].s]++;
		d[g->edges[i].t]++;
	}

	g->cd=malloc((g->n+1)*sizeof(unsigned long));
	g->cd[0]=0;
	for (i=1;i<g->n+1;i++) {
		g->cd[i]=g->cd[i-1]+d[i-1];
		d[i-1]=0;
	}

	g->adj=malloc(2*g->e*sizeof(unsigned long));

	for (i=0;i<g->e;i++) {
		u=g->edges[i].s;
		v=g->edges[i].t;
		g->adj[ g->cd[u] + d[u]++ ]=v;
		g->adj[ g->cd[v] + d[v]++ ]=u;
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


unsigned *bfs(adjlist *g,unsigned long u) {
	unsigned long n=g->n,l=1,i,j,v;

	static unsigned *dist=NULL,*list=NULL;

	if (dist==NULL){
		dist=malloc(n*sizeof(unsigned));
		list=malloc(n*sizeof(unsigned));//this will not be freed
	}

	for (i=0;i<n;i++) {
		dist[i]=-1;
	}

	list[0]=u;
	
	dist[u]=0;

	for (i=0;i<l;i++) {
		v=list[i];
		for (j=g->cd[v];j<g->cd[v+1];j++) {
			if (dist[g->adj[j]]==-1) {
				list[l++]=g->adj[j];
				dist[g->adj[j]]=dist[v]+1;
			}
		}
	}

	return dist;

}

unsigned long maxvect(unsigned long n, unsigned* vect){
	unsigned long i,imax;
	unsigned vmax=0;
	for (i=0;i<n;i++){
		if ((vect[i]!=-1) && (vect[i]>=vmax)){
			imax=i;
			vmax=vect[i];
		}
	}
	return imax;
}

int main(int argc,char** argv){
	adjlist* g;
	unsigned *dist,dmax=0;
	unsigned long i,u=0;
;
	bool *chosen;
	FILE* file;

	time_t t1,t2;


	printf("Reading edgelist from file %s\n",argv[1]);
	g=readedgelist(argv[1]);

	printf("Number of nodes: %lu\n",g->n);
	printf("Number of edges: %lu\n",g->e);

	printf("Building the adjacency list\n");
	mkadjlist(g);

	t1=time(NULL);
	chosen=calloc(g->n,sizeof(bool));
	for (i=0;i<100;i++) {
		if (chosen[u]==0)
			i--;
		else {
			u=i;
			if (chosen[u]==1)
				continue;
		}
		chosen[u]=1;
		printf("Computing distances from node %lu\n",u);
		dist=bfs(g,u);

		u=maxvect(g->n,dist);
		dmax=(dmax>dist[u])?dmax:dist[u];
		printf("lower bound on diameter = %u\n",dmax);
	} 
	printf("diameter = %u\n",dmax);

	free_adjlist(g);

	t2=time(NULL);

	printf("- Overall time = %ldh%ldm%lds\n",(t2-t1)/3600,((t2-t1)%3600)/60,((t2-t1)%60));

	return 0;
}

