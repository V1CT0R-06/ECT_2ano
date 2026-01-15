//
// Algoritmos e Estruturas de Dados --- 2025/2026
//
// Joaquim Madeira, Joao Manuel Rodrigues - Dec 2025
//
// Testing all the developed functions
//
// MODIFY THIS FILE FOR FURTHER TESTS
//

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "Graph.h"
#include "GraphDominatingSets.h"
#include "IndicesSet.h"

int ValidateDominatingSet(Graph* g, IndicesSet* dset) {
  // The union of the dset with the set of neighbors of each dset vertex MUST
  // BE the vertices set of the graph

  IndicesSet* temp_set = IndicesSetCreateCopy(dset);
  // Walking through the elements of the mdset
  int e = IndicesSetGetFirstElem(dset);
  assert(e != -1);
  IndicesSet* neighbors_set = GraphGetSetAdjacentsTo(g, e);
  IndicesSetUnion(temp_set, neighbors_set);
  IndicesSetDestroy(&neighbors_set);
  while ((e = IndicesSetGetNextElem(dset)) != -1) {
    neighbors_set = GraphGetSetAdjacentsTo(g, e);
    IndicesSetUnion(temp_set, neighbors_set);
    IndicesSetDestroy(&neighbors_set);
  }

  // Checking
  IndicesSet* vertices_set = GraphGetSetVertices(g);
  int is_equal = IndicesSetIsEqual(vertices_set, temp_set);

  IndicesSetDestroy(&temp_set);
  IndicesSetDestroy(&vertices_set);

  return is_equal;
}

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

  printf("\n*** simple_graph.DOT ***\n");

  for (unsigned int i = 0; i < GraphGetNumVertices(g01); i++) {
    printf("Vertex %u - Adjacent vertices:\n", i);
    IndicesSet* set = GraphGetSetAdjacentsTo(g01, i);
    IndicesSetDisplay(set);
    IndicesSetDestroy(&set);
  }

  double* vertex_weights = GraphComputeVertexWeights(g01);
  for (unsigned int i = 0; i < GraphGetNumVertices(g01); i++) {
    printf("Vertex %u - Weight: %f\n", i, vertex_weights[i]);
  }
  free(vertex_weights);

  // A subgraph of g01
  IndicesSet* vertices = IndicesSetCreateEmpty(8);
  IndicesSetAdd(vertices, 2);
  IndicesSetAdd(vertices, 3);
  IndicesSetAdd(vertices, 4);
  IndicesSetAdd(vertices, 5);
  IndicesSetAdd(vertices, 6);
  Graph* subg01 = GraphGetSubgraph(g01, vertices);
  GraphCheckInvariants(subg01);
  IndicesSetDestroy(&vertices);
  // Displaying in DOT format
  printf("*** subgraph of simple_graph.DOT ***\n");
  GraphDisplayDOT(subg01);
  GraphDestroy(&subg01);
  printf("\n");

  printf("Testing sets for dominance\n");
  // A full vertex set is a dominating set
  IndicesSet* full_set = IndicesSetCreateFull(GraphGetNumVertices(g01));
  assert(GraphIsDominatingSet(g01, full_set) == 1);
  IndicesSetDestroy(&full_set);
  // Another set for testing domination
  IndicesSet* test_set = IndicesSetCreateEmpty(GraphGetNumVertices(g01));
  IndicesSetAdd(test_set, 2);
  IndicesSetAdd(test_set, 5);
  assert(GraphIsDominatingSet(g01, test_set) == 1);
  IndicesSetDestroy(&test_set);
  // Another set for testing domination
  test_set = IndicesSetCreateEmpty(GraphGetNumVertices(g01));
  IndicesSetAdd(test_set, 0);
  IndicesSetAdd(test_set, 4);
  IndicesSetAdd(test_set, 7);
  assert(GraphIsDominatingSet(g01, test_set) == 1);
  IndicesSetDestroy(&test_set);
  // Another set for testing domination
  test_set = IndicesSetCreateEmpty(GraphGetNumVertices(g01));
  IndicesSetAdd(test_set, 0);
  IndicesSetAdd(test_set, 7);
  assert(GraphIsDominatingSet(g01, test_set) == 0);
  IndicesSetDestroy(&test_set);

  printf("Finding a MIN dominating set\n");
  IndicesSet* mdset = GraphComputeMinDominatingSet(g01);
  IndicesSetDisplay(mdset);
  assert(ValidateDominatingSet(g01, mdset));
  IndicesSetDestroy(&mdset);

  printf("Finding a MIN WEIGHT dominating set\n");
  IndicesSet* mwdset = GraphComputeMinWeightDominatingSet(g01);
  IndicesSetDisplay(mwdset);
  assert(ValidateDominatingSet(g01, mwdset));
  IndicesSetDestroy(&mwdset);

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

  printf("\n*** simple_weighted_graph.DOT ***\n");

  for (unsigned int i = 2; i < 14; i += 2) {
    printf("Vertex %u - Adjacent vertices:\n", i);
    IndicesSet* set = GraphGetSetAdjacentsTo(g02, i);
    IndicesSetDisplay(set);
    IndicesSetDestroy(&set);
  }

  vertex_weights = GraphComputeVertexWeights(g02);
  for (unsigned int i = 0; i < GraphGetVertexRange(g02); i++) {
    printf("Vertex %u - Weight: %f\n", i, vertex_weights[i]);
  }
  free(vertex_weights);

  printf("Finding a MIN dominating set\n");
  mdset = GraphComputeMinDominatingSet(g02);
  IndicesSetDisplay(mdset);
  assert(ValidateDominatingSet(g02, mdset));
  IndicesSetDestroy(&mdset);

  printf("Finding a MIN WEIGHT dominating set\n");
  mwdset = GraphComputeMinWeightDominatingSet(g02);
  IndicesSetDisplay(mwdset);
  assert(ValidateDominatingSet(g02, mwdset));
  IndicesSetDestroy(&mwdset);

  // SWtinyG.DOT
  file = fopen("GRAPHS/SWtinyG.txt", "r");
  Graph* g03 = GraphFromFile(file);
  fclose(file);
  GraphCheckInvariants(g03);

  printf("\n*** SWtinyG.txt ***\n");

  for (unsigned int i = 0; i < GraphGetNumVertices(g03); i++) {
    printf("Vertex %u - Adjacent vertices:\n", i);
    IndicesSet* set = GraphGetSetAdjacentsTo(g03, i);
    IndicesSetDisplay(set);
    IndicesSetDestroy(&set);
  }

  vertex_weights = GraphComputeVertexWeights(g03);
  for (unsigned int i = 0; i < GraphGetNumVertices(g03); i++) {
    printf("Vertex %u - Weight: %f\n", i, vertex_weights[i]);
  }
  free(vertex_weights);

  // A subgraph of g03
  vertices = IndicesSetCreateEmpty(GraphGetVertexRange(g03));
  IndicesSetAdd(vertices, 1);
  IndicesSetAdd(vertices, 3);
  IndicesSetAdd(vertices, 4);
  IndicesSetAdd(vertices, 5);
  IndicesSetAdd(vertices, 6);
  IndicesSetAdd(vertices, 10);
  Graph* subg03 = GraphGetSubgraph(g03, vertices);
  GraphCheckInvariants(subg03);
  IndicesSetDestroy(&vertices);
  // Displaying in DOT format
  printf("*** subgraph of SWtinyG.txt ***\n");
  GraphDisplayDOT(subg03);
  GraphDestroy(&subg03);
  printf("\n");

  printf("Testing sets for dominance\n");
  // A set for testing domination
  test_set = IndicesSetCreateEmpty(GraphGetNumVertices(g03));
  IndicesSetAdd(test_set, 0);
  IndicesSetAdd(test_set, 3);
  IndicesSetAdd(test_set, 7);
  IndicesSetAdd(test_set, 9);
  assert(GraphIsDominatingSet(g03, test_set) == 1);
  IndicesSetDestroy(&test_set);
  // Another set for testing domination
  test_set = IndicesSetCreateEmpty(GraphGetNumVertices(g03));
  IndicesSetAdd(test_set, 0);
  IndicesSetAdd(test_set, 2);
  IndicesSetAdd(test_set, 4);
  IndicesSetAdd(test_set, 6);
  IndicesSetAdd(test_set, 8);
  IndicesSetAdd(test_set, 10);
  IndicesSetAdd(test_set, 12);
  assert(GraphIsDominatingSet(g03, test_set) == 1);
  IndicesSetDestroy(&test_set);
  // Another set for testing domination
  test_set = IndicesSetCreateEmpty(GraphGetNumVertices(g03));
  IndicesSetAdd(test_set, 1);
  IndicesSetAdd(test_set, 3);
  IndicesSetAdd(test_set, 5);
  IndicesSetAdd(test_set, 7);
  IndicesSetAdd(test_set, 9);
  IndicesSetAdd(test_set, 11);
  assert(GraphIsDominatingSet(g03, test_set) == 0);
  IndicesSetDestroy(&test_set);

  printf("Finding a MIN dominating set\n");
  mdset = GraphComputeMinDominatingSet(g03);
  IndicesSetDisplay(mdset);
  assert(ValidateDominatingSet(g03, mdset));
  IndicesSetDestroy(&mdset);

  printf("Finding a MIN WEIGHT dominating set\n");
  mwdset = GraphComputeMinWeightDominatingSet(g03);
  IndicesSetDisplay(mwdset);
  assert(ValidateDominatingSet(g03, mwdset));
  IndicesSetDestroy(&mwdset);

  // Clearing
  GraphDestroy(&g01);
  GraphDestroy(&g02);
  GraphDestroy(&g03);

  return 0;
}
