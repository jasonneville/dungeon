#include <endian.h>
#include <math.h>
#include <ncurses.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <string>
#include <vector>

#include "equipment_io.h"
#include "heap.h"
#include "monster.h"
#include "mparser.h"
#include "objects.h"
#include "oparser.h"
#include "pathfind.h"
#include "spells.h"

#define MAX_ROOMS 10
#define MIN_ROOMS 6
#define MAX_UPSTAIRS 5
#define MAX_DOWNSTAIRS 5
#define DEFAULT_MONSTER 10
#define WINDOW_START 1

char dungeon[21][80];
char character[21][80];
uint8_t tunneling[21][80];
uint8_t non_tunneling[21][80];
uint8_t hardness[21][80];
obj_t *objects[21][80];
spell_t *traps[21][80];
uint16_t num_rooms = 0;
uint16_t num_upward_stairs;
uint16_t num_downward_stairs;
uint8_t nummon;

class staircase_up {
 public:
  uint8_t x_pos;
  uint8_t y_pos;
};

staircase_up *upstairs;

class staircase_down {
 public:
  uint8_t x_pos;
  uint8_t y_pos;
};

staircase_down *downstairs;

class room {
 public:
  uint8_t x_pos;
  uint8_t y_pos;
  uint8_t x_size;
  uint8_t y_size;
};

room *dung_rooms;

typedef enum {

  save_dungeon,
  both,
  none,
  load_dungeon

} action_t;

void create_room(int number) {
  int room_success = 0;
  int redo = 0;

  while (room_success != 1) {
    redo = 0;

    uint8_t room_size_x = (rand() % 16) + 4;
    uint8_t room_size_y = (rand() % 5) + 3;
    uint8_t room_loc_x = (rand() % 76) + 2;
    uint8_t room_loc_y = (rand() % 17) + 2;
    int y, x;

    dung_rooms[number].x_pos = room_loc_x;
    dung_rooms[number].y_pos = room_loc_y;
    dung_rooms[number].x_size = room_size_x;
    dung_rooms[number].y_size = room_size_y;

    if (room_loc_y + room_size_y > 18 || room_loc_x + room_size_x > 78) {
      redo = 1;
    }

    else {
      for (y = room_loc_y; y < room_loc_y + room_size_y; y++) {
        for (x = room_loc_x; x < room_loc_x + room_size_x; x++) {
          if (dungeon[y][x] != ' ' || dungeon[y + 1][x] == '.' ||
              dungeon[y - 1][x] == '.' || dungeon[y][x + 1] == '.' ||
              dungeon[y][x - 1] == '.' || dungeon[y - 1][x - 1] == '.' ||
              dungeon[y + 1][x - 1] == '.' || dungeon[y - 1][x + 1] == '.' ||
              dungeon[y + 1][x + 1] == '.') {
            redo = 1;
            break;
          }
        }
        if (redo == 1) {
          break;
        }
      }
    }

    if (redo != 1) {
      for (y = room_loc_y; y < room_loc_y + room_size_y; y++) {
        for (x = room_loc_x; x < room_loc_x + room_size_x; x++) {
          dungeon[y][x] = '.';
        }
      }

      room_success = 1;
    }
  }
}

void connect_rooms(void) {
  int i = 0;

  while (i + 1 < num_rooms) {
    uint8_t current_x = dung_rooms[i].x_pos;
    uint8_t current_y = dung_rooms[i].y_pos;
    uint8_t x_loc_1 = dung_rooms[i + 1].x_pos;
    uint8_t y_loc_1 = dung_rooms[i + 1].y_pos;

    int dist_y = abs(y_loc_1 - current_y);
    int dist_x = abs(x_loc_1 - current_x);

    int up = 0;
    int right = 0;

    if (current_y - y_loc_1 >= 0) {
      up = 1;
    }

    if (current_x - x_loc_1 <= 0) {
      right = 1;
    }

    while (dist_x > 0 && dist_y > 0) {
      int direction = rand() % 3;

      if (direction == 0 || direction == 1) {
        if (right == 1) {
          if (dungeon[current_y][current_x] != '.') {
            dungeon[current_y][current_x] = '#';
          }

          current_x++;
        } else {
          if (dungeon[current_y][current_x] != '.') {
            dungeon[current_y][current_x] = '#';
          }
          current_x--;
        }
        dist_x--;
      }

      else {
        if (up == 1) {
          if (dungeon[current_y][current_x] != '.') {
            dungeon[current_y][current_x] = '#';
          }
          current_y--;
        } else {
          if (dungeon[current_y][current_x] != '.') {
            dungeon[current_y][current_x] = '#';
          }

          current_y++;
        }
        dist_y--;
      }
    }

    if (dist_y != 0) {
      int y;
      int x = x_loc_1;

      if (up == 1) {
        for (y = current_y; y >= y_loc_1; y--) {
          if (dungeon[y][x] != '.') {
            dungeon[y][x] = '#';
          }
        }
      } else {
        for (y = current_y; y <= y_loc_1; y++) {
          if (dungeon[y][x] == ' ') {
            dungeon[y][x] = '#';
          }
        }
      }

    } else if (dist_x != 0) {
      int x;
      int y = y_loc_1;

      if (right == 1) {
        for (x = current_x; x <= x_loc_1; x++) {
          if (dungeon[y][x] == ' ') {
            dungeon[y][x] = '#';
          }
        }
      } else {
        for (x = current_x; x >= x_loc_1; x--) {
          if (dungeon[y][x] == ' ') {
            dungeon[y][x] = '#';
          }
        }
      }
    }
    i++;
  }
}

