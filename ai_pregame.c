#include "settlers.h"

int map_getdat(int index, int pos);
int data_poslegal(int pos);
void ai_buildpregame(int player, int pos);
void ai_randomroad(int player, int pos, int pregame);
char* itoa(int i);
int map_islegalvert(int pos);
int* ai_rateres(int player);
int map_getdat(int index, int pos);

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

int ai_weight(int player, int hex)
{
 int num = map_getdat(hex, 1);
 if(num == 0) return 0;
 if(num > 7) num = num * -1 + 14;
 if(num > 0) num--;
 num += ai_rateres(player)[map_getdat(hex, 0)];
 return num;
}

//weighs the vert based on surrounding numbers
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
 if(port == 5 && ! data_hasanyport(player)) weight += 5;
 else {
  weight += ai_rateres(player)[port] * -1 + 5;
 }
 return weight;
}

int ai_bestvert(int player)
{
 int i, besti, bestweight;
 besti = bestweight = 0;
 for(i = 2; i < 64; i++)
  if(data_poslegal(i) && ai_vertweight(player, i) > bestweight) {
   besti = i;
   bestweight = ai_vertweight(player, i);
  }
 return besti;
}

void ai_pregame(int player)
{
 int vert = ai_bestvert(player);
 ai_buildpregame(player, vert);
 ai_randomroad(player, vert, 1);
}
