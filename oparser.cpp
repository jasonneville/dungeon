#include "oparser.h"

#include <bits/stdc++.h>
#include <stdio.h>
#include <stdlib.h>

#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "dice.h"
#include "monster.h"

using namespace std;

int o_line_count = 0;
int obj_count = 0;

obj_desc *o_point;
dice *o_dice;
string o_line;
string o_delimiter = " ";
vector<obj_desc> obj_vec;

bool eval_obj(obj_desc *p) {
  if (p->name != "" && p->description.size() != 0 && p->color.size() != 0 &&
      p->type.size() != 0 && p->hit != NULL && p->damage != NULL &&
      p->dodge != NULL && p->defense != NULL && p->weight != NULL &&
      p->speed != NULL && p->attribute != NULL && p->value != NULL &&
      p->rarity != -1) {
    return true;
  } else {
    return false;
  }
}

bool o_color_valid(string s) {
  if (s == "COLOR_RED" || s == "COLOR_GREEN" || s == "COLOR_BLUE" ||
      s == "COLOR_CYAN" || s == "COLOR_YELLOW" || s == "COLOR_MAGENTA" ||
      s == "COLOR_WHITE" || s == "COLOR_BLACK") {
    return true;
  } else {
    return false;
  }
}

bool type_valid(string s, obj_desc *p) {
  if (s == "WEAPON") {
    p->symbol = '|';
    return true;
  } else if (s == "OFFHAND") {
    p->symbol = ')';
    return true;
  } else if (s == "RANGED") {
    p->symbol = '}';
    return true;
  } else if (s == "ARMOR") {
    p->symbol = '[';
    return true;
  } else if (s == "HELMET") {
    p->symbol = ']';
    return true;
  } else if (s == "CLOAK") {
    p->symbol = '(';
    return true;
  } else if (s == "GLOVES") {
    p->symbol = '{';
    return true;
  } else if (s == "BOOTS") {
    p->symbol = '\\';
    return true;
  } else if (s == "RING") {
    p->symbol = '=';
    return true;
  } else if (s == "AMULET") {
    p->symbol = '"';
    return true;
  } else if (s == "LIGHT") {
    p->symbol = '_';
    return true;
  } else if (s == "SCROLL") {
    p->symbol = '~';
    return true;
  } else if (s == "BOOK") {
    p->symbol = '?';
    return true;
  } else if (s == "FLASK") {
    p->symbol = '!';
    return true;
  } else if (s == "GOLD") {
    p->symbol = '$';
    return true;
  } else if (s == "AMMUNITION") {
    p->symbol = '/';
    return true;
  } else if (s == "FOOD") {
    p->symbol = ',';
    return true;
  } else if (s == "WAND") {
    p->symbol = '-';
    return true;
  } else if (s == "CONTAINER") {
    p->symbol = '%';
    return true;
  }

  else {
    return false;
  }
}

bool parse_dice(string l, obj_desc *p, string token) {
  o_dice = new dice;

  if (l.find("d") == string::npos || l.find("+") == string::npos) {
    std::cout << "Not all found for " << p->name << "\n";
    o_dice = NULL;
    return false;
  } else if (l.find("d") < l.find("+")) {
    o_dice = NULL;
    return false;

  } else {
    int plus_pos = l.find("+");
    string plus_str = l.substr(0, plus_pos);

    o_dice->set_base(stoi(plus_str));

    int d_pos = l.find("d");
    string d_str = l.substr(plus_pos + 1, d_pos);
    o_dice->set_number(stoi(d_str));

    string sides_str = l.substr(d_pos + 1, l.length());
    o_dice->set_sides(stoi(sides_str));
  }

  if (token == "HIT") {
    p->hit = o_dice;
  } else if (token == "DAM") {
    p->damage = o_dice;
  } else if (token == "DODGE") {
    p->dodge = o_dice;
  } else if (token == "DEF") {
    p->defense = o_dice;
  } else if (token == "WEIGHT") {
    p->weight = o_dice;
  } else if (token == "SPEED") {
    p->speed = o_dice;
  } else if (token == "ATTR") {
    p->attribute = o_dice;
  } else {
    p->value = o_dice;
  }
  return true;
}

