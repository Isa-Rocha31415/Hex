#include "set.h"

void uf_init(set* uf, int size){
  uf->size=size;
  for(int i=0;i<size;i++){
    uf->parent[i]=i;
    uf->rank[i]=0;
  }
}
int uf_find(set* uf,int value){
  if(uf->parent[value]!=value)
    uf->parent[value]=uf_find(uf,uf->parent[value]);
  return uf->parent[value];
}
void uf_union(set* uf, int a,int b){
  int root_a = uf_find(uf, a);
  int root_b = uf_find(uf, b);

  if(root_a==root_b) return; //Los dos elementos están en el mismo conjunto
  if(uf->rank[root_a]<uf->rank[root_b]){
    uf->parent[root_a]=root_b;
  }else{
    uf->parent[root_b]=root_a;
    if(uf->rank[root_a]==uf->rank[root_b]) uf->rank[root_a]++;
  }
}
