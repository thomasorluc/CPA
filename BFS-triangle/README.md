## Info:

Feel free to use these lines as you wish. This program computes shortest paths using BFS.  
Two extensions are implemented: one to compute a (good) lower bound to the diameter (such as described in [1]) and one to compute an approximation of the closeness centrality of each node in the graph (such as described in [2]).  
The graph is assumed to be undirected.

## To compile:

type "Make", or type
- gcc bfs.c -O3 -o bfs
- gcc diameter.c -O3 -o diameter
- gcc centrality.c -O3 -o centrality
- gcc centralityPAR.c -O3 -o centralityPAR -fopenmp

## To execute:

"./bfs edgelist.txt n1 n2 n3 ... n_k".
- "edgelist.txt" should contain the graph: one edge on each line (two unsigned long (nodes' ID)) separated by a space.
- It computes the shotest path from node n1 to all nodes in the graph and writes the result in a file called "n1". It the does the same for n2, n3,... and n_k

"./diameter edgelist.txt".
- "edgelist.txt" should contain the graph: one edge on each line (two unsigned long (nodes' ID)) separated by a space.
- The approximation (lower bound) to the diameter will be printed in the terminal.
- Type "ctrl C" to stop the program.
- How it works: computes a BFS from a node u, then takes the fursest node from node u and computes a bfs from it. Iterates over all nodes in the graph. This algorithm is described in [1].

"./centrality k edgelist.txt output.txt".
- "k" is the number of nodes to consider to compute the approximation of the diameter such as described in [2].
- "edgelist.txt" should contain the graph: one edge on each line (two unsigned long (nodes' ID)) separated by a space.
- "output.txt" will contain the closeness centrality values: "nodeID closeness_centrality_approximation" on each line.
- How it works: samples k nodes uniformly at random and computes the distances (using BFS) to each one of these reference nodes to estimate the value of the closeness centrality of each node in the graph. This algorithm is described in [2]. Note that the theorems about the approximation guarantee assume that the graph is connected, while this program works even if the graph is not connected: CAREFUL!!!


"./centralityPAR k edgelist.txt output.txt".
- "p" is the number of threads to use.
- The rest is the same as "centrality".

## Note:

If the graph is directed (and weighted) with selfloops and you want to make it undirected unweighted without selfloops, use the following linux command line.  
awk '{if ($1<$2) print $1" "$2;else if ($2<$1) print $2" "$1}' net.txt | sort -n -k1,2 -u > net2.txt

## Performance:

Up to 500 million edges on my laptop with 8G of RAM:  
Takes more or less 1.6G of RAM and 25 seconds (I have an SSD hardrive) for 100M edges to compute a bfs.

A lower bound on diameter of http://snap.stanford.edu/data/com-Amazon.html is 47  
A lower bound on diameter of http://snap.stanford.edu/data/com-Orkut.html is 10

## Reference:

- [1] https://arxiv.org/abs/0904.2728
- [2] https://papers-gamma.link/paper/35

## Initial contributors

Maximilien Danisch  
October 2017  
http://bit.ly/danisch  
maximilien.danisch@gmail.com

