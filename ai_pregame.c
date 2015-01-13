#include "settlers.h"

int data_poslegal(int pos);
void ai_randomroad(int player, int pos, int pregame);

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

void ai_buildpregame(int player, int pos)
{
 marker_setposition(pos);
 builder_placesettlement(pos, player, 1);
}

void ai_pregame(int player)
{
 int vert = ai_bestvert(player);
 ai_buildpregame(player, vert);
 ai_randomroad(player, vert, 1);
}
