#ifndef OPARSER_H
#define OPARSER_H


#include <vector>
#include <string>
#include "dice.h"
#include "monster.h"
#include "objects.h"



class obj_desc {

	public:
		
		std::string name = "";
		std::vector<std::string> description;
		std::vector<std::string> color;
		std::vector<std::string> type;
		dice *speed = NULL;	
		dice *hit = NULL;
		dice *damage = NULL;
		dice *dodge = NULL;
		dice *defense = NULL;
		dice *weight = NULL;
		dice *attribute = NULL;
		dice *value = NULL;
		int rarity = -1;
		int artifact = -1;
		bool on_ground = false;
		char symbol;
		
	    obj_t * generate_obj() {
		
			obj_t *tmp = new obj_t;
			tmp->speed = speed->roll();
		   	tmp->hit = hit->roll();
			tmp->damage = damage->roll();
			tmp->dodge = dodge->roll();
			tmp->defense = defense->roll();
			tmp->weight = weight->roll();
			tmp->attribute = attribute->roll();
			tmp->value = value->roll();
			tmp->name = name;
			tmp->rarity = rarity;
			tmp->artifact = artifact;
			tmp->color = color;
			tmp->type = type;
			tmp->symbol = symbol;	
			tmp->description = description;		

			return tmp;
		}
};



bool parse_objects();


extern std::vector<obj_desc> obj_vec;

void print_o_vector();


#endif