void init_character(void) {
  int y, x;

  for (y = 0; y < 21; y++) {
    for (x = 0; x < 80; x++) {
      character[y][x] = '\0';
    }
  }
}

void fill_dungeon(void) {
  int r, c;

  for (r = 0; r < 21; r++) {
    for (c = 0; c < 80; c++) {
      if (r == 0 || r == 20) {
        dungeon[r][c] = '-';
      } else if (c == 0 || c == 79) {
        dungeon[r][c] = '|';
      }

      else {
        dungeon[r][c] = ' ';
      }
    }
  }
}

void display_dungeon(void) {
  int r, c;
  for (r = 0; r < 21; r++) {
    for (c = 0; c < 80; c++) {
      if (character[r][c] == '@') {
        mvaddch(r + 1, c, '@');
      }

      else if (character[r][c] != '\0') {
        int index = find_npc(c, r, nummon);

        npc_t tmp = ch[index];
        int col = get_color_int(tmp.coloring[0]);
        attron(COLOR_PAIR(col));
        mvaddch(r + 1, c, (uint8_t)character[r][c]);
        attroff(COLOR_PAIR(col));

      } else if (objects[r][c] != NULL) {
        int col = get_color_int(objects[r][c]->color[0]);
        attron(COLOR_PAIR(col));
        mvaddch(r + 1, c, objects[r][c]->symbol);
        attroff(COLOR_PAIR(col));

      } else {
        mvaddch(r + 1, c, (uint8_t)dungeon[r][c]);
      }
      if (c == 80) {
        mvaddch(r + 1, c, (uintptr_t) "\n");
      }
    }
  }

  move(22, 0);
  clrtobot();
  mvprintw(22, 0, "Hitpoints: %d 				Mana: %d", pc.hp,
           pc.mana);
}

void create_stairs(int load) {
  int i = 0;

  if (load == 1) {
    while (i < num_upward_stairs) {
      dungeon[upstairs[i].y_pos][upstairs[i].x_pos] = '<';
      i++;
    }

    i = 0;

    while (i < num_downward_stairs) {
      dungeon[downstairs[i].y_pos][downstairs[i].x_pos] = '>';
      i++;
    }

  }

  else {
    num_upward_stairs = (rand() % 5) + 1;
    num_downward_stairs = (rand() % 5) + 1;

    upstairs =
        (staircase_up *)malloc(num_downward_stairs * sizeof(staircase_up));
    downstairs =
        (staircase_down *)malloc(num_upward_stairs * sizeof(staircase_down));

    while (i != num_upward_stairs) {
      int random_x = (rand() % 76) + 2;
      int random_y = (rand() % 17) + 2;

      if (dungeon[random_y][random_x] == '.' ||
          dungeon[random_y][random_x] == '#') {
        dungeon[random_y][random_x] = '<';
        upstairs[i].x_pos = random_x;
        upstairs[i].y_pos = random_y;
        i++;
      }
    }
    i = 0;
    while (i != num_downward_stairs) {
      int random_x = (rand() % 76) + 2;
      int random_y = (rand() % 17) + 2;

      if (dungeon[random_y][random_x] == '.' ||
          dungeon[random_y][random_x] == '#') {
        dungeon[random_y][random_x] = '>';
        downstairs[i].x_pos = random_x;
        downstairs[i].y_pos = random_y;
        i++;
      }
    }
  }
}

void place_character(int load) {
  if (load == 0) {
    int i = 0;

    while (i != 1) {
      int y = (rand() % 17) + 2;
      int x = (rand() % 76) + 2;

      if (dungeon[y][x] == '.') {
        character[y][x] = '@';
        pc.x = x;
        pc.y = y;
        i = 1;
      }
    }
  }

  else {
    character[pc.y][pc.x] = '@';
  }
}

void calculate_hardness(void) {
  int y, x;

  for (y = 0; y < 21; y++) {
    for (x = 0; x < 80; x++) {
      if (dungeon[y][x] == ' ') {
        uint8_t num = (uint8_t)((rand() % 254) + 1);

        hardness[y][x] = num;

      }

      else if (dungeon[y][x] == '-' || dungeon[y][x] == '|' ||
               dungeon[y][x] == 'r') {
        hardness[y][x] = 255;

      }

      else {
        hardness[y][x] = 0;
      }
    }
  }
}

