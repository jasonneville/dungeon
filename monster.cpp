#include "monster.h"

#include <bits/stdc++.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <algorithm>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "combat.h"
#include "equipment_io.h"
#include "mparser.h"
#include "pathfind.h"

int num_monsters;

npc_t *ch;
player_t pc;

int32_t monster_cmp(const void *key, const void *with) {
  return ((character_t *)key)->turn - ((character_t *)with)->turn;
}

void killm(uint8_t y, uint8_t x) {
  if (pc.y == y && pc.x == x) {
    pc.alive = 0;
  }
  int i;
  for (i = 0; i < num_monsters; i++) {
    if (ch[i].x == x && ch[i].y == y) {
      ch[i].alive = 0;
    }
  }
}

int place_monster(char dungeon[21][80], char character[21][80], uint8_t x,
                  uint8_t y) {
  if ((dungeon[y][x] == '.' || dungeon[y][x] == '#') &&
      character[y][x] == '\0') {
    return 1;
  } else {
    return 0;
  }
}

void generate_monsters(char dungeon[21][80], char character[21][80], int nummon,
                       uint8_t pc_x, uint8_t pc_y, character_t *p) {
  num_monsters = nummon;

  ch = (npc_t *)calloc(1, sizeof(npc_t) * nummon);

  pc.is_pc = 1;
  pc.x = pc_x;
  pc.y = pc_y;
  pc.speed = 10;
  pc.hex = '@';
  pc.alive = 1;
  pc.max_hp = 100;
  pc.hp = 100;
  pc.turn = 0;
  pc.damage = new dice;
  pc.damage->set_base(0);
  pc.damage->set_number(1);
  pc.damage->set_sides(4);

  int i, j;

  for (i = 0; i < 21; i++) {
    for (j = 0; j < 80; j++) {
      pc.remembered[i][j] = '\0';
    }
  }

  for (i = 0; i < nummon; i++) {
    int erratic = rand() % 2;
    int tunneling = rand() % 2;
    int telepathy = rand() % 2;
    int intelligence = rand() % 2;
    bool able = false;
    int vec_rand;

    while (able == false) {
      vec_rand = rand() % vec.size();

      int l = 0;
      if (vec[vec_rand].can_place == false) {
        continue;
      } else {
        for (l = 0; l < vec[vec_rand].abilities.size(); l++) {
          if (vec[vec_rand].abilities[l] == "UNIQ") {
            vec[vec_rand].can_place = false;
            break;
          }
        }

        int rare_rand = rand() % 100;
        if (rare_rand < vec[vec_rand].rarity) {
          able = true;
        }
      }
    }

    npc_t *monster = vec[vec_rand].generate_pc();

    monster->alive = 1;
    monster->is_pc = 0;
    monster->turn = 0;
    monster->last_x = 0;
    monster->last_y = 0;
    int l = 0;

    int placed = 0;
    while (placed != 1) {
      uint8_t x = (rand() % 76) + 2;
      uint8_t y = (rand() % 17) + 2;

      if (place_monster(dungeon, character, x, y) == 1) {
        monster->x = x;
        monster->y = y;
        character[y][x] = monster->hex;
        placed = 1;
      }
    }
    ch[i] = *monster;
  }
}

bool can_displace(npc_t *p, int x, int y, char character[21][80],
                  uint8_t hardness[21][80]) {
  if (character[y - 1][x] == '\0' && hardness[y - 1][x] == 0) {
    return true;
  } else if (character[y - 1][x + 1] == '\0' && hardness[y - 1][x + 1] == 0) {
    return true;
  } else if (character[y][x + 1] == '\0' && hardness[y][x + 1] == 0) {
    return true;
  } else if (character[y + 1][x + 1] == '\0' && hardness[y + 1][x + 1] == 0) {
    return true;
  } else if (character[y + 1][x] == '\0' && hardness[y + 1][x] == 0) {
    return true;
  } else if (character[y + 1][x - 1] == '\0' && hardness[y + 1][x - 1] == 0) {
    return true;
  } else if (character[y][x - 1] == '\0' && hardness[y][x - 1] == 0) {
    return true;
  } else if (character[y - 1][x - 1] == '\0' && hardness[y - 1][x - 1] == 0) {
    return true;
  } else {
    return false;
  }
}

