#include "equipment_io.h"

#include <ncurses.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

int get_color_int(std::string color) {
  if (color == "COLOR_BLACK") {
    return 7;
  } else if (color == "COLOR_RED") {
    return 1;
  } else if (color == "COLOR_GREEN") {
    return 2;
  } else if (color == "COLOR_YELLOW") {
    return 3;
  } else if (color == "COLOR_BLUE") {
    return 4;
  } else if (color == "COLOR_MAGENTA") {
    return 5;
  } else if (color == "COLOR_CYAN") {
    return 6;
  } else if (color == "COLOR_WHITE") {
    return 7;
  } else {
    return 0;
  }
}
void print_inventory(WINDOW *window, player_t *p) {
  int i;
  int j = 0;

  for (i = 3; i < 18; i++) {
    mvwprintw(window, i, 20, "                                            ");
  }

  for (i = 0; i < 10; i++) {
    std::string item = "";
    if (p->inventory[i] != NULL) {
      item = p->inventory[i]->name;
      mvwprintw(window, 5 + j, 25, "Slot %d: %s (%d)", i, item.c_str(),
                p->inventory[i]->speed);
    }

    else {
      mvwprintw(window, 5 + j, 25, "Slot %d: empty", i);
    }

    j++;
  }

  mvwprintw(window, 3, 35, "Inventory");
}

std::string get_slot_name(int i) {
  switch (i) {
    case 0:
      return "WEAPON";
    case 1:
      return "OFFHAND";
    case 2:
      return "RANGED";
    case 3:
      return "ARMOR";
    case 4:
      return "HELMET";
    case 5:
      return "CLOAK";
    case 6:
      return "GLOVES";
    case 7:
      return "BOOTS";
    case 8:
      return "AMULET";
    case 9:
      return "LIGHT";
    case 10:
      return "RING #1";
    case 11:
      return "RING #2";
  }

  return "";
}

void print_equipment(WINDOW *win, player_t *p) {
  int i;
  int j = 0;

  for (i = 3; i < 16; i++) {
    mvwprintw(win, i, 20, "                                            ");
  }

  for (i = 0; i < 12; i++) {
    std::string slot_name = get_slot_name(i);
    if (p->equipment[i] != NULL) {
      mvwprintw(win, 5 + j, 25, "%s: %s (%d)", slot_name.c_str(),
                p->equipment[i]->name.c_str(), p->equipment[i]->speed);
    } else {
      mvwprintw(win, 5 + j, 25, "%s: empty", slot_name.c_str());
    }
    j++;
  }

  mvwprintw(win, 3, 35, "Equipment");
}

int slot_input(int ch) {
  switch (ch) {
    case 'a':
    case '0':
      return 0;
    case 'b':
    case '1':
      return 1;
    case 'c':
    case '2':
      return 2;
    case 'd':
    case '3':
      return 3;
    case 'e':
    case '4':
      return 4;
    case 'f':
    case '5':
      return 5;
    case 'g':
    case '6':
      return 6;
    case 'h':
    case '7':
      return 7;
    case 'i':
    case '8':
      return 8;
    case 'j':
    case '9':
      return 9;
    case 'k':
      return 10;
    case 'l':
      return 11;
    case 27:
      return -2;
    default:
      return -1;
  }
}

int equipment_slot(std::string in) {
  if (in == "WEAPON") {
    return 0;
  } else if (in == "OFFHAND") {
    return 1;
  } else if (in == "RANGED") {
    return 2;
  } else if (in == "ARMOR") {
    return 3;
  } else if (in == "HELMET") {
    return 4;
  } else if (in == "CLOAK") {
    return 5;
  } else if (in == "GLOVES") {
    return 6;
  } else if (in == "BOOTS") {
    return 7;
  } else if (in == "AMULET") {
    return 8;
  } else if (in == "LIGHT") {
    return 9;
  } else if (in == "RING") {
    return 10;
  } else {
    return -1;
  }
}