void make_dungeon(void) {
  std::cout << "here";
  srand(time(NULL));

  int total_rooms;

  num_rooms = (rand() % 4) + 6;

  dung_rooms = (room *)malloc(4 * num_rooms);
  fill_dungeon();
  init_character();

  int i;
  i = 0;

  while (i < num_rooms) {
    create_room(i);
    i++;
  }

  connect_rooms();
  place_character(0);
  create_stairs(0);
  calculate_hardness();
  //	generate_monsters(dungeon, character, nummon, pc.x, pc.y, NULL);
}

int go_down_up(character_t *p, heap_t *h, char c) {
  int i, j;
  uint8_t x = p->x;
  uint8_t y = p->y;

  if (dungeon[y][x] != c) {
    return 0;
  }

  else {
    num_rooms = (rand() % 4) + 6;

    dung_rooms = (room *)malloc(4 * num_rooms);

    fill_dungeon();

    init_character();

    i = 0;

    while (i < num_rooms) {
      create_room(i);
      i++;
    }

    connect_rooms();

    for (i = 0; i < 21; i++) {
      for (j = 0; j < 80; j++) {
        objects[i][j] = NULL;
      }
    }

    place_character(0);

    create_stairs(0);
    calculate_hardness();
    free(dung_rooms);
    free(upstairs);
    free(downstairs);
    while (heap_remove_min(h)) {
      continue;
    }
    generate_monsters(dungeon, character, nummon, pc.x, pc.y, p);

    generate_objects(hardness, objects);
    //	generate_monsters(dungeon, character, nummon, pc.x, pc.y, NULL);

    for (i = 0; i < nummon; i++) {
      heap_insert(h, &ch[i]);
    }
    return 5;
  }
}

void load_rooms(void) {
  int i = 0;

  while (i < num_rooms) {
    int y, x;

    uint8_t room_loc_y = dung_rooms[i].y_pos;
    uint8_t room_loc_x = dung_rooms[i].x_pos;
    uint8_t room_x_size = dung_rooms[i].x_size;
    uint8_t room_y_size = dung_rooms[i].y_size;

    for (y = room_loc_y; y < room_loc_y + room_y_size; y++) {
      for (x = room_loc_x; x < room_loc_x + room_x_size; x++) {
        dungeon[y][x] = '.';
      }
    }
    i++;
  }
}

void recreate_dungeon(void) {
  fill_dungeon();

  init_character();

  load_rooms();

  place_character(1);

  create_stairs(1);

  int y, x;

  for (y = 0; y < 21; y++) {
    for (x = 0; x < 80; x++) {
      if (hardness[y][x] == 0 && dungeon[y][x] == ' ') {
        dungeon[y][x] = '#';
      }
    }
  }

  generate_monsters(dungeon, character, nummon, pc.x, pc.y, NULL);
}

void save_dung(FILE *f) {
  char str[13] = "RLG327-S2019";

  fwrite(str, 12, 1, f);

  uint32_t zero = 0;
  uint32_t file_size = htobe32(sizeof(f));

  fwrite(&zero, 4, 1, f);
  fwrite(&file_size, 4, 1, f);
  fwrite(&pc.x, 1, 1, f);
  fwrite(&pc.y, 1, 1, f);

  int y, x;

  for (y = 0; y < 21; y++) {
    for (x = 0; x < 80; x++) {
      fwrite(&hardness[y][x], 1, 1, f);
    }
  }

  uint16_t be_num_rooms = htobe16(num_rooms);

  fwrite(&be_num_rooms, 2, 1, f);

  int n = 0;
  while (n < num_rooms) {
    fwrite(&dung_rooms[n].x_pos, 1, 1, f);
    fwrite(&dung_rooms[n].y_pos, 1, 1, f);
    fwrite(&dung_rooms[n].x_size, 1, 1, f);
    fwrite(&dung_rooms[n].y_size, 1, 1, f);
    n++;
  }

  uint16_t be_num_upward_stairs = htobe16(num_upward_stairs);

  fwrite(&be_num_upward_stairs, 2, 1, f);

  n = 0;

  while (n < num_upward_stairs) {
    fwrite(&upstairs[n].x_pos, 1, 1, f);
    fwrite(&upstairs[n].y_pos, 1, 1, f);

    n++;
  }

  uint16_t be_num_downward_stairs = htobe16(num_downward_stairs);

  fwrite(&be_num_downward_stairs, 2, 1, f);

  n = 0;

  while (n < num_downward_stairs) {
    fwrite(&downstairs[n].x_pos, 1, 1, f);
    fwrite(&downstairs[n].y_pos, 1, 1, f);
    n++;
  }

  free(downstairs);
  free(upstairs);
  free(dung_rooms);
}

void load_dung(FILE *f, int destroy) {
  char *str;
  str = (char *)malloc(12);
  uint32_t zero;
  uint32_t file_size;
  uint32_t be_file_size;

  fread(str, 12, 1, f);
  fread(&zero, 4, 1, f);
  fread(&be_file_size, 4, 1, f);
  fread(&pc.x, 1, 1, f);
  fread(&pc.y, 1, 1, f);

  file_size = be32toh(be_file_size);

  int y, x;

  for (y = 0; y < 21; y++) {
    for (x = 0; x < 80; x++) {
      fread(&hardness[y][x], 1, 1, f);
    }
  }

  uint16_t be_num_rooms;

  fread(&be_num_rooms, 2, 1, f);

  num_rooms = be16toh(be_num_rooms);

  dung_rooms = (room *)malloc(4 * num_rooms);

  int n = 0;
  while (n < num_rooms) {
    fread(&dung_rooms[n].x_pos, 1, 1, f);
    fread(&dung_rooms[n].y_pos, 1, 1, f);
    fread(&dung_rooms[n].x_size, 1, 1, f);
    fread(&dung_rooms[n].y_size, 1, 1, f);

    n++;
  }

  uint16_t be_num_upward_stairs;

  fread(&be_num_upward_stairs, 2, 1, f);

  num_upward_stairs = be16toh(be_num_upward_stairs);

  upstairs = (staircase_up *)malloc(2 * num_upward_stairs);

  n = 0;

  while (n < num_upward_stairs) {
    fread(&upstairs[n].x_pos, 1, 1, f);
    fread(&upstairs[n].y_pos, 1, 1, f);
    n++;
  }

  uint16_t be_num_downward_stairs;

  fread(&be_num_downward_stairs, 2, 1, f);

  num_downward_stairs = be16toh(be_num_downward_stairs);

  n = 0;

  downstairs = (staircase_down *)malloc(2 * num_downward_stairs);

  while (n < num_downward_stairs) {
    fread(&downstairs[n].x_pos, 1, 1, f);
    fread(&downstairs[n].y_pos, 1, 1, f);
    n++;
  }

  recreate_dungeon();
  if (destroy == 1) {
    free(downstairs);
    free(upstairs);
    free(dung_rooms);
  }
}

void display_non_tunneling(void) {
  int y, x;

  for (y = 0; y < 21; y++) {
    for (x = 0; x < 80; x++) {
      if (hardness[y][x] == 255) {
        printf("X");
      } else if (hardness[y][x] > 0) {
        printf(" ");
      } else if (non_tunneling[y][x] == 0) {
        printf("@");
      } else {
        printf("%d", non_tunneling[y][x] % 10);
      }
      if (x == 79) {
        printf("\n");
      }
    }
  }
}

void display_tunneling(void) {
  int y, x;

  for (y = 0; y < 21; y++) {
    for (x = 0; x < 80; x++) {
      if (hardness[y][x] == 255) {
        printf("X");
      } else if (tunneling[y][x] == 0) {
        printf("@");
      } else {
        printf("%d", tunneling[y][x] % 10);
      }

      if (x == 79) {
        printf("\n");
      }
    }
  }
}

int get_num_alive(int before, int value) {
  int i;
  int num_alive = 0;

  if (before == 1) {
    for (i = value - 1; i > 0; i--) {
      if (ch[i].alive == 1) {
        num_alive++;
      }
    }
  } else {
    for (i = value + 1; i < nummon + 1; i++) {
      if (ch[i].alive == 1) {
        num_alive++;
      }
    }
  }
  return num_alive;
}

void print_monster_list(WINDOW *window, int top) {
  int p = 0;
  int i;
  uint8_t x = pc.x;
  uint8_t y = pc.y;

  for (i = 3; i < 16; i++) {
    mvwprintw(window, i, 25, "                                  ");
  }

  for (i = top; i < nummon; i++) {
    if (ch[i].alive == 1) {
      int8_t dist_y = ch[i].y - y;
      int8_t dist_x = ch[i].x - x;
      const char *dir_x;
      const char *dir_y;
      if (dist_y <= 0) {
        dir_y = "north";
      } else {
        dir_y = "south";
      }
      if (dist_x <= 0) {
        dir_x = "west";
      } else {
        dir_x = "east";
      }

      mvwprintw(window, 5 + p, 30, "%c, %d %s and %d %s", ch[i].hex,
                abs(dist_y), dir_y, abs(dist_x), dir_x);
      p++;
    }
    if (p == 10) {
      break;
    }
  }
  mvwprintw(window, 3, 35, "Monster List");
}

void update_fog() {
  uint8_t x = pc.x;
  uint8_t y = pc.y;

  int i, j;

  int range;

  if (pc.equipment[9] != NULL) {
    range = 3;
  }

  else {
    range = 2;
  }

  for (i = pc.y - range; i <= pc.y + range; i++) {
    for (j = pc.x - range; j <= pc.x + range; j++) {
      if (i > 0 && i < 20 && j > 0 && j < 79) {
        pc.remembered[i][j] = dungeon[i][j];
      }
    }
  }
}

void display_fog() {
  update_fog();

  int r, c;
  for (r = 0; r < 21; r++) {
    for (c = 0; c < 80; c++) {
      if (character[r][c] == '@') {
        mvaddch(r + 1, c, '@');
      }

      else if (character[r][c] != '\0' &&
               (abs(pc.y - r) <= 3 && abs(pc.x - c) <= 3) &&
               pc.equipment[9] != NULL) {
        int index = find_npc(c, r, nummon);
        npc_t tmp = ch[index];
        int col = get_color_int(tmp.coloring[0]);
        attron(COLOR_PAIR(col));
        mvaddch(r + 1, c, character[r][c]);
        attroff(COLOR_PAIR(col));
      }

      else if (character[r][c] != '\0' &&
               (abs(pc.y - r) <= 2 && abs(pc.x - c) <= 2)) {
        int index = find_npc(c, r, nummon);
        npc_t tmp = ch[index];
        int col = get_color_int(tmp.coloring[0]);
        attron(COLOR_PAIR(col));
        mvaddch(r + 1, c, character[r][c]);
        attroff(COLOR_PAIR(col));
      }

      else if (objects[r][c] != NULL &&
               (abs(pc.y - r) <= 3 && abs(pc.x - c) <= 3) &&
               pc.equipment[9] != NULL) {
        int col = get_color_int(objects[r][c]->color[0]);
        attron(COLOR_PAIR(col));
        mvaddch(r + 1, c, objects[r][c]->symbol);
        attroff(COLOR_PAIR(col));
      }

      else if (objects[r][c] != NULL &&
               (abs(pc.y - r) <= 2 && abs(pc.x - c) <= 2)) {
        int col = get_color_int(objects[r][c]->color[0]);
        attron(COLOR_PAIR(col));
        mvaddch(r + 1, c, objects[r][c]->symbol);
        attroff(COLOR_PAIR(col));
      }

      else if (dungeon[r][c] == '|' || dungeon[r][c] == '-') {
        mvaddch(r + 1, c, (uint8_t)dungeon[r][c]);
      } else if (pc.remembered[r][c] != '\0') {
        int range;

        if (pc.equipment[9] != NULL) {
          range = 4;
        } else {
          range = 2;
        }
        if (abs(pc.y - r) <= range && abs(pc.x - c) <= range) {
          attron(A_BOLD);
          mvaddch(r + 1, c, (uint8_t)pc.remembered[r][c]);
          attroff(A_BOLD);
        } else {
          mvaddch(r + 1, c, (uint8_t)pc.remembered[r][c]);
        }
      }

      else {
        mvaddch(r + 1, c, ' ');
      }

      if (c == 80) {
        mvaddch(r + 1, c, (uintptr_t) "\n");
      }
    }
  }
  move(22, 0);
  clrtobot();
  mvprintw(22, 0, "Hitpoints: %d/%d	Mana: %d", pc.hp, pc.max_hp, pc.mana);
}

void display_teleporting(int y, int x, WINDOW *win) {
  int r, c;
  for (r = 0; r < 21; r++) {
    for (c = 0; c < 80; c++) {
      if (r == y && c == x) {
        mvaddch(r + 1, c, (uint8_t)'*');
      } else if (character[r][c] != '\0' && character[r][c] != '@') {
        int index = find_npc(c, r, nummon);
        npc_t tmp = ch[index];
        int col = get_color_int(tmp.coloring[0]);
        attron(COLOR_PAIR(col));
        mvaddch(r + 1, c, (uint8_t)character[r][c]);
        attroff(COLOR_PAIR(col));
      } else if (objects[r][c] != NULL) {
        int col = get_color_int(objects[r][c]->color[0]);
        attron(COLOR_PAIR(col));
        mvaddch(r + 1, c, objects[r][c]->symbol);
        attroff(COLOR_PAIR(col));
      } else {
        mvaddch(r + 1, c, (uint8_t)dungeon[r][c]);
      }
      if (c == 80) {
        mvaddch(r + 1, c, (uintptr_t) "\n");
      }
    }
  }
  move(22, 0);
  clrtobot();
  mvprintw(22, 0, "Hitpoints: %d 				Mana: %d", pc.hp,
           pc.mana);

  // wrefresh(win);
}

void teleport(int y, int x, character_t *p, int random) {
  if (random == 0) {
    if (y > 20 || y < 0 || hardness[y][x] == 255 || x > 80 || x < 0) {
      return;
    }

    else {
      character[p->y][p->x] = '\0';
      if (character[y][x] != '\0') {
        killm(y, x);
      }
      p->x = x;
      p->y = y;
      character[p->y][p->x] = '@';

      update_fog();
    }
  } else {
    int rand_y = (rand() % 17) + 2;
    int rand_x = (rand() % 76) + 2;
    character[p->y][p->x] = '\0';
    if (character[rand_y][rand_x] != '\0') {
      killm(rand_y, rand_x);
    }
    p->x = rand_x;
    p->y = rand_y;
    character[p->y][p->x] = '@';
    update_fog();
  }
}

bool inventory_has_size(player_t *p) {
  int i;

  for (i = 0; i < 10; i++) {
    if (p->inventory[i] == NULL) {
      return true;
    }
  }

  return false;
}

