//
// Additional tests for the graph implementation
//

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "Graph.h"
#include "GraphDominatingSets.h"
#include "IndicesSet.h"

void test_weighted_graph() {
    printf("=== Testing Weighted Graph ===\n");
    
    // Create a weighted graph
    Graph* wg = GraphCreate(5, 0, 1);
    GraphAddWeightedEdge(wg, 0, 1, 2.5);
    GraphAddWeightedEdge(wg, 0, 2, 1.5);
    GraphAddWeightedEdge(wg, 1, 3, 3.0);
    GraphAddWeightedEdge(wg, 2, 3, 2.0);
    GraphAddWeightedEdge(wg, 3, 4, 1.0);
    
    printf("Weighted graph:\n");
    GraphDisplayDOT(wg);
    
    // Test vertex weights
    double* weights = GraphComputeVertexWeights(wg);
    printf("\nVertex weights:\n");
    for (unsigned int i = 0; i < GraphGetVertexRange(wg); i++) {
        if (weights[i] != -1.0) {
            printf("Vertex %u: weight = %.2f\n", i, weights[i]);
        }
    }
    free(weights);
    
    // Test min weight dominating set
    printf("\nFinding MIN WEIGHT dominating set:\n");
    IndicesSet* mwdset = GraphComputeMinWeightDominatingSet(wg);
    IndicesSetDisplay(mwdset);
    IndicesSetDestroy(&mwdset);
    
    GraphDestroy(&wg);
    printf("\n");
}

void test_small_graphs() {
    printf("=== Testing Small Graphs ===\n");
    
    // Test 1: Triangle graph (K3)
    Graph* g1 = GraphCreate(3, 0, 0);
    GraphAddEdge(g1, 0, 1);
    GraphAddEdge(g1, 1, 2);
    GraphAddEdge(g1, 2, 0);
    
    printf("Triangle graph:\n");
    GraphDisplayDOT(g1);
    
    IndicesSet* ds1 = GraphComputeMinDominatingSet(g1);
    printf("Min dominating set (should be size 1):\n");
    IndicesSetDisplay(ds1);
    assert(IndicesSetGetNumElems(ds1) == 1);
    IndicesSetDestroy(&ds1);
    
    GraphDestroy(&g1);
    
    // Test 2: Path graph P4
    Graph* g2 = GraphCreate(4, 0, 0);
    GraphAddEdge(g2, 0, 1);
    GraphAddEdge(g2, 1, 2);
    GraphAddEdge(g2, 2, 3);
    
    printf("\nPath graph (4 vertices):\n");
    GraphDisplayDOT(g2);
    
    IndicesSet* ds2 = GraphComputeMinDominatingSet(g2);
    printf("Min dominating set (should be size 2):\n");
    IndicesSetDisplay(ds2);
    assert(IndicesSetGetNumElems(ds2) == 2);
    IndicesSetDestroy(&ds2);
    
    GraphDestroy(&g2);
    printf("\n");
}

void test_subgraph_operations() {
    printf("=== Testing Subgraph Operations ===\n");
    
    Graph* g = GraphCreate(5, 0, 0);
    GraphAddEdge(g, 0, 1);
    GraphAddEdge(g, 1, 2);
    GraphAddEdge(g, 2, 3);
    GraphAddEdge(g, 3, 4);
    GraphAddEdge(g, 4, 0);
    
    printf("Original graph (cycle):\n");
    GraphDisplayDOT(g);
    
    // Create subgraph with vertices {0, 1, 2}
    IndicesSet* subset = IndicesSetCreateEmpty(5);
    IndicesSetAdd(subset, 0);
    IndicesSetAdd(subset, 1);
    IndicesSetAdd(subset, 2);
    
    Graph* subg = GraphGetSubgraph(g, subset);
    printf("\nSubgraph with vertices {0, 1, 2}:\n");
    GraphDisplayDOT(subg);
    
    assert(GraphGetNumVertices(subg) == 3);
    assert(GraphGetNumEdges(subg) == 2);
    
    IndicesSetDestroy(&subset);
    GraphDestroy(&subg);
    GraphDestroy(&g);
    printf("\n");
}

int main(void) {
    printf("Running Additional Tests\n");
    printf("========================\n\n");
    
    test_small_graphs();
    test_subgraph_operations();
    test_weighted_graph();
    
    printf("All additional tests passed!\n");
    
    return 0;
}
