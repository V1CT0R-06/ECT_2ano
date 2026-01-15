//
// Algoritmos e Estruturas de Dados --- 2025/2026
//
// Joaquim Madeira, Joao Manuel Rodrigues - Dec 2025
//
// Testing the MinDominatingSet function
//
// MODIFY THIS FILE FOR FURTHER TESTS
//

#include <assert.h>

#include "Graph.h"
#include "GraphDominatingSets.h"
#include "IndicesSet.h"

int main(void) {
  FILE* file;

  // simple_graph.DOT
  Graph* g01 = GraphCreateEmpty(8, 0, 0);
  GraphAddVertex(g01, 7);
  GraphAddVertex(g01, 6);
  GraphAddVertex(g01, 5);
  GraphAddVertex(g01, 4);
  GraphAddVertex(g01, 3);
  GraphAddVertex(g01, 2);
  GraphAddVertex(g01, 1);
  GraphAddVertex(g01, 0);
  GraphAddEdge(g01, 0, 1);
  GraphAddEdge(g01, 0, 2);
  GraphAddEdge(g01, 1, 2);
  GraphAddEdge(g01, 1, 3);
  GraphAddEdge(g01, 2, 4);
  GraphAddEdge(g01, 3, 5);
  GraphAddEdge(g01, 3, 4);
  GraphAddEdge(g01, 4, 5);
  GraphAddEdge(g01, 5, 6);
  GraphAddEdge(g01, 5, 7);
  GraphAddEdge(g01, 7, 6);
  GraphCheckInvariants(g01);

  printf("*** simple_graph.DOT ***\n");
  printf("Finding a MIN dominating set\n");
  IndicesSet* mdset = GraphComputeMinDominatingSet(g01);
  IndicesSetDisplay(mdset);

  // CHECKING
  // The union of the mdset with the set of neighbors of each mdset vertex MUST
  // BE the vertices set of the graph

  IndicesSet* temp_set = IndicesSetCreateCopy(mdset);
  // Walking through the elements of the mdset
  int e = IndicesSetGetFirstElem(mdset);
  assert(e != -1);
  IndicesSet* neighbors_set = GraphGetSetAdjacentsTo(g01, e);
  IndicesSetUnion(temp_set, neighbors_set);
  IndicesSetDestroy(&neighbors_set);
  while ((e = IndicesSetGetNextElem(mdset)) != -1) {
    neighbors_set = GraphGetSetAdjacentsTo(g01, e);
    IndicesSetUnion(temp_set, neighbors_set);
    IndicesSetDestroy(&neighbors_set);
  }

  // Checking
  IndicesSet* vertices_set = GraphGetSetVertices(g01);
  assert(IndicesSetIsEqual(vertices_set, temp_set));

  IndicesSetDestroy(&mdset);
  IndicesSetDestroy(&temp_set);
  IndicesSetDestroy(&vertices_set);
  printf("\n");

  // simple_weighted_graph.DOT
  Graph* g02 = GraphCreateEmpty(15, 0, 1);
  GraphAddVertex(g02, 6);
  GraphAddVertex(g02, 8);
  GraphAddVertex(g02, 2);
  GraphAddVertex(g02, 10);
  GraphAddVertex(g02, 12);
  GraphAddVertex(g02, 4);
  GraphAddWeightedEdge(g02, 8, 12, 5.0);
  GraphAddWeightedEdge(g02, 10, 12, 1.0);
  GraphAddWeightedEdge(g02, 10, 8, 2.0);
  GraphAddWeightedEdge(g02, 4, 6, 1.0);
  GraphAddWeightedEdge(g02, 4, 8, 3.0);
  GraphAddWeightedEdge(g02, 6, 10, 4.0);
  GraphAddWeightedEdge(g02, 2, 6, 5.0);
  GraphAddWeightedEdge(g02, 2, 4, 1.0);
  GraphCheckInvariants(g02);

  printf("*** simple_weighted_graph.DOT ***\n");
  printf("Finding a MIN dominating set\n");
  mdset = GraphComputeMinDominatingSet(g02);
  IndicesSetDisplay(mdset);

  // CHECKING
  // The union of the mdset with the set of neighbors of each mdset vertex MUST
  // BE the vertices set of the graph

  temp_set = IndicesSetCreateCopy(mdset);
  // Walking through the elements of the mdset
  e = IndicesSetGetFirstElem(mdset);
  assert(e != -1);
  neighbors_set = GraphGetSetAdjacentsTo(g02, e);
  IndicesSetUnion(temp_set, neighbors_set);
  IndicesSetDestroy(&neighbors_set);
  while ((e = IndicesSetGetNextElem(mdset)) != -1) {
    neighbors_set = GraphGetSetAdjacentsTo(g02, e);
    IndicesSetUnion(temp_set, neighbors_set);
    IndicesSetDestroy(&neighbors_set);
  }

  // Checking
  vertices_set = GraphGetSetVertices(g02);
  assert(IndicesSetIsEqual(vertices_set, temp_set));

  IndicesSetDestroy(&mdset);
  IndicesSetDestroy(&temp_set);
  IndicesSetDestroy(&vertices_set);
  printf("\n");

  // SWtinyG.DOT
  file = fopen("GRAPHS/SWtinyG.txt", "r");
  Graph* g03 = GraphFromFile(file);
  fclose(file);
  GraphCheckInvariants(g03);

  printf("*** SWtinyG.txt ***\n");
  printf("Finding a MIN dominating set\n");
  mdset = GraphComputeMinDominatingSet(g03);
  IndicesSetDisplay(mdset);

  // CHECKING
  // The union of the mdset with the set of neighbors of each mdset vertex MUST
  // BE the vertices set of the graph

  temp_set = IndicesSetCreateCopy(mdset);
  // Walking through the elements of the mdset
  e = IndicesSetGetFirstElem(mdset);
  assert(e != -1);
  neighbors_set = GraphGetSetAdjacentsTo(g03, e);
  IndicesSetUnion(temp_set, neighbors_set);
  IndicesSetDestroy(&neighbors_set);
  while ((e = IndicesSetGetNextElem(mdset)) != -1) {
    neighbors_set = GraphGetSetAdjacentsTo(g03, e);
    IndicesSetUnion(temp_set, neighbors_set);
    IndicesSetDestroy(&neighbors_set);
  }

  // Checking
  vertices_set = GraphGetSetVertices(g03);
  assert(IndicesSetIsEqual(vertices_set, temp_set));

  IndicesSetDestroy(&mdset);
  IndicesSetDestroy(&temp_set);
  IndicesSetDestroy(&vertices_set);
  printf("\n");

  // Clearing
  GraphDestroy(&g01);
  GraphDestroy(&g02);
  GraphDestroy(&g03);

  return 0;
}
