#ifndef SYMB_TAB_H
#define SYMB_TAB_H

#define TABLESIZE 101
typedef struct City {
  char id[4]; // 3 letters + '\0'
  char *name;
  double x, y;       // coordinates
  struct City *next; // collision separate chaining
} City;

typedef struct PathNode {
  char cityId[4];
  struct PathNode *next;
} PathNode;

typedef struct Tour {
  char id[4];
  char *name;
  char currentCityId[4];
  double distance;
  int stopsCount;
  PathNode *pathHead;
  PathNode *pathTail;
  struct Tour *next;     // separate chaining
  struct Tour *listNext; // for iterating during the output
} Tour;

unsigned int hash(const char *s);
City *insertCity(const char id[static 4], const char *name, double x, double y);
City *lookupCity(const char id[static 4]);
Tour *insertTour(const char id[static 4], const char *name,
                 const char startCityId[static 4]);
Tour *lookupTour(const char id[static 4]);
void addStop(const char tourId[static 4], const char cityId[static 4]);
void printResults();
#endif