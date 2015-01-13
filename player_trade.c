#include "settlers.h"

int ai_accepttrade(int *trade, int player);
void data_addresource(int p, int res, int num);
int data_getresource(int p, int res);
int data_isai(int p);
int* trade_routine(int player, int credits, int type, int *start);
void io_selectTrade(int accepted[3], int cursor, int player);

//+|- in trade[] is from the point of view of player2
void doTrade(int *trade, int player1, int player2)
{
 int i;
 for(i = 0; i < 5; i++) {
  data_addresource(player1, i, trade[i] * -1);
  data_addresource(player2, i, trade[i]);
 }
}

void trade_up(int credits, int type);
void setup(int *start, int player);
void data_refresh(int p);
void trade_refresh(void);
void io_printmap(void);
void io_printtrade(void);
void trade_down(int player);

int selectTrade(int accepted[4][6], int player)
{
 int cursor = 0;
 int i, j, accepted2[3];
 for(i = 0; i < 4; i++)
  for(j = 0; j < 5; j++)
   accepted[i][j] *= -1;
 for(i = 0; i < 4; i++)
  if(accepted[i][5] == 3) {
   for(j = 0; j < i; j++) accepted2[j] = accepted[j][5];
   for(j = i+1; j < 4; j++) accepted2[j-1] = accepted[j][5];
  }
 while(1) {
  if(accepted2[cursor] > 0) {
   int cursplayer;
   if(cursor >= player) cursplayer = cursor + 1;
   else cursplayer = cursor;
   trade_up(0, 0);
   setup(accepted[cursplayer], player);
   data_refresh(player);
   trade_refresh();
   io_printmap();
   io_printtrade();
  } else {
   trade_down(player);
   io_printmap();
  }
  io_selectTrade(accepted2, cursor, player);
  switch(io_getkey()) {
   case LEFT: cursor = (cursor+2)%3;
    break;
   case RIGHT: cursor = (cursor+1)%3;
    break;
   case ENTER:
   case 't':
    if(cursor >= player) cursor++;
    return cursor;
    break;
   case 'c':
    return -1;
   default:
    break;
  }
  trade_down(player);
 }
}

#define RETSIZE 100
static int retpool[RETSIZE];
static int *retp = retpool;

int* accepttrade(int *tr, int player, int playerTrading)
{
 int *ret = retp;
 int i;
 retp += 6;
 if(retp >= retpool + RETSIZE) retp = retpool;
 if(data_isai(player)) {
  if(ai_accepttrade(tr, player)) {
   ret[5] = 1;
   for(i = 0; i < 5; i++) ret[i] = tr[i];
   return ret;
  }
 } else {
  for(i = 0; i < 5; i++)
   if(data_getresource(player, i) + tr[i] < 0) {
    ret[5] = 0;
    return ret;
   }
  int *tr2 = trade_routine(player, 0, 5, tr);
  for(i = 0; i < 5; i++) ret[i] = tr2[i];
  int accept = 1;
  for(i = 0; i < 5; i++)
   if(tr[i] != tr2[i]) accept = 0;
  for(i = 0; i < 5; i++)
   if(data_getresource(player, i) + ret[i] < 0) {
    ret[5] = 0;
    return ret;
   }
  if(accept) {
   ret[5] = 1;
   return ret;
  }
  ret[5] = 2;
  return ret;
 }
 ret[5] = 0;
 return ret;
}

int player_trade(int player, int* tr)
{
 int accepted[4][6], i, j;
 accepted[player][5] = 3;
 for(i = 1; i < 4; i++) {
  int *pointer = accepttrade(tr, (player+i)%4, player);
  for(j = 0; j < 6; j++) accepted[(player+i)%4][j] = pointer[j];
 }
 int selected = selectTrade(accepted, player);
 if(selected > -1 && accepted[selected][5] > 0)
  doTrade(accepted[selected], selected, player);
 else map_setmessage("Trade cancelled");
 return 1;
}