void displace(npc_t *p, int x, int y, char character[21][80],
              uint8_t hardness[21][80]) {
  int r_x, r_y;
  bool displaced = false;

  while (displaced == false) {
    r_x = (rand() % 3) - 1;
    r_y = (rand() % 3) - 1;

    if (character[y + r_y][x + r_x] != '\0' ||
        hardness[y + r_y][x + r_x] != 0) {
      continue;
    }

    else {
      character[y + r_y][x + r_x] = p->hex;
      character[p->y][p->x] = '\0';
      p->y = y + r_y;
      p->x = x + r_x;
      displaced = true;
    }
  }
}

void move_erratic_non_tunneling(npc_t *p, char character[21][80],
                                uint8_t hardness[21][80]) {
  int random_y;
  int random_x;
  int randomized = 0;

  while (randomized != 1) {
    random_y = (rand() % 3) - 1;
    random_x = (rand() % 3) - 1;

    if (random_x != 0 || random_y != 0) {
      randomized = 1;
    }
  }

  uint8_t x = p->x;
  uint8_t y = p->y;

  if (hardness[y + random_y][x + random_x] == 0) {
    if (character[y + random_y][x + random_x] != '\0') {
      if (can_attack(p, x + random_x, y + random_y, num_monsters)) {
        int dead = attack(p, x + random_x, y + random_y, num_monsters);
        if (dead == 2) {
          character[y + random_y][x + random_x] = '\0';
          //	character[y][x] = '\0';
        }
      } else {
        if (can_displace(p, x + random_x, y + random_y, character, hardness)) {
          displace(p, x + random_x, y + random_y, character, hardness);
        }

        else {
          int monster = find_npc(x + random_x, y + random_y, num_monsters);
          std::swap(p->x, ch[monster].x);
          std::swap(p->y, ch[monster].y);
          character[y][x] = ch[monster].hex;
          character[y + random_y][x + random_x] = p->hex;
        }
      }

    } else {
      character[y + random_y][x + random_x] = p->hex;
      character[y][x] = '\0';
      p->y = p->y + random_y;
      p->x = p->x + random_x;
    }
  }
}

void move_erratic_tunneling(npc_t *p, char dungeon[21][80],
                            char character[21][80], uint8_t hardness[21][80]) {
  int random_y;
  int random_x;
  int randomized = 0;

  while (randomized != 1) {
    random_y = (rand() % 3) - 1;
    random_x = (rand() % 3) - 1;

    if (random_x != 0 || random_y != 0) {
      randomized = 1;
    }
  }

  uint8_t x = p->x;
  uint8_t y = p->y;

  if (hardness[y + random_y][x + random_x] != 255) {
    if (hardness[y + random_y][x + random_x] > 85) {
      hardness[y + random_y][x + random_x] =
          hardness[y + random_y][x + random_x] - 85;
    }

    else {
      //	character[y][x] = '\0';
      if (character[y + random_y][x + random_x] != '\0') {
        if (can_attack(p, x + random_x, y + random_y, num_monsters)) {
          int dead = attack(p, x + random_x, y + random_y, num_monsters);
          if (dead == 2) {
            character[y + random_y][x + random_x] = '\0';
            //	character[y][x] = '\0';
          }
        } else {
          if (can_displace(p, x + random_x, y + random_y, character,
                           hardness)) {
            displace(p, x + random_x, y + random_y, character, hardness);
          }

          else {
            int monster = find_npc(x + random_x, y + random_y, num_monsters);
            std::swap(p->x, ch[monster].x);
            std::swap(p->y, ch[monster].y);
            character[y][x] = ch[monster].hex;
            character[y + random_y][x + random_x] = p->hex;
          }
        }

      } else {
        character[y][x] = '\0';
        character[y + random_y][x + random_x] = p->hex;
        if (hardness[y + random_y][x + random_x] != 0) {
          hardness[y + random_y][x + random_x] = 0;
          dungeon[y + random_y][x + random_x] = '#';
        }

        p->y = p->y + random_y;
        p->x = p->x + random_x;
      }
    }
  }
}

