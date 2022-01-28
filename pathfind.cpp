#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#include "heap.h"

class distance_point_t {
 public:
  heap_node_t *hn;
  uint8_t pos[2];
  int32_t cost;
};

static int32_t distance_point_cmp(const void *key, const void *with) {
  return ((distance_point_t *)key)->cost - ((distance_point_t *)with)->cost;
}

typedef enum dim { dim_x, dim_y, num_dims } dim_t;

void dijkstra_tunneling(char dungeon[21][80], uint8_t hard[21][80],
                        uint8_t pc_x, uint8_t pc_y, uint8_t tunneling[21][80])

{
  static distance_point_t path[21][80], *p;
  static uint32_t initialized = 0;
  heap_t h;
  uint32_t x, y;

  if (!initialized) {
    for (y = 0; y < 21; y++) {
      for (x = 0; x < 80; x++) {
        path[y][x].pos[dim_y] = y;
        path[y][x].pos[dim_x] = x;
      }
    }
    initialized = 1;
  }

  for (y = 0; y < 21; y++) {
    for (x = 0; x < 80; x++) {
      path[y][x].cost = INT_MAX;
    }
  }

  path[pc_y][pc_x].cost = 0;

  heap_init(&h, distance_point_cmp, NULL);

  for (y = 0; y < 21; y++) {
    for (x = 0; x < 80; x++) {
      if (hard[y][x] != 255) {
        path[y][x].hn = heap_insert(&h, &path[y][x]);
      } else {
        path[y][x].hn = NULL;
      }
    }
  }

  while ((p = (distance_point_t *)heap_remove_min(&h))) {
    p->hn = NULL;

    if ((path[p->pos[dim_y] - 1][p->pos[dim_x]].hn) &&
        (path[p->pos[dim_y] - 1][p->pos[dim_x]].cost >
         p->cost + hard[p->pos[dim_y]][p->pos[dim_x]])) {
      path[p->pos[dim_y] - 1][p->pos[dim_x]].cost =
          p->cost + (1 + hard[p->pos[dim_y]][p->pos[dim_x]] / 85);
      heap_decrease_key_no_replace(&h,
                                   path[p->pos[dim_y] - 1][p->pos[dim_x]].hn);
    }
    if ((path[p->pos[dim_y]][p->pos[dim_x] - 1].hn) &&
        (path[p->pos[dim_y]][p->pos[dim_x] - 1].cost >
         p->cost + hard[p->pos[dim_y]][p->pos[dim_x]])) {
      path[p->pos[dim_y]][p->pos[dim_x] - 1].cost =
          p->cost + (1 + hard[p->pos[dim_y]][p->pos[dim_x]] / 85);
      heap_decrease_key_no_replace(&h,
                                   path[p->pos[dim_y]][p->pos[dim_x] - 1].hn);
    }
    if ((path[p->pos[dim_y]][p->pos[dim_x] + 1].hn) &&
        (path[p->pos[dim_y]][p->pos[dim_x] + 1].cost >
         p->cost + hard[p->pos[dim_y]][p->pos[dim_x]])) {
      path[p->pos[dim_y]][p->pos[dim_x] + 1].cost =
          p->cost + (1 + hard[p->pos[dim_y]][p->pos[dim_x]] / 85);

      heap_decrease_key_no_replace(&h,
                                   path[p->pos[dim_y]][p->pos[dim_x] + 1].hn);
    }

    if ((path[p->pos[dim_y] + 1][p->pos[dim_x]].hn) &&
        (path[p->pos[dim_y] + 1][p->pos[dim_x]].cost >
         p->cost + hard[p->pos[dim_y]][p->pos[dim_x]])) {
      path[p->pos[dim_y] + 1][p->pos[dim_x]].cost =
          p->cost + (1 + hard[p->pos[dim_y]][p->pos[dim_x]] / 85);

      heap_decrease_key_no_replace(&h,
                                   path[p->pos[dim_y] + 1][p->pos[dim_x]].hn);
    }

    if ((path[p->pos[dim_y] - 1][p->pos[dim_x] - 1].hn) &&
        (path[p->pos[dim_y] - 1][p->pos[dim_x] - 1].cost >
         p->cost + hard[p->pos[dim_y]][p->pos[dim_x]])) {
      path[p->pos[dim_y] - 1][p->pos[dim_x] - 1].cost =
          p->cost + (1 + hard[p->pos[dim_y]][p->pos[dim_x]] / 85);

      heap_decrease_key_no_replace(
          &h, path[p->pos[dim_y] - 1][p->pos[dim_x] - 1].hn);
    }
    if ((path[p->pos[dim_y] + 1][p->pos[dim_x] - 1].hn) &&
        (path[p->pos[dim_y] + 1][p->pos[dim_x] - 1].cost >
         p->cost + hard[p->pos[dim_y]][p->pos[dim_x]])) {
      path[p->pos[dim_y] + 1][p->pos[dim_x] - 1].cost =
          p->cost + (1 + hard[p->pos[dim_y]][p->pos[dim_x]] / 85);

      heap_decrease_key_no_replace(
          &h, path[p->pos[dim_y] + 1][p->pos[dim_x] - 1].hn);
    }
    if ((path[p->pos[dim_y] - 1][p->pos[dim_x] + 1].hn) &&
        (path[p->pos[dim_y] - 1][p->pos[dim_x] + 1].cost >
         p->cost + hard[p->pos[dim_y]][p->pos[dim_x]])) {
      path[p->pos[dim_y] - 1][p->pos[dim_x] + 1].cost =
          p->cost + (1 + hard[p->pos[dim_y]][p->pos[dim_x]] / 85);

      heap_decrease_key_no_replace(
          &h, path[p->pos[dim_y] - 1][p->pos[dim_x] + 1].hn);
    }

    if ((path[p->pos[dim_y] + 1][p->pos[dim_x] + 1].hn) &&
        (path[p->pos[dim_y] + 1][p->pos[dim_x] + 1].cost >
         p->cost + hard[p->pos[dim_y]][p->pos[dim_x]])) {
      path[p->pos[dim_y] + 1][p->pos[dim_x] + 1].cost =
          p->cost + (1 + hard[p->pos[dim_y]][p->pos[dim_x]] / 85);

      heap_decrease_key_no_replace(
          &h, path[p->pos[dim_y] + 1][p->pos[dim_x] + 1].hn);
    }
  }

  for (y = 0; y < 21; y++) {
    for (x = 0; x < 80; x++) {
      tunneling[y][x] = path[y][x].cost;
    }
  }
}

