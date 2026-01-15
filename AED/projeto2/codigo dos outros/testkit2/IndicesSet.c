/// IndicesSet - A simple ADT for storing a set ofindices in a
///              given range
///
/// This module is part of a programming project for the course
/// AED, DETI / UA.PT
///
/// DO NOT MODIFY THIS FILE
///
/// The AED Team <jmadeira@ua.pt, jmr@ua.pt, ...>
/// 2025

#include "IndicesSet.h"

#include <assert.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "instrumentation.h"

struct _IndicesSet {
  uint16_t range;        // elements in range 0, 1, ... (range - 1)
  uint16_t num_elems;    // number of stored elements
  int32_t current_elem;  // iterator for the set
  uint8_t* set;
};

// LOCAL AUXILIARY FUNCTIONS

static void _SetIteratorToFirstElem(IndicesSet* p) {
  if (p->num_elems > 0) {
    for (uint16_t i = 0; i < p->range; i++) {
      if (p->set[i] == 1) {
        p->current_elem = i;  // The first element found
        break;
      }
    }

  } else {
    p->current_elem = -1;  // No elements yet
  }
}

// Create sets

IndicesSet* IndicesSetCreateEmpty(uint16_t max_size) {
  assert(max_size > 0);

  IndicesSet* p = malloc(sizeof(struct _IndicesSet));
  if (p == NULL) abort();

  p->set = calloc(max_size, sizeof(uint8_t));
  if (p->set == NULL) abort();

  p->range = max_size;
  p->num_elems = 0;
  p->current_elem = -1;  // No element  s yet

  return p;
}

IndicesSet* IndicesSetCreateFull(uint16_t max_size) {
  assert(max_size > 0);

  IndicesSet* p = malloc(sizeof(struct _IndicesSet));
  if (p == NULL) abort();

  p->set = malloc(max_size * sizeof(uint8_t));
  if (p->set == NULL) abort();

  for (uint16_t i = 0; i < max_size; i++) {
    p->set[i] = 1;
  }

  p->range = max_size;
  p->num_elems = max_size;
  p->current_elem = 0;  // The first element

  return p;
}

IndicesSet* IndicesSetCreateComplement(const IndicesSet* p) {
  IndicesSet* comp = IndicesSetCreateEmpty(p->range);

  if (p->num_elems == p->range) {
    // The complementar set is the empty set
    return comp;
  }

  // Setting the elements of the complementar set
  for (uint16_t i = 0; i < p->range; i++) {
    if (p->set[i] == 0) {
      comp->set[i] = 1;
    }
  }

  comp->num_elems = p->range - p->num_elems;

  // To be independent from the iterator position on the original set
  _SetIteratorToFirstElem(comp);

  return comp;
}

IndicesSet* IndicesSetCreateCopy(const IndicesSet* p) {
  IndicesSet* copy = IndicesSetCreateEmpty(p->range);

  if (p->num_elems == 0) {
    return copy;
  }

  // Original set is not empty

  memcpy(copy->set, p->set, p->range);

  copy->num_elems = p->num_elems;

  // To be independent from the iterator position on the original set

  _SetIteratorToFirstElem(copy);

  return copy;
}

void IndicesSetDestroy(IndicesSet** pp) {
  assert(*pp != NULL);

  IndicesSet* p = *pp;

  free(p->set);

  free(*pp);

  *pp = NULL;
}

// Operations with individual elements

int IndicesSetContains(const IndicesSet* p, uint16_t v) {
  assert(v < p->range);

  return p->set[v];
}

int IndicesSetAdd(IndicesSet* p, uint16_t v) {
  assert(v < p->range);

  if (p->set[v]) {
    // Already in set
    return 0;
  }

  p->set[v] = 1;

  p->num_elems++;

  // Reset the set iterator
  _SetIteratorToFirstElem(p);

  return 1;
}

int IndicesSetRemove(IndicesSet* p, uint16_t v) {
  assert(v < p->range);

  if (p->set[v]) {
    p->set[v] = 0;
    p->num_elems--;

    // Reset the set iterator
    _SetIteratorToFirstElem(p);

    return 1;
  }

  // NOT in set
  return 0;
}

// Basic properties

uint16_t IndicesSetGetRange(const IndicesSet* p) { return p->range; }

int IndicesSetIsEmpty(const IndicesSet* p) { return (p->num_elems == 0); }

uint16_t IndicesSetGetNumElems(const IndicesSet* p) { return p->num_elems; }

