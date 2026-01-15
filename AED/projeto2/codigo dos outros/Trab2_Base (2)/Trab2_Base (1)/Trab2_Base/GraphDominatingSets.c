//
// Algoritmos e Estruturas de Dados --- 2025/2026
//
// Joaquim Madeira - Nov 2025
//
// GraphDominatingSets - Computing Vertex Dominating Sets for UNDIRECTED graphs
//

// Student Name : Fábio Emanuel Moreira Renda
// Student Number : 125718
// Student Name : Santiago Rodrigues dos Santos
// Student Number : 124926

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

  IndicesSet* allVertices = GraphGetSetVertices(g);                   //obter todos os vértices do grafo

  unsigned int range = GraphGetVertexRange(g);                        

  for (unsigned v=0; v<range; v++){
    if (!IndicesSetContains(allVertices,v)){                           //ignora os vértices que não estão no grafo
      continue;
    }

    if (IndicesSetContains(vertSet,v)){                                 //so os vértices estiverem no dominant sets, já foram cobertos
    continue;
  }
  
                                                                      //se o vértice não estiver no dominant set, verificar os pontos adjacentes
  IndicesSet* adjanctoV = GraphGetSetAdjacentsTo(g,v);                // buscar todos os vértices adjacentes a v no grafo g

  unsigned int adjancenttoDom = 0;

  for (unsigned int w = 0; w<range; w++){                              //ir por todos os vértices adacentes  
    if (IndicesSetContains(adjanctoV,w) && IndicesSetContains(vertSet,w)){
      adjancenttoDom = 1;                                              //é adjacente se w é adjacente a v e se w estiver no set dominante
      break;
    }
  }
  IndicesSetDestroy(&adjanctoV);


  if (adjancenttoDom==0){                                                //se o vértice não está no set dominante e não é adjacente a nenhum vértice nele, então não é um set dominante
    IndicesSetDestroy(&allVertices);
    return 0;
  }
  }

  IndicesSetDestroy(&allVertices);                                     
  return 1;                                                           //se o resto das outras condições não se confirmarem, retorna 1 e o set é dominante 
} 


//
// COMPLETED
//


//helper utilizado para auxiliar a função recursiva no GraphComputeMinDominatingSet
static void backtrackHelper(Graph* g, unsigned int* vertices, unsigned int numVertices, unsigned int pos, unsigned int currentSize, IndicesSet* currentSet, IndicesSet** bestSet, unsigned int* bestSize) {
  
  if (currentSize >= *bestSize) {               //se o tamanho atual já é >= melhor tamanho encontrado
    return;
  }

  if (pos == numVertices) {                     // Caso 1: consideramos todos os vértices
    if (currentSize > 0 && GraphIsDominatingSet(g, currentSet)) {  // Verifica se o conjunto atual é um conjunto dominante
      if (*bestSet != NULL){
        IndicesSetDestroy(bestSet);               // Liberta o conjunto anterior
      }
      *bestSet = IndicesSetCreateCopy(currentSet);  // Cria cópia do novo conjunto
      *bestSize = currentSize;                  // Atualiza o melhor tamanho
    }
    return;

  }

  backtrackHelper(g, vertices, numVertices, pos + 1, currentSize, currentSet, bestSet, bestSize);     // Opção 1: não incluir este vértice no conjunto

  IndicesSetAdd(currentSet, vertices[pos]);           // Opção 2: incluir este vértice no conjunto
  backtrackHelper(g, vertices, numVertices, pos + 1, currentSize + 1, currentSet, bestSet, bestSize);
  
  IndicesSetRemove(currentSet, vertices[pos]);        //remove o vértice que acabamos de adicionar

}
// Compute a MIN VERTEX DOMINATING SET of the graph
// using an EXHAUSTIVE SEARCH approach
// Return the/a dominating set
//
IndicesSet* GraphComputeMinDominatingSet(const Graph* g){
  assert(g != NULL);
  assert(GraphIsDigraph(g) == 0);

  IndicesSet* allVertices = GraphGetSetVertices(g);

  unsigned int range= GraphGetVertexRange(g);
  unsigned int numVertices = IndicesSetGetNumElems(allVertices);

  IndicesSet* bestSet= GraphGetSetVertices(g);                                //melhor solução encontrada (começa com todos os vértices inicialmente)
  unsigned int bestSize = IndicesSetGetNumElems(bestSet);


  IndicesSet* currentSet = IndicesSetCreateEmpty(range);                      //o set que está a ser atualmente testado

  unsigned int* vertices = malloc(numVertices * sizeof(unsigned int));        //array de todos os vértices
  if (vertices == NULL){ 
    IndicesSetDestroy(&bestSet);                                              //se náo houver vértices destroi os sets
    IndicesSetDestroy(&currentSet);
    IndicesSetDestroy(&allVertices);
    abort();}

  unsigned int index = 0;
  for (unsigned int v = 0; v < range; v++) {                                  //preencher o array com os vértices do grafo
    if (IndicesSetContains(allVertices,v)){
      vertices[index++]=v;
    }
  }

   backtrackHelper((Graph*)g, vertices, numVertices, 0, 0, currentSet, &bestSet, &bestSize);  //começar o backtrack (vai para a função auxiliar)

  IndicesSetDestroy(&currentSet);                                           //liberta e destroy os sets não necessários ao retorno
  IndicesSetDestroy(&allVertices);
  free(vertices);

  return bestSet;
}

