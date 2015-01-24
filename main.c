#include "color.h"
#include "settlers.h"
#include <stdlib.h>

void io_printmap(int printsdat);
int io_getkey(void);
void io_init(void);
void io_printhelp(void);
void main_routine(int p);
int pregame_routine(void);
void map_setmessage(char s[]);
void devcards_init(void);
void data_init(void);
void data_refresh(int p);
void data_cheat(int p);
int build_routine(int player, int ispregame);
void trade_routine(int player, int credits, int type);
void marker_hide(void);
void marker_show(void);
int marker_getposition(void);
void road_routine(int player, int pregame);
void robber_routine(int player);
void roll(int player);
int devcards_routine(int player, int hascarded);
void ai_pregame(int player);
void data_pregameres(int p, int vert);
int data_isai(int p);
void ai_routine(int player, int hascarded);
int setup_routine(void);
void map_create(int randomnums);
char* cat(char a[], char b[]);
char* itoa(int i);
int devcards_knightoption(int player);
void io_printgeneric(char *s);
void pass_between_humans(int to);
void devcards_newturn(void);
int data_sleep(void);

main()
{
 ROLLED = 0;
 data_init();
 trade_down(0);
 io_init();
 map_create(setup_routine());
 devcards_init();
 io_printmap(0);
 int p = pregame_routine();
 roll(p);
 main_routine(p);
}

void halt()
{
 endwin();
 exit(0);
}

void quit(int printres)
{
 map_setmessage("Are you sure you want to quit? (y/n)");
 io_printmap(printres);
 if(io_getkey() == 'y') {
  halt();
 }
 map_setmessage("");
}

void pregame(int player, int res)
{
 int c;
 int go = 1;
 map_setmessage(cat(cat("Player ", itoa(player+1)), " pregame, please build a settlement."));
 if(data_isai(player)) {
  ai_pregame(player);
  data_refresh(player);
  io_printmap(0);
  data_sleep();
  go = 0;
 }
 while(go) {
  data_refresh(player);
  io_printmap(0);
  switch(c = io_getkey()) {
   case LEFT:
   case RIGHT:
   case UP:
   case DOWN: marker_move(c);
    break;
   case ENTER:
   case 'b':
    marker_show();
    if(! build_routine(player, 1)) break;
    road_routine(player, 1);
    go = 0;
    break;
   case 'h':
    io_printhelp();
    io_getkey();
    break;
   case 'q': quit(0);
    break;
   case 'a':
    ai_pregame(player);
    go = 0;
    break;
   case 'u':
    pass_between_humans(1);
    break;
   default: map_setmessage("Press 'b' to build");
    break;
  }
 }
 if(res) data_pregameres(player, marker_getposition()); 
}

int pregame_routine()
{
 marker_move(RIGHT);
 int c, player, p;
 p = rand() % 4; //player that starts
 for(player = 0; player < 4; player++) pregame((player+p)%4, 0);
 for(player = 3; player >= 0; player--) pregame((player+p)%4, 1);
 map_setmessage("");
 return p;
}

void main_routine(int p)
{
 int c;
 int hascarded;
 int player = p;
 while(1) {
  data_refresh(player);
  io_printmap(! data_isai(player));
  map_setmessage("");
  if(data_isai(player)) {
   ai_routine(player, hascarded);
   player = (player+1)%4;
   data_refresh(player);
   devcards_newturn();
   hascarded = devcards_knightoption(player);
   roll(player);
   continue;
  }
  switch(c = io_getkey()) {
   case LEFT:
   case RIGHT:
   case UP:
   case DOWN: marker_move(c);
    break;
//   case 'a': ai_routine(player, 0);
   case 'p': player = (player+1)%4;
             if(! data_isai(player))
              pass_between_humans(player);
             data_refresh(player);
             devcards_newturn();
             hascarded = devcards_knightoption(player);
             roll(player);
    break;
   case 'b': marker_show();
             build_routine(player, 0);
    break;
   case 'c': hascarded += devcards_routine(player, hascarded);
    break;
   case 't': marker_hide();
             trade_routine(player, 0, 0);
    break;
   case 'r': marker_show();
             road_routine(player, 0);
    break;
   case 'h': io_printhelp();
             io_getkey();
    break;
   case 'q': quit(1);
    break;
   default: map_setmessage("Undefined key, press 'h' for help");
    break;
  }
 }
}

void pass_between_humans(int to)
{
 io_printmap(0);
 io_printgeneric(cat(cat("Please pass control to Player ", itoa(to+1)), ".\n    Press any key to continue."));
 io_getkey();
}
