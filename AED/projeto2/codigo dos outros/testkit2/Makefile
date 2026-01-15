# To compile all programs, run:
#   make
#
# AED, UA, 2025

# The program to show differences:
DIFF = meld

CFLAGS += -g -Wall -Wextra

#TARGETS = TestDominatingSets TestGraphBasics TestIndicesSet
#TARGETS += TestGraphBasics2

TARGETS += TestGetSetAdjacentsTo TestComputeVertexWeights TestGetSubgraph
TARGETS += TestIsDominatingSet TestMinDominatingSet TestMinWeightDominatingSet
TARGETS += TestAllFunctions

all: $(TARGETS)

# make new   # limpa a pasta para preparar uma nova avaliação
new: clean
	rm -f test? Graph.c GraphDominatingSets.c
	@echo
	@echo "### NOVA AVALIACAO ###"
	@echo "- Descarrega o arquivo ZIP do trabalho."
	@echo "- Extrai os ficheiros: Graph.c GraphDominatingSets.c"
	@echo "  para dentro da pasta testkit2/."
	@echo

clean:
	rm -f *.o
	rm -f $(TARGETS)


TestGraphBasics2: TestGraphBasics2.o \
  Graph.o \
  SortedList.o IntegersStack.o IndicesSet.o instrumentation.o

TestIndicesSet: TestIndicesSet.o IndicesSet.o instrumentation.o


test1: TestGetSetAdjacentsTo
	./$^ > $@  2>&1
	$(DIFF) $@ expected/$@

TestGetSetAdjacentsTo: TestGetSetAdjacentsTo.o \
  Graph.o \
  SortedList.o IntegersStack.o IndicesSet.o instrumentation.o


test2: TestComputeVertexWeights
	./$^ > $@  2>&1
	$(DIFF) $@ expected/$@

TestComputeVertexWeights: TestComputeVertexWeights.o \
  Graph.o \
  SortedList.o IntegersStack.o IndicesSet.o instrumentation.o


test3: TestGetSubgraph
	./$^ > $@  2>&1
	$(DIFF) $@ expected/$@

TestGetSubgraph: TestGetSubgraph.o \
  Graph.o \
  SortedList.o IntegersStack.o IndicesSet.o instrumentation.o
  

test4: TestIsDominatingSet
	./$^ > $@  2>&1
	$(DIFF) $@ expected/$@

TestIsDominatingSet: TestIsDominatingSet.o \
  GraphDominatingSets.o \
  Graph-REFERENCE.o \
  SortedList.o IntegersStack.o IndicesSet.o instrumentation.o


test5: TestMinDominatingSet
	./$^ > $@  2>&1
	$(DIFF) $@ expected/$@

TestMinDominatingSet: TestMinDominatingSet.o \
  GraphDominatingSets.o \
  Graph-REFERENCE.o \
  SortedList.o IntegersStack.o IndicesSet.o instrumentation.o


test6: TestMinWeightDominatingSet
	./$^ > $@  2>&1
	$(DIFF) $@ expected/$@

TestMinWeightDominatingSet: TestMinWeightDominatingSet.o \
  GraphDominatingSets.o \
  Graph-REFERENCE.o \
  SortedList.o IntegersStack.o IndicesSet.o instrumentation.o


test7: TestAllFunctions
	valgrind ./$^ > $@  2>&1
	$(DIFF) $@ expected/$@

TestAllFunctions: TestAllFunctions.o \
  GraphDominatingSets.o \
  Graph.o \
  SortedList.o IntegersStack.o IndicesSet.o instrumentation.o

# Dependencies of source files

Graph.o: Graph.c Graph.h \
  SortedList.h IndicesSet.h instrumentation.h

GraphDominatingSets.o: GraphDominatingSets.c GraphDominatingSets.h \
  Graph.h SortedList.h IndicesSet.h instrumentation.h

IntegersStack.o: IntegersStack.c IntegersStack.h instrumentation.h

instrumentation.o: instrumentation.c instrumentation.h

SortedList.o: SortedList.c SortedList.h instrumentation.h

IndicesSet.o: IndicesSet.c IndicesSet.h instrumentation.h

TestDominatingSets.o: TestDominatingSets.c Graph.h GraphDominatingSets.h \
  IndicesSet.h instrumentation.h

TestGraphBasics.o: TestGraphBasics.c Graph.h IndicesSet.h instrumentation.h 

TestGraphBasics2.o: TestGraphBasics2.c Graph.h IndicesSet.h instrumentation.h 

TestIndicesSet.o: TestIndicesSet.c IndicesSet.h instrumentation.h


TestGetSetAdjacentsTo.o: TestGetSetAdjacentsTo.c \
  Graph.h \
  SortedList.h IntegersStack.h IndicesSet.h instrumentation.h

TestComputeVertexWeights.o: TestComputeVertexWeights.c \
  Graph.h \
  SortedList.h IntegersStack.h IndicesSet.h instrumentation.h

TestGetSubgraph.o: TestGetSubgraph.c \
  Graph.h \
  SortedList.h IntegersStack.h IndicesSet.h instrumentation.h

TestIsDominatingSet.o: TestIsDominatingSet.c Graph.h \
  GraphDominatingSets.h SortedList.h IntegersStack.h \
  IndicesSet.h instrumentation.h

TestMinDominatingSet.o: TestMinDominatingSet.c Graph.h \
  GraphDominatingSets.h SortedList.h IntegersStack.h \
  IndicesSet.h instrumentation.h  

TestMinWeightDominatingSet.o: TestMinWeightDominatingSet.c Graph.h \
  GraphDominatingSets.h SortedList.h IntegersStack.h \
  IndicesSet.h instrumentation.h  

TestAllFunctions.o: TestAllFunctions.c \
  Graph.h GraphDominatingSets.h \
  SortedList.h IntegersStack.h IndicesSet.h instrumentation.h

