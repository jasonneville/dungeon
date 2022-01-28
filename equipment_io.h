#ifndef EQUIPMENT_IO_H
#define EQUIPMENT_IO_H


#include <vector>
#include <string>
#include <ncurses.h>
#include "monster.h"
#include "objects.h"




int discard_item(WINDOW* win, player_t *player, obj_t *objects[21][80], int action);
int inspect_item(WINDOW* win, player_t *player);
int show_inventory(WINDOW* win, player_t *player, obj_t *objects[21][80]);
int show_equipment(WINDOW* win, player_t *player, obj_t *objects[21][80]);
int wear_equipment(WINDOW* win, player_t *player, obj_t *objects[21][80]);
int take_off_item(WINDOW* win, player_t *player, obj_t *objects[21][80]);
int look_at_monster(WINDOW* win, player_t *player, int x, int y, int nummon);
int find_npc(int x, int y, int nummon);

int get_color_int(std::string color);
#endif
