#include "settlers.h"
#include <unistd.h>

int ai_road(int player, int free);
int* ai_canroad(int player, int free);
void devcards_monopoly(int player);
int rate_hand(int *hand, int player);
int* gethand(int player);
char* map_getresname(int res);
char* itoa(int i);
int* ai_rateres(int player);
char* cat(char a[], char b[]);
int* stats_getlast5rolls(void);
int map_getdat(int index, int pos); //pos 0 is resource, pos 1 is number
int* ai_vertsonhex(int hex);
int data_playeratvertex(int vertex);
int data_atvertex(int p, int vertex);

int ai_knighton(int player)
{
 return data_onhex(player, map_getrobberhex());
}

int ai_playknight(int player)
{
 if(data_getcards(player, KNIGHT) >= 1 && ai_knighton(player)) {
  devcards_knight(player); //handles the removal of the knight card
  return 1;
 }
 return 0;
}

int ai_playroad(int player)
{
 if(data_getcards(player, BUILDROAD) && ai_canroad(player, 1)) {
  map_setmessage(cat(cat("Player ", itoa(player+1)), " played road building"));
  data_addcards(player, BUILDROAD, -1);
  ai_road(player, 1);
  sleep(1);
  ai_road(player, 1);
  return 1;
 }
 return 0;
}

int ai_playmonopoly(int player)
{
 if(data_getcards(player, MONOPOLY)) {
  data_addcards(player, MONOPOLY, -1);
  int *res = ai_rateres(player);
  int *nums = stats_getlast5rolls();
  int i, j, k, max, maxi, sum, *verts;
  int resources[5];
  for(i = 0; i < 5; i++) resources[i] = 0;
  max = maxi = 0;
  for(i = 1; i < 22; i++) {
   sum = 0;
   for(j = 0; j < 5; j++) {
    if(nums[j] == map_getdat(i, 1)) {
     verts = ai_vertsonhex(i);
     for(k = 0; k < 6; k++) {
      if(data_atvertex(data_playeratvertex(verts[k]), verts[k]) > 0) {
       resources[map_getdat(i, 0)] += data_atvertex(data_playeratvertex(verts[k]), verts[k]);
      }
     }
    }
   }
  }
  for(i = 0; i < 5; i++) {
   if(max < resources[i] - data_getresource(player, i) && res[i] > 1) {
    max = resources[i] - data_getresource(player, i);
    maxi = i;
   }
  }
  if(max < 4) return 0;
  map_setmessage(cat(cat(cat("Player ", itoa(player+1)), " monopolized "), map_getresname(maxi)));
  devcards_domonopoly(player, maxi);
  return 1;
 }
 return 0;
}

int ai_playyearoplenty(int player)
{
 if(data_getcards(player, YEAROPLENTY)) {
  int *hand = gethand(player);
  int rating = rate_hand(hand, player);
  int i, j, maxi, maxj;
  for(i = 0; i < 5; i++) {
   hand[i]++;
   for(j = 0; j < 5; j++) {
    hand[j]++;
    if(rate_hand(hand, player) > rating + 8) {
     rating = rate_hand(hand, player);
     maxi = i;
     maxj = j;
    }
    hand[j]--;
   }
   hand[i]--;
  }
  if(rating > rate_hand(hand, player)) { //we actually selected stuff
   data_addresource(player, maxi, 1);
   data_addresource(player, maxj, 1);
   map_setmessage(cat(cat("Player ", itoa(player+1)), " had a year of plenty"));
   data_addcards(player, YEAROPLENTY, -1);
   return 1;
  }
 }
 return 0;
}

int ai_playcard(int player)
{
 if(ai_playknight(player)) return 1;
 if(ai_playroad(player)) return 1;
 if(ai_playmonopoly(player)) return 1;
 if(ai_playyearoplenty(player)) return 1;
 return 0;
}
