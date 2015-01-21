#include "settlers.h"

int data_getresource(int p, int res);
void data_addresource(int p, int res, int num);
int data_totresources(int p);
int* ai_rateres(int player);

void ai_discard(int player, int num)
{
 int discarded;
 while(num > 0) {
  discarded = 0;
  if(num > 0 && data_getresource(player, STONE) > 3) {
   num--;
   discarded = 1;
   data_addresource(player, STONE, -1);
  }
  if(num > 0 && data_getresource(player, WHEAT) > 2) {
   num--;
   discarded = 1;
   data_addresource(player, WHEAT, -1);
  }
  if(num > 0 && data_getresource(player, SHEEP) > 1) {
   num--;
   discarded = 1;
   data_addresource(player, SHEEP, -1);
  }
  if(num > 0 && data_getresource(player, WOOD) > 1) {
   num--;
   discarded = 1;
   data_addresource(player, WOOD, -1);
  }
  if(num > 0 && data_getresource(player, BRICK) > 1) {
   num--;
   discarded = 1;
   data_addresource(player, BRICK, -1);
  }
  if(discarded == 0) {
   int *res = ai_rateres(player);
   int i, mini;
   int INF = 100; //arbitrary
   while(num > 0) {
    mini = 0;
    for(i = 0; i < 5; i++) {
     if(res[i] < res[mini]) {
      mini = i;
     }
    }
    if(data_getresource(player, mini) > num) {
     data_addresource(player, mini, num * -1);
     num = 0;
    } else {
     num -= data_getresource(player, mini);
     data_addresource(player, mini, data_getresource(player, mini) * -1);
    }
    res[mini] = INF;
   }
  }
 }
}

void robber_erase(int hex);
void robber_place(int hex);
int robber_steal(int player);
int map_getrobberhex(void);
int data_playeratvertex(int vertex);
int data_atvertex(int p, int vertex);
int data_rank(int p); //what place is player p in
int map_getdat(int index, int pos);
int abs(int);
int* ai_vertsonhex(int hex);

static int ai_playerweight(int player, int p, int vert) //player is the one who rolled, p is the player on the vert
{
 if(p == -1) return 0;
 int num = 1;
 if(p == player) num *= -1;
 num *= data_atvertex(p, vert);
 if(data_rank(p) == 1) num *= 3;
 return num;
}

static int ai_robweight(int player, int hex)
{
 int *verts = ai_vertsonhex(hex);
 int sum = 0;
 int i;
 for(i = 0; i < 6; i++)
  sum += ai_playerweight(player, data_playeratvertex(verts[i]), verts[i]);
 sum *= abs(map_getdat(hex, 1) - 7) * -1 + 6;
 return sum;
}

void ai_moverobber(int player)
{
 int hexorig = map_getrobberhex();
 robber_erase(hexorig);
 int hexes[22], i;
 for(i = 0; i < 22; i++)
  hexes[i] = 0;
 hexes[0] = hexes[2] = hexes[20] = -1000;
 for(i = 1; i < 22; i++) {
  if(i == 2 || i == 20) continue;
  hexes[i] = ai_robweight(player, i);
 }
 int max, hex;
 hex = max = 0;
 for(i = 1; i < 22; i++) {
  if(hexes[i] > max && i != hexorig) {
   max = hexes[i];
   hex = i;
  }
 }
 robber_place(hex);
 int *verts = ai_vertsonhex(hex);
 max = 0;
 int maxp = 0;
 for(i = 0; i < 6; i++) {
  if(ai_playerweight(player, data_playeratvertex(verts[i]), verts[i]) > max) {
    max = ai_playerweight(player, data_playeratvertex(verts[i]), verts[i]);
    maxp = data_playeratvertex(verts[i]);
  }
 }
 if(data_totresources(maxp) > 0)
  data_addresource(player, robber_steal(maxp), 1);
 data_refresh(player);
}
