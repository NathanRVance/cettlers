#include "settlers.h"

int io_getkey(void);
void io_printsetup(int cursor, int *set);
void data_setai(int p, int set);

int apply_settings(int *set)
{
 int i;
 for(i = 0; i < 4; i++) data_setai(i, set[i]);
 return set[4];
}

int setup_routine()
{
 int go = 1;
 static int set[5];
 int i;
 for(i = 1; i < 4; i++) set[i] = 1;
 int cursor = 0;
 while(go) {
  io_printsetup(cursor, set);
  switch(io_getkey()) {
   case LEFT: 
   case RIGHT:
    set[cursor] = ! set[cursor];
    break;
   case UP: 
    cursor--;
    if(cursor < 0) cursor = 4;
    break;
   case DOWN: cursor = (cursor+1)%5;
    break;
   case ENTER: go = 0;
    break;
   default:
    break;
  }
 }
 return apply_settings(set);
}
