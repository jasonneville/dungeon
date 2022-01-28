#include "spells.h"

#include <ncurses.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

#include "equipment_io.h"
#include "monster.h"

int num_mon = -1;

void print_spells(WINDOW *window) {
  int i;
  int j = 0;

  for (i = 3; i < 18; i++) {
    mvwprintw(window, i, 20, "                                            ");
  }

  spellbook_t *book;

  if (pc.book_num == 0) {
    book = pc.elemental;
  }

  else if (pc.book_num == 1) {
    book = pc.trap;
  } else {
    book = pc.terrain;
  }

  for (i = 0; i < 3; i++) {
    if (pc.spell == NULL || book->spells[i]->name != pc.spell->name) {
      mvwprintw(window, 5 + j, 25, "Spell %d: %s", i,
                book->spells[i]->name.c_str());

    }

    else {
      mvwprintw(window, 5 + j, 25, "Spell %d: %s (Selected)", i,
                book->spells[i]->name.c_str());
    }

    j++;
  }
  if (book->unlocked == false) {
    mvwprintw(window, 3, 30, "Spellbook: %s (Not unlocked)",
              book->name.c_str());
  } else {
    mvwprintw(window, 3, 30, "Spellbook: %s", book->name.c_str());
  }
}

int cast_terrain(int x, int y, spell_t *s, char dungeon[21][80],
                 uint8_t hardness[21][80]) {
  if (hardness[y][x] == 255) {
    return 0;
  }

  if (s->name == "Make Rock") {
    int rand_hardness = rand() % 254 + 1;

    if (hardness[y][x] == 0) {
      hardness[y][x] = rand_hardness;
    }

    dungeon[y][x] = ' ';
    return 3;
  }

  else if (s->name == "Make Corridor") {
    if (hardness[y][x] == 0) {
      return 3;
    }

    else {
      hardness[y][x] = 0;
      dungeon[y][x] = '#';
      return 3;
    }
  }

  else if (s->name == "Make Staircase") {
    if (dungeon[y][x] == '<' || dungeon[y][x] == '>') {
      return 3;
    } else {
      dungeon[y][x] = '<';
      hardness[y][x] = 0;
      return 3;
    }
  }

  return 0;
}

int cast_elemental(int x, int y, int num) {
  int index = find_npc(x, y, num);

  if (index == -1) {
    return 0;
  } else {
    spell_t *s = pc.spell;

    int dmg = s->damage;

    ch[index].hp = ch[index].hp - dmg;

    pc.mana = pc.mana - pc.spell->cost;

    if (ch[index].hp <= 0) {
      ch[index].alive = 0;
      return 2;
    }

    return 1;
  }
}

int cast_trap(int x, int y, obj_t *objects[21][80]) {
  obj_t *tmp = new obj_t;

  tmp->name = pc.spell->name;
  tmp->x = x;
  tmp->y = y;
  tmp->symbol = '!';
  tmp->speed = 0;
  tmp->hit = 0;
  tmp->damage = pc.spell->damage;
  tmp->dodge = 0;
  tmp->defense = 0;
  tmp->weight = 0;
  tmp->attribute = 0;
  tmp->value = 0;
  tmp->rarity = 0;
  tmp->artifact = 0;
  std::vector<std::string> type;
  type.push_back("TRAP");
  tmp->type = type;
  std::vector<std::string> description;
  description.push_back("A Magical Trap");
  tmp->description = description;
  std::vector<std::string> color;
  if (pc.spell->name == "Spike Trap") {
    color.push_back("COLOR_YELLOW");
  } else if (pc.spell->name == "Ice Trap") {
    color.push_back("COLOR_CYAN");
  } else {
    color.push_back("COLOR_RED");
  }

  tmp->color = color;

  pc.mana = pc.mana - pc.spell->cost;

  objects[y][x] = tmp;

  return 3;
}

