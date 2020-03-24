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
	int *label; //label
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
	g->label = malloc(2*g->e*sizeof(int));
	for (i=0; i<g->n;i++){ 
		g->label[i]=i;
	}
	for (i=0;i<g->e;i++) {
		u=g->edges[i].s;
		v=g->edges[i].t;
		g->adj[ g->cd[u] + d[u]++ ]=v;
		g->adj[ g->cd[v] + d[v]++ ]=u; 
		
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


//permet de trouver si deux liste de voisins ont un noeud en commun
int intersection(int* l1, int* l2){
	for(int i=0;i<(sizeof(l1)/sizeof(unsigned long));i++){
		for(int j=0;j<(sizeof(l2)/sizeof(unsigned long));j++){
			if(i==j){
				return 1;
			 }
		 }
	}
	return 0;
}


void sortList(int *l, int n){
	int i = 0, j = 0, tmp = 0;
	
	for(i = 1; i < n; i++){
		tmp = l[i];
		j = i - 1;
		while(j >= 0 && tmp < l[j]){
			l[j+1] = l[j];
			j--;		
		}
		l[j+1] = tmp;
	}
}

int main(int argc,char** argv){
	adjlist* g;
	unsigned *dist;
	unsigned i;
	unsigned long j,u;
	FILE* file;

	time_t t1,t2;

	printf("readedge\n");
	g=readedgelist(argv[1]);

	printf("mkadjlist\n");
	mkadjlist(g);
	
	t1=time(NULL);
	printf("step1\n");
	// copie de la liste des noeuds
	int *copyListNoeuds = malloc(g->n*sizeof(int));
	int *listeLabelOld = malloc(g->n*sizeof(int));
	for (i=0; i<g->n;i++){
		copyListNoeuds[i]=g->label[i];
		listeLabelOld[i] = -1;
		g->label[i] = -1;
	}

	printf("step2\n");
	int nbClusters = g->n / 1000;
	if(nbClusters < 10) nbClusters = 10;
	int indexClusters[nbClusters];
	int maxDeg = 0;

	for(int i = 0; i < g->n; i++){
		int degreI = g->cd[i+1] - g->cd[i];
		if(degreI >= maxDeg){			
			for(int j = 1; j < nbClusters; j++){
				indexClusters[j] = indexClusters[j-1];
			}
			indexClusters[0] = i;
			maxDeg = degreI;
		}
	}
	printf("step3\n");
	g->label[indexClusters[0]] = 0;
	listeLabelOld[i] = 0;
	for(int i = 1; i < nbClusters; i++){
		int isConnected = 0;		
		for(int j = g->cd[i]; j < g->cd[i+1]; j++){
			for(int k = 0; k < i; k++){
				if(indexClusters[k] == g->adj[j] && g->label[j] >= 0){
					g->label[i] = g->label[j];
					listeLabelOld[i] = g->label[j];
					isConnected = 1;
					break;
				}
			}
			if(isConnected) break;
		}
		if(!isConnected){						
			g->label[i] = i;
			listeLabelOld[i] = i;
		}
	}

	printf("step4\n");
	int boolContinue = 1;
	int emergencyExit = 100000;
	do
	{
		for(int i1 = 0; i1 < g->n; i1++){
			int i = copyListNoeuds[i1];
			if(listeLabelOld[i] != -1) continue;
			int degreI = g->cd[i+1] - g->cd[i];
			
			int listeLabelsVoisins[degreI];
			for(int j = 0; j < degreI ; j++){
				listeLabelsVoisins[j] = g->label[g->adj[g->cd[i]+j]];
				//printf("%d %d\n",i,listeLabelsVoisins[j]);
				if(listeLabelsVoisins[j] == -1){
					degreI--;
				}
			}
			sortList(listeLabelsVoisins,degreI);

			if(!degreI) { continue;}
			// calcul du label le plus fréquent parmi les voisins
			int labelsFrequents[degreI];
			int nbFrequents = 0;
			int cpt1 = 1, cptmax = 0;
			for(int j = 1; j < degreI; j++){
				if(listeLabelsVoisins[j] == listeLabelsVoisins[j - 1]){
					cpt1++;
				}
				else{
					if(listeLabelsVoisins[j-1] == -1){
						cpt1 = 0;
					}
					if(cpt1 == cptmax){
						labelsFrequents[++nbFrequents] = listeLabelsVoisins[j-1];
					}	 // ordre des deux if important (cptmax change)
					if(cpt1 > cptmax){
						nbFrequents = 0;
						labelsFrequents[0] = listeLabelsVoisins[j-1];
						cptmax = cpt1;
					}
					cpt1 = 1;		
				}		
			}	
			if(cpt1 == cptmax){
				labelsFrequents[++nbFrequents] = listeLabelsVoisins[degreI-1];
			}		
			if(cpt1 > cptmax){
				nbFrequents = 0;
				labelsFrequents[0] = listeLabelsVoisins[degreI-1];
			}
			// choix du label
			if(nbFrequents == 0){
				g->label[i] = labelsFrequents[0];
			}
			if(nbFrequents > 0){ // condition inutile mais bon
				int tmp = 0;
				for(int j = 0; j < nbFrequents; j++){
					if(g->label[i] == labelsFrequents[j]){
						tmp = 1;
						break;
					}
				}
				if(!tmp){ // si on a pas un label dans les + présents on en prend un des plus présents
					g->label[i] = labelsFrequents[0];
				}
			}
			//printf("%d\n",nbFrequents);			
		}
		boolContinue = 0;
		for(int i = 0; i < g->n; i++){
			if(g->label[i] != listeLabelOld[i]){
				boolContinue = 1;
				listeLabelOld[i] = g->label[i];
			}			
		}
		//printf("%d\n",emergencyExit);
	} while(boolContinue && (emergencyExit--));
	/*
	for(int i = 0; i < g->n; i++){
		printf("%d %d\n",i,g->label[i]);
	}
	*/

	free_adjlist(g);

	t2=time(NULL);

	printf("- Overall time = %ldh%ldm%lds\n",(t2-t1)/3600,((t2-t1)%3600)/60,((t2-t1)%60));

	return 0;
}


