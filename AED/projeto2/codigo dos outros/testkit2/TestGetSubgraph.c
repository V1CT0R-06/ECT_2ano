//
// Algoritmos e Estruturas de Dados --- 2025/2026
//
// Joaquim Madeira, Joao Manuel Rodrigues - Dec 2025
//
// Graph EXAMPLE : Testing the GetSubgraph function
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

  // A subgraph of g01
  IndicesSet* vertices = IndicesSetCreateEmpty(9);
  IndicesSetAdd(vertices, 2);
  IndicesSetAdd(vertices, 3);
  IndicesSetAdd(vertices, 4);
  IndicesSetAdd(vertices, 5);
  IndicesSetAdd(vertices, 8);

  Graph* subg01 = GraphGetSubgraph(g01, vertices);
  GraphCheckInvariants(subg01);

  IndicesSetDestroy(&vertices);

  // Displaying in DOT format
  printf("*** subgraph of simple_digraph.DOT ***\n");
  GraphDisplayDOT(subg01);
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

  // A subgraph of g02
  vertices = IndicesSetCreateEmpty(8);
  IndicesSetAdd(vertices, 2);
  IndicesSetAdd(vertices, 3);
  IndicesSetAdd(vertices, 4);
  IndicesSetAdd(vertices, 5);
  IndicesSetAdd(vertices, 6);

  Graph* subg02 = GraphGetSubgraph(g02, vertices);
  GraphCheckInvariants(subg02);

  IndicesSetDestroy(&vertices);

  // Displaying in DOT format
  printf("*** subgraph of simple_graph.DOT ***\n");
  GraphDisplayDOT(subg02);
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

  // A subgraph of g03
  vertices = IndicesSetCreateEmpty(15);
  IndicesSetAdd(vertices, 4);
  IndicesSetAdd(vertices, 8);
  IndicesSetAdd(vertices, 10);
  IndicesSetAdd(vertices, 12);

  Graph* subg03 = GraphGetSubgraph(g03, vertices);
  GraphCheckInvariants(subg03);

  IndicesSetDestroy(&vertices);

  // Displaying in DOT format
  printf("*** subgraph of simple_weighted_digraph.DOT ***\n");
  GraphDisplayDOT(subg03);
  printf("\n");

  // SWtinyG.DOT
  file = fopen("GRAPHS/SWtinyG.txt", "r");
  Graph* g04 = GraphFromFile(file);
  fclose(file);
  GraphCheckInvariants(g04);

  // A subgraph of g04
  vertices = IndicesSetCreateEmpty(GraphGetVertexRange(g04));
  IndicesSetAdd(vertices, 1);
  IndicesSetAdd(vertices, 3);
  IndicesSetAdd(vertices, 4);
  IndicesSetAdd(vertices, 5);
  IndicesSetAdd(vertices, 6);
  IndicesSetAdd(vertices, 10);

  Graph* subg04 = GraphGetSubgraph(g04, vertices);
  GraphCheckInvariants(subg04);

  IndicesSetDestroy(&vertices);

  // Displaying in DOT format
  printf("*** subgraph of SWtinyG.txt ***\n");
  GraphDisplayDOT(subg04);
  printf("\n");

  // SWtinyEWD.DOT
  file = fopen("DIGRAPHS/SWtinyEWD.txt", "r");
  Graph* g05 = GraphFromFile(file);
  fclose(file);
  GraphCheckInvariants(g05);

  // A subgraph of g05
  vertices = IndicesSetCreateEmpty(GraphGetVertexRange(g05));
  IndicesSetAdd(vertices, 0);
  IndicesSetAdd(vertices, 2);
  IndicesSetAdd(vertices, 3);
  IndicesSetAdd(vertices, 6);
  IndicesSetAdd(vertices, 7);

  Graph* subg05 = GraphGetSubgraph(g05, vertices);
  GraphCheckInvariants(subg05);

  IndicesSetDestroy(&vertices);

  // Displaying in DOT format
  printf("*** subgraph of SWtinyEWD.txt ***\n");
  GraphDisplayDOT(subg05);
  printf("\n");

  // Clearing
  GraphDestroy(&g01);
  GraphDestroy(&g02);
  GraphDestroy(&g03);
  GraphDestroy(&g04);
  GraphDestroy(&g05);
  GraphDestroy(&subg01);
  GraphDestroy(&subg02);
  GraphDestroy(&subg03);
  GraphDestroy(&subg04);
  GraphDestroy(&subg05);

  return 0;
}
