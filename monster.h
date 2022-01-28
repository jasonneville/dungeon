#ifndef MONSTER_H
#define MONSTER_H

#include <stdio.h>
#include <string>
#include <vector>
#include "objects.h"
#include "spells.h"

class character_t {

	public:
	
		uint8_t is_pc;
		char hex;
		uint8_t x;
		uint8_t y;
		int32_t speed;
		uint8_t alive;
		int turn;
		int last_turn;
		uint8_t last_x;
		uint8_t last_y;
		int32_t max_hp;
		int32_t hp;
		dice *damage;
};

class player_t : public character_t{
	public:
		char remembered[21][80];
		obj_t *inventory[10];
		obj_t *equipment[12];
		int book_num = 0;
		int mana = 100;
		int max_mana = 100;
		int last_mana = 0;
		spellbook_t *elemental;
		spellbook_t *trap;
		spellbook_t *terrain;
		spell_t *spell;
		
	
};

class npc_t : public character_t  {
	public:
		std::vector<std::string> coloring;		
		std::string npc_name;
		std::vector<std::string> perks;
		std::vector<std::string> description;
};

extern npc_t *ch;

extern player_t pc;

void killm(uint8_t y, uint8_t x);
int32_t monster_cmp(const void *key, const void *with);
void generate_monsters(char dungeon[21][80], char character[21][80], int nummon, uint8_t pc_x, uint8_t pc_y, character_t *p);
void move_character(npc_t *h, char dungeon[21][80], char character[21][80], uint8_t hardness[21][80], obj_t *objects[21][80]);
int move_pc(int ch, uint8_t hardness[21][80], char character[21][80], character_t *p);

#endif
