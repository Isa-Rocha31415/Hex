/*
 * Se define la eswtructura de conjunto para construir el algoritmo de Union-Find
 * Consiste en ver los grafos como un  conjunto. Si nos insteresa sabe ris un punto a esta conectado con el punto b, podemos preguntarnos, S hay un conjunto G, el elemento x estará con el elemento y, de esta manera, las operaciones union find son O(1) amortizadas, optimizando el BFS propuesto en clase
 *
 * */

#ifndef SET_H
#define SET_H
#define MAX_SET_SIZE 1024 
typedef struct{
  int parent[MAX_SET_SIZE];
  int rank[MAX_SET_SIZE];
  int size; //Este sera el tamaño del rank 
}set;
void uf_init(set* uf, int size);
int uf_find(set* uf,int value);
void uf_union(set* uf, int a,int b);
#endif // !DEBUG
