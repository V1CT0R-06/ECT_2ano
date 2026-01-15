//
// Algoritmos e Estruturas de Dados --- 2025/2026
//
// Joaquim Madeira - Nov 2025
//
// GraphDominatingSets - Computing Vertex Dominating Sets for UNDIRECTED graphs
//

// Student Name : João Pinto
// Student Number : 113093
// Student Name : Juan
// Student Number : 103583

/*** COMPLETE THE GraphIsDominatingSet FUNCTION ***/
/*** COMPLETE THE GraphMinDominatingSet FUNCTION ***/
/*** COMPLETE THE GraphMinWeightDominatingSet FUNCTION ***/

#include "GraphDominatingSets.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "Graph.h"
#include "IndicesSet.h"
#include "instrumentation.h"

//
// TO BE COMPLETED
//
// Check if the given set is a dominating set for the graph
// Return 1 if true, or 0 otherwise
//
// A dominating set is a set of graph vertices such that every other
// graph vertex not in the set is adjacent to a graph vertex in the set
//
int GraphIsDominatingSet(const Graph* g, IndicesSet* vertSet) {
  assert(g != NULL);
  assert(GraphIsDigraph(g) == 0);
  assert(IndicesSetIsEmpty(vertSet) == 0);

  // Get all vertices in the graph
  IndicesSet* allVertices = GraphGetSetVertices(g);
  
  // Create a set of vertices that are either in vertSet or adjacent to it
  IndicesSet* dominated = IndicesSetCreateCopy(vertSet);
  
  // For each vertex in the dominating set, add its neighbors
  int v = IndicesSetGetFirstElem(vertSet);
  while (v != -1) {
    IndicesSet* adjacents = GraphGetSetAdjacentsTo(g, (unsigned int)v);
    IndicesSetUnion(dominated, adjacents);
    IndicesSetDestroy(&adjacents);
    v = IndicesSetGetNextElem(vertSet);
  }
  
  // Check if all vertices are dominated
  int result = IndicesSetIsSubset(allVertices, dominated);
  
  IndicesSetDestroy(&allVertices);
  IndicesSetDestroy(&dominated);

  return result;
}

//
// TO BE COMPLETED
//
// Compute a MIN VERTEX DOMINATING SET of the graph
// using an EXHAUSTIVE SEARCH approach
// Return the/a dominating set
//
IndicesSet* GraphComputeMinDominatingSet(const Graph* g) {
  assert(g != NULL);
  assert(GraphIsDigraph(g) == 0);

  unsigned int range = GraphGetVertexRange(g);
  unsigned int numVertices = GraphGetNumVertices(g);
  
  // Start with an empty set and iterate through all possible subsets
  IndicesSet* candidate = IndicesSetCreateEmpty(range);
  IndicesSet* result = NULL;
  unsigned int minSize = numVertices + 1;  // Initialize with a value larger than possible
  
  // Iterate through all possible subsets in binary table order
  // Start with empty set (already created)
  do {
    // Check if candidate has any elements and is a dominating set
    if (!IndicesSetIsEmpty(candidate)) {
      InstrCount[0]++;  // Count dominating set checks
      
      if (GraphIsDominatingSet(g, candidate)) {
        unsigned int candidateSize = IndicesSetGetNumElems(candidate);
        
        // If this is the smallest dominating set so far, save it
        if (candidateSize < minSize) {
          minSize = candidateSize;
          if (result != NULL) {
            IndicesSetDestroy(&result);
          }
          result = IndicesSetCreateCopy(candidate);
        }
      }
    }
  } while (IndicesSetNextSubset(candidate));
  
  IndicesSetDestroy(&candidate);
  
  // If no dominating set was found, return empty set
  if (result == NULL) {
    result = IndicesSetCreateEmpty(range);
  }

  return result;
}

//
// TO BE COMPLETED
//
// Compute a MIN WEIGHT VERTEX DOMINATING SET of the graph
// using an EXHAUSTIVE SEARCH approach
// Return the dominating set
//
IndicesSet* GraphComputeMinWeightDominatingSet(const Graph* g) {
  assert(g != NULL);
  assert(GraphIsDigraph(g) == 0);

  unsigned int range = GraphGetVertexRange(g);
  
  // Get the weights of all vertices
  double* weights = GraphComputeVertexWeights(g);
  
  // Start with an empty set and iterate through all possible subsets
  IndicesSet* candidate = IndicesSetCreateEmpty(range);
  IndicesSet* result = NULL;
  double minWeight = -1.0;  // -1.0 means no valid set found yet
  
  // Iterate through all possible subsets in binary table order
  do {
    // Check if candidate has any elements and is a dominating set
    if (!IndicesSetIsEmpty(candidate)) {
      InstrCount[1]++;  // Count dominating set checks
      
      if (GraphIsDominatingSet(g, candidate)) {
        // Calculate the total weight of this dominating set
        double candidateWeight = 0.0;
        int v = IndicesSetGetFirstElem(candidate);
        while (v != -1) {
          candidateWeight += weights[v];
          v = IndicesSetGetNextElem(candidate);
        }
        
        // If this is the smallest weight dominating set so far, save it
        if (minWeight < 0.0 || candidateWeight < minWeight) {
          minWeight = candidateWeight;
          if (result != NULL) {
            IndicesSetDestroy(&result);
          }
          result = IndicesSetCreateCopy(candidate);
        }
      }
    }
  } while (IndicesSetNextSubset(candidate));
  
  IndicesSetDestroy(&candidate);
  free(weights);
  
  // If no dominating set was found, return empty set
  if (result == NULL) {
    result = IndicesSetCreateEmpty(range);
  }

  return result;
}
