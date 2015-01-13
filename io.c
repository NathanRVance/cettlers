#include <curses.h>
#include <stdlib.h>
#include "settlers.h"
#include "color.h"

void io_printdat(int printsdat);

int io_getkey()
{
int c;
 switch(c = getch()) {
 case KEY_LEFT:
  return LEFT;
  break;
 case KEY_RIGHT:
  return RIGHT;
  break;
 case KEY_UP:
  return UP;
  break;
 case KEY_DOWN:
  return DOWN;
  break;
 case KEY_ENTER:
 case '\n':
 case '\r':
  return ENTER;
  break;
 default:
  return c;
  break;
 }
}

void io_init()
{
 initscr();
 clear();
 noecho();
 cbreak();
 curs_set(0);
 keypad(stdscr, TRUE);
 start_color();
 use_default_colors();

//This may seem slightly conveluted, but it's the curses way.
 init_pair(31, COLOR_RED, -1);
 init_pair(32, COLOR_GREEN, -1);
 init_pair(33, COLOR_YELLOW, -1);
 init_pair(34, COLOR_BLUE, -1);
 init_pair(35, COLOR_MAGENTA, -1);
 init_pair(36, COLOR_CYAN, -1);
 init_pair(37, COLOR_WHITE, -1);
 if(can_change_color() && 0) { //disabled
  init_color(COLOR_BROWN, 38, 5, 94);
  init_pair(38, COLOR_BROWN, -1);
 }
 else {
  init_pair(38, COLOR_YELLOW, -1);
  init_pair(33, COLOR_MAGENTA, -1);
 }
}

static void print(int rows, int cols, char m[rows][cols])
{
 int i, j, c, num;
 for(i = 0; i < rows; i++, addch('\n'))
  for(j = 0; (c = m[i][j]) != '\0'; j++) {
   if(c != ROAD[0] && c != VERTEX[0] && c != ROBBER[0])
    if(c == '\x1B' && m[i][++j] == '[') {
     num = 0;
     while((c = m[i][++j]) != 'm' && c != ';') {
      num *= 10;
      num += m[i][j] - '0';
     }
     if(c == ';')
      while(m[i][++j] != 'm');
     if(num == 0)
      attrset(A_NORMAL);
     else if(num == 5)
      attron(A_BLINK | A_REVERSE);
     else
      attron(COLOR_PAIR(num));
    } 
    else {
     if(c == '*') {
      attr_t at;
      short pair;
      attr_get(&at, &pair, NULL);
      attrset(A_NORMAL);
      addch('*');
      attr_set(at, pair, NULL);
     } else
        addch(c);
    }
  }
}

void io_printmap(int printsdat)
{
 clear();
 print(MAPROWS, MAPCOLS, map);
 print(1, DATCOLS, dat);
 io_printdat(printsdat);
 refresh();
}

void io_printhelp()
{
 int x = 14;
 int y = 4;
 mvprintw(y++, x, "##########################");
 mvprintw(y++, x, "#    C-Settlers Help:    #");
 mvprintw(y++, x, "# Keys:                  #");
 mvprintw(y++, x, "#  Arrows move cursor    #");
 mvprintw(y++, x, "#  b - build settlement  #");
 mvprintw(y++, x, "#    - builds city if    #");
 mvprintw(y++, x, "#      settlement exists #");
 mvprintw(y++, x, "#  r - build a road      #");
 mvprintw(y++, x, "#  t - trade             #");
 mvprintw(y++, x, "#  c - access devel card #");
 mvprintw(y++, x, "#      window            #");
 mvprintw(y++, x, "#  p - pass turn         #");
 mvprintw(y++, x, "#  q - exit              #");
 mvprintw(y++, x, "#  h - display this      #");
 mvprintw(y++, x, "#      message           #");
 mvprintw(y++, x, "##########################");
}

void io_printcards(int c, int s, int r, int m, int p, int v, int pos)
{
 int x = 0;
 int y = 10;
 mvprintw(y++, x, "###########################################################");
 mvprintw(y++, x, "#  Buy   Soldier     Road    Monopoly   Year of   Victory #");
 mvprintw(y++, x, "# Card             Building             Plenty     Point  #");
 mvprintw(y++, x, "#  %2d      %2d        %2d         %2d        %2d        %2d    #", c, s, r, m, p, v);
 mvprintw(y++, x, "#   %c       %c         %c          %c         %c         %c    #", (pos == 0)? '*' : ' ', (pos == 1)? '*' : ' ', (pos == 2)? '*' : ' ', (pos == 3)? '*' : ' ', (pos == 4)? '*' : ' ', (pos == 5)? '*' : ' ');
 mvprintw(y++, x, "###########################################################");
}