void move_non_tunneling(npc_t *p, char dungeon[21][80], char character[21][80],
                        uint8_t hardness[21][80], uint8_t p_x, uint8_t p_y) {
  uint8_t non_tunnel[21][80];

  dijkstra_non_tunneling(dungeon, hardness, p_x, p_y, non_tunnel);

  uint8_t x = p->x;
  uint8_t y = p->y;
  int min = 1000;
  uint8_t min_x;
  uint8_t min_y;

  if (hardness[y - 1][x] == 0) {
    if (non_tunnel[y - 1][x] < min) {
      min = non_tunnel[y - 1][x];
      min_x = x;
      min_y = y - 1;
    }
  }
  if (hardness[y - 1][x + 1] == 0) {
    if (non_tunnel[y - 1][x + 1] < min) {
      min = non_tunnel[y - 1][x + 1];
      min_x = x + 1;
      min_y = y - 1;
    }
  }

  if (hardness[y][x + 1] == 0) {
    if (non_tunnel[y][x + 1] < min) {
      min = non_tunnel[y][x + 1];
      min_x = x + 1;
      min_y = y;
    }
  }
  if (hardness[y + 1][x + 1] == 0) {
    if (non_tunnel[y + 1][x + 1] < min) {
      min = non_tunnel[y + 1][x + 1];
      min_x = x + 1;
      min_y = y + 1;
    }
  }
  if (hardness[y + 1][x] == 0) {
    if (non_tunnel[y + 1][x] < min) {
      min = non_tunnel[y + 1][x];
      min_x = x;
      min_y = y + 1;
    }
  }
  if (hardness[y + 1][x - 1] == 0) {
    if (non_tunnel[y + 1][x - 1] < min) {
      min = non_tunnel[y + 1][x - 1];
      min_x = x - 1;
      min_y = y + 1;
    }
  }
  if (hardness[y][x - 1] == 0) {
    if (non_tunnel[y][x - 1] < min) {
      min = non_tunnel[y][x - 1];
      min_x = x - 1;
      min_y = y;
    }
  }
  if (hardness[y - 1][x - 1] == 0) {
    if (non_tunnel[y - 1][x - 1] < min) {
      min = non_tunnel[y - 1][x - 1];
      min_x = x - 1;
      min_y = y - 1;
    }
  }

  //	character[y][x] = '\0';

  if (character[min_y][min_x] != '\0') {
    if (can_attack(p, min_x, min_y, num_monsters)) {
      int dead = attack(p, min_x, min_y, num_monsters);
      if (dead == 2) {
        character[min_y][min_x] = '\0';
        //	character[y][x] = '\0';
      }
    } else {
      int monster = find_npc(min_x, min_y, num_monsters);
      if (can_displace(p, min_x, min_y, character, hardness)) {
        displace(p, min_x, min_y, character, hardness);
      } else {
        std::swap(p->x, ch[monster].x);
        std::swap(p->y, ch[monster].y);
        character[y][x] = ch[monster].hex;
        character[min_y][min_x] = p->hex;
      }
    }

  }

  else {
    character[y][x] = '\0';
    character[min_y][min_x] = p->hex;
    p->x = min_x;
    p->y = min_y;
  }
}

void move_tunneling(npc_t *p, char dungeon[21][80], char character[21][80],
                    uint8_t hardness[21][80], uint8_t p_x, uint8_t p_y) {
  uint8_t tunnel[21][80];

  dijkstra_tunneling(dungeon, hardness, p_x, p_y, tunnel);

  uint8_t x = p->x;
  uint8_t y = p->y;
  int min = 1000;
  uint8_t min_x;
  uint8_t min_y;

  if (tunnel[y - 1][x] < min) {
    min = tunnel[y - 1][x];
    min_x = x;
    min_y = y - 1;
  }
  if (tunnel[y - 1][x + 1] < min) {
    min = tunnel[y - 1][x + 1];
    min_x = x + 1;
    min_y = y - 1;
  }
  if (tunnel[y][x + 1] < min) {
    min = tunnel[y][x + 1];
    min_x = x + 1;
    min_y = y;
  }
  if (tunnel[y + 1][x + 1] < min) {
    min = tunnel[y + 1][x + 1];
    min_x = x + 1;
    min_y = y + 1;
  }
  if (tunnel[y + 1][x] < min) {
    min = tunnel[y + 1][x];
    min_x = x;
    min_y = y + 1;
  }
  if (tunnel[y + 1][x - 1] < min) {
    min = tunnel[y + 1][x - 1];
    min_x = x - 1;
    min_y = y + 1;
  }
  if (tunnel[y][x - 1] < min) {
    min = tunnel[y][x - 1];
    min_x = x - 1;
    min_y = y;
  }
  if (tunnel[y - 1][x - 1] < min) {
    min = tunnel[y - 1][x - 1];
    min_x = x - 1;
    min_y = y - 1;
  }

  if (hardness[min_y][min_x] > 85) {
    hardness[min_y][min_x] = hardness[min_y][min_x] - 85;
  }

  else {
    //	character[y][x] = '\0';
    if (character[min_y][min_x] != '\0') {
      if (can_attack(p, min_x, min_y, num_monsters)) {
        int dead = attack(p, min_x, min_y, num_monsters);
        if (dead == 2) {
          character[min_y][min_x] = '\0';
        }
      } else {
        if (can_displace(p, min_x, min_y, character, hardness)) {
          displace(p, min_x, min_y, character, hardness);
        }

        else {
          int monster = find_npc(min_x, min_y, num_monsters);
          std::swap(p->x, ch[monster].x);
          std::swap(p->y, ch[monster].y);
          character[y][x] = ch[monster].hex;
          character[min_y][min_x] = p->hex;
        }
      }

    }

    else {
      character[y][x] = '\0';
      character[min_y][min_x] = p->hex;
      if (hardness[min_y][min_x] != 0) {
        hardness[min_y][min_x] = 0;
        dungeon[min_y][min_x] = '#';
      }

      p->y = min_y;
      p->x = min_x;
    }
  }
}

