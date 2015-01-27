#include "settlers.h"

void io_print(int x, int y, char* s, int color);
int map_getrowhex(int hex); //*2+5 for map index
int map_getcolhex(int hex); //*6+12 for map index
int millasleep(int ms);
void io_printmap(int printsdat);
int abs(int a);

static int doesanimate = 0;

void setanimate(int set)
{
 doesanimate = set;
}

void animate(int resource, int hex, int player)
{
 if(! doesanimate) return;
 int color;
 switch(resource) {
  case WHEAT: color = 35;
   break;
  case WOOD: color = 33;
   break;
  case STONE: color = 0;
   break;
  case SHEEP: color = 32;
   break;
  case BRICK: color = 31;
   break;
  default: color = 0;
   break;
 }

 int x1 = map_getcolhex(hex)*6+12;
 int y1 = map_getrowhex(hex)*2+5;
 int x2 = 54;
 int y2 = player*6+1;
 while(x1 != x2 || y1 != y2) {
  int rise = y2-y1;
  int run = x2-x1;
  if(abs(run) > 5) {
   rise /= (run / 2);
   run /= (run / 2);
  }
  while(rise != 0 || run != 0) {
   if(rise > 0) rise--, y1++;
   if(rise < 0) rise++, y1--;
   if(run > 0) run--, x1++;
   if(run < 0) run++, x1--;
  }
  io_print(x1, y1, "#", color);
  millasleep(50);
  io_printmap(0);
 }
}
