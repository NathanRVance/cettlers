#include "color.h"
#include "settlers.h"
#include <stdlib.h>
#include <unistd.h>

void io_printmap(void);
int io_getkey(void);
void io_init(void);
void io_printhelp(void);
void main_routine(void);
void pregame_routine(void);
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
void devcards_routine(int player);
void ai_pregame(int player);
void data_pregameres(int p, int vert);
int data_isai(int p);
void ai_routine(int player);
int setup_routine(void);
void map_create(int randomnums);
char* cat(char a[], char b[]);
char* itoa(int i);

main()
{
 data_init();
 trade_down(0);
 io_init();
 map_create(setup_routine());
 devcards_init();
 io_printmap();
 pregame_routine();
 roll(0);
 main_routine();
}

void halt()
{
 endwin();
 exit(0);
}

void quit()
{
 map_setmessage("Are you sure you want to quit? (y/n)");
 io_printmap();
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
  io_printmap();
  sleep(1);
  go = 0;
 }
 while(go) {
  data_refresh(player);
  io_printmap();
  switch(c = io_getkey()) {
   case LEFT:
   case RIGHT:
   case UP:
   case DOWN: marker_move(c);
    break;
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
   case 'q': quit();
    break;
   case 'a':
    ai_pregame(player);
    go = 0;
    break;
   default: map_setmessage("Press 'b' to build");
    break;
  }
 }
 if(res) data_pregameres(player, marker_getposition()); 
}

void pregame_routine()
{
 marker_move(RIGHT);
 int c, player;
 for(player = 0; player <= 3; player++) pregame(player, 0);
 for(player = 3; player >= 0; player--) pregame(player, 1);
}

void main_routine()
{
 int c;
 int player = 0;
 while(1) {
  data_refresh(player);
  io_printmap();
  map_setmessage("");
  if(data_isai(player)) {
   ai_routine(player);
   player = (player+1)%4;
   //sleep(1);
   roll(player);
   //sleep(1);
   continue;
  }
  switch(c = io_getkey()) {
   case LEFT:
   case RIGHT:
   case UP:
   case DOWN: marker_move(c);
    break;
//   case 'a': ai_routine(player);
   case 'p': player = (player+1)%4;
             roll(player);
    break;
   case 'b': marker_show();
             build_routine(player, 0);
    break;
   case 'x': data_cheat(player);
    break;
   case 'c': devcards_routine(player);
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
   case 'q': quit();
    break;
   default: map_setmessage("Undefined key, press 'h' for help");
    break;
  }
 }
}