void pick_up_item(player_t *p) {
  int i;

  for (i = 0; i < 10; i++) {
    if (p->inventory[i] == NULL) {
      p->inventory[i] = objects[p->y][p->x];
      return;
    }
  }
}

int eval_input(player_t *p, int c, heap_t *h, int other_scr, WINDOW *win) {
  int complete = 0;

  switch (other_scr) {
    case 0:
      switch (c) {
        case 55:

        case 'y':
          complete = move_pc(55, hardness, character, p);

          break;
        case 56:

        case 'k':
          complete = move_pc(56, hardness, character, p);
          break;
        case 57:

        case 'u':
          complete = move_pc(57, hardness, character, p);
          break;

        case 54:

        case 'l':
          complete = move_pc(54, hardness, character, p);
          break;
        case 51:

        case 'n':
          complete = move_pc(51, hardness, character, p);
          break;

        case 50:

        case 'j':
          complete = move_pc(50, hardness, character, p);
          break;
        case 49:

        case 'b':
          complete = move_pc(49, hardness, character, p);
          break;
        case 52:
        case 'h':
          complete = move_pc(52, hardness, character, p);

          break;

        case '<':

          return go_down_up(p, h, '<');

          break;
        case '>':

          return go_down_up(p, h, '>');

          break;
        case 53:
        case 32:
        case '.':
          complete = 1;
          break;
        case 'q':
          return -1;
          break;
        case 'm':
          return eval_input(p, c, h, 1, win);
          break;
        case 'g':
          return eval_input(p, c, h, 2, win);
        case 'f':

          return 4;
          break;

        case 'i':
          return show_inventory(win, p, objects);
          break;
        case ',':

          if (objects[pc.y][pc.x] == NULL ||
              objects[pc.y][pc.x]->type[0] == "TRAP") {
            return 0;
          } else {
            if (inventory_has_size(p)) {
              pick_up_item(p);
              objects[p->y][p->x] = NULL;
              return 3;
            } else {
              return 0;
            }
          }
          break;

        case 'd': {
          return discard_item(win, p, objects, 'd');

          break;
        }

        case 'x': {
          return discard_item(win, p, objects, 'x');
          break;
        }

        case 'I': {
          return inspect_item(win, p);
          break;
        }

        case 'L':
          return eval_input(p, c, h, 3, win);
          break;
        case 'e':
          return show_equipment(win, p, objects);
          break;

        case 'w':
          return wear_equipment(win, p, objects);
          break;

        case 't':
          return take_off_item(win, p, objects);
          break;

        case 'c':
          return eval_input(p, c, h, 4, win);
          break;
        case 's':
          return show_spells(win, dungeon, hardness, objects, character,
                             nummon);
          break;
        case '?':

          if (pc.book_num == 0) {
            pc.book_num = 1;
            if (pc.trap->unlocked == true) {
              pc.spell = pc.trap->spells[0];
            }
          } else if (pc.book_num == 1) {
            pc.book_num = 2;
            if (pc.terrain->unlocked == true) {
              pc.spell = pc.terrain->spells[0];
            }
          } else if (pc.book_num == 2) {
            pc.book_num = 0;
            pc.spell = pc.elemental->spells[0];
          }
          break;

        default:
          return 0;
          break;
      }
      break;
    case 1: {
      print_monster_list(win, 1);
      wrefresh(win);
      int i = 0;
      int scroll = 1;

      while (i != 1) {
        int in = getch();

        switch (in) {
          case KEY_UP:

            if (scroll > 1 && (get_num_alive(1, scroll) > 0)) {
              print_monster_list(win, scroll - 1);
              scroll--;
            }

            break;

          case KEY_DOWN:

            if (get_num_alive(0, scroll) >= 10) {
              print_monster_list(win, scroll + 1);
              scroll++;
            }

            break;
          case 27:
            return 3;
            break;
          case 'q':
            return -1;
            break;
          default:
            break;
        }
      }
    } break;
    case 2: {
      uint8_t y = p->y;
      uint8_t x = p->x;

      display_teleporting(y, x, win);
      wrefresh(win);
      int i = 0;
      int counter = 0;
      while (i != 1) {
        int in = getch();

        switch (in) {
          case 55:

          case 'y':
            y = y - 1;
            x = x - 1;
            display_teleporting(y, x, win);

            break;
          case 56:

          case 'k':
            y = y - 1;
            display_teleporting(y, x, win);
            break;
          case 57:

          case 'u':
            y = y - 1;
            x = x + 1;
            display_teleporting(y, x, win);
            break;

          case 54:

          case 'l':
            x = x + 1;
            display_teleporting(y, x, win);
            break;
          case 51:

          case 'n':
            y = y + 1;
            x = x + 1;
            display_teleporting(y, x, win);
            break;

          case 50:

          case 'j':
            y = y + 1;
            display_teleporting(y, x, win);
            break;
          case 49:

          case 'b':
            y = y + 1;
            x = x - 1;
            display_teleporting(y, x, win);
            break;
          case 52:
          case 'h':
            x = x - 1;
            display_teleporting(y, x, win);

            break;

          case 'r':
            teleport(y, x, p, 1);
            return 3;
            break;

          case 't':
            teleport(y, x, p, 0);
            return 3;
            break;
          case 27:
            return 3;
            break;
          default:
            break;
        }
      }
    } break;

    case 3: {
      uint8_t y = p->y;
      uint8_t x = p->x;

      display_teleporting(y, x, win);
      wrefresh(win);
      int i = 0;
      int counter = 0;
      while (i != 1) {
        int in = getch();
        wmove(win, 24, 0);
        wclrtobot(win);

        switch (in) {
          case 55:

          case 'y':
            y = y - 1;
            x = x - 1;
            display_teleporting(y, x, win);

            break;
          case 56:

          case 'k':
            y = y - 1;
            display_teleporting(y, x, win);
            break;
          case 57:

          case 'u':
            y = y - 1;
            x = x + 1;
            display_teleporting(y, x, win);
            break;

          case 54:

          case 'l':
            x = x + 1;
            display_teleporting(y, x, win);
            break;
          case 51:

          case 'n':
            y = y + 1;
            x = x + 1;
            display_teleporting(y, x, win);
            break;

          case 50:

          case 'j':
            y = y + 1;
            display_teleporting(y, x, win);
            break;
          case 49:

          case 'b':
            y = y + 1;
            x = x - 1;
            display_teleporting(y, x, win);
            break;
          case 52:
          case 'h':
            x = x - 1;
            display_teleporting(y, x, win);

            break;

          case 27:
            return 3;
            break;

          case 't':
            look_at_monster(win, p, x, y, nummon);
            break;
          default:
            break;
        }
      }
    } break;

    case 4: {
      uint8_t y = p->y;
      uint8_t x = p->x;

      display_teleporting(y, x, win);
      wrefresh(win);
      int i = 0;
      int counter = 0;
      while (i != 1) {
        int in = getch();
        wmove(win, 24, 0);
        wclrtobot(win);

        switch (in) {
          case 55:

          case 'y':
            y = y - 1;
            x = x - 1;
            display_teleporting(y, x, win);

            break;
          case 56:

          case 'k':
            y = y - 1;
            display_teleporting(y, x, win);
            break;
          case 57:

          case 'u':
            y = y - 1;
            x = x + 1;
            display_teleporting(y, x, win);
            break;

          case 54:

          case 'l':
            x = x + 1;
            display_teleporting(y, x, win);
            break;
          case 51:

          case 'n':
            y = y + 1;
            x = x + 1;
            display_teleporting(y, x, win);
            break;

          case 50:

          case 'j':
            y = y + 1;
            display_teleporting(y, x, win);
            break;
          case 49:

          case 'b':
            y = y + 1;
            x = x - 1;
            display_teleporting(y, x, win);
            break;
          case 52:
          case 'h':
            x = x - 1;
            display_teleporting(y, x, win);

            break;

          case 27:
            return 3;
            break;

          case 'c':
            return cast_spell(win, x, y, dungeon, hardness, objects, character,
                              nummon);
            break;
          default:
            break;
        }
      }
    } break;
  }

  if (complete == 1 || complete == 2) {
    return complete;
  }

  else {
    return 0;
  }
}

