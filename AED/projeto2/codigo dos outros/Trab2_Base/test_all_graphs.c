#include <stdio.h>
#include "Graph.h"
#include "GraphDominatingSets.h"
#include "IndicesSet.h"

int main(void) {
  const char* files[] = {"G_1.txt", "G_3.txt", "G_4.txt", "G_2.txt"};
  
  for (int i = 0; i < 4; i++) {
    printf("=== Testing %s ===\n", files[i]);
    FILE* f = fopen(files[i], "r");
    if (!f) {
      printf("File not found!\n\n");
      continue;
    }
    
    Graph* g = GraphFromFile(f);
    fclose(f);
    
    printf("Vertices: %u, Edges: %u\n", 
           GraphGetNumVertices(g), GraphGetNumEdges(g));
    
    IndicesSet* mdset = GraphComputeMinDominatingSet(g);
    printf("MIN Dominating Set: ");
    IndicesSetDisplay(mdset);
    
    IndicesSetDestroy(&mdset);
    GraphDestroy(&g);
    printf("\n");
  }
  
  return 0;
}
