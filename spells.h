#ifndef SPELLS_H
#define SPELLS_H

#include <ncurses.h>
#include <stdint.h>

#include <string>
#include <vector>

//#include "monster.h"

class spell_t {
 public:
  std::string name = "";
  int32_t damage;
  int cost;
};

class spellbook_t {
 public:
  std::string name = "";
  bool unlocked = false;
  spell_t *spells[3];
};

int cast_spell(WINDOW *win, int x, int y, char dungeon[21][80],
               uint8_t hardness[21][80], obj_t *objects[21][80],
               char character[21][80], int num);
void init_spellbooks();
int show_spells(WINDOW *win, char dungeon[21][80], uint8_t hardness[21][80],
                obj_t *objects[21][80], char character[21][80], int num);

#endif
