//
// Algoritmos e Estruturas de Dados --- 2025/2026
//
// Joaquim Madeira, Joao Manuel Rodrigues - Nov 2025
//
// Testing the Dominating Sets algorithms
//
// MODIFY THIS FILE FOR FURTHER TESTS
//

#include <assert.h>

#include "Graph.h"
#include "GraphDominatingSets.h"
#include "IndicesSet.h"

int main(void) {
  // Reading an undirected graph from file
  FILE* file = fopen("G_2.txt", "r");
  Graph* g02 = GraphFromFile(file);
  fclose(file);
  // Displaying in DOT format
  GraphDisplayDOT(g02);
  printf("\n");

  GraphCheckInvariants(g02);

  // A full vertex set is a dominating set
  IndicesSet* full_set = IndicesSetCreateFull(GraphGetNumVertices(g02));
  printf("The full set is a dominating set? %d\n",
         GraphIsDominatingSet(g02, full_set));
  IndicesSetDestroy(&full_set);
  printf("\n");

  // Another set for testing domination
  IndicesSet* test_set = IndicesSetCreateEmpty(GraphGetNumVertices(g02));
  IndicesSetAdd(test_set, 0);
  IndicesSetAdd(test_set, 1);
  IndicesSetDisplay(test_set);
  printf("The test set is a dominating set? %d\n",
         GraphIsDominatingSet(g02, test_set));

  IndicesSetDestroy(&test_set);
  printf("\n");

  printf("Finding a MIN dominating set\n");
  IndicesSet* mdset = GraphComputeMinDominatingSet(g02);
  IndicesSetDisplay(mdset);
  printf("The subgraph defined by the MIN dominating set\n");
  Graph* mdset_graph = GraphGetSubgraph(g02, mdset);
  GraphDisplayDOT(mdset_graph);
  // The union of the mdset with the set of neighbors of each mdset vertex MUST
  // BE the full vertices set of the graph And the induced subgraph is the graph
  // itself
  // This could be a way for checking
  IndicesSetDestroy(&mdset);
  GraphDestroy(&mdset_graph);
  printf("\n");

  printf("Finding a MIN WEIGHT dominating set\n");
  IndicesSet* mwdset = GraphComputeMinWeightDominatingSet(g02);
  IndicesSetDisplay(mwdset);
  printf("The subgraph defined by the MIN WEIGHT dominating set\n");
  Graph* mwdset_graph = GraphGetSubgraph(g02, mwdset);
  GraphDisplayDOT(mwdset_graph);
  IndicesSetDestroy(&mwdset);
  GraphDestroy(&mwdset_graph);
  printf("\n");
  // Creating another graph

  Graph* g03 = GraphCreateEmpty(4, 0, 0);
  GraphAddVertex(g03, 2);
  GraphAddVertex(g03, 0);
  GraphAddVertex(g03, 1);
  GraphAddVertex(g03, 3);
  GraphAddEdge(g03, 0, 1);
  GraphAddEdge(g03, 0, 2);
  GraphAddEdge(g03, 0, 3);
  GraphAddEdge(g03, 2, 1);
  GraphAddEdge(g03, 2, 3);

  // Displaying in DOT format
  GraphDisplayDOT(g03);
  printf("\n");

  GraphCheckInvariants(g03);

  printf("Finding a MIN dominating set\n");
  mdset = GraphComputeMinDominatingSet(g03);
  IndicesSetDisplay(mdset);
  printf("The subgraph defined by the MIN dominating set\n");
  mdset_graph = GraphGetSubgraph(g03, mdset);
  GraphDisplayDOT(mdset_graph);

  // The union of the mdset with the set of neighbors of each mdset vertex MUST
  // BE the full vertices set of the graph and the induced subgraph is the graph
  // itself
  // This could be a way for checking

  // POSSO PEDIR O CONNECTED DOMINATING SET --- POR CAUSA DA APLICAÇÃO NAS REDES

  IndicesSetDestroy(&mdset);
  GraphDestroy(&mdset_graph);
  printf("\n");

  // Clearing

  GraphDestroy(&g02);
  GraphDestroy(&g03);

  return 0;
}
