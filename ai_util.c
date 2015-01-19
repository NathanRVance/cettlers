#include "settlers.h"

int builder_placesettlement(int pos, int player, int isPregame);
void marker_show(void);
void marker_setposition(int pos);
int road_prospect(int player, int dir);
int road_build(int player, int pregame);
int map_getdat(int index, int pos);
int data_onhex(int p, int hex);
int* ai_surroundingverts(int vert);
int ai_vertweight(int player, int vert);
int data_poslegal(int pos);
int* get_intarray(int size);

int* ai_surroundinghexes(int vert)
{
 static int ret[3];
 int vnum = vert % 12;
 
 //find the hex above
 if(vnum < 6) ret[0] = vnum / 2 - 5 + 5 * (vert / 12); 
 else if (vnum > 6 && vnum < 11) ret[0] = (vnum-1) / 2 - 5 + 5 * (vert / 12);
 else ret[0] = -100;
 
 //hex below
 if(ret[0] != -100) ret[1] = ret[0] + 5;
 else ret[1] = -100;
 
 //hex to the side
 if(vnum < 5 && vnum > 0) ret[2] = (vnum-1) / 2 - 2 + 5 * (vert / 12);
 else if (vnum > 5) ret[2] = vnum / 2 - 3 + 5 * (vert / 12);
 else ret[2] = -100;
 
 return ret;
}

int abs(int i)
{
 if(i < 0) i *= -1;
 return i;
}

/*
 rate[resource] is its weight, with rarer resources getting a larger number. Rates are approx 1-5
*/
int* ai_rateres(int player)
{
 static int res[5];
 int weight[5];
 int i, j;
 for(i = 0; i < 5; i++)
  weight[i] = 0;
 for(i = 0; i < 22; i++)
  if(map_getdat(i, 0) != 5 && map_getdat(i, 0) != -1)
   weight[map_getdat(i, 0)] += data_onhex(player, i) * (abs(map_getdat(i, 1) - 7) * -1 + 6);

 int max = 0;
 for(i = 0; i < 5; i++)
  if(max < weight[i]) max = weight[i];
 if(max == 0) {
  for(i = 0; i < 5; i++) res[i] = 1;
  return res;
 }
 for(i = 0; i < 5; i++) {
  res[i] = 5 - weight[i] * 4 / max;
 }
 return res;
}

//Weighs a hex based on its resource and number
int ai_weight(int player, int hex)
{
 int num = map_getdat(hex, 1);
 if(num == 0) return 0;
 if(num > 7) num = num * -1 + 14;
 if(num > 0) num--;
 num += ai_rateres(player)[map_getdat(hex, 0)];
 return num;
}

//weighs the vert based on surrounding hexes
int ai_vertweight(int player, int vert)
{
 if(! map_islegalvert(vert)) return 0;
 int *hexes = ai_surroundinghexes(vert);
 int weight = 0;
 int i;
 for(i = 0; i < 3; i++) {
  if(hexes[i] > 0 && hexes[i] < 22 && hexes[i] != 2 && hexes[i] != 20)
   weight += ai_weight(player, hexes[i]);
 }
 int port = data_getport(vert);
 if(port == -1) return weight;
 if(port == 5 && ! data_hasanyport(player)) weight += 4;
 else {
  weight += ai_rateres(player)[port] * -1 + 5;
 }
 return weight;
}

int map_getrowvert(int vert);
int map_getcolvert(int vert);

int* ai_surroundingverts(int vert)
{
 int *sur = get_intarray(3);
 sur[0] = sur[1] = sur[2] = 0;
 if(map_getrowvert(vert) % 2 == map_getcolvert(vert) % 2) {
  if((vert-11)%12 != 0) {
   sur[0] = vert + 1;
  }
 }
 else if((vert+6)%12 != 0) {
  sur[0] = vert - 1;
 }
 sur[1] = vert + 6;
 sur[2] = vert - 6;
 return sur;
}

int getupperleft(int hex); //returns upper left vertex

int* ai_vertsonhex(int hex)
{
 int *sur = get_intarray(6);
 sur[0] = getupperleft(hex);
 sur[1] = sur[0]+1;
 sur[2] = sur[0]+6;
 sur[3] = sur[0]+7;
 sur[4] = sur[0]+12;
 sur[5] = sur[0]+13;
 return sur;
}

//returns destination vertex
int* ai_roadfromvert(int player, int vert)
{
 int i, j, weight;
 int maxweight = 0;
 int dest = 0;
 int *sur = ai_surroundingverts(vert);
 int *sur2; 
 for(i = 0; i < 3; i++) {
  weight = 0;
  if(road_freeedge(vert, sur[i])) {
   if(data_poslegal(sur[i])) weight += ai_vertweight(player, sur[i]); //we will count this one again in the following loop
   sur2 = ai_surroundingverts(sur[i]); 
   for(j = 0; j < 3; j++) {
    if(road_freeedge(sur[i], sur2[j]) && data_poslegal(sur2[j])) weight += ai_vertweight(player, sur2[j]) * ai_vertweight(player, sur2[j]); //square it!
   }
   if(weight > maxweight) {
    maxweight = weight;
    dest = sur[i];
   }
  }
 }
 int *ret =  get_intarray(2);
 ret[0] = dest;
 ret[1] = maxweight;
 return ret;
}
