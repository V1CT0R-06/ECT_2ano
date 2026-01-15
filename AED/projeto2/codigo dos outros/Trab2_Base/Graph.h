//
// Algoritmos e Estruturas de Dados --- 2025/2026
//
// Joaquim Madeira, Joao Manuel Rodrigues - June 2021, Nov 2023, Nov/Dec 2024,
// Nov 2025
//
// DO NOT MODIFY THIS FILE
//
// Graph - Using a list of adjacency lists representation
//

#ifndef _GRAPH_
#define _GRAPH_

#include <stdio.h>

#include "IndicesSet.h"

typedef struct _GraphHeader Graph;

Graph* GraphCreateEmpty(unsigned int indicesRange, int isDigraph,
                        int isWeighted);

Graph* GraphCreate(unsigned int numVertices, int isDigraph, int isWeighted);

Graph* GraphCreateComplete(unsigned int numVertices, int isDigraph);

void GraphDestroy(Graph** p);

Graph* GraphFromFile(FILE* f);

Graph* GraphGetSubgraph(const Graph* g, IndicesSet* vertSet);

// Graph

int GraphIsDigraph(const Graph* g);

int GraphIsComplete(const Graph* g);

int GraphIsWeighted(const Graph* g);

unsigned int GraphGetVertexRange(const Graph* g);

unsigned int GraphGetNumVertices(const Graph* g);

unsigned int GraphGetNumEdges(const Graph* g);

//
// For a graph
//
double GraphGetAverageDegree(const Graph* g);

//
// For a graph
//
unsigned int GraphGetMaxDegree(const Graph* g);

//
// For a digraph
//
unsigned int GraphGetMaxOutDegree(const Graph* g);

// Vertices

int GraphAddVertex(Graph* g, unsigned int v);

IndicesSet* GraphGetSetVertices(const Graph* g);

IndicesSet* GraphGetSetAdjacentsTo(const Graph* g, unsigned int v);

double* GraphComputeVertexWeights(const Graph* g);

//
// For a graph
//
unsigned int GraphGetVertexDegree(Graph* g, unsigned int v);

//
// For a digraph
//
unsigned int GraphGetVertexOutDegree(Graph* g, unsigned int v);

//
// For a digraph
//
unsigned int GraphGetVertexInDegree(Graph* g, unsigned int v);

// Edges

int GraphAddEdge(Graph* g, unsigned int v, unsigned int w);

int GraphAddWeightedEdge(Graph* g, unsigned int v, unsigned int w,
                         double weight);

// CHECKING

int GraphCheckInvariants(const Graph* g);

// DISPLAYING on the console

void GraphDisplay(const Graph* g);

void GraphListAdjacents(const Graph* g, unsigned int v);

void GraphDisplayDOT(const Graph* g);

void GraphToDOTFile(const Graph* g, FILE* f);

#endif  // _GRAPH_