int parse_line(string l, obj_desc *p, bool descrip) {
  if (descrip == true) {
    if (l == ".") {
      return 1;
    }
    if (l.length() > 78) {
      return 0;

    } else {
      p->description.push_back(l);
      return 2;
    }
  }

  int pos = 0;
  pos = l.find(o_delimiter);
  string token = l.substr(0, pos);
  l.erase(0, pos + o_delimiter.length());

  if (token == "HIT") {
    if (p->hit != NULL) {
      return 0;
    }

    bool success = parse_dice(l, p, token);

    if (success == false) {
      return 0;
    }
  } else if (token == "DAM") {
    if (p->damage != NULL) {
      return 0;
    }

    bool success = parse_dice(l, p, token);

    if (success == false) {
      return 0;
    }
  }

  else if (token == "DODGE") {
    if (p->dodge != NULL) {
      return 0;
    }

    bool success = parse_dice(l, p, token);

    if (success == false) {
      return 0;
    }
  }

  else if (token == "DEF") {
    if (p->defense != NULL) {
      return 0;
    }

    bool success = parse_dice(l, p, token);

    if (success == false) {
      return 0;
    }
  }

  else if (token == "WEIGHT") {
    if (p->weight != NULL) {
      return 0;
    }

    bool success = parse_dice(l, p, token);

    if (success == false) {
      return 0;
    }
  }

  else if (token == "SPEED") {
    if (p->speed != NULL) {
      return 0;
    }

    bool success = parse_dice(l, p, token);

    if (success == false) {
      return 0;
    }
  } else if (token == "ATTR") {
    if (p->attribute != NULL) {
      return 0;
    }

    bool success = parse_dice(l, p, token);

    if (success == false) {
      return 0;
    }
  }

  else if (token == "VAL") {
    if (p->value != NULL) {
      return 0;
    }

    bool success = parse_dice(l, p, token);
    if (success == false) {
      return 0;
    }
  }

  else if (token == "COLOR") {
    if (p->color.size() != 0) {
      return 0;
    }
    vector<string> v;
    stringstream check1(l);

    string intermediate;

    while (getline(check1, intermediate, ' ')) {
      std::string tmp = "COLOR_" + intermediate;

      if (std::find(v.begin(), v.end(), intermediate) != v.end()) {
        continue;
      } else {
        v.push_back(tmp);
      }
    }

    int i = 0;
    for (i = 0; i < v.size(); i++) {
      if (o_color_valid(v[i]) == false) {
        return 0;
      }
    }

    p->color = v;

  }

  else if (token == "NAME") {
    string tmp = l;
    p->name = tmp;

  } else if (token == "RRTY") {
    int rare = stoi(l);
    p->rarity = rare;

  } else if (token == "TYPE") {
    if (p->type.size() != 0) {
      return 0;
    }

    vector<string> v;
    stringstream check1(l);
    string intermediate;

    while (getline(check1, intermediate, ' ')) {
      if (std::find(v.begin(), v.end(), intermediate) != v.end()) {
        continue;
      } else {
        v.push_back(intermediate);
      }
    }

    int i = 0;
    for (i = 0; i < v.size(); i++) {
      if (type_valid(v[i], p) == false) {
        return 0;
      }
    }
    p->type = v;
  } else if (token == "DESC") {
    if (p->description.size() != 0) {
      return 0;
    }
    return 2;
  } else if (token == "ART") {
    if (l == "FALSE") {
      p->artifact = 0;
      return 1;
    } else if (l == "TRUE") {
      p->artifact = 1;
      return 1;
    } else {
      return 0;
    }

  } else {
    return 0;
  }
  return 1;
}

