#include "mparser.h"

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

int line_count = 0;
int mon_count = 0;

desc *point;
dice *p_dice;
string m_line;
string delimiter = " ";
vector<desc> vec;

bool eval_npc(desc *p) {
  if (p->name != "" && p->speed != NULL && p->rarity != -1 &&
      p->hitpoints != NULL && p->symbol != '\0' && p->damage != NULL &&
      p->abilities.size() != 0 && p->description.size() != 0 &&
      p->color.size() != 0) {
    return true;
  } else {
    return false;
  }
}

bool color_valid(string s) {
  if (s == "COLOR_RED" || s == "COLOR_GREEN" || s == "COLOR_BLUE" ||
      s == "COLOR_CYAN" || s == "COLOR_YELLOW" || s == "COLOR_MAGENTA" ||
      s == "COLOR_WHITE" || s == "COLOR_BLACK") {
    return true;
  } else {
    return false;
  }
}

bool trait_valid(string s, desc *p) {
  if (s == "SMART") {
    return true;
  } else if (s == "TELE") {
    return true;
  } else if (s == "TUNNEL") {
    return true;
  } else if (s == "ERRATIC") {
    return true;
  } else if (s == "PASS") {
    return true;
  } else if (s == "PICKUP") {
    return true;
  } else if (s == "DESTROY") {
    return true;
  } else if (s == "UNIQ") {
    return true;
  } else if (s == "BOSS") {
    return true;
  } else {
    return false;
  }
}

bool parse_dice(string l, desc *p, string token) {
  p_dice = new dice;

  if (l.find("d") == string::npos || l.find("+") == string::npos) {
    std::cout << "Not all found for " << p->name << "\n";
    p_dice = NULL;
    return false;
  } else if (l.find("d") < l.find("+")) {
    p_dice = NULL;
    return false;

  } else {
    int plus_pos = l.find("+");
    string plus_str = l.substr(0, plus_pos);
    p_dice->set_base(stoi(plus_str));

    int d_pos = l.find("d");
    string d_str = l.substr(plus_pos + 1, d_pos);
    p_dice->set_number(stoi(d_str));

    string sides_str = l.substr(d_pos + 1, l.length());
    p_dice->set_sides(stoi(sides_str));
  }

  if (token == "HP") {
    p->hitpoints = p_dice;
  } else if (token == "DAM") {
    p->damage = p_dice;
  } else {
    p->speed = p_dice;
  }
  return true;
}

int parse_line(string l, desc *p, bool descrip) {
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
  pos = l.find(delimiter);
  string token = l.substr(0, pos);
  l.erase(0, pos + delimiter.length());

  if (token == "SYMB") {
    if (p->symbol != '\0') {
      return 0;
    }
    p->symbol = l[0];

  } else if (token == "HP") {
    if (p->hitpoints != NULL) {
      return 0;
    }

    bool success = parse_dice(l, p, token);

    if (success == false) {
      std::cout << "HP\n";
      return 0;
    }
  } else if (token == "DAM") {
    if (p->damage != NULL) {
      return 0;
    }

    bool success = parse_dice(l, p, token);

    if (success == false) {
      std::cout << "DAM\n";
      return 0;
    }
  } else if (token == "COLOR") {
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
      if (color_valid(v[i]) == false) {
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

  } else if (token == "ABIL") {
    if (p->abilities.size() != 0) {
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
      if (trait_valid(v[i], p) == false) {
        return 0;
      }
    }
    p->abilities = v;
  } else if (token == "SPEED") {
    if (p->speed != NULL) {
      return 0;
    }
    bool success = parse_dice(l, p, token);

    if (success == false) {
      std::cout << "speed";
      return 0;
    }

  } else if (token == "DESC") {
    if (p->description.size() != 0) {
      return 0;
    }
    return 2;
  } else {
    return 0;
  }
  return 1;
}

void print_vector() {
  int i;

  for (i = 0; i < vec.size(); i++) {
    desc d = vec[i];

    std::cout << "Name: " << d.name << "\n";
    int j = 0;
    std::cout << "Desc: ";
    for (j = 0; j < d.description.size(); j++) {
      std::cout << d.description[j] << "\n";
    }
    std::cout << "Color(s): ";
    for (j = 0; j < d.color.size(); j++) {
      std::cout << d.color[j];
      if (j != d.color.size() - 1) {
        std::cout << " ";
      }
    }
    std::cout << "\n";
    std::cout << "Speed: " << d.speed->get_base() << "+"
              << d.speed->get_number() << "d" << d.speed->get_sides() << "\n";
    std::cout << "Abilities: ";
    for (j = 0; j < d.abilities.size(); j++) {
      std::cout << d.abilities[j];
      if (j != d.abilities.size() - 1) {
        std::cout << " ";
      }
    }
    std::cout << "\n";
    std::cout << "Hitpoints: " << d.hitpoints->get_base() << "+"
              << d.hitpoints->get_number() << "d" << d.hitpoints->get_sides()
              << "\n";
    std::cout << "Damage: " << d.damage->get_base() << "+"
              << d.damage->get_number() << "d" << d.damage->get_sides() << "\n";
    std::cout << "Symbol: " << d.symbol << "\n";
    std::cout << "Rarity: " << d.rarity << "\n";
    std::cout << "\n";
  }
}

bool parse_monsters() {
  char *home = getenv("HOME");
  char *path;
  path = (char *)malloc(strlen(home) + strlen("/.rlg327/monster_desc.txt") + 1);
  strcpy(path, home);
  strcat(path, "/.rlg327/monster_desc.txt");
  bool monster_in_progress = false;

  bool done = false;
  bool desc_line = false;

  ifstream m_file;

  m_file.open(path, ios::in);

  if (m_file.fail()) {
    printf("error opening");
    return false;
  }

  while (!done && !m_file.eof()) {
    getline(m_file, m_line);

    if (line_count == 0) {
      if (m_line != "RLG327 MONSTER DESCRIPTION 1") {
        return false;
      }
    } else if (m_line == "") {
      continue;
    } else if (monster_in_progress == false) {
      if (m_line == "BEGIN MONSTER") {
        monster_in_progress = true;
        desc npc;
        point = &npc;
      }
    }

    else {
      if (m_line == "END") {
        bool complete = eval_npc(point);
        if (complete == false) {
          monster_in_progress = false;

          continue;
        } else {
          monster_in_progress = false;

          vec.push_back(*point);
          mon_count++;
        }
      } else {
        int success;

        if (desc_line == true) {
          success = parse_line(m_line, point, true);
        }

        else {
          success = parse_line(m_line, point, false);
        }

        if (success == 0) {
          monster_in_progress = false;

        } else if (success == 1) {
          desc_line = false;
        }

        else {
          desc_line = true;
        }
      }
    }
    line_count++;
  }

  vec.shrink_to_fit();

  return true;
}
