//
// Algoritmos e Estruturas de Dados --- 2025/2026
//
// Joaquim Madeira, Joao Manuel Rodrigues - Nov 2025
//
// Graph EXAMPLE : Creating and displaying graphs and subgraphs
//
// MODIFY THIS FILE FOR FURTHER TESTS
//

#include <assert.h>
#include <stdlib.h>

#include "Graph.h"
#include "IndicesSet.h"

int main(void) {
  // What kind of graph is g01?
  Graph* g01 = GraphCreate(6, 1, 0);
  GraphAddEdge(g01, 1, 2);
  GraphAddEdge(g01, 1, 4);
  GraphAddEdge(g01, 3, 4);
  printf("The graph:\n");
  // Displaying in DOT format
  GraphDisplayDOT(g01);
  // Saving in DOT format
  FILE* file = fopen("simple_graph.DOT", "w");
  GraphToDOTFile(g01, file);
  fclose(file);
  printf("\n");

  GraphCheckInvariants(g01);

  // The lists of adjacent vertices
  for (unsigned int i = 0; i < GraphGetNumVertices(g01); i++) {
    GraphListAdjacents(g01, i);
  }

  printf("Reading an undirected graph from file\n");
  file = fopen("G_2.txt", "r");
  Graph* g02 = GraphFromFile(file);
  fclose(file);
  // Displaying in DOT format
  GraphDisplayDOT(g02);
  printf("\n");

  GraphCheckInvariants(g02);

  printf("Creating another graph (3)\n");

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

  // The lists of adjacent vertices
  for (unsigned int i = 0; i < GraphGetNumVertices(g03); i++) {
    GraphListAdjacents(g03, i);
  }

  printf("The computed vertex weights\n");
  double* weights_g03 = GraphComputeVertexWeights(g03);
  for (unsigned int i = 0; i < GraphGetVertexRange(g03); i++) {
    if (weights_g03[i] != -1) {
      printf("Vertex %u : weight = %f\n", i, weights_g03[i]);
    }
  }
  printf("\n");
  free(weights_g03);

  printf("A subgraph of g03");
  IndicesSet* vertices = IndicesSetCreateEmpty(4);
  IndicesSetAdd(vertices, 0);
  IndicesSetAdd(vertices, 1);
  IndicesSetAdd(vertices, 2);
  IndicesSetAdd(vertices, 3);

  Graph* subg031 = GraphGetSubgraph(g03, vertices);
  IndicesSetDestroy(&vertices);

  // Displaying in DOT format
  GraphDisplayDOT(subg031);
  printf("\n");
  
  GraphCheckInvariants(subg031);
	
  printf("Another subgraph of g03\n");
  vertices = IndicesSetCreateEmpty(4);
  IndicesSetAdd(vertices, 0);
  IndicesSetAdd(vertices, 2);
  IndicesSetAdd(vertices, 3);
	
  Graph* subg032 = GraphGetSubgraph(g03, vertices);
  IndicesSetDestroy(&vertices);

  // Displaying in DOT format
  GraphDisplayDOT(subg032);
  printf("\n");
  
  GraphCheckInvariants(subg032);
 
  printf("Reading a directed graph from file\n");
  file = fopen("DG_2.txt", "r");
  Graph* g04 = GraphFromFile(file);
  fclose(file);
  // Displaying in DOT format
  GraphDisplayDOT(g04);
  printf("\n");

  GraphCheckInvariants(g04);

  printf("A subgraph of g04\n");
  vertices = IndicesSetCreateEmpty(15);
  IndicesSetAdd(vertices, 1);
  IndicesSetAdd(vertices, 3);
  IndicesSetAdd(vertices, 5);
  IndicesSetAdd(vertices, 7);
  IndicesSetAdd(vertices, 9);

  Graph* subg041 = GraphGetSubgraph(g04, vertices);
  IndicesSetDestroy(&vertices);

  // Displaying in DOT format
  GraphDisplayDOT(subg041);
  printf("\n");
  
  
  printf("Reading a BIG undirected graph from file\n");
  file = fopen("G20.txt", "r");
  Graph* g05 = GraphFromFile(file);
  fclose(file);
  // Displaying in DOT format
  GraphDisplayDOT(g05);
  printf("\n");
  
  GraphCheckInvariants(g05);
  
  printf("A subgraph of g05\n");
  IndicesSet* vertices2 = IndicesSetCreateEmpty(20);
  IndicesSetAdd(vertices2, 0);
  IndicesSetAdd(vertices2, 1);
  IndicesSetAdd(vertices2, 2);
  IndicesSetAdd(vertices2, 3);

  Graph* subg051 = GraphGetSubgraph(g05, vertices2);
  IndicesSetDestroy(&vertices2);

  // Displaying in DOT format
  GraphDisplayDOT(subg051);
  printf("\n");
  
  GraphCheckInvariants(subg051);

  printf("Reading a BIG directed graph from file\n");
  file = fopen("DG20.txt", "r");
  Graph* g06 = GraphFromFile(file);
  fclose(file);
  // Displaying in DOT format
  GraphDisplayDOT(g06);
  printf("\n");

  GraphCheckInvariants(g06);

  printf("A subgraph of g06\n");
  vertices2 = IndicesSetCreateEmpty(20);
  IndicesSetAdd(vertices2, 1);
  IndicesSetAdd(vertices2, 3);
  IndicesSetAdd(vertices2, 5);
  IndicesSetAdd(vertices2, 7);
  IndicesSetAdd(vertices2, 9);

  Graph* subg061 = GraphGetSubgraph(g06, vertices2);
  IndicesSetDestroy(&vertices2);

  // Displaying in DOT format
  GraphDisplayDOT(subg061);
  printf("\n");
  
  // Clearing
  GraphDestroy(&g01);
  GraphDestroy(&g02);
  GraphDestroy(&g03);
  GraphDestroy(&subg031);
  GraphDestroy(&subg032);
  GraphDestroy(&g04);
  GraphDestroy(&subg041);
  GraphDestroy(&g05);
  GraphDestroy(&subg051);
  GraphDestroy(&g06);
  GraphDestroy(&subg061);

  return 0;
}
