#ifndef OBJECTS_H
#define OBJECTS_H

#include <string>
#include <vector>

#include "dice.h"
#include "monster.h"

class obj_t {
 public:
  uint8_t x;
  uint8_t y;
  std::string name = "";
  std::vector<std::string> color;
  std::vector<std::string> type;
  std::vector<std::string> description;
  char symbol;
  int32_t speed;
  int32_t hit;
  int32_t damage;
  int32_t dodge;
  int32_t defense;
  int32_t weight;
  int32_t attribute;
  int32_t value;
  int rarity = -1;
  int artifact = -1;
};

void generate_objects(uint8_t hardness[21][80], obj_t *objects[21][80]);

#endif
