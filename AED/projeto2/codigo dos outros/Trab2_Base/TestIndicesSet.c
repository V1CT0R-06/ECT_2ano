//
// Testing the simple Indices Set ADT, that stores indices
// in a given range
//
// J. Madeira - November 2025
//

#include <assert.h>
#include <stdio.h>

#include "IndicesSet.h"

int main(void) {
  // Create some sets

  printf("Empty set\n");
  IndicesSet* empty_set = IndicesSetCreateEmpty(12);
  assert(IndicesSetIsEmpty(empty_set));
  assert(IndicesSetContains(empty_set, 5) == 0);
  IndicesSetDisplay(empty_set);
  printf("\n");

  printf("Full set\n");
  IndicesSet* full_set = IndicesSetCreateFull(12);
  assert(IndicesSetGetNumElems(full_set) == 12);
  assert(IndicesSetContains(full_set, 5));
  IndicesSetDisplay(full_set);
  printf("\n");

  printf("The complement set of the empty set\n");
  IndicesSet* comp_set = IndicesSetCreateComplement(empty_set);
  assert(IndicesSetGetNumElems(comp_set) == IndicesSetGetNumElems(full_set));
  IndicesSetDisplay(comp_set);
  printf("\n");

  printf("A copy of the empty set\n");
  IndicesSet* another_set = IndicesSetCreateCopy(empty_set);
  assert(IndicesSetIsEqual(another_set, empty_set));
  IndicesSetDisplay(another_set);
  printf("\n");

  printf("A copy of the full set\n");
  IndicesSet* another_full_set = IndicesSetCreateCopy(full_set);
  assert(IndicesSetIsEqual(another_full_set, full_set));
  IndicesSetDisplay(another_full_set);
  printf("\n");

  // Add and remove elements

  for (uint16_t e = 1; e < 13; e += 2) {
    IndicesSetAdd(another_set, e);
  }
  assert(IndicesSetGetNumElems(another_set) == 6);

  printf("After adding some elements\n");
  IndicesSetDisplay(another_set);
  printf("\n");

  for (uint16_t e = 0; e < 7; e++) {
    IndicesSetRemove(another_set, e);
  }
  assert(IndicesSetGetNumElems(another_set) == 3);

  printf("After removing some elements\n");
  IndicesSetDisplay(another_set);
  printf("\n");

  IndicesSet* small_set = IndicesSetCreateCopy(another_set);
  assert(IndicesSetIsEqual(small_set, another_set));

  for (uint16_t e = 4; e < 12; e += 2) {
    IndicesSetAdd(another_set, e);
  }
  assert(IndicesSetGetNumElems(another_set) == 7);

  printf("After adding some elements\n");
  IndicesSetDisplay(another_set);
  printf("\n");

  assert(IndicesSetIsSubset(small_set, another_set));

  // Union, intersection and difference
  // First set is the resulting set

  IndicesSetIntersection(empty_set, full_set);
  assert(IndicesSetGetNumElems(empty_set) == 0);

  IndicesSetUnion(full_set, small_set);
  assert(IndicesSetGetNumElems(full_set) == 12);

  IndicesSetUnion(empty_set, small_set);
  assert(IndicesSetIsEqual(empty_set, small_set));

  IndicesSetDifference(empty_set, small_set);
  assert(IndicesSetIsEmpty(empty_set));

  printf("Set difference\n");
  IndicesSetDisplay(another_set);
  IndicesSetDisplay(small_set);
  IndicesSetDifference(another_set, small_set);
  IndicesSetDisplay(another_set);
  printf("\n");

  printf("Set Union\n");
  IndicesSetDisplay(another_set);
  IndicesSetDisplay(small_set);
  IndicesSetUnion(another_set, small_set);
  IndicesSetDisplay(another_set);
  printf("\n");

  printf("Set intersection\n");
  IndicesSetDisplay(another_set);
  IndicesSetDisplay(full_set);
  IndicesSetIntersection(another_set, full_set);
  IndicesSetDisplay(another_set);
  printf("\n");

  printf("Set intersection\n");
  IndicesSetDisplay(full_set);
  IndicesSetDisplay(another_set);
  IndicesSetIntersection(full_set, another_set);
  IndicesSetDisplay(full_set);
  printf("\n");

  // Iterating over the elements

  // Checking the empty set
  IndicesSetDisplay(empty_set);
  int e = IndicesSetGetFirstElem(empty_set);
  assert(e == -1);
  printf("\n");

  IndicesSetDisplay(small_set);
  printf("Walking through the elements of the set\n");
  e = IndicesSetGetFirstElem(small_set);
  assert(e != -1);
  printf("%d ", e);
  while ((e = IndicesSetGetNextElem(small_set)) != -1) {
    printf("%d ", e);
  }
  printf("\n");

  IndicesSetRemove(small_set, 11);
  IndicesSetDisplay(small_set);
  printf("Walking through the elements of the set\n");
  e = IndicesSetGetFirstElem(small_set);
  assert(e != -1);
  printf("%d ", e);
  while ((e = IndicesSetGetNextElem(small_set)) != -1) {
    printf("%d ", e);
  }
  printf("\n");

  // Walking through all subsets of a set of n integers {0, 1, ..., (n - 1)}

  printf("\n");
  // Set {0}
  IndicesSet* set_1 = IndicesSetCreateEmpty(1);
  do {
    IndicesSetDisplay(set_1);
  } while (IndicesSetNextSubset(set_1));
  printf("\n");

  printf("\n");
  // Set {0, 1}
  IndicesSet* set_2 = IndicesSetCreateEmpty(2);
  do {
    IndicesSetDisplay(set_2);
  } while (IndicesSetNextSubset(set_2));
  printf("\n");

  printf("\n");
  // Set {0, 1, 2}
  IndicesSet* set_3 = IndicesSetCreateEmpty(3);
  do {
    IndicesSetDisplay(set_3);
  } while (IndicesSetNextSubset(set_3));
  printf("\n");

  // Housekeeping

  IndicesSetDestroy(&empty_set);
  IndicesSetDestroy(&full_set);
  IndicesSetDestroy(&comp_set);
  IndicesSetDestroy(&another_set);
  IndicesSetDestroy(&another_full_set);
  IndicesSetDestroy(&small_set);
  IndicesSetDestroy(&set_1);
  IndicesSetDestroy(&set_2);
  IndicesSetDestroy(&set_3);

  return 0;
}