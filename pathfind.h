#ifndef PATHFIND_H
#define PATHFIND_H

void dijkstra_tunneling(char dungeon[21][80], uint8_t hard[21][80],
                        uint8_t pc_x, uint8_t pc_y, uint8_t tunneling[21][80]);

void dijkstra_non_tunneling(char dungeon[21][80], uint8_t hard[21][80],
                            uint8_t pc_x, uint8_t pc_y,
                            uint8_t non_tunneling[21][80]);

struct distance_point;

int32_t distance_point_cmp(const void *key, const void *with);

#endif