int IndicesSetIsSubset(const IndicesSet* p1, const IndicesSet* p2) {
  assert(p1->range == p2->range);

  if (p1->num_elems > p2->num_elems) {
    return 0;
  }

  for (uint16_t i = 0; i < p1->range; i++) {
    if (p1->set[i] && !p2->set[i]) {
      return 0;
    }
  }

  return 1;
}

int IndicesSetIsEqual(const IndicesSet* p1, const IndicesSet* p2) {
  assert(p1->range == p2->range);

  if (p1->num_elems != p2->num_elems) {
    return 0;
  }

  if (memcmp(p1->set, p2->set, p1->range) == 0) {
    return 1;
  }

  return 0;
}

int IndicesSetIsDifferent(const IndicesSet* p1, const IndicesSet* p2) {
  return !IndicesSetIsEqual(p1, p2);
}

// Union, intersection and difference operations
// The first set is updated
// The second set is untouched
void IndicesSetUnion(IndicesSet* p1, const IndicesSet* p2) {
  assert(p1->range == p2->range);

  for (uint16_t i = 0; i < p1->range; i++) {
    if (!p1->set[i] && p2->set[i]) {
      p1->set[i] = 1;
      p1->num_elems++;
    }
  }

  // Reset the iterator of the first set
  _SetIteratorToFirstElem(p1);
}

// Union, intersection and difference operations
// The first set is updated
// The second set is untouched
void IndicesSetIntersection(IndicesSet* p1, const IndicesSet* p2) {
  assert(p1->range == p2->range);

  for (uint16_t i = 0; i < p1->range; i++) {
    if (p1->set[i] && !p2->set[i]) {
      p1->set[i] = 0;
      p1->num_elems--;
    }
  }

  // Reset the iterator of the first set
  _SetIteratorToFirstElem(p1);
}

// Union, intersection and difference operations
// The first set is updated
// The second set is untouched
void IndicesSetDifference(IndicesSet* p1, const IndicesSet* p2) {
  assert(p1->range == p2->range);

  for (uint16_t i = 0; i < p1->range; i++) {
    if (p1->set[i] && p2->set[i]) {
      p1->set[i] = 0;
      p1->num_elems--;
    }
  }

  // Reset the iterator of the first set
  _SetIteratorToFirstElem(p1);
}

// To iterate over the elements of a set

/// Return the first element in the set OR -1, if the set is empty
/// The set iterator is reset to the first element or set to -1
int IndicesSetGetFirstElem(IndicesSet* p) {
  // Reset the iterator
  _SetIteratorToFirstElem(p);

  return p->current_elem;
}

/// Return the next element in the set OR -1, if past the last
/// The set iterator moves to the next element or set to -1
int IndicesSetGetNextElem(IndicesSet* p) {
  assert(p->num_elems > 0);

  // Move to next element, if any
  uint16_t i = p->current_elem + 1;
  for (; i < p->range; i++) {
    if (p->set[i]) {
      break;
    }
  }

  if (i == p->range) {
    // Past the last element
    p->current_elem = -1;
  } else {
    p->current_elem = i;
  }

  return p->current_elem;
}

// To iterate over all possible subsets of {0, 1, ... , (range - 1)}
// in binary table order
// USE WITH CARE: going over ALL elements in the range
// The set is UPDATED to the next subset in binary table order,
// by adding 1 to the corresponding binary number
// Return 0, if past the last subset (i.e., past the full set)
// Return 1, otherwise
int IndicesSetNextSubset(IndicesSet* p) {
  uint16_t i = 0;

  while ((i < p->range) && (p->set[i] == 1)) {
    p->set[i] = 0;
    p->num_elems--;
    i++;
  }

  if (i < p->range) {
    p->set[i] = 1;
    p->num_elems++;
    _SetIteratorToFirstElem(p);
    return 1;
  }

  /* Overflow */
  _SetIteratorToFirstElem(p);
  return 0;
}

// Help functions

void IndicesSetDisplay(const IndicesSet* p) {
  printf("Number of elements = %u\n", p->num_elems);

  if (p->num_elems == 0) {
    printf("{ }\n");
    return;
  }

  int first = 1;
  printf("{ ");
  for (uint16_t i = 0; i < p->range; i++) {
    if (p->set[i]) {
      if (first) {
        printf("%d", (int)i);
        first = 0;
      } else {
        printf(", %d", (int)i);
      }
    }
  }
  printf(" }\n");
}