void print_o_vector() {
  int i;

  if (obj_vec.size() == 0) {
    std::cout << "size is 0" << std::endl;
  }

  for (i = 0; i < obj_vec.size(); i++) {
    obj_desc d = obj_vec[i];

    std::cout << "Name: " << d.name << "\n";
    int j = 0;
    std::cout << "Desc: ";
    for (j = 0; j < d.description.size(); j++) {
      std::cout << d.description[j] << "\n";
    }
    std::cout << "Type: ";
    for (j = 0; j < d.type.size(); j++) {
      std::cout << d.type[j];
      if (j != d.type.size() - 1) {
        std::cout << " ";
      }
    }
    std::cout << "\n";

    std::cout << "Color(s): ";
    for (j = 0; j < d.color.size(); j++) {
      std::cout << d.color[j];
      if (j != d.color.size() - 1) {
        std::cout << " ";
      }
    }

    std::cout << std::endl;

    std::cout << "Hit: " << d.hit->get_base() << "+" << d.hit->get_number()
              << "d" << d.hit->get_sides() << "\n";

    std::cout << "Damage: " << d.damage->get_base() << "+"
              << d.damage->get_number() << "d" << d.damage->get_sides() << "\n";

    std::cout << "Dodge: " << d.dodge->get_base() << "+"
              << d.dodge->get_number() << "d" << d.dodge->get_sides() << "\n";

    std::cout << "Defense: " << d.defense->get_base() << "+"
              << d.defense->get_number() << "d" << d.defense->get_sides()
              << "\n";

    std::cout << "Weight: " << d.weight->get_base() << "+"
              << d.weight->get_number() << "d" << d.weight->get_sides() << "\n";

    std::cout << "Speed: " << d.speed->get_base() << "+"
              << d.speed->get_number() << "d" << d.speed->get_sides() << "\n";

    std::cout << "Attribute: " << d.attribute->get_base() << "+"
              << d.attribute->get_number() << "d" << d.attribute->get_sides()
              << "\n";

    std::cout << "Value: " << d.value->get_base() << "+"
              << d.value->get_number() << "d" << d.value->get_sides() << "\n";

    std::cout << "Artifact: " << d.artifact << "\n";

    std::cout << "Rarity: " << d.rarity << "\n";

    std::cout << "\n";
  }
}

bool parse_objects() {
  char *home = getenv("HOME");
  char *path;
  path = (char *)malloc(strlen(home) + strlen("/.rlg327/object_desc.txt") + 1);
  strcpy(path, home);
  strcat(path, "/.rlg327/object_desc.txt");
  bool object_in_progress = false;

  bool done = false;
  bool desc_line = false;

  ifstream o_file;

  o_file.open(path, ios::in);

  if (o_file.fail()) {
    printf("error opening");
    return false;
  }

  while (!done && !o_file.eof()) {
    getline(o_file, o_line);

    if (o_line_count == 0) {
      if (o_line != "RLG327 OBJECT DESCRIPTION 1") {
        return false;
      }
    } else if (o_line == "") {
      continue;
    } else if (object_in_progress == false) {
      if (o_line == "BEGIN OBJECT") {
        object_in_progress = true;
        obj_desc obj;
        o_point = &obj;
      }
    }

    else {
      if (o_line == "END") {
        bool complete = eval_obj(o_point);
        if (complete == false) {
          object_in_progress = false;

          continue;
        } else {
          object_in_progress = false;

          obj_vec.push_back(*o_point);
          obj_count++;
        }
      } else {
        int success;

        if (desc_line == true) {
          success = parse_line(o_line, o_point, true);
        }

        else {
          success = parse_line(o_line, o_point, false);
        }

        if (success == 0) {
          object_in_progress = false;

        } else if (success == 1) {
          desc_line = false;
        }

        else {
          desc_line = true;
        }
      }
    }
    o_line_count++;
  }

  obj_vec.shrink_to_fit();

  return true;
}
