/*
Maximilien Danisch
October 2017
http://bit.ly/danisch
maximilien.danisch@gmail.com

Info:
Feel free to use these lines as you wish. This program computes an approximation of the closeness centrality such as described in https://papers-gamma.link/paper/35. The graph is assumed to be undirected.

To compile:
"gcc centrality.c -O3 -o centrality".

To execute:
"./centrality k edgelist.txt output.txt".  
"k" is the number of nodes to consider to compute the approximation of the diameter such as described in [2].  
"edgelist.txt" should contain the graph: one edge on each line (two unsigned long (nodes' ID)) separated by a space.  
"output.txt" will contain the closeness centrality values: "nodeID closeness_centrality_approximation" on each line.  
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


int main(int argc,char** argv){
	adjlist* g;
	unsigned *dist,*sumdist;
	unsigned i,k;
	double r;
	unsigned long j,u;
	bool *tab;
	FILE* file;

	time_t t1,t2;

	t1=time(NULL);

	printf("Number of reference nodes to consider %s\n",argv[1]);
	k=atoi(argv[1]);

	printf("Reading edgelist from file %s\n",argv[2]);

	g=readedgelist(argv[2]);

	printf("Number of nodes: %lu\n",g->n);
	printf("Number of edges: %lu\n",g->e);

	printf("Building the adjacency list\n");
	mkadjlist(g);

	srand(time(NULL));//initialisation of rand

	sumdist=calloc(g->n,sizeof(unsigned long));
	tab=calloc(g->n,sizeof(bool));

	for (i=0;i<k;i++) {
		
		u=rand() % g->n;
		printf("Computing distances from node %lu\n",u);

		dist=bfs(g,u);

		for (j=0;j<g->n;j++){
			if (dist[j]==-1)
				tab[j]=1;
			else
				sumdist[j]+=dist[j];
		}
	}

	free(dist);

	printf("Writting to file %s\n",argv[3]);
	file=fopen(argv[3],"w");
	r=((double)(k*(g->n-1)))/((double)(g->n));
	for (j=0;j<g->n;j++) {
		if (tab[j])
			fprintf(file,"%lu %le\n",j,0.);
		else
			fprintf(file,"%lu %le\n",j,r/((double)sumdist[j]));
	}
	fclose(file);

	free_adjlist(g);
	free(tab);
	free(sumdist);

	t2=time(NULL);

	printf("- Overall time = %ldh%ldm%lds\n",(t2-t1)/3600,((t2-t1)%3600)/60,((t2-t1)%60));

	return 0;
}

