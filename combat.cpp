#include <ncurses.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

#include "dice.h"
#include "equipment_io.h"
#include "monster.h"
#include "objects.h"

bool can_attack(character_t *p, int x, int y, int nummon) {
  int index = find_npc(x, y, nummon);

  if (index != -1 && p->is_pc == 0) {
    return false;
  }

  else if (p->is_pc == 0 && pc.x == x && pc.y == y) {
    return true;
  }

  else if (p->is_pc == 1 && find_npc(x, y, nummon) != -1) {
    return true;
  } else {
    return false;
  }
}

int attack(character_t *p, int x, int y, int nummon) {
  if (p->is_pc == 1) {
    int monster = find_npc(x, y, nummon);

    player_t *player = (player_t *)p;

    int32_t dmg = 0;

    if (player->equipment[0] == NULL) {
      dmg = player->damage->roll();
    }

    else {
      dmg = player->equipment[0]->damage;
    }

    int i = 0;

    for (i = 1; i < 12; i++) {
      if (player->equipment[i] != NULL) {
        dmg = dmg + player->equipment[i]->damage;
      }
    }

    ch[monster].hp = ch[monster].hp - dmg;

    if (ch[monster].hp <= 0) {
      ch[monster].alive = 0;
      int i = 0;

      for (i = 0; i < ch[monster].perks.size(); i++) {
        if (ch[monster].perks[i] == "BOSS") {
          return 1;
        }
      }
    }

    return 0;
  } else {
    int32_t dmg = p->damage->roll();

    pc.hp = pc.hp - dmg;

    if (pc.hp <= 0) {
      pc.alive = 0;
      return 2;
    }

    return 0;
  }
}
