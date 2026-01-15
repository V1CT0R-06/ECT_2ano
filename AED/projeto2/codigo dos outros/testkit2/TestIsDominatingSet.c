//
// Algoritmos e Estruturas de Dados --- 2025/2026
//
// Joaquim Madeira, Joao Manuel Rodrigues - Dec 2025
//
// Testing the IsDominatingSet function
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

  printf("*** simple_graph.DOT ***\n\n");

  // A full vertex set is a dominating set
  IndicesSet* full_set = IndicesSetCreateFull(GraphGetNumVertices(g01));
  printf("The full set is a dominating set? %d\n",
         GraphIsDominatingSet(g01, full_set));
  IndicesSetDestroy(&full_set);
  printf("\n");

  // Another set for testing domination
  IndicesSet* test_set = IndicesSetCreateEmpty(GraphGetNumVertices(g01));
  IndicesSetAdd(test_set, 2);
  IndicesSetAdd(test_set, 5);
  IndicesSetDisplay(test_set);
  printf("The test set is a dominating set? %d\n",
         GraphIsDominatingSet(g01, test_set));
  IndicesSetDestroy(&test_set);
  printf("\n");

  // Another set for testing domination
  test_set = IndicesSetCreateEmpty(GraphGetNumVertices(g01));
  IndicesSetAdd(test_set, 0);
  IndicesSetAdd(test_set, 4);
  IndicesSetAdd(test_set, 7);
  IndicesSetDisplay(test_set);
  printf("The test set is a dominating set? %d\n",
         GraphIsDominatingSet(g01, test_set));
  IndicesSetDestroy(&test_set);
  printf("\n");

  // Another set for testing domination
  test_set = IndicesSetCreateEmpty(GraphGetNumVertices(g01));
  IndicesSetAdd(test_set, 0);
  IndicesSetAdd(test_set, 7);
  IndicesSetDisplay(test_set);
  printf("The test set is a dominating set? %d\n",
         GraphIsDominatingSet(g01, test_set));
  IndicesSetDestroy(&test_set);
  printf("\n");

  // SWtinyG.DOT
  file = fopen("GRAPHS/SWtinyG.txt", "r");
  Graph* g02 = GraphFromFile(file);
  fclose(file);
  GraphCheckInvariants(g02);

  printf("*** SWtinyG.txt ***\n\n");

  // A set for testing domination
  test_set = IndicesSetCreateEmpty(GraphGetNumVertices(g02));
  IndicesSetAdd(test_set, 0);
  IndicesSetAdd(test_set, 3);
  IndicesSetAdd(test_set, 7);
  IndicesSetAdd(test_set, 9);
  IndicesSetDisplay(test_set);
  printf("The test set is a dominating set? %d\n",
         GraphIsDominatingSet(g02, test_set));
  IndicesSetDestroy(&test_set);
  printf("\n");

  // Another set for testing domination
  test_set = IndicesSetCreateEmpty(GraphGetNumVertices(g02));
  IndicesSetAdd(test_set, 0);
  IndicesSetAdd(test_set, 2);
  IndicesSetAdd(test_set, 4);
  IndicesSetAdd(test_set, 6);
  IndicesSetAdd(test_set, 8);
  IndicesSetAdd(test_set, 10);
  IndicesSetAdd(test_set, 12);
  IndicesSetDisplay(test_set);
  printf("The test set is a dominating set? %d\n",
         GraphIsDominatingSet(g02, test_set));
  IndicesSetDestroy(&test_set);
  printf("\n");

  // Another set for testing domination
  test_set = IndicesSetCreateEmpty(GraphGetNumVertices(g02));
  IndicesSetAdd(test_set, 1);
  IndicesSetAdd(test_set, 3);
  IndicesSetAdd(test_set, 5);
  IndicesSetAdd(test_set, 7);
  IndicesSetAdd(test_set, 9);
  IndicesSetAdd(test_set, 11);
  IndicesSetDisplay(test_set);
  printf("The test set is a dominating set? %d\n",
         GraphIsDominatingSet(g02, test_set));
  IndicesSetDestroy(&test_set);
  printf("\n");

  // Clearing
  GraphDestroy(&g01);
  GraphDestroy(&g02);

  return 0;
}
