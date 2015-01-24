#include "settlers.h"
#include <unistd.h>

int data_getresource(int p, int res);
int builder_placesettlement(int pos, int player, int isPregame);
int builder_placecity(int pos, int player);
void devcards_buy(int player);
int devcards_remaining(void);
int data_elementsplaced(int p, int item);
int* data_getsettlements(int p);
int ai_vertweight(int player, int vert);
void marker_setposition(int pos);
int data_poslegal(int pos);
int data_atvertex(int player, int position);
int* ai_surroundingverts(int vert);
int road_freeedge(int pos1, int pos2);
int data_getcards(int p, int card);
void data_addcards(int p, int card, int num);
int map_getrobberhex(void);
int ai_trade(int player);
int* ai_rateres(int player);
void map_setmessage(char s[]);
void devcards_knight(int player);
char* cat(char a[], char b[]);
char* itoa(int i);
void io_printmap(int printsdat);
int* ai_roadfromvert(int player, int vert);
char* itoa(int i);
int ai_playcard(int player);
int data_sleep();

int ai_shouldsettle(int player, int vert)
{
 int *surrounding = ai_surroundingverts(vert);
 int i;
 for(i = 0; i < 3; i++) {
  if(data_poslegal(surrounding[i]) && ai_vertweight(player, surrounding[i]) > ai_vertweight(player, vert) + 3) //3 is arbitrary, adjust as need be.
   return 0;
 }
 return 1;
}

int ai_cansettle(int player) //both can and should
{
 int i;
 for(i = 1; i < 64; i++) {
  if(data_elementsremaining(player, 1) && data_atvertex(player, i) == 0 && data_poslegal(i) && ai_shouldsettle(player, i))
   return i;
 }
 return 0;
}

int ai_settle(int player)
{
 if(data_getresource(player, WOOD) >= 1 && data_getresource(player, WHEAT) >= 1 && data_getresource(player, SHEEP) >= 1 && data_getresource(player, BRICK) >= 1) {
  int vert = ai_cansettle(player);
  if(vert != 0) {
   marker_setposition(vert);
   return builder_placesettlement(vert, player, 0);
  }
 }
 return 0;
}

int ai_canupgrade(int player)
{
 if(data_elementsremaining(player, 2) >= 1 && data_elementsplaced(player, 1) >= 1) return 1;
 return 0;
}

int ai_city(int player)
{
 if(data_getresource(player, STONE) < 3 || data_getresource(player, WHEAT) < 2 || ! ai_canupgrade(player)) {
   return 0;
 }
 int *settlements = data_getsettlements(player);
 int i, max, vert;
 max = vert = -100;
 for(i = 0; settlements[i] != 0; i++) {
  if(ai_vertweight(player, settlements[i]) > max) {
   max = ai_vertweight(player, settlements[i]);
   vert = settlements[i];
  }
 }
 marker_setposition(vert);
 builder_placecity(vert, player);
 return 1;
}

int ai_card(int player)
{
 if(data_getresource(player, STONE) < 1 || data_getresource(player, WHEAT) < 1 || data_getresource(player, SHEEP) < 1 || devcards_remaining() == 0) {
  return 0;
 }
 if((data_getresource(player, STONE) >= 3 || (data_getresource(player, STONE) == 2 && data_getresource(player, WHEAT) == 2)) && ai_canupgrade(player)) {
  return 0; //save up for a city
 }
 devcards_buy(player);
 map_setmessage(cat(cat("Player ", itoa(player+1)), " purchased a dev card"));
 return 1;
}

int* ai_canroad(int player, int free)
{
 static int road[2];
 road[0] = road[1] = 0;
 if(! free) {
  //has resources
  if(data_getresource(player, WOOD) < 1 || data_getresource(player, BRICK) < 1 || data_elementsremaining(player, 3) == 0) {
   return road;
  }
  //should use these resources rather than save for settlement
  if((data_getresource(player, SHEEP) >= 1 || data_getresource(player, WHEAT) >= 1) && (data_getresource(player, WOOD) == 1 || data_getresource(player, BRICK) == 1) && ai_cansettle(player)) {
   return road;
  }
 }
 //somewhere to build to
 int i, j, *surrounding, maxi;
 int weight = 0;
 for(i = 1; i < 64; i++) {
  if(road_legalpos(player, i)) {
   if(ai_roadfromvert(player, i)[1] > weight) {
    weight = ai_roadfromvert(player, i)[1];
    maxi = i;
   }
  }
 }
 if(weight < 1 && ! free) return road;
 road[0] = maxi;
 road[1] = ai_roadfromvert(player, maxi)[0];
 return road;
}

int ai_road(int player, int free)
{
 int *road = ai_canroad(player, free);
 if(! road[0]) {
  return 0;
 }
 road_vertbuild(player, road[0], road[1], free);
 if(! free) map_setmessage(cat(cat("Player ", itoa(player+1)), " built a road"));
// if(! free) map_setmessage(cat(cat(cat(cat(cat("Player ", itoa(player+1)), " built a road from "), itoa(road[0])), " to "), itoa(road[1])));
 return 1;
}

void ai_routine(int player, int hascarded)
{
 int didsomething = 1;
 if(! hascarded) hascarded = ai_playcard(player);
 while(didsomething) {
  io_printmap(0);
  data_sleep();
  if(ai_settle(player) || ai_city(player) || ai_road(player, 0))
   didsomething = 1;
  else
   if(! ai_card(player) && ! ai_trade(player))
    didsomething = 0;
 }
}
