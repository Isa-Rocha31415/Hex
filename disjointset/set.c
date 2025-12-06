#include "set.h"

void uf_init(set *uf, int size) {
  /*
   *Inicializa la estructura de conjunto
   EN esta funcion se le debe de pasar una referencia de tu estructura set,
   junto al tamaño que tendrá en entero. Aquí, la estructura se almacena el
   tamaño. Dentro de la estrucutra tienes dos arrglos, el parent que es donde
   todos lo elementos se considerán conjuntos disjuntos y el rank que es la
   heurística, empieza en 0. No devuelve nada sino se le pasa el puntero de tu
   conjunto.
   * */
  uf->size = size;
  for (int i = 0; i < size; i++) {
    uf->parent[i] = i;
    uf->rank[i] = 0;
  }
}
int uf_find(set *uf, int value) {
  /*
   * ESta es la funcion encargada de hacer el algoritmo de busqueda de union
   * find Necesita la estrucutra de set, la referencia, y un valor que se desea
   * buscar en entero. ES un algoritmo recursvio que si el valor de entreada no
   * es igual al del indiice con ese mismo valor, lo va a seguir buscando. SI es
   * igual Retorna ese valor dentro de parent
   */
  if (uf->parent[value] != value)
    uf->parent[value] = uf_find(uf, uf->parent[value]);
  return uf->parent[value];
}
void uf_union(set *uf, int a, int b) {
  /*
   * En esta funcion, realza la operacion de union
   * Se pasa la refgerencia de la estructura de set, y se pasan los daos valores
   * a unir. Primero, se determina el elemento raiz donde estan estos valroes.
   * SI ambos son iguales, implica que ya estn unidos Si la raiz de a, su valor
   * e smenor que b, entonces a une a al elemento b, sino,al reves y en ese caso
   * se pregunta si el optimizador de rank es igual al valor b, si ees asi se le
   * aumenta l valor del rank a ese valor. ESot es debido a que ya le se tomo y
   * forma parte de b el elemento raiz de a.
   * */
  int root_a = uf_find(uf, a);
  int root_b = uf_find(uf, b);

  if (root_a == root_b)
    return; // Los dos elementos están en el mismo conjunto
  if (uf->rank[root_a] < uf->rank[root_b]) {
    uf->parent[root_a] = root_b;
  } else {
    uf->parent[root_b] = root_a;
    if (uf->rank[root_a] == uf->rank[root_b])
      uf->rank[root_a]++;
  }
}
