#ifndef COMBAT_H
#define COMBAT_H

#include <ncurses.h>

#include <string>
#include <vector>

#include "monster.h"
#include "objects.h"

bool can_attack(character_t *p, int x, int y, int nummon);

int attack(character_t *p, int x, int y, int nummon);

#endif
