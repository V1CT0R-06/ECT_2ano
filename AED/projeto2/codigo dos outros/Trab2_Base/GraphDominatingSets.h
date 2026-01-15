//
// Algoritmos e Estruturas de Dados --- 2025/2026
//
// Joaquim Madeira - Nov 2025
//
// DO NOT MODIFY THIS FILE
//
// GraphDominatingSets - Computing Vertex Dominating Sets for UNDIRECTED graphs
//

#ifndef _GRAPH_DOMINATING_SETS_
#define _GRAPH_DOMINATING_SETS_

#include "Graph.h"
#include "IndicesSet.h"

int GraphIsDominatingSet(const Graph* g, IndicesSet* vertSet);

IndicesSet* GraphComputeMinDominatingSet(const Graph* g);

IndicesSet* GraphComputeMinWeightDominatingSet(const Graph* g);

#endif  // _GRAPH_DOMINATING_SETS_