void move_character(npc_t *p, char dungeon[21][80], char character[21][80],
                    uint8_t hardness[21][80], obj_t *objects[21][80]) {
  int moved = 0;
  int random = rand() % 2;

  int mvmt;

  std::string erratic = "ERRATIC";
  std::string tunneling = "TUNNEL";
  std::string telepathic = "TELE";

  std::vector<std::string> abil = p->perks;

  if (std::find(abil.begin(), abil.end(), erratic) != abil.end() &&
      std::find(abil.begin(), abil.end(), tunneling) != abil.end()) {
    if (random == 1) {
      move_erratic_tunneling(p, dungeon, character, hardness);
      moved = 1;

    }

    else if (std::find(abil.begin(), abil.end(), telepathic) != abil.end()) {
      move_tunneling(p, dungeon, character, hardness, pc.x, pc.y);
      moved = 1;
    }

  }

  else if (std::find(abil.begin(), abil.end(), erratic) != abil.end() &&
           std::find(abil.begin(), abil.end(), tunneling) == abil.end()) {
    if (random == 1) {
      move_erratic_non_tunneling(p, character, hardness);
      moved = 1;

    }

    else if (std::find(abil.begin(), abil.end(), telepathic) != abil.end()) {
      move_non_tunneling(p, dungeon, character, hardness, pc.x, pc.y);
      moved = 1;
    }

  }

  else if (std::find(abil.begin(), abil.end(), telepathic) != abil.end()) {
    if (std::find(abil.begin(), abil.end(), tunneling) != abil.end()) {
      move_tunneling(p, dungeon, character, hardness, pc.x, pc.y);
      mvmt = 2;
      moved = 1;
    } else {
      move_non_tunneling(p, dungeon, character, hardness, pc.x, pc.y);
      moved = 1;
    }
  }

  if (moved == 1) {
    if (objects[p->y][p->x] != NULL && objects[p->y][p->x]->symbol == '!') {
      p->hp = p->hp - objects[p->y][p->x]->damage;

      if (p->hp <= 0) {
        p->alive = 0;
        character[p->y][p->x] = '\0';
      }

      objects[p->y][p->x] = NULL;
    }
  }
}

