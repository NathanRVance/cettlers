#include "settlers.h"

int io_getkey(void);
void io_printsetup(int cursor, int *set);
void data_setai(int p, int set);
void data_setsleeptime(int ds);
void setanimate(int set);

int apply_settings(int *set)
{
 int i;
 for(i = 0; i < 4; i++) data_setai(i, set[i]);
 data_setsleeptime(set[5]);
 setanimate(set[6]);
 return set[4];
}

void change(int *set, int index, int c)
{
 if(index != 5) {
  set[index] = ! set[index];
 }
 else {
  if(c == RIGHT) {
   if(set[index] < 99) set[index]++;
  } else if(set[index] > 0) set[index]--;
 }
}

int setup_routine()
{
 int go = 1;
 static int set[7];
 int i, c;
 for(i = 1; i < 4; i++) set[i] = 1;
 set[5] = 10;
 set[6] = 0;
 int cursor = 0;
 while(go) {
  io_printsetup(cursor, set);
  switch(c = io_getkey()) {
   case LEFT: 
   case RIGHT:
    change(set, cursor, c);
    break;
   case UP: 
    cursor--;
    if(cursor < 0) cursor = 6;
    break;
   case DOWN: cursor = (cursor+1)%7;
    break;
   case ENTER: go = 0;
    break;
   default:
    break;
  }
 }
 return apply_settings(set);
}
