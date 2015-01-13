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
 devcards_buy(player);
 return 1;
}

int ai_knighton(int player)
{
 return data_onhex(player, map_getrobberhex());
}

int ai_road(int player, int free);
int* ai_canroad(int player, int free);
void devcards_monopoly(int player);
int rate_hand(int *hand, int player);
int* gethand(int player);
char* map_getresname(int res);
char* itoa(int i);

int ai_playcard(int player)
{
 if(data_getcards(player, KNIGHT) >= 1 && ai_knighton(player))
 {
  devcards_knight(player);
 } else if(data_getcards(player, BUILDROAD) && ai_canroad(player, 1)) {
  map_setmessage(cat(cat("Player ", itoa(player+1)), " played road building"));
  data_addcards(player, BUILDROAD, -1);
  ai_road(player, 1);
  ai_road(player, 1);
 } else if(data_getcards(player, MONOPOLY)) {
  data_addcards(player, MONOPOLY, -1);
  int *res = ai_rateres(player);
  int i, max, maxi;
  max = maxi = 0;
  for(i = 0; i < 5; i++) {
   if(res[i] > max) {
    max = res[i];
    maxi = i;
   }
  }
  map_setmessage(cat(cat(cat("Player ", itoa(player+1)), " monopolized "), map_getresname(maxi)));
  devcards_domonopoly(player, maxi);
 } else if(data_getcards(player, YEAROPLENTY)) {
  data_addcards(player, YEAROPLENTY, -1);
  int *hand = gethand(player);
  int rating = rate_hand(hand, player);
  int i, j, maxi, maxj;
  for(i = 0; i < 5; i++) {
   hand[i]++;
   for(j = 0; j < 5; j++) {
    hand[j]++;
    if(rate_hand(hand, player) > rating) {
     rating = rate_hand(hand, player);
     maxi = i;
     maxj = j;
    }
    hand[j]--;
   }
   hand[i]--;
  }
  data_addresource(player, maxi, 1);
  data_addresource(player, maxj, 1);
  map_setmessage(cat(cat("Player ", itoa(player+1)), " had a year of plenty"));
 }
 return 0;
}

int* ai_canroad(int player, int free)
{
 static int road[2];
 road[0] = road[1] = 0;
 //has resources
 if(! free && (data_getresource(player, WOOD) < 1 || data_getresource(player, BRICK) < 1 || data_elementsremaining(player, 3) == 0)) {
  return road;
 }
 //should use these resources rather than save for settlement
 if(! free && (data_getresource(player, SHEEP) >= 1 || data_getresource(player, WHEAT) >= 1) && (data_getresource(player, WOOD) == 1 || data_getresource(player, BRICK) == 1) && ai_cansettle(player)) {
  return road;
 }
 //somewhere to build to
 int i, j, *surrounding;
 int weight = 0;
 int primaryweight = 0;
 //start with spaces within one road
 for(i = 1; i < 64; i++) {
  if(! road_legalpos(player, i)) continue;
  surrounding = ai_surroundingverts(i);
  for(j = 0; j < 3; j++) {
   if(road_freeedge(surrounding[j], i) && data_poslegal(surrounding[j])) {
    if(weight < ai_vertweight(player, surrounding[j])) {
     weight = ai_vertweight(player, surrounding[j]);
     road[0] = i;
     road[1] = surrounding[j];
     primaryweight = 3; //arbitrary, this is the penalty against building two away
    }
   }
  }
 }
 //look 2 roads away
 int *surrounding2, k;
 for(i = 1; i < 64; i++) {
  if(! road_legalpos(player, i)) continue;
  surrounding = ai_surroundingverts(i);
  for(j = 0; j < 3; j++) {
   if(road_freeedge(surrounding[j], i)) {
    surrounding2 = ai_surroundingverts(surrounding[j]);
    for(k = 0; k < 3; k++) {
     if(road_freeedge(surrounding2[k], surrounding[j]) && data_poslegal(surrounding2[k])) {
      if(weight + primaryweight < ai_vertweight(player, surrounding2[k])) {
       weight = ai_vertweight(player, surrounding2[k]);
       road[0] = i;
       road[1] = surrounding[j];
       primaryweight = 0;
      }
     }
    }
   }
  }
 }
 return road;
}

int ai_road(int player, int free)
{
 int *road = ai_canroad(player, free);
 if(! road[0]) {
  return 0;
 }
 marker_setposition(road[0]);
 int dir;
 if(road[1] == road[0]-1) dir = 2; //left
 else if(road[1] == road[0]+1) dir = 3; //right
 else if(road[1] < road[0]) dir = 0; //up
 else if(road[1] > road[0]) dir = 1; //down
 int ret = road_prospect(player, dir);
 if(ret) road_build(player, free);
 return ret;
}

void ai_routine(int player)
{
 int didsomething = 1;
 ai_playcard(player);
 while(didsomething) {
  sleep(1);
  if(ai_settle(player) || ai_city(player) || ai_road(player, 0))
   didsomething = 1;
  else
   if(! ai_card(player) && ! ai_trade(player))
    didsomething = 0;
 }
}