//
// TO BE COMPLETED
//
// Compute a MIN WEIGHT VERTEX DOMINATING SET of the graph
// using an EXHAUSTIVE SEARCH approach
// Return the dominating set
//

static void backtrackweightaux(Graph* g, unsigned int* vertices, unsigned int numVertices, double* vertexWeight, unsigned int pos, double currentWeight, IndicesSet* currentSet, IndicesSet** bestSet, double* bestWeight){

  if (currentWeight>=*bestWeight){                                                  //se o weight atual já é >= melhor weight encontrado
    return;
  }
  if (pos == numVertices) {                                                         // Caso 1: consideramos todos os vértices
    if ((!IndicesSetIsEmpty(currentSet)) && GraphIsDominatingSet(g, currentSet)){   // Verifica se o conjunto atual é um conjunto dominante
      if (*bestSet!=NULL){                                                          
        IndicesSetDestroy(bestSet);}                                                // Liberta o conjunto anterior
        *bestSet = IndicesSetCreateCopy(currentSet);                                // Cria cópia do novo conjunto
        *bestWeight=currentWeight;                                                  // Atualiza o melhor weight
      
    }

    return;
}

  backtrackweightaux(g, vertices, numVertices, vertexWeight, pos + 1, currentWeight, currentSet, bestSet, bestWeight);     // Opção 1: não incluir este vértice no conjunto

  IndicesSetAdd(currentSet, vertices[pos]);           // Opção 2: incluir este vértice no conjunto
  backtrackweightaux(g, vertices, numVertices, vertexWeight, pos + 1, currentWeight + vertexWeight[vertices[pos]], currentSet, bestSet, bestWeight);

  IndicesSetRemove(currentSet, vertices[pos]);        //remove o vértice que acabamos de adicionar


}



IndicesSet* GraphComputeMinWeightDominatingSet(const Graph* g) {
  assert(g != NULL);
  assert(GraphIsDigraph(g) == 0);

  double* vertexWeight = GraphComputeVertexWeights(g);                                  //obter peso dos vértices
  unsigned int range = GraphGetVertexRange(g);  

  IndicesSet* allVertices = GraphGetSetVertices(g);                                    //obter todos os vértices do grafo
  unsigned int numVertices = IndicesSetGetNumElems(allVertices);

  unsigned int* vertices = malloc(numVertices * sizeof(unsigned int));                //array para armazenar os vértices
  if (vertices==NULL){                                                                //se náo houver vértices destroi os sets
    IndicesSetDestroy(&allVertices);
    free(vertexWeight);
    abort();}


  unsigned int index = 0; 
  for (unsigned int v=0; v<range; v++){                                               //preencher o array com os vértices do grafo                         
    if(IndicesSetContains(allVertices,v)){
      vertices[index++] = v;
    }
  }

  IndicesSet* bestSet = IndicesSetCreateCopy(allVertices);                             //melhor solução encontrada (começa com todos os vértices inicialmente)
  double bestWeight = 0.0;

  for (unsigned int w=0; w<range; w++){                                               //meter bestWeight com o weight dos vértices w
    if(vertexWeight[w]>=0){
      bestWeight += vertexWeight[w];
    }
  }

  IndicesSet* currentSet = IndicesSetCreateEmpty(range);                               //o set que está a ser atualmente testado

  backtrackweightaux(g,vertices,numVertices,vertexWeight,0,0.0,currentSet,&bestSet,&bestWeight);    //começar o backtrack (vai para a função auxiliar)

  IndicesSetDestroy(&allVertices);                                                     //liberta e destroy os sets não necessários ao retorno
  IndicesSetDestroy(&currentSet);
  free(vertices);
  free(vertexWeight);

  return bestSet;
}
