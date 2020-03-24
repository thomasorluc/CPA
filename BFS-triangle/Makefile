CC=gcc
CFLAGS=-O9

all: bfs bfs2 bfs3 diameter centrality centralityPAR

bfs : bfs.c
	$(CC) $(CFLAGS) bfs.c -o bfs

bfs2 : triangle.c
	$(CC) $(CFLAGS) triangle.c -o triangle.exe
	
bfs3 : bfsPourcent.c
	$(CC) $(CFLAGS) bfsPourcent.c -o bfsPourcent
	
diameter : diameter.c
	$(CC) $(CFLAGS) diameter.c -o diameter

centrality : centrality.c
	$(CC) $(CFLAGS) centrality.c -o centrality

centralityPAR : centralityPAR.c
	$(CC) $(CFLAGS) centralityPAR.c -o centralityPAR -fopenmp

clean:
	rm bfs diameter centrality centralityPAR bfsPourcent triangle
