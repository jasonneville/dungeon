#ifndef MPARSER_H
#define MPARSER_H

#include <string>
#include <vector>

#include "dice.h"
#include "monster.h"

class desc {
 public:
  std::string name = "";
  std::vector<std::string> description;
  std::vector<std::string> color;
  std::vector<std::string> abilities;
  dice *speed = NULL;
  int rarity = -1;
  dice *hitpoints = NULL;
  char symbol = '\0';
  dice *damage = NULL;
  bool can_place = 1;

  npc_t *generate_pc() {
    npc_t *tmp = new npc_t;
    tmp->speed = speed->roll();
    tmp->hp = hitpoints->roll();
    tmp->max_hp = tmp->hp;
    tmp->hex = symbol;
    tmp->npc_name = name;
    tmp->perks = abilities;
    tmp->coloring = color;
    tmp->damage = damage;
    tmp->description = description;

    return tmp;
  }
};

bool parse_monsters();

extern std::vector<desc> vec;

void print_vector();

#endif
