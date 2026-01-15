//
// Algoritmos e Estruturas de Dados --- 2025/2026
//
// Joaquim Madeira, Joao Manuel Rodrigues - Dec 2025
//
// Graph EXAMPLE : Testing the GetSetAdjacentsTo function
//
// MODIFY THIS FILE FOR FURTHER TESTS
//

#include <assert.h>
#include <stdlib.h>

#include "Graph.h"
#include "IndicesSet.h"

int main(void) {
  FILE* file;

  // simple_digraph.DOT
  Graph* g01 = GraphCreate(9, 1, 0);
  GraphAddEdge(g01, 0, 1);
  GraphAddEdge(g01, 1, 2);
  GraphAddEdge(g01, 1, 3);
  GraphAddEdge(g01, 2, 3);
  GraphAddEdge(g01, 3, 5);
  GraphAddEdge(g01, 5, 4);
  GraphAddEdge(g01, 5, 6);
  GraphAddEdge(g01, 4, 2);
  GraphAddEdge(g01, 4, 6);
  GraphAddEdge(g01, 6, 7);
  GraphCheckInvariants(g01);

  // The sets of adjacent vertices
  printf("*** simple_digraph.DOT ***\n");
  for (unsigned int i = 0; i < GraphGetNumVertices(g01); i++) {
    printf("Vertex %u - Adjacent vertices:\n", i);
    IndicesSet* set = GraphGetSetAdjacentsTo(g01, i);
    IndicesSetDisplay(set);
    IndicesSetDestroy(&set);
  }
  printf("\n");

  // simple_graph.DOT
  Graph* g02 = GraphCreateEmpty(8, 0, 0);
  GraphAddVertex(g02, 7);
  GraphAddVertex(g02, 6);
  GraphAddVertex(g02, 5);
  GraphAddVertex(g02, 4);
  GraphAddVertex(g02, 3);
  GraphAddVertex(g02, 2);
  GraphAddVertex(g02, 1);
  GraphAddVertex(g02, 0);
  GraphAddEdge(g02, 0, 1);
  GraphAddEdge(g02, 0, 2);
  GraphAddEdge(g02, 1, 2);
  GraphAddEdge(g02, 1, 3);
  GraphAddEdge(g02, 2, 4);
  GraphAddEdge(g02, 3, 5);
  GraphAddEdge(g02, 3, 4);
  GraphAddEdge(g02, 4, 5);
  GraphAddEdge(g02, 5, 6);
  GraphAddEdge(g02, 5, 7);
  GraphAddEdge(g02, 7, 6);
  GraphCheckInvariants(g02);

  // The sets of adjacent vertices
  printf("*** simple_graph.DOT ***\n");
  for (unsigned int i = 0; i < GraphGetNumVertices(g02); i++) {
    printf("Vertex %u - Adjacent vertices:\n", i);
    IndicesSet* set = GraphGetSetAdjacentsTo(g02, i);
    IndicesSetDisplay(set);
    IndicesSetDestroy(&set);
  }
  printf("\n");

  // simple_weighted_digraph.DOT
  Graph* g03 = GraphCreateEmpty(15, 1, 1);
  GraphAddVertex(g03, 6);
  GraphAddVertex(g03, 8);
  GraphAddVertex(g03, 2);
  GraphAddVertex(g03, 10);
  GraphAddVertex(g03, 12);
  GraphAddVertex(g03, 4);
  GraphAddWeightedEdge(g03, 8, 12, 5.0);
  GraphAddWeightedEdge(g03, 10, 12, 1.0);
  GraphAddWeightedEdge(g03, 10, 8, 2.0);
  GraphAddWeightedEdge(g03, 4, 6, 1.0);
  GraphAddWeightedEdge(g03, 4, 8, 3.0);
  GraphAddWeightedEdge(g03, 6, 10, 4.0);
  GraphAddWeightedEdge(g03, 2, 6, 5.0);
  GraphAddWeightedEdge(g03, 2, 4, 1.0);
  GraphCheckInvariants(g03);

  // The sets of adjacent vertices
  printf("*** simple_weighted_digraph.DOT ***\n");
  for (unsigned int i = 2; i < 14; i += 2) {
    printf("Vertex %u - Adjacent vertices:\n", i);
    IndicesSet* set = GraphGetSetAdjacentsTo(g03, i);
    IndicesSetDisplay(set);
    IndicesSetDestroy(&set);
  }
  printf("\n");

  // SWtinyG.DOT
  file = fopen("GRAPHS/SWtinyG.txt", "r");
  Graph* g04 = GraphFromFile(file);
  fclose(file);
  GraphCheckInvariants(g04);

  // The sets of adjacent vertices
  printf("*** SWtinyG.DOT ***\n");
  for (unsigned int i = 0; i < GraphGetNumVertices(g04); i++) {
    printf("Vertex %u - Adjacent vertices:\n", i);
    IndicesSet* set = GraphGetSetAdjacentsTo(g04, i);
    IndicesSetDisplay(set);
    IndicesSetDestroy(&set);
  }
  printf("\n");

  // SWtinyEWD.DOT
  file = fopen("DIGRAPHS/SWtinyEWD.txt", "r");
  Graph* g05 = GraphFromFile(file);
  fclose(file);
  GraphCheckInvariants(g05);

  // The sets of adjacent vertices
  printf("*** SWtinyEWD.DOT ***\n");
  for (unsigned int i = 0; i < GraphGetNumVertices(g05); i++) {
    printf("Vertex %u - Adjacent vertices:\n", i);
    IndicesSet* set = GraphGetSetAdjacentsTo(g05, i);
    IndicesSetDisplay(set);
    IndicesSetDestroy(&set);
  }
  printf("\n");

  // Clearing
  GraphDestroy(&g01);
  GraphDestroy(&g02);
  GraphDestroy(&g03);
  GraphDestroy(&g04);
  GraphDestroy(&g05);

  return 0;
}
