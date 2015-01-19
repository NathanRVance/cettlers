#include "settlers.h"
#include <string.h>

static int cards[DEVCARDS];
static int cardsp;
void devcards_distribute(int num, int card);
char* itoa(int i);
char* cat(char* a, char* b);
void io_printcards(int c, int s, int r, int m, int p, int v, int pos);
int io_getkey(void);
void data_addcards(int p, int card, int num);
int data_getcards(int p, int card);
int data_getresource(int p, int res);
void data_addresource(int p, int res, int num);
void data_playknight(int p);
void map_setmessage(char s[]);
void trade_routine(int player, int credits, int type);
int road_routine(int player, int pregame);

void devcards_init()
{
 int i;
 for(i = 0; i < DEVCARDS; i++)
  cards[i] = -1;
 devcards_distribute(14, KNIGHT);
 devcards_distribute(2, BUILDROAD);
 devcards_distribute(2, MONOPOLY);
 devcards_distribute(2, YEAROPLENTY);
 devcards_distribute(5, VP);
 cardsp = DEVCARDS-1;
 
 for(i = 0; i < CARDROWS; i++)
  strcpy(cardDisplay[i], "");
 strcpy(cardDisplay[0], " Buy   Soldier    Road     Manopoly   Year Of   Victory");
 strcpy(cardDisplay[1], "Card             Building             Pletnty    Point");
}

void devcards_distribute(int num, int card)
{ 
 /* 
 The algorithm to distribute cards is similar to the one for
 distributing map data. Initialize the cards array to all -1 and
 an integer k to the size of the array. Then choose random integers r
 between 0 and k, and find the rth -1 in cards and set it to that card.
 Decrement k and repeat.
 */
 int i, r, k;
 for(i = 0, k = 0; i < DEVCARDS; i++)
  if(cards[i] == -1) k++;
 int stop = k-num;
 while(k > stop) {
  r = rand() % k--;
  for(i = 0; r >= 0; i++)
   if(cards[i] == -1) r--;
  cards[i-1] = card;
 }
}

int devcards_draw()
{
 if(cardsp >= 0) return cards[cardsp--];
 else return -1;
}

int devcards_remaining()
{
 return cardsp+1;
}

void devcards_buy(int player)
{
 if(devcards_remaining() >= 1) {
  data_addresource(player, WHEAT, -1);
  data_addresource(player, SHEEP, -1);
  data_addresource(player, STONE, -1);
  data_addcards(player, devcards_draw(), 1);
 }
}

void devcards_refresh(int p, int pos)
{
 io_printcards(devcards_remaining(), data_getcards(p, KNIGHT), data_getcards(p, BUILDROAD), data_getcards(p, MONOPOLY), data_getcards(p, YEAROPLENTY), data_getcards(p, VP), pos);
}

int devcards_knight(int player)
{
 if(data_getcards(player, KNIGHT) >= 1) {
  map_setmessage(cat(cat("Player ", itoa(player+1)), " played a knight"));
  data_addcards(player, KNIGHT, -1);
  data_playknight(player);
  robber_routine(player);
  return 1;
 }
 return 0;
}

int devcards_buildroad(int player)
{
 if(data_getcards(player, BUILDROAD) >= 1) {
  data_addcards(player, BUILDROAD, -1);
  int i = 2;
  int c;
  while(i >= 1) {
   map_setmessage(cat(cat(cat("Build ", itoa(i)), " road"), (i == 2)? "s" : ""));
   data_refresh(player);
   io_printmap();
   switch(c = io_getkey()) {
    case LEFT:
    case RIGHT:
    case UP:
    case DOWN: marker_move(c);
     break;
    case ENTER:
    case 'r': marker_show();
              if(road_routine(player, 1)) i--;
     break;
    default:
     break;
   }
  }
  map_setmessage("");
  io_printmap();
  return 1;
 }
 return 0;
}

int devcards_monopoly(int player)
{
 if(data_getcards(player, MONOPOLY) >= 1) {
  data_addcards(player, MONOPOLY, -1);
  trade_routine(player, 1, 4);
  return 1;
 }
 return 0;
}

void devcards_domonopoly(int player, int res)
{
 int tot = 0;
 int i;
 for(i = 1; i < 4; i++) {
  tot += data_getresource((player+i)%4, res);
  data_addresource((player+i)%4, res, data_getresource((player+i)%4, res) * -1);
 }
 data_addresource(player, res, tot);
}

int devcards_YOP(int player)
{
 if(data_getcards(player, YEAROPLENTY) >= 1) {
  data_addcards(player, YEAROPLENTY, -1);
  trade_routine(player, 2, 3);
  return 1;
 }
 return 0;
}

int devcards_VP(int player)
{
 //do nothing
 return 0;
}

int devcards_handle(int player, int pos, int hascarded)
{
 if(pos > 0 && hascarded) {
  map_setmessage("Cannot play more than one card per turn");
  return 0;
 }
 switch(pos) {
  case 0:
   if(data_getresource(player, STONE) >= 1 && data_getresource(player, WHEAT >= 1) && data_getresource(player, SHEEP) >= 1)
    devcards_buy(player);
   else map_setmessage("Not enough resources, requires one stone, wheat and sheep.");
   break;
  case 1: return devcards_knight(player);
   break;
  case 2: return devcards_buildroad(player);
   break;
  case 3: return devcards_monopoly(player);
   break;
  case 4: return devcards_YOP(player);
   break;
  case 5: return devcards_VP(player);
   break;
  default:
   break;
 } 
}

int devcards_routine(int player, int hascarded)
{
 int pos = 0;
 while(1) {
  devcards_refresh(player, pos);
  switch(io_getkey()) {
   case LEFT:
    if(--pos < 0) pos = 5;
    break;
   case RIGHT: pos = ++pos % 6;
    break;
   case ENTER: return devcards_handle(player, pos, hascarded);
    break;
   case 'c': return 0;
    break;
   default:
    break;
  }
 }
}

void io_printgeneric(char *s);

int devcards_knightoption(int player)
{
 if(data_getcards(player, KNIGHT) == 0)
  return 0;
 if(data_isai(player)) {
  return ai_playknight(player);
 }
 io_printgeneric("Play knight card?\n      (y/n)");
 while(1) {
  switch(io_getkey()) {
   case 'y':
    devcards_knight(player);
    return 1;
   case 'n':
    return 0;
   default:
    break;
  }
 }
 return 0;
}