int take_off_item(WINDOW *win, player_t *p, obj_t *objects[21][80]) {
  int i = 0;
  print_equipment(win, p);
  wmove(win, 24, 0);
  wclrtobot(win);
  mvwprintw(win, 24, 0, "Choose an equipment slot to take off");
  while (i != 1) {
    int ch = getch();

    int slot = slot_input(ch);

    if (slot == -2) {
      wmove(win, 24, 0);
      wclrtobot(win);
      return 3;

    }

    else if (slot != -1) {
      wmove(win, 24, 0);
      wclrtobot(win);

      int j;
      bool open_slot = false;

      obj_t *tmp = p->equipment[slot];

      for (j = 0; j < 10; j++) {
        if (p->inventory[j] == NULL) {
          open_slot = true;
          break;
        }
      }

      if (open_slot == true && p->equipment[slot] != NULL) {
        pc.speed = pc.speed - tmp->speed;
        p->inventory[j] = tmp;
        p->equipment[slot] = NULL;
      }

      return 3;

    }

    else {
      wmove(win, 24, 0);
      wclrtobot(win);
      mvwprintw(win, 24, 0, "Not a valid slot");
    }
  }

  return 0;
}

int discard_item(WINDOW *win, player_t *p, obj_t *objects[21][80], int action) {
  int i = 0;

  print_inventory(win, p);

  wmove(win, 24, 0);
  wclrtobot(win);
  if (action == 'x') {
    mvwprintw(win, 24, 0, "Choose an inventory slot to expunge");
  } else {
    mvwprintw(win, 24, 0, "Choose an inventory slot to drop");
  }

  while (i != 1) {
    int ch = getch();

    int slot = slot_input(ch);

    if (slot == -2) {
      wmove(win, 24, 0);
      wclrtobot(win);
      return 3;

    }

    else if (slot != -1 && slot < 10) {
      wmove(win, 24, 0);
      wclrtobot(win);

      if (action == 'd') {
        objects[p->y][p->x] = p->inventory[slot];
      }
      p->inventory[slot] = NULL;

      return 3;

    }

    else {
      wmove(win, 24, 0);
      wclrtobot(win);
      mvwprintw(win, 24, 0, "Not a valid slot");
    }
  }
  return 0;
}

int find_npc(int x, int y, int nummon) {
  int i;

  for (i = 0; i < nummon; i++) {
    if (ch[i].x == x && ch[i].y == y && ch[i].alive == 1) {
      return i;
    }
  }

  return -1;
}

int look_at_monster(WINDOW *win, player_t *p, int x, int y, int nummon) {
  wmove(win, 24, 0);
  wclrtobot(win);
  mvwprintw(win, 24, 0, "Select a monster to inspect");

  int i = find_npc(x, y, nummon);

  if (i != -1) {
    wmove(win, 24, 0);
    wclrtobot(win);
    std::vector<std::string> v = ch[i].description;

    int j;
    mvwprintw(win, 24, 0, ch[i].npc_name.c_str());
    for (j = 0; j < v.size(); j++) {
      mvwprintw(win, 25 + j, 0, v[j].c_str());
    }

    mvwprintw(win, 25 + j, 0, "%d", ch[i].hp);

    return 3;

  } else {
    return 3;
  }
}

