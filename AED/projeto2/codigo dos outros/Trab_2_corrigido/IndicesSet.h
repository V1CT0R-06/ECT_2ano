/// IndicesSet - A simple ADT for storing a set of indices in a
///              given range
///
/// This module is part of a programming project for the course
/// AED, DETI / UA.PT
///
/// DO NOT MODIFY THIS FILE
///
/// The AED Team <jmadeira@ua.pt, jmr@ua.pt, ...>
/// 2025

#ifndef _IndicesSet_H_
#define _IndicesSet_H_

#include <inttypes.h>

typedef struct _IndicesSet IndicesSet;

// Create and destroy sets

IndicesSet* IndicesSetCreateEmpty(uint16_t range);

IndicesSet* IndicesSetCreateFull(uint16_t range);

IndicesSet* IndicesSetCreateComplement(const IndicesSet* p);

IndicesSet* IndicesSetCreateCopy(const IndicesSet* p);

void IndicesSetDestroy(IndicesSet** pp);

// Operations with individual elements

int IndicesSetContains(const IndicesSet* p, uint16_t v);

int IndicesSetAdd(IndicesSet* p, uint16_t v);

int IndicesSetRemove(IndicesSet* p, uint16_t v);

// Basic properties

uint16_t IndicesSetGetRange(const IndicesSet* p);

int IndicesSetIsEmpty(const IndicesSet* p);

uint16_t IndicesSetGetNumElems(const IndicesSet* p);

int IndicesSetIsSubset(const IndicesSet* p1, const IndicesSet* p2);

int IndicesSetIsEqual(const IndicesSet* p1, const IndicesSet* p2);

int IndicesSetIsDifferent(const IndicesSet* p1, const IndicesSet* p2);

// Union, intersection and difference operations
// The first set is updated
// The second set is untouched

void IndicesSetUnion(IndicesSet* p1, const IndicesSet* p2);

void IndicesSetIntersection(IndicesSet* p1, const IndicesSet* p2);

void IndicesSetDifference(IndicesSet* p1, const IndicesSet* p2);

// To iterate over the elements of a set

int IndicesSetGetFirstElem(IndicesSet* p);

int IndicesSetGetNextElem(IndicesSet* p);

// To iterate over all possible subsets of the range in binary table order

int IndicesSetNextSubset(IndicesSet* p);

// Help functions

void IndicesSetDisplay(const IndicesSet* p);

#endif  // _IndicesSet_H_
