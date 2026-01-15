# To compile all programs, run:
#   make
#
# AED, UA, 2025

CFLAGS += -g -Wall -Wextra

TARGETS = TestDominatingSets TestGraphBasics TestIndicesSet

all: $(TARGETS)

TestDominatingSets: TestDominatingSets.o Graph.o \
  GraphDominatingSets.o SortedList.o \
  IndicesSet.o instrumentation.o

TestGraphBasics: TestGraphBasics.o Graph.o SortedList.o \
  IndicesSet.o instrumentation.o

TestIndicesSet: TestIndicesSet.o IndicesSet.o instrumentation.o

# Dependencies of source files

Graph.o: Graph.c Graph.h SortedList.h IndicesSet.h instrumentation.h

GraphDominatingSets.o: GraphDominatingSets.c GraphDominatingSets.h \
   Graph.h SortedList.h IndicesSet.h instrumentation.h

IntegersStack.o: IntegersStack.c IntegersStack.h instrumentation.h

instrumentation.o: instrumentation.c instrumentation.h

SortedList.o: SortedList.c SortedList.h instrumentation.h

IndicesSet.o: IndicesSet.c IndicesSet.h instrumentation.h

TestDominatingSets.o: TestDominatingSets.c Graph.h GraphDominatingSets.h \
  IndicesSet.h instrumentation.h

TestGraphBasics.o: TestGraphBasics.c Graph.h IndicesSet.h instrumentation.h 

TestIndicesSet.o: TestIndicesSet.c IndicesSet.h instrumentation.h

clean:
	rm -f *.o
	rm -f $(TARGETS)
