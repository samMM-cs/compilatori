#include "symb_tab.h"
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

City *cityTable[TABLESIZE] = {NULL};
Tour *tourTable[TABLESIZE] = {NULL};
Tour *tourListHead = NULL;
Tour *tourListTail = NULL;

static inline void idDup(char dest[static 4], const char src[static 4]) {
  for (int i = 0; i < 4; i++)
    dest[i] = src[i];
}

unsigned int hash(const char *s) {
  unsigned int h = 0;
  for (; *s; s++)
    h = (127 * h + (unsigned char)*s) % TABLESIZE;
  return h;
}

City *lookupCity(const char id[4]) {
  for (City *city = cityTable[hash(id)]; city; city = city->next)
    if (strcmp(id, city->id) == 0)
      return city;
  return NULL;
}

City *insertCity(const char id[4], const char *name, double x, double y) {
  if (lookupCity(id)) {
    fprintf(stderr, "Semantic Error: City ID %s is already defined.\n", id);
    exit(EXIT_FAILURE);
  }
  City *city = (City *)malloc(sizeof(City));
  if (!city)
    exit(EXIT_FAILURE);
  idDup(city->id, id);
  city->name = strdup(name);
  city->x = x;
  city->y = y;
  unsigned int h = hash(id);
  city->next = cityTable[h];
  cityTable[h] = city;
  return city;
}

Tour *lookupTour(const char id[4]) {
  for (Tour *tour = tourTable[hash(id)]; tour; tour = tour->next)
    if (strcmp(id, tour->id) == 0)
      return tour;
  return NULL;
}

Tour *insertTour(const char id[4], const char *name,
                 const char startCityId[4]) {
  if (lookupTour(id)) {
    fprintf(stderr, "Semantic error: Tour ID %s already defined\n", id);
    exit(EXIT_FAILURE);
  }
  Tour *tour = (Tour *)malloc(sizeof(Tour));
  if (!tour)
    exit(EXIT_FAILURE);
  idDup(tour->id, id);
  tour->name = strdup(name);
  if (!lookupCity(startCityId)) {
    fprintf(stderr,
            "Semantic Error: Start city %s for tour %s has not been defined\n",
            startCityId, id);
    exit(EXIT_FAILURE);
  }
  for (int i = 0; i < 4; i++)
    tour->currentCityId[i] = startCityId[i];
  tour->distance = 0;
  tour->stopsCount = 1;

  PathNode *node = (PathNode *)malloc(sizeof(PathNode));
  if (!node)
    exit(EXIT_FAILURE);
  idDup(node->cityId, startCityId);
  node->next = NULL;
  tour->pathHead = tour->pathTail = node;

  unsigned int h = hash(id);
  tour->next = tourTable[h];
  tourTable[h] = tour;
  tour->listNext = NULL;
  if (!tourListHead)
    tourListHead = tour;
  else
    tourListTail->listNext = tour;
  tourListTail = tour;
  return tour;
}

void addStop(const char tourId[4], const char nextCityId[4]) {
  Tour *tour = lookupTour(tourId);
  if (!tour) {
    fprintf(stderr, "Semantic error: Tour %s not defined.\n", tourId);
    exit(EXIT_FAILURE);
  }
  City *currentCity = lookupCity(tour->currentCityId);
  City *nextCity = lookupCity(nextCityId);
  if (!nextCity) {
    fprintf(stderr, "Semantic error: City %s not defined.\n", nextCityId);
    exit(EXIT_FAILURE);
  }
  if (!currentCity || !nextCity)
    return;
  double dist =
      hypot(currentCity->x - nextCity->x, currentCity->y - nextCity->y);
  tour->distance += dist;
  tour->stopsCount++;

  PathNode *node = (PathNode *)malloc(sizeof(PathNode));
  if (!node)
    exit(EXIT_FAILURE);
  idDup(node->cityId, nextCity->id);
  node->next = NULL;

  tour->pathTail->next = node;
  tour->pathTail = node;

  idDup(tour->currentCityId, nextCityId);
}

void printResults() {
  for (Tour *tour = tourListHead; tour; tour = tour->listNext) {
    double totalCost = 250 * tour->stopsCount + 10 * tour->distance;
    printf("%s: ", tour->name);
    PathNode *curr = tour->pathHead;
    while (curr) {
      City *city = lookupCity(curr->cityId);
      if (!city)
        return;
      printf("%s", city->name);
      if (curr->next)
        printf(" -> ");
      curr = curr->next;
    }
    printf(" %.2lf€\n", totalCost);
  }
}