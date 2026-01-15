//
// Algoritmos e Estruturas de Dados --- 2025/2026
//
// Joaquim Madeira, Joao Manuel Rodrigues - Dec 2025
//
// Graph EXAMPLE : Creating and displaying graphs and digraphs
//
// MODIFY THIS FILE FOR FURTHER TESTS, IF NEEDED
//

#include <assert.h>
#include <stdlib.h>

#include "Graph.h"
#include "IndicesSet.h"

int main(void) {
  // What kind of graph is g01?
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

  printf("\n");
  // Displaying in DOT format
  GraphDisplayDOT(g01);
  // Saving in DOT format
  FILE* file = fopen("simple_digraph.DOT", "w");
  GraphToDOTFile(g01, file);
  fclose(file);
  printf("\n");
  GraphCheckInvariants(g01);

  // The lists of adjacent vertices
  for (unsigned int i = 0; i < GraphGetNumVertices(g01); i++) {
    GraphListAdjacents(g01, i);
  }
  GraphCheckInvariants(g01);

  // Creating another graph
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

  // Displaying in DOT format
  printf("\n");
  GraphDisplayDOT(g02);
  printf("\n");
  // Saving in DOT format
  file = fopen("simple_graph.DOT", "w");
  GraphToDOTFile(g02, file);
  fclose(file);
  printf("\n");

  // The lists of adjacent vertices
  for (unsigned int i = 0; i < GraphGetNumVertices(g02); i++) {
    GraphListAdjacents(g02, i);
  }

  // Reading an undirected graph from file
  file = fopen("GRAPHS/G_2.txt", "r");
  Graph* g03 = GraphFromFile(file);
  fclose(file);
  GraphCheckInvariants(g03);

  // Displaying in DOT format
  printf("\n");
  GraphDisplayDOT(g03);
  printf("\n");
  GraphCheckInvariants(g03);

  // Reading an undirected graph from file
  file = fopen("GRAPHS/SWtinyG.txt", "r");
  Graph* g04 = GraphFromFile(file);
  fclose(file);
  GraphCheckInvariants(g04);

  // Displaying in DOT format
  // printf("\n");
  // GraphDisplayDOT(g04);
  // printf("\n");
  // Saving in DOT format
  file = fopen("SWtinyG.DOT", "w");
  GraphToDOTFile(g04, file);
  fclose(file);
  printf("\n");

  // Reading an undirected graph from file
  file = fopen("GRAPHS/SWmediumG.txt", "r");
  Graph* g05 = GraphFromFile(file);
  fclose(file);
  GraphCheckInvariants(g05);

  // Displaying in DOT format
  // printf("\n");
  // GraphDisplayDOT(g05);
  // printf("\n");
  // Saving in DOT format
  file = fopen("SWmediumG.DOT", "w");
  GraphToDOTFile(g05, file);
  fclose(file);
  printf("\n");

  // Reading a directed graph from file
  file = fopen("DIGRAPHS/DG_1.txt", "r");
  Graph* g06 = GraphFromFile(file);
  fclose(file);
  GraphCheckInvariants(g06);

  // Displaying in DOT format
  printf("\n");
  GraphDisplayDOT(g06);
  printf("\n");
  // Saving in DOT format
  file = fopen("DG_1.DOT", "w");
  GraphToDOTFile(g06, file);
  fclose(file);
  printf("\n");

  // Reading a directed graph from file
  file = fopen("DIGRAPHS/DG_2.txt", "r");
  Graph* g07 = GraphFromFile(file);
  fclose(file);
  GraphCheckInvariants(g07);

  // Displaying in DOT format
  printf("\n");
  GraphDisplayDOT(g07);
  printf("\n");

  // Reading a directed graph from file
  file = fopen("DIGRAPHS/DAG_1.txt", "r");
  Graph* g08 = GraphFromFile(file);
  fclose(file);
  GraphCheckInvariants(g08);

  // Displaying in DOT format
  printf("\n");
  GraphDisplayDOT(g08);
  printf("\n");

  // Reading a directed graph from file
  file = fopen("DIGRAPHS/DAG_2.txt", "r");
  Graph* g09 = GraphFromFile(file);
  fclose(file);
  GraphCheckInvariants(g09);

  // Displaying in DOT format
  printf("\n");
  GraphDisplayDOT(g09);
  printf("\n");

  // Reading a directed graph from file
  file = fopen("DIGRAPHS/DAG_3.txt", "r");
  Graph* g10 = GraphFromFile(file);
  fclose(file);
  GraphCheckInvariants(g10);

  // Displaying in DOT format
  printf("\n");
  GraphDisplayDOT(g10);
  printf("\n");

  // Reading a directed graph from file
  file = fopen("DIGRAPHS/DAG_4.txt", "r");
  Graph* g11 = GraphFromFile(file);
  fclose(file);
  GraphCheckInvariants(g11);

  // Displaying in DOT format
  printf("\n");
  GraphDisplayDOT(g11);
  printf("\n");

  // Reading a directed graph from file
  file = fopen("DIGRAPHS/SWtinyDG.txt", "r");
  Graph* g12 = GraphFromFile(file);
  fclose(file);
  GraphCheckInvariants(g12);

  // Displaying in DOT format
  printf("\n");
  GraphDisplayDOT(g12);
  printf("\n");

  // Reading a directed graph from file
  file = fopen("DIGRAPHS/SWtinyDAG.txt", "r");
  Graph* g13 = GraphFromFile(file);
  fclose(file);
  GraphCheckInvariants(g13);

  // Displaying in DOT format
  printf("\n");
  GraphDisplayDOT(g13);
  printf("\n");

  // Creating a weighted digraph
  Graph* g14 = GraphCreateEmpty(15, 1, 1);
  GraphAddVertex(g14, 6);
  GraphAddVertex(g14, 8);
  GraphAddVertex(g14, 2);
  GraphAddVertex(g14, 10);
  GraphAddVertex(g14, 12);
  GraphAddVertex(g14, 4);
  GraphAddWeightedEdge(g14, 8, 12, 5.0);
  GraphAddWeightedEdge(g14, 10, 12, 1.0);
  GraphAddWeightedEdge(g14, 10, 8, 2.0);
  GraphAddWeightedEdge(g14, 4, 6, 1.0);
  GraphAddWeightedEdge(g14, 4, 8, 3.0);
  GraphAddWeightedEdge(g14, 6, 10, 4.0);
  GraphAddWeightedEdge(g14, 2, 6, 5.0);
  GraphAddWeightedEdge(g14, 2, 4, 1.0);
  GraphCheckInvariants(g14);

  // Displaying in DOT format
  printf("\n");
  GraphDisplayDOT(g14);
  printf("\n");
  // Saving in DOT format
  file = fopen("simple_weighted_digraph.DOT", "w");
  GraphToDOTFile(g14, file);
  fclose(file);
  printf("\n");

  // Reading a directed graph from file
  file = fopen("DIGRAPHS/SWtinyEWD.txt", "r");
  Graph* g15 = GraphFromFile(file);
  fclose(file);
  GraphCheckInvariants(g15);

  // Displaying in DOT format
  // printf("\n");
  // GraphDisplayDOT(g15);
  // printf("\n");
  // Saving in DOT format
  file = fopen("SWtinyEWD.DOT", "w");
  GraphToDOTFile(g15, file);
  fclose(file);
  printf("\n");

  // Reading a directed graph from file
  file = fopen("DIGRAPHS/SWmediumEWD.txt", "r");
  Graph* g16 = GraphFromFile(file);
  fclose(file);
  GraphCheckInvariants(g16);

  // Displaying in DOT format
  // printf("\n");
  // GraphDisplayDOT(g16);
  // printf("\n");
  // Saving in DOT format
  file = fopen("SWmediumEWD.DOT", "w");
  GraphToDOTFile(g16, file);
  fclose(file);
  printf("\n");

  // Clearing
  GraphDestroy(&g01);
  GraphDestroy(&g02);
  GraphDestroy(&g03);
  GraphDestroy(&g04);
  GraphDestroy(&g05);
  GraphDestroy(&g06);
  GraphDestroy(&g07);
  GraphDestroy(&g08);
  GraphDestroy(&g09);
  GraphDestroy(&g10);
  GraphDestroy(&g11);
  GraphDestroy(&g12);
  GraphDestroy(&g13);
  GraphDestroy(&g14);
  GraphDestroy(&g15);
  GraphDestroy(&g16);

  return 0;
}