void io_printtrade()
{
 mvprintw(27, 0, "          %3d %c  %3d %c   %3d %c   %3d %c   %3d %c   %s %c", trade[0], (trade[5] == 0)? '*' : ' ', trade[1], (trade[5] == 1)? '*' : ' ', trade[2], (trade[5] == 2)? '*' : ' ', trade[3], (trade[5] == 3)? '*' : ' ', trade[4], (trade[5] == 4)? '*' : ' ', &trade[6], (trade[5] == 5)? '*' : ' ');
}

void io_printdat(int printsdat)
{
 if(printsdat)
  printw("          %3d    %3d     %3d     %3d     %3d\n", dat[1][0], dat[1][1], dat[1][2], dat[1][3], dat[1][4]);
 int y = 1;
 int x = 54;
 int i;
 for(i = 0; i < 4; i++) {
  attron(COLOR_PAIR(IOP1 + i));
  mvprintw(y++, x, "Player %d:", i+1);
  attrset(A_NORMAL);
  mvprintw(y++, x, " Resources: %d", dat[2][7*i]);
  mvprintw(y++, x, " Devel cards: %d", dat[2][7*i+1]);
  mvprintw(y++, x, " Knights: ");
  if(dat[2][7*i+2]) attron(COLOR_PAIR(31));
  printw("%d", dat[2][7*i+3]);
  attrset(A_NORMAL);
  mvprintw(y++, x, " Road: ");
  if(dat[2][7*i+4]) attron(COLOR_PAIR(31));
  printw("%d", dat[2][7*i+5]);
  attrset(A_NORMAL);
  mvprintw(y++, x, " VP's: %d", dat[2][7*i+6]);
//  y++;
 }
}

#include <string.h>

void io_selectTrade(int accepted[3], int cursor, int player)
{
 player++;
 char status[3][10];
 strcpy(status[0], "Declined");
 strcpy(status[1], "Accepted");
 strcpy(status[2], "Counter ");
 int y = 10;
 int x = 10;
 mvprintw(y++, x, "##################################");
 mvprintw(y++, x, "# Player %d   Player %d   Player %d #", (player == 1)? 2 : 1, (player <= 2)? 3 : 2, (player <= 3)? 4 : 3);
 mvprintw(y++, x, "# %s   %s   %s #", status[accepted[0]], status[accepted[1]], status[accepted[2]]);
 mvprintw(y++, x, "#    %c          %c          %c     #", (cursor == 0)? '*' : ' ', (cursor == 1)? '*' : ' ', (cursor == 2)? '*' : ' ');
 mvprintw(y++, x, "##################################");
}

void io_printwin(int player, int cards)
{
 mvprintw(11, 10, "#################################");
 mvprintw(12, 10, "# Player %d Won with %d VP card%s #", player+1, cards, (cards == 1)? "! " : "s!");
 mvprintw(13, 10, "#      Press 'q' to exit.       #");
 mvprintw(14, 10, "#################################");
}

void io_printsetup(int cursor, int *set)
{
 clear();
 int y = 0;
 int x = 0;
 mvprintw(y++, x, "###########################################");
 mvprintw(y++, x, "#  Cettlers pre-game config               #");
 mvprintw(y++, x, "#  %c  Player 1: %s                     #", (cursor == 0)? '*' : ' ', (set[0] == 0)? "human" : "ai   ");
 mvprintw(y++, x, "#  %c  Player 2: %s                     #", (cursor == 1)? '*' : ' ', (set[1] == 0)? "human" : "ai   ");
 mvprintw(y++, x, "#  %c  Player 3: %s                     #", (cursor == 2)? '*' : ' ', (set[2] == 0)? "human" : "ai   ");
 mvprintw(y++, x, "#  %c  Player 4: %s                     #", (cursor == 3)? '*' : ' ', (set[3] == 0)? "human" : "ai   ");
 mvprintw(y++, x, "#  %c  Map number distribution: %s  #", (cursor == 4)? '*' : ' ', (set[4] == 0)? "patterned" : "random   ");
 mvprintw(y++, x, "#                                         #");
 mvprintw(y++, x, "#    Use arrow keys to change settings    #");
 mvprintw(y++, x, "#        Press enter to begin game        #");
 mvprintw(y++, x, "###########################################");
}