void init_spellbooks() {
  pc.elemental = new spellbook_t;
  pc.trap = new spellbook_t;
  pc.terrain = new spellbook_t;

  spellbook_t *elemental = new spellbook_t;
  spellbook_t *trap = new spellbook_t;
  spellbook_t *terrain = new spellbook_t;
  pc.elemental = elemental;
  pc.trap = trap;
  pc.terrain = terrain;
  pc.elemental->name = "Elemental";
  pc.elemental->unlocked = true;
  pc.trap->name = "Traps";
  pc.trap->unlocked = false;
  pc.terrain->name = "Terrain";
  pc.terrain->unlocked = false;

  spell_t *air_strike = new spell_t;
  spell_t *water_strike = new spell_t;
  spell_t *fire_strike = new spell_t;

  air_strike->name = "Air Strike";
  air_strike->damage = 10;
  air_strike->cost = 5;
  water_strike->name = "Water Strike";
  water_strike->damage = 20;
  water_strike->cost = 7;
  fire_strike->name = "Fire Strike";
  fire_strike->damage = 30;
  fire_strike->cost = 10;

  elemental->spells[0] = air_strike;
  elemental->spells[1] = water_strike;
  elemental->spells[2] = fire_strike;

  spell_t *spike_trap = new spell_t;
  spell_t *ice_trap = new spell_t;
  spell_t *fire_trap = new spell_t;

  spike_trap->name = "Spike Trap";
  spike_trap->damage = 25;
  spike_trap->cost = 5;
  ice_trap->name = "Ice Trap";
  ice_trap->damage = 20;
  ice_trap->cost = 20;
  fire_trap->name = "Fire Trap";
  fire_trap->damage = 50;
  fire_trap->cost = 30;

  trap->spells[0] = spike_trap;
  trap->spells[1] = ice_trap;
  trap->spells[2] = fire_trap;

  spell_t *remove_terrain = new spell_t;
  spell_t *make_corridor = new spell_t;
  spell_t *create_staircase = new spell_t;

  remove_terrain->name = "Make Rock";
  remove_terrain->damage = 0;
  remove_terrain->cost = 10;
  make_corridor->name = "Make Corridor";
  make_corridor->damage = 0;
  make_corridor->cost = 10;
  create_staircase->name = "Make Staircase";
  create_staircase->damage = 0;
  create_staircase->cost = 15;

  terrain->spells[0] = remove_terrain;
  terrain->spells[1] = make_corridor;
  terrain->spells[2] = create_staircase;

  pc.spell = pc.elemental->spells[0];
}

void cast_terrain(int x, int y, char dungeon[21][80],
                  uint8_t hardness[21][80]) {
  if (pc.spell->name == "Make Rock") {
    int hardness_rand = rand() % 254 + 1;
    dungeon[y][x] = ' ';
    hardness[y][x] = hardness_rand;

  }

  else if (pc.spell->name == "Make Corridor") {
    dungeon[y][x] = '#';
    hardness[y][x] = 0;

  }

  else if (pc.spell->name == "Make Staircase") {
    dungeon[y][x] = '<';
    hardness[y][x] = 0;
  }
}

int cast_spell(WINDOW *win, int x, int y, char dungeon[21][80],
               uint8_t hardness[21][80], obj_t *objects[21][80],
               char character[21][80], int num) {
  int range = 2;

  if (pc.equipment[9] != NULL) {
    range = 4;
  }

  if (abs(pc.x - x) > range || abs(pc.y - y) > range) {
    return 0;
  }

  else {
    if ((pc.mana - pc.spell->cost) < 0) {
      return 0;
    }
    if (pc.book_num == 0) {
      int dead = cast_elemental(x, y, num);
      if (dead == 2) {
        character[y][x] = '\0';
      }
      return 3;
    }

    else if (pc.book_num == 2) {
      cast_terrain(x, y, dungeon, hardness);

      return 3;

    }

    else if (pc.book_num == 1) {
      cast_trap(x, y, objects);

      return 3;
    }
  }

  return 0;
}

int show_spells(WINDOW *win, char dungeon[21][80], uint8_t hardness[21][80],
                obj_t *objects[21][80], char character[21][80], int num) {
  num_mon = num;

  print_spells(win);

  int i = 0;

  while (i != 1) {
    int ch = getch();

    switch (ch) {
      case 27:
        return 3;
        break;

      case 'q':
        return -1;
        break;

      case '0':
        if (pc.book_num == 0) {
          pc.spell = pc.elemental->spells[0];
        } else if (pc.book_num == 1 && pc.trap->unlocked == true) {
          pc.spell = pc.trap->spells[0];
        } else if (pc.terrain->unlocked == true) {
          pc.spell = pc.terrain->spells[0];
        }
        return show_spells(win, dungeon, hardness, objects, character, num);
        break;
      case '1':
        if (pc.book_num == 0) {
          pc.spell = pc.elemental->spells[1];
        } else if (pc.book_num == 1 && pc.trap->unlocked == true) {
          pc.spell = pc.trap->spells[1];
        } else if (pc.terrain->unlocked == true) {
          pc.spell = pc.terrain->spells[1];
        }
        return show_spells(win, dungeon, hardness, objects, character, num);
        break;
      case '2':
        if (pc.book_num == 0) {
          pc.spell = pc.elemental->spells[2];
        } else if (pc.book_num == 1 && pc.trap->unlocked == true) {
          pc.spell = pc.trap->spells[2];
        } else if (pc.terrain->unlocked == true) {
          pc.spell = pc.terrain->spells[2];
        }
        return show_spells(win, dungeon, hardness, objects, character, num);
        break;

      case '?':
        if (pc.book_num < 2) {
          pc.book_num++;
          if (pc.book_num == 1) {
            if (pc.trap->unlocked == true) {
              pc.spell = pc.trap->spells[0];
            }
          } else {
            if (pc.terrain->unlocked == true) {
              pc.spell = pc.terrain->spells[0];
            }
          }

        } else {
          pc.book_num = 0;
          pc.spell = pc.elemental->spells[0];
        }
        return show_spells(win, dungeon, hardness, objects, character, num);

      default:
        break;
    }
  }
  return 0;
}
