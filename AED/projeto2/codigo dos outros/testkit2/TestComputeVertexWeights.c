//
// Algoritmos e Estruturas de Dados --- 2025/2026
//
// Joaquim Madeira, Joao Manuel Rodrigues - Dec 2025
//
// Graph EXAMPLE : Testing the ComputeVertexWeights function
//
// MODIFY THIS FILE FOR FURTHER TESTS
//

#include <assert.h>
#include <stdlib.h>

#include "Graph.h"
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

  // Computing the vertex weights
  double* vertex_weights = GraphComputeVertexWeights(g01);

  printf("*** simple_graph.DOT ***\n");
  for (unsigned int i = 0; i < GraphGetNumVertices(g01); i++) {
    printf("Vertex %u - Weight: %f\n", i, vertex_weights[i]);
  }
  printf("\n");

  free(vertex_weights);

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

  // Computing the vertex weights
  vertex_weights = GraphComputeVertexWeights(g02);

  printf("*** simple_weighted_graph.DOT ***\n");
  for (unsigned int i = 0; i < GraphGetVertexRange(g02); i++) {
    printf("Vertex %u - Weight: %f\n", i, vertex_weights[i]);
  }
  printf("\n");

  free(vertex_weights);

  // SWtinyG.DOT
  file = fopen("GRAPHS/SWtinyG.txt", "r");
  Graph* g03 = GraphFromFile(file);
  fclose(file);
  GraphCheckInvariants(g03);

  // Computing the vertex weights
  vertex_weights = GraphComputeVertexWeights(g03);

  printf("*** SWtinyG.DOT ***\n");
  for (unsigned int i = 0; i < GraphGetNumVertices(g03); i++) {
    printf("Vertex %u - Weight: %f\n", i, vertex_weights[i]);
  }
  printf("\n");

  free(vertex_weights);

  // SWmediumG.DOT
  file = fopen("GRAPHS/SWmediumG.txt", "r");
  Graph* g04 = GraphFromFile(file);
  fclose(file);
  GraphCheckInvariants(g04);

  // Computing the vertex weights
  vertex_weights = GraphComputeVertexWeights(g04);

  printf("*** SWmediumG.DOT ***\n");
  for (unsigned int i = 0; i < GraphGetNumVertices(g04); i++) {
    printf("Vertex %u - Weight: %f\n", i, vertex_weights[i]);
  }
  printf("\n");

  free(vertex_weights);

  // Clearing
  GraphDestroy(&g01);
  GraphDestroy(&g02);
  GraphDestroy(&g03);
  GraphDestroy(&g04);

  return 0;
}