void dijkstra_non_tunneling(char dungeon[21][80], uint8_t hard[21][80],
                            uint8_t pc_x, uint8_t pc_y,
                            uint8_t non_tunneling[21][80]) {
  static distance_point_t path[21][80], *p;
  static uint32_t initialized = 0;
  heap_t h;
  uint32_t x, y;

  if (!initialized) {
    for (y = 0; y < 21; y++) {
      for (x = 0; x < 80; x++) {
        path[y][x].pos[dim_y] = y;
        path[y][x].pos[dim_x] = x;
      }
    }
    initialized = 1;
  }

  for (y = 0; y < 21; y++) {
    for (x = 0; x < 80; x++) {
      path[y][x].cost = INT_MAX;
    }
  }

  path[pc_y][pc_x].cost = 0;

  heap_init(&h, distance_point_cmp, NULL);

  for (y = 0; y < 21; y++) {
    for (x = 0; x < 80; x++) {
      if (hard[y][x] == 0) {
        path[y][x].hn = heap_insert(&h, &path[y][x]);
      } else {
        path[y][x].hn = NULL;
      }
    }
  }

  while ((p = (distance_point_t *)heap_remove_min(&h))) {
    p->hn = NULL;

    if ((path[p->pos[dim_y] - 1][p->pos[dim_x]].hn) &&
        (path[p->pos[dim_y] - 1][p->pos[dim_x]].cost >
         p->cost + hard[p->pos[dim_y]][p->pos[dim_x]])) {
      path[p->pos[dim_y] - 1][p->pos[dim_x]].cost = p->cost + 1;
      heap_decrease_key_no_replace(&h,
                                   path[p->pos[dim_y] - 1][p->pos[dim_x]].hn);
    }
    if ((path[p->pos[dim_y]][p->pos[dim_x] - 1].hn) &&
        (path[p->pos[dim_y]][p->pos[dim_x] - 1].cost >
         p->cost + hard[p->pos[dim_y]][p->pos[dim_x]])) {
      path[p->pos[dim_y]][p->pos[dim_x] - 1].cost = p->cost + 1;
      heap_decrease_key_no_replace(&h,
                                   path[p->pos[dim_y]][p->pos[dim_x] - 1].hn);
    }
    if ((path[p->pos[dim_y]][p->pos[dim_x] + 1].hn) &&
        (path[p->pos[dim_y]][p->pos[dim_x] + 1].cost >
         p->cost + hard[p->pos[dim_y]][p->pos[dim_x]])) {
      path[p->pos[dim_y]][p->pos[dim_x] + 1].cost = p->cost + 1;

      heap_decrease_key_no_replace(&h,
                                   path[p->pos[dim_y]][p->pos[dim_x] + 1].hn);
    }

    if ((path[p->pos[dim_y] + 1][p->pos[dim_x]].hn) &&
        (path[p->pos[dim_y] + 1][p->pos[dim_x]].cost >
         p->cost + hard[p->pos[dim_y]][p->pos[dim_x]])) {
      path[p->pos[dim_y] + 1][p->pos[dim_x]].cost = p->cost + 1;

      heap_decrease_key_no_replace(&h,
                                   path[p->pos[dim_y] + 1][p->pos[dim_x]].hn);
    }

    if ((path[p->pos[dim_y] - 1][p->pos[dim_x] - 1].hn) &&
        (path[p->pos[dim_y] - 1][p->pos[dim_x] - 1].cost >
         p->cost + hard[p->pos[dim_y]][p->pos[dim_x]])) {
      path[p->pos[dim_y] - 1][p->pos[dim_x] - 1].cost = p->cost + 1;

      heap_decrease_key_no_replace(
          &h, path[p->pos[dim_y] - 1][p->pos[dim_x] - 1].hn);
    }
    if ((path[p->pos[dim_y] + 1][p->pos[dim_x] - 1].hn) &&
        (path[p->pos[dim_y] + 1][p->pos[dim_x] - 1].cost >
         p->cost + hard[p->pos[dim_y]][p->pos[dim_x]])) {
      path[p->pos[dim_y] + 1][p->pos[dim_x] - 1].cost = p->cost + 1;

      heap_decrease_key_no_replace(
          &h, path[p->pos[dim_y] + 1][p->pos[dim_x] - 1].hn);
    }
    if ((path[p->pos[dim_y] - 1][p->pos[dim_x] + 1].hn) &&
        (path[p->pos[dim_y] - 1][p->pos[dim_x] + 1].cost >
         p->cost + hard[p->pos[dim_y]][p->pos[dim_x]])) {
      path[p->pos[dim_y] - 1][p->pos[dim_x] + 1].cost = p->cost + 1;

      heap_decrease_key_no_replace(
          &h, path[p->pos[dim_y] - 1][p->pos[dim_x] + 1].hn);
    }

    if ((path[p->pos[dim_y] + 1][p->pos[dim_x] + 1].hn) &&
        (path[p->pos[dim_y] + 1][p->pos[dim_x] + 1].cost >
         p->cost + hard[p->pos[dim_y]][p->pos[dim_x]])) {
      path[p->pos[dim_y] + 1][p->pos[dim_x] + 1].cost = p->cost + 1;

      heap_decrease_key_no_replace(
          &h, path[p->pos[dim_y] + 1][p->pos[dim_x] + 1].hn);
    }
  }

  for (y = 0; y < 21; y++) {
    for (x = 0; x < 80; x++) {
      non_tunneling[y][x] = path[y][x].cost;
    }
  }
}
