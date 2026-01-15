//
// Algoritmos e Estruturas de Dados --- 2025/2026
//
// Joaquim Madeira - Nov 2025
//
// GraphDominatingSets - Computing Vertex Dominating Sets for UNDIRECTED graphs
//

#include "GraphDominatingSets.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "Graph.h"
#include "IndicesSet.h"
#include "instrumentation.h"

//
// ===============================
// FUNCTION: GraphIsDominatingSet
// ===============================
//
// Check if the given set of vertices is a dominating set for the graph.
// Returns 1 if true, 0 otherwise.
//
// A dominating set D of a graph G = (V,E) satisfies:
//   Every vertex not in D is adjacent to at least one vertex in D.
//
int GraphIsDominatingSet(const Graph* g, IndicesSet* vertSet) {
    assert(g != NULL);
    assert(GraphIsDigraph(g) == 0); // only undirected graphs
    assert(vertSet != NULL);

    unsigned int range = GraphGetVertexRange(g);

    // Iterate over all vertices in the graph
    for (unsigned int v = 0; v < range; ++v) {
        // Skip vertices already in the candidate dominating set
        if (IndicesSetContains(vertSet, v)) continue;

        // Get the set of vertices adjacent to v
        IndicesSet* adjSet = GraphGetSetAdjacentsTo(g, v);
        assert(adjSet != NULL); // ensure adjSet is valid

        int dominated = 0;

        // Iterate over neighbors to check if any are in vertSet
        int cur = IndicesSetGetFirstElem(adjSet);
        while (cur != -1) {
            if (IndicesSetContains(vertSet, (uint16_t)cur)) {
                dominated = 1;
                break;
            }
            cur = IndicesSetGetNextElem(adjSet);
        }

        // Free memory for adjacency set
        IndicesSetDestroy(&adjSet);

        // If vertex is not dominated, the set is not a dominating set
        if (!dominated) return 0;
    }

    // All vertices are dominated
    return 1;
}

//
// ===============================
// FUNCTION: GraphComputeMinDominatingSet
// ===============================
//
// Compute a MIN VERTEX DOMINATING SET using recursive backtracking
//

// Recursive helper function for minimum dominating set search
static void _minDominatingSetHelper(const Graph* g,
                                    IndicesSet* currentSet,
                                    IndicesSet** bestSet,
                                    unsigned int start) {
    // Prune if current set size is already >= best found
    if (IndicesSetGetNumElems(currentSet) >= IndicesSetGetNumElems(*bestSet)) return;

    // If current set is a dominating set, update bestSet
    if (GraphIsDominatingSet(g, currentSet)) {
        IndicesSet* tmp = *bestSet;
        *bestSet = IndicesSetCreateCopy(currentSet);
        IndicesSetDestroy(&tmp); // safely destroy previous best set
        return;
    }

    unsigned int range = GraphGetVertexRange(g);

    // Try adding each remaining vertex recursively
    for (unsigned int v = start; v < range; ++v) {
        if (!IndicesSetContains(currentSet, v)) {
            IndicesSetAdd(currentSet, (uint16_t)v);
            _minDominatingSetHelper(g, currentSet, bestSet, v + 1);
            IndicesSetRemove(currentSet, (uint16_t)v); // backtrack
        }
    }
}

IndicesSet* GraphComputeMinDominatingSet(const Graph* g) {
    assert(g != NULL);
    assert(GraphIsDigraph(g) == 0); // undirected graphs only

    unsigned int range = GraphGetVertexRange(g);

    // Current candidate dominating set (starts empty)
    IndicesSet* currentSet = IndicesSetCreateEmpty(range);

    // Initialize best set as all vertices (worst-case solution)
    // GraphGetSetVertices already returns a freshly allocated copy,
    // so use it directly to avoid creating a temporary copy that is
    // never freed (memory leak).
    IndicesSet* bestSet = GraphGetSetVertices(g);

    // Begin recursive search
    _minDominatingSetHelper(g, currentSet, &bestSet, 0);

    // Clean up current set
    IndicesSetDestroy(&currentSet);

    return bestSet;
}

//
// ===============================
// FUNCTION: GraphComputeMinWeightDominatingSet
// ===============================
//
// Compute a MIN WEIGHT VERTEX DOMINATING SET using recursive backtracking
// considering vertex weights.
//

// Recursive helper function for minimum weight dominating set
static void _minWeightDominatingSetHelper(const Graph* g,
                                          IndicesSet* currentSet,
                                          IndicesSet** bestSet,
                                          double* weights,
                                          double* minWeight,
                                          unsigned int start,
                                          double currentWeight) {
    // Prune if current weight already exceeds minimum found
    if (currentWeight >= *minWeight) return;

    // If current set dominates all vertices, update bestSet
    if (GraphIsDominatingSet(g, currentSet)) {
        *minWeight = currentWeight;
        IndicesSet* tmp = *bestSet;
        *bestSet = IndicesSetCreateCopy(currentSet);
        IndicesSetDestroy(&tmp); // safely destroy previous best
        return;
    }

    unsigned int range = GraphGetVertexRange(g);

    // Try adding each remaining vertex recursively
    for (unsigned int v = start; v < range; ++v) {
        if (!IndicesSetContains(currentSet, v)) {
            IndicesSetAdd(currentSet, (uint16_t)v);
            _minWeightDominatingSetHelper(g, currentSet, bestSet, weights, minWeight,
                                          v + 1, currentWeight + weights[v]);
            IndicesSetRemove(currentSet, (uint16_t)v); // backtrack
        }
    }
}

IndicesSet* GraphComputeMinWeightDominatingSet(const Graph* g) {
    assert(g != NULL);
    assert(GraphIsDigraph(g) == 0); // undirected graphs only

    unsigned int range = GraphGetVertexRange(g);

    // Compute vertex weights
    double* weights = GraphComputeVertexWeights(g);
    if (!weights) return IndicesSetCreateEmpty(range); // fallback if allocation failed

    // Current candidate dominating set (starts empty)
    IndicesSet* currentSet = IndicesSetCreateEmpty(range);

    // Initialize best set as all vertices (worst-case solution)
    // GraphGetSetVertices already returns a freshly allocated copy,
    // so use it directly to avoid creating a temporary copy that is
    // never freed (memory leak).
    IndicesSet* bestSet = GraphGetSetVertices(g);

    // Initialize minWeight as sum of all vertex weights
    double minWeight = 0.0;
    for (unsigned int i = 0; i < range; ++i) minWeight += weights[i];

    // Begin recursive search
    _minWeightDominatingSetHelper(g, currentSet, &bestSet, weights, &minWeight, 0, 0.0);

    // Clean up temporary structures
    free(weights);
    IndicesSetDestroy(&currentSet);

    return bestSet;
}