int move_pc(int c, uint8_t hardness[21][80], char character[21][80],
            character_t *p) {
  uint8_t x = p->x;
  uint8_t y = p->y;

  int in = c;

  switch (in) {
    case 55:
      if (hardness[y - 1][x - 1] == 0) {
        if (character[y - 1][x - 1] != '\0') {
          if (can_attack(p, x - 1, y - 1, num_monsters)) {
            int boss = attack(p, x - 1, y - 1, num_monsters);
            int index = find_npc(x - 1, y - 1, num_monsters);
            if (ch[index].alive == 0) {
              character[y - 1][x - 1] = '\0';
              if (boss == 1) {
                return 2;
              }
            }
          }

        } else {
          character[y - 1][x - 1] = p->hex;
          character[y][x] = '\0';
          p->x = x - 1;
          p->y = y - 1;
        }
        return 1;
      } else {
        return 0;
      }
      break;
    case 56:
      if (hardness[y - 1][x] == 0) {
        if (character[y - 1][x] != '\0') {
          if (can_attack(p, x, y - 1, num_monsters)) {
            int boss = attack(p, x, y - 1, num_monsters);
            int index = find_npc(x, y - 1, num_monsters);
            if (ch[index].alive == 0) {
              character[y - 1][x] = '\0';
              if (boss == 1) {
                return 2;
              }
            }
          }

        } else {
          character[y - 1][x] = p->hex;
          character[y][x] = '\0';
          p->y = y - 1;
        }
        return 1;
      }

      else {
        return 0;
      }
      break;

    case 57:
      if (hardness[y - 1][x + 1] == 0) {
        if (character[y - 1][x + 1] != '\0') {
          if (can_attack(p, x + 1, y - 1, num_monsters)) {
            int boss = attack(p, x + 1, y - 1, num_monsters);
            int index = find_npc(x + 1, y - 1, num_monsters);
            if (ch[index].alive == 0) {
              character[y - 1][x + 1] = '\0';
              if (boss == 1) {
                return 2;
              }
            }
          }

        } else {
          character[y - 1][x + 1] = p->hex;
          character[y][x] = '\0';
          p->x = x + 1;
          p->y = y - 1;
        }
        return 1;
      } else {
        return 0;
      }

      break;
    case 54:
      if (hardness[y][x + 1] == 0) {
        if (character[y][x + 1] != '\0') {
          if (can_attack(p, x + 1, y, num_monsters)) {
            int boss = attack(p, x + 1, y, num_monsters);
            int index = find_npc(x + 1, y, num_monsters);
            if (ch[index].alive == 0) {
              character[y][x + 1] = '\0';
              if (boss == 1) {
                return 2;
              }
            }
          }
        } else {
          character[y][x + 1] = p->hex;
          character[y][x] = '\0';
          p->x = x + 1;
        }
        return 1;
      } else {
        return 0;
      }
      break;
    case 51:
      if (hardness[y + 1][x + 1] == 0) {
        if (character[y + 1][x + 1] != '\0') {
          if (can_attack(p, x + 1, y + 1, num_monsters)) {
            int boss = attack(p, x + 1, y + 1, num_monsters);
            int index = find_npc(x + 1, y + 1, num_monsters);
            if (ch[index].alive == 0) {
              character[y + 1][x + 1] = '\0';
              if (boss == 1) {
                return 2;
              }
            }
          }
        } else {
          character[y + 1][x + 1] = p->hex;
          character[y][x] = '\0';
          p->x = x + 1;
          p->y = y + 1;
        }
        return 1;
      } else {
        return 0;
      }
      break;
    case 50:
      if (hardness[y + 1][x] == 0) {
        if (character[y + 1][x] != '\0') {
          if (can_attack(p, x, y + 1, num_monsters)) {
            int boss = attack(p, x, y + 1, num_monsters);
            int index = find_npc(x, y + 1, num_monsters);
            if (ch[index].alive == 0) {
              character[y + 1][x] = '\0';
              if (boss == 1) {
                return 2;
              }
            }
          }
        } else {
          character[y + 1][x] = p->hex;
          character[y][x] = '\0';
          p->y = y + 1;
        }
        return 1;
      } else {
        return 0;
      }
      break;
    case 49:
      if (hardness[y + 1][x - 1] == 0) {
        if (character[y + 1][x - 1] != '\0') {
          if (can_attack(p, x - 1, y + 1, num_monsters)) {
            int boss = attack(p, x - 1, y + 1, num_monsters);
            int index = find_npc(x - 1, y + 1, num_monsters);
            if (ch[index].alive == 0) {
              character[y + 1][x - 1] = '\0';
              if (boss == 1) {
                return 2;
              }
            }
          }
        } else {
          character[y + 1][x - 1] = p->hex;
          character[y][x] = '\0';
          p->x = x - 1;
          p->y = y + 1;
        }
        return 1;
      } else {
        return 0;
      }
      break;
    case 52:
      if (hardness[y][x - 1] == 0) {
        if (character[y][x - 1] != '\0') {
          if (can_attack(p, x - 1, y, num_monsters)) {
            int boss = attack(p, x - 1, y, num_monsters);
            int index = find_npc(x - 1, y, num_monsters);
            if (ch[index].alive == 0) {
              character[y][x - 1] = '\0';
              if (boss == 1) {
                return 2;
              }
            }
          }
        } else {
          character[y][x - 1] = p->hex;
          character[y][x] = '\0';
          p->x = x - 1;
        }
        return 1;
      } else {
        return 0;
      }
      break;

    default:
      return 0;
      break;
  }
}