int wear_equipment(WINDOW *win, player_t *p, obj_t *objects[21][80]) {
  int i = 0;
  print_inventory(win, p);

  wmove(win, 24, 0);
  wclrtobot(win);

  mvwprintw(win, 24, 0, "Choose an item slot to wear");

  while (i != 1) {
    int ch = getch();

    int slot = slot_input(ch);

    if (slot == -2) {
      wmove(win, 24, 0);
      wclrtobot(win);
      return 3;

    }

    else if (slot != -1 && slot < 10) {
      wmove(win, 24, 0);
      wclrtobot(win);

      if (p->inventory[slot] == NULL) {
        return 3;
      }

      else if (p->inventory[slot]->type[0] == "SPELLBOOK") {
        if (p->inventory[slot]->name == "Trap Spellbook") {
          pc.trap->unlocked = true;
        }

        else {
          pc.terrain->unlocked = true;
        }

        p->inventory[slot] = NULL;
        return 3;
      }

      int equip_slot = equipment_slot(p->inventory[slot]->type[0]);

      if (p->equipment[equip_slot] == NULL) {
        p->equipment[equip_slot] = p->inventory[slot];
        p->inventory[slot] = NULL;
        pc.speed = pc.speed + p->equipment[equip_slot]->speed;
      } else if (equip_slot == 10 && p->equipment[equip_slot] != NULL &&
                 p->equipment[11] == NULL) {
        p->equipment[11] = p->inventory[slot];
        p->inventory[slot] = NULL;
        pc.speed = pc.speed + p->equipment[11]->speed;
      } else {
        pc.speed = pc.speed - p->equipment[equip_slot]->speed +
                   p->inventory[slot]->speed;

        std::swap(p->equipment[equip_slot], p->inventory[slot]);
      }

      return 3;

    }

    else {
      wmove(win, 24, 0);
      wclrtobot(win);
      mvwprintw(win, 24, 0, "Not a valid slot");
    }
  }

  return 0;
}

int inspect_item(WINDOW *win, player_t *p) {
  int i = 0;

  print_inventory(win, p);

  wmove(win, 24, 0);
  wclrtobot(win);
  mvwprintw(win, 24, 0, "What inventory slot would you like to inspect?");

  while (i != 1) {
    int ch = getch();

    int slot = slot_input(ch);

    if (slot == -2) {
      wmove(win, 24, 0);
      wclrtobot(win);
      return 3;

    }

    else if (slot != -1 && slot < 10) {
      if (p->inventory[slot] != NULL) {
        wmove(win, 24, 0);
        wclrtobot(win);
        std::vector<std::string> v = p->inventory[slot]->description;
        mvwprintw(win, 24, 0, p->inventory[slot]->name.c_str());
        int j = 0;
        for (j = 0; j < v.size(); j++) {
          mvwprintw(win, 25 + j, 0, v[j].c_str());
        }
      }

    }

    else {
      wmove(win, 24, 0);
      wclrtobot(win);
      mvwprintw(win, 24, 0, "Not a valid slot");
    }
  }

  return 0;
}

int show_inventory(WINDOW *win, player_t *p, obj_t *objects[21][80]) {
  int i = 0;

  print_inventory(win, p);

  while (i != 1) {
    int ch = getch();

    switch (ch) {
      case 'w':
        return wear_equipment(win, p, objects);
        break;

      case 'e':
        return show_equipment(win, p, objects);
        break;

      case 'd':

        return discard_item(win, p, objects, 'd');
        break;
      case 'x':
        return discard_item(win, p, objects, 'x');
        break;
      case 27:
        return 3;
        break;
      case 'I':
        return inspect_item(win, p);
        break;
      default:
        break;
    }
  }
  return 0;
}

int show_equipment(WINDOW *win, player_t *p, obj_t *objects[21][80]) {
  int i = 0;

  print_equipment(win, p);

  while (i != 1) {
    int ch = getch();

    switch (ch) {
      case 't':
        return take_off_item(win, p, objects);

      case 'w':
        return wear_equipment(win, p, objects);
        break;

      case 'i':
        return show_inventory(win, p, objects);
        break;

      case 'd':
        return discard_item(win, p, objects, 'd');
        break;
      case 'x':
        return discard_item(win, p, objects, 'x');
        break;
      case 27:
        return 3;
        break;
      case 'I':
        return inspect_item(win, p);
        break;
      default:
        break;
    }
  }
  return 0;
}
