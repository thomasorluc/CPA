/*
Maximilien Danisch
October 2017
http://bit.ly/danisch
maximilien.danisch@gmail.com

Info:
Feel free to use these lines as you wish. This program computes shortest paths using BFS. The graph is assumed to be undirected.

To compile:
"gcc bfs.c -O9 -o bfs".

To execute:
"./bfs edgelist.txt n1 n2 n3 ... n_k".
"edgelist.txt" should contain the graph: one edge on each line (two unsigned long (nodes' ID)) separated by a space.
It computes the shotest path from node n1 to all nodes in the graph and writes the result in a file called "n1". It the does the same for n2, n3,... and n_k

Note:
If the graph is directed (and weighted) with selfloops and you want to make it undirected unweighted without selfloops, use the following linux command line.
awk '{if ($1<$2) print $1" "$2;else if ($2<$1) print $2" "$1}' net.txt | sort -n -k1,2 -u > net2.txt

Performance:
Up to 500 million edges on my laptop with 8G of RAM:
Takes more or less 1.6G of RAM and 25 seconds (I have an SSD hardrive) for 100M edges.
*/

#include <stdlib.h>
#include <stdio.h>
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
		g->adj[ g->cd[v] + d[v]++ ]=u; //-> evite le triple triangle
	}
	free(d);
	//free(g->edges);
}

//freeing memory
void free_adjlist(adjlist *g){
	free(g->edges);
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




//permet de trouver si deux liste de voisins ont un noeud en commun
int intersection(unsigned long *l1, unsigned long *l2,int t1,int t2){
	int cpt=0;
	for(int i=0;i<(t1);i++){
		for(int j=0;j<(t2);j++){
			if(l1[i]==l2[j]){
				cpt++;
			 }
		 }
	}
	//printf("%d %d\n",t1,t2);
	return cpt;
}


int main(int argc,char** argv){
	adjlist* g;
	unsigned *dist;
	unsigned i;
	unsigned long j,u;
	FILE* file;

	time_t t1,t2;

	printf("Reading edgelist from file %s\n",argv[1]);
	g=readedgelist(argv[1]);
	printf("Building the adjacency list\n");

	mkadjlist(g);
	printf("Adjacency list built\n");

	t1=time(NULL);
	// calcul du nombre de triangles
	int *trianglesNode = malloc(g->n*sizeof(int)); 
	for(int i = 0; i < g->n; i++) trianglesNode[i] = 0;
	for(int i = 0; i < g->e; i++){
		
		edge ee = (g->edges[i]);
		//printf("1\n");
		unsigned long s= ee.s;
		unsigned long t= ee.t;
		int degS = g->cd[s+1]-g->cd[s];
		int degT = g->cd[t+1]-g->cd[t];
		unsigned long l1[degS];
		unsigned long l2[degT];
		//printf("2\n");
		for (int j = 0;j<degS;j++){
			l1[j]=g->adj[j+g->cd[s]];
		}
		//printf("3\n");
		for (int k = 0;k<degT;k++){
			l2[k]=g->adj[k+g->cd[t]];
		}
		//printf("4\n");
		//cptT+=intersection(l1,l2,degS,degT);
		for(int i1=0;i1<degS;i1++){
			for(int j1=0;j1<degT;j1++){
				if(l1[i1]==l2[j1]){
					trianglesNode[l1[i1]]++;
				 }
			 }
		}
	}	
	
	long int cptV = 0;
	int sommeTN = 0;
	int nbNodes2N = 0; // nombre de noeuds avec au moins 2 voisins
	float clusteringT = 0; // somme des clustering coefficients
	for(int i = 0; i < g->n; i++){
		// somme triangleNode
		sommeTN +=trianglesNode[i];
		
		// calcul du nombre de V-edges
		int degreI = g->cd[i+1] - g->cd[i];
		int pairOfN = degreI*( degreI - 1 )/2; // nombre de paires de voisins
		cptV += pairOfN;
		
		//clustering coefficient
		if(degreI >= 2){
			nbNodes2N++;
			clusteringT += (1.0 * trianglesNode[i]) / pairOfN;
		}		
	}
	printf("\nnombre de triangles : %d\n",sommeTN/3);	
	printf("nombre de V-edges : %ld\n",cptV);	
	printf("transitivity ratio : %f\n",(1.0*sommeTN)/cptV);
	printf("clustering coefficient : %f\n",clusteringT/nbNodes2N);
	//printf("tests : %f\n",clusteringT/(g->n));
	//printf("%f %d %lu\n",clusteringT,nbNodes2N,g->n);
	
	
		
	
	free_adjlist(g);

	t2=time(NULL);

	printf("- Overall time = %ldh%ldm%lds\n",(t2-t1)/3600,((t2-t1)%3600)/60,((t2-t1)%60));

	return 0;
}


