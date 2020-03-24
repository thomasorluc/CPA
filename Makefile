
all: load


load: edgelist.c adjmatrix.c adjarray.c proplabel.c algolabel.c
	gcc edgelist.c -O9 -o edgelist.exe
	gcc adjmatrix.c -O9 -o adjmatrix.exe
	gcc adjarray.c -O9 -o adjarray.exe
	gcc proplabel.c -O9 -o proplabel.exe
	gcc algolabel.c -O9 -o algolabel.exe

