#include <bits/stdc++.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "monster.h"
#include "oparser.h"
#include "pathfind.h"

int place_object(uint8_t hardness[21][80], obj_t *objects[21][80], uint8_t x,
                 uint8_t y) {
  if (hardness[y][x] == 0 && objects[y][x] == NULL) {
    return 1;
  } else {
    return 0;
  }
}

void generate_objects(uint8_t hardness[21][80], obj_t *objects[21][80]) {
  int num_objects = (rand() % 10) + 10;

  int i = 0;

  for (i = 0; i < num_objects; i++) {
    bool able = false;
    int obj_rand;

    while (!able) {
      obj_rand = rand() % obj_vec.size();

      if (obj_vec[obj_rand].artifact == 1 && obj_vec[obj_rand].on_ground == 1) {
        continue;
      }

      int rare_rand = rand() % 100;

      if (rare_rand >= obj_vec[obj_rand].rarity) {
        continue;
      }

      able = true;
    }

    obj_t *tmp = obj_vec[obj_rand].generate_obj();

    obj_vec[obj_rand].on_ground = 1;

    int placed = 0;
    while (placed != 1) {
      uint8_t x = (rand() % 76) + 2;
      uint8_t y = (rand() % 17) + 2;

      if (place_object(hardness, objects, x, y) == 1) {
        tmp->x = x;
        tmp->y = y;
        objects[y][x] = new obj_t;
        objects[y][x] = tmp;
        placed = 1;
      }
    }
  }

  if (pc.trap->unlocked == false) {
    obj_t *book1 = new obj_t;
    book1->name = "Trap Spellbook";
    book1->symbol = '$';
    book1->speed = 0;
    book1->hit = 0;
    book1->damage = 0;
    book1->dodge = 0;
    book1->defense = 0;
    book1->weight = 0;
    book1->attribute = 0;
    book1->value = 0;
    book1->rarity = 0;
    book1->artifact = 0;

    std::vector<std::string> color1;
    color1.push_back("COLOR_GREEN");
    book1->color = color1;

    std::vector<std::string> description;
    description.push_back("A very ancient and magical spellbook");
    book1->description = description;

    std::vector<std::string> type;
    type.push_back("SPELLBOOK");
    book1->type = type;

    i = 0;

    while (i != 1) {
      uint8_t x = (rand() % 76) + 2;
      uint8_t y = (rand() % 17) + 2;

      if (place_object(hardness, objects, x, y) == 1) {
        book1->x = x;
        book1->y = y;
        objects[y][x] = new obj_t;
        objects[y][x] = book1;
        i = 1;
      }
    }
  }

  if (pc.terrain->unlocked == false) {
    obj_t *book2 = new obj_t;
    book2->name = "Terrain Spellbook";
    book2->symbol = '$';
    book2->speed = 0;
    book2->hit = 0;
    book2->damage = 0;
    book2->dodge = 0;
    book2->defense = 0;
    book2->weight = 0;
    book2->attribute = 0;
    book2->value = 0;
    book2->rarity = 0;
    book2->artifact = 0;

    std::vector<std::string> color2;
    color2.push_back("COLOR_YELLOW");
    book2->color = color2;

    std::vector<std::string> description;
    description.push_back("A very ancient and magical spellbook");
    book2->description = description;

    std::vector<std::string> type;
    type.push_back("SPELLBOOK");
    book2->type = type;

    i = 0;

    while (i != 1 && pc.terrain->unlocked == false) {
      uint8_t x = (rand() % 76) + 2;
      uint8_t y = (rand() % 17) + 2;

      if (place_object(hardness, objects, x, y) == 1) {
        book2->x = x;
        book2->y = y;
        objects[y][x] = new obj_t;
        objects[y][x] = book2;
        i = 1;
      }
    }
  }
}