int main(int argc, char *argv[]) {
  char *home = getenv("HOME");
  char *path;
  path = (char *)malloc(strlen(home) + strlen("/.rlg327/dungeon") + 1);
  strcpy(path, home);
  strcat(path, "/.rlg327/dungeon");
  FILE *f;

  action_t action;

  if (argc == 1) {
    action = none;
    nummon = DEFAULT_MONSTER;
  }

  else if (argc > 5) {
    fprintf(stderr, "too many arguments\n");

  } else {
    int l;
    int save = 0;
    int load = 0;
    int num = -1;

    for (l = 1; l < argc; l++) {
      if (!strcmp(argv[l], "--load")) {
        load = 1;
      } else if (!strcmp(argv[l], "--save")) {
        save = 1;
      } else if (!strcmp(argv[l], "--nummon")) {
        num = l + 1;
      } else if (num == l) {
        nummon = (uint8_t)strtol(argv[l], NULL, 10);
      } else {
        fprintf(stderr, "Invalid argument\n");
        return -1;
      }
    }

    if (save == 1 && load == 1) {
      action = both;
    } else if (save == 1) {
      action = save_dungeon;
    } else if (load == 1) {
      action = load_dungeon;
    } else {
      action = none;
    }

    if (num == -1 || num == argc) {
      nummon = DEFAULT_MONSTER;
    }
  }

  switch (action) {
    case save_dungeon:

      if (!(f = fopen(path, "w"))) {
        std::cout << path;
        fprintf(stderr, "failed to open file\n");
        return -3;
      }

      else {
        make_dungeon();
        save_dung(f);
      }
      free(path);

      fclose(f);

      break;

    case both:

      if (!(f = fopen(path, "rw"))) {
        fprintf(stderr, "failed to open file\n");
        return -2;
      } else {
        load_dung(f, 0);
        //	display_dungeon();
        save_dung(f);
      }
      fclose(f);
      free(path);

      break;

    case load_dungeon:

      if (!(f = fopen(path, "r"))) {
        fprintf(stderr, "failed to open file\n");
        return -3;
      } else {
        load_dung(f, 1);

        //	recreate_dungeon();
        //	display_dungeon();
      }

      fclose(f);
      free(path);
      break;

    case none:

      make_dungeon();

      free(path);
      break;
  }
  init_spellbooks();
  parse_monsters();
  parse_objects();
  generate_objects(hardness, objects);
  generate_monsters(dungeon, character, nummon, pc.x, pc.y, NULL);

  dijkstra_non_tunneling(dungeon, hardness, pc.x, pc.y, non_tunneling);
  dijkstra_tunneling(dungeon, hardness, pc.x, pc.y, tunneling);

  int game_over = 0;
  heap_t h;

  character_t *p;
  heap_init(&h, monster_cmp, NULL);

  int i;

  int turn = 0;

  heap_insert(&h, &pc);

  for (i = 0; i < nummon; i++) {
    heap_insert(&h, &ch[i]);
  }

  WINDOW *win = newwin(21, 80, 1, 0);
  WINDOW *top = newwin(1, 80, 0, 0);
  win = initscr();
  top = initscr();
  keypad(win, true);
  scrollok(win, true);
  curs_set(0);
  cbreak();
  noecho();
  start_color();
  init_pair(COLOR_CYAN, COLOR_CYAN, COLOR_BLACK);
  init_pair(COLOR_BLACK, COLOR_WHITE, COLOR_BLACK);
  init_pair(COLOR_RED, COLOR_RED, COLOR_BLACK);
  init_pair(COLOR_GREEN, COLOR_GREEN, COLOR_BLACK);
  init_pair(COLOR_YELLOW, COLOR_YELLOW, COLOR_BLACK);
  init_pair(COLOR_MAGENTA, COLOR_MAGENTA, COLOR_BLACK);
  init_pair(COLOR_WHITE, COLOR_WHITE, COLOR_BLACK);
  init_pair(COLOR_BLUE, COLOR_BLUE, COLOR_BLACK);

  update_fog();
  display_fog();

  while (game_over != 1) {
    p = (character_t *)heap_remove_min(&h);

    int turn_time = 1000;
    if (p->alive == 1 && p->is_pc == 0) {
      npc_t *tmp = (npc_t *)p;
      turn_time = 3000;
      move_character(tmp, dungeon, character, hardness, objects);
    }

    p->turn = p->turn + (1000 / p->speed);

    if (p->turn - p->last_turn >= turn_time) {
      double regeneration = ceil(0.5 * (((p->max_hp / 400) * 0.85) + 0.15));
      p->last_turn = p->turn;
      p->hp = p->hp + (int32_t)regeneration;

      if (p->hp > p->max_hp) {
        p->hp = p->max_hp;
      }
    }

    if (p->is_pc == 1 && p->alive == 1) {
      player_t *tmp = (player_t *)p;

      if (tmp->turn - tmp->last_mana >= 750) {
        double regeneration = ceil(0.5 * (((tmp->max_mana / 7) * 0.85) + 0.15));
        tmp->last_mana = tmp->turn;
        tmp->mana = tmp->mana + (int)regeneration;

        if (tmp->mana > tmp->max_mana) {
          tmp->mana = tmp->max_mana;
        }
      }
      int f_key = 0;
      int complete = 0;
      wrefresh(win);

      while (complete != 1) {
        int c = getch();
        complete = eval_input(tmp, c, &h, 0, win);
        if (complete == 0) {
          wmove(top, 0, 0);
          wclrtoeol(top);
          mvwprintw(top, 0, 0, "Enter a valid character");
          wrefresh(top);
        } else if (complete == -1) {
          endwin();
          printf("Game ended.\n");
          exit(0);
        }

        else if (complete == 2) {
          display_dungeon();
          wrefresh(win);
          usleep(2000000);
          endwin();
          std::cout << "You won!" << std::endl;
          exit(0);

        }

        else if (complete == 3) {
          display_fog();
          wrefresh(win);
        }

        else if (complete == 4) {
          if (f_key == 0) {
            display_dungeon();
            wrefresh(win);
            f_key = 1;
          } else {
            display_fog();
            wrefresh(win);
            f_key = 0;
          }
        }

        else {
          if (complete == 5) {
            complete = 1;
          }
          wmove(top, 0, 0);
          wclrtoeol(top);
          wrefresh(top);
        }
      }

      update_fog();
      display_fog();
      //	wrefresh(win);
    }
    if (p->alive == 1) {
      heap_insert(&h, p);
    }
    if (p->is_pc == 1 && p->alive == 0) {
      display_dungeon();
      game_over = 1;

      mvprintw(22, 0, "You died");
      wrefresh(win);
      usleep(2000000);
    }
  }

  endwin();
  return 0;
}
