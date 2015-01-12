#include "settlers.h"
#include "color.h"

int marker_getposition(void);
void marker_show(void);
int map_getrowmapvert(int vert);
char util_strreplace(char a[], char x[], char y[], int n);
int map_getcolmapvert(int vert);
char* cat(char a[], char b[]);
int map_getroadnum(int pos, int direction);
int data_atvertex(int p, int vertex);
void map_setmessage(char s[]);
void data_addresource(int p, int res, int num);
void data_road(int p, int pos1, int pos2);
int io_getkey();
void io_printmap();
int data_elementsremaining(int p, int item);
int* data_getroad(int player, int road);
int* ai_surroundingverts(int vert);

static int pos2 = 0;

int road_legalpos(int player, int pos)
{
 if(data_atvertex(player, pos) >= 0) {
  int i;
  for(i = 0; i < 3; i++)
   if(data_atvertex(++player % 4, pos) > 0)
    return 0;
  return 1;
 }
 return 0;
}

int road_freeedge(int pos1, int pos2)
{
 int pos = (pos1 < pos2)? pos1 : pos2;
 pos2 = (pos1 > pos2)? pos1 : pos2;
 pos1 = pos;
 int i, j;
 j = 0;
 int *surrounding = ai_surroundingverts(pos1);
 for(i = 0; i < 3; i++) {
  if(surrounding[i] == pos2) j = 1;
 }
 if(j == 0) return 0;
 int *road;
 for(i = 0; i < 4; i++)
  for(j = 0; j < 15; j++) {
   road = data_getroad(i, j);
   if(road[0] == pos1 && road[1] == pos2)
    return 0;
  }
 return 1;
}

void road_erase(int player)
{
 int i;
 for(i = 0; i < MAPROWS; i++)
  util_strreplace(map[i], cat(getcolor(player), BLINK), "", 0); 
 pos2 = 0;
}

int road_build(int player, int pregame)
{
 if(! pos2)
  return 0;
 int i;
 for(i = 0; i < MAPROWS; i++)
  util_strreplace(map[i], cat(getcolor(player), BLINK), getcolor(player), 0);
 if(! pregame) {
  data_addresource(player, WOOD, -1);
  data_addresource(player, BRICK, -1);
 }
 data_road(player, marker_getposition(), pos2);
 return 1;
}

int road_left_prospect(int player)
{
 int pos = marker_getposition();
 int prospos = pos - 1;
 road_erase(player);
 if(! road_freeedge(pos, prospos) || (pos % 12 == 0 || pos % 12 == 2 || pos % 12 == 4 || pos % 12 == 6 || pos % 12 == 7 || pos % 12 == 9 || pos % 12 == 11))
  return 0;
 else {
  int i = map_getrowmapvert(pos);
  util_strreplace(map[i], VERTEX "____" VERTEX "*", cat(getcolor(player), BLINK VERTEX "____" VERTEX "*"), 1); 
  util_strreplace(map[i], VERTEX "___" VERTEX "*", cat(getcolor(player), BLINK VERTEX "___" VERTEX "*"), 1);
 util_strreplace(map[i], VERTEX "^___" VERTEX "*", cat(getcolor(player), BLINK VERTEX "^___" VERTEX "*"), 1);
 util_strreplace(map[i], VERTEX "^__" VERTEX "*", cat(getcolor(player), BLINK VERTEX "^__" VERTEX "*"), 1);
  pos2 = prospos;
  return 1;
 }
}

int road_right_prospect(int player)
{
 int pos = marker_getposition();
 int prospos = pos + 1;
 road_erase(player);
 if(! road_freeedge(pos, prospos) || (pos % 12 == 1 || pos % 12 == 3 || pos % 12 == 5 || pos % 12 == 6 || pos % 12 == 8 || pos % 12 == 10 || pos % 12 == 11))
  return 0;
 else {
  int i = map_getrowmapvert(marker_getposition());
  util_strreplace(map[i], VERTEX "*", cat(cat(getcolor(player), BLINK), VERTEX "*"), 1);
  pos2 = prospos;
  return 1;
 }
}

int road_up_prospect(int player)
{
 int pos = marker_getposition();
 int prospos = pos - 6;
 road_erase(player);
 if(! road_freeedge(pos, prospos) || (pos == 2 || pos == 3 || pos == 7 || pos == 10 || pos == 12 || pos == 17))
  return 0;
 else {
  int i = map_getrowmapvert(marker_getposition());
  while(i >= map_getrowmapvert(marker_getposition()) - 1)
   util_strreplace(map[i--], ROAD, cat(getcolor(player), BLINK ROAD), map_getroadnum(marker_getposition(), 0) + 1);
  pos2 = prospos;
  return 1;
 }
}

int road_down_prospect(int player)
{
 int pos = marker_getposition();
 int prospos = pos + 6;
 road_erase(player);
 if(! road_freeedge(pos, prospos) || (pos == 48 || pos == 53 || pos == 55 || pos == 58 || pos == 62 || pos == 63))
  return 0;
 else {
  int i = map_getrowmapvert(marker_getposition())+1;
  while(i <= map_getrowmapvert(marker_getposition()) + 2)
   util_strreplace(map[i++], ROAD, cat(getcolor(player), BLINK ROAD), map_getroadnum(marker_getposition(), 1) + 1);
  pos2 = prospos;
  return 1;
 }
}

/* Direction (dir) is:
    0: up
    1: down
    2: left
    3: right
*/
int road_prospect(int player, int dir)
{
 //if the direction doesn't work for the position, we erase.
 int pos = marker_getposition();
 marker_show();
 switch(dir) {
  case 0: return road_up_prospect(player);
   break;
  case 1: return road_down_prospect(player);
   break;
  case 2: return road_left_prospect(player);
   break;
  case 3: return road_right_prospect(player);
   break;
  default: return 0;
   break;
 }
}

int road_routine(int player, int pregame)
{
 int build = 1;
 if(! road_legalpos(player, marker_getposition())) {
  map_setmessage("Error, can't build here.");
  return 0;
 }
 if(! pregame) {
  if(data_elementsremaining(player, 3) < 1) {
   map_setmessage("Error, out of roads!");
   return;
  } else if(! data_getresource(player, WOOD) >= 1 || ! data_getresource(player, BRICK) >= 1) {
   map_setmessage("Error, not enough resources.");
   return;
  }
 }
 while(build) {
  io_printmap();
  switch(io_getkey()) {
   case UP: road_prospect(player, 0);
    break;
   case DOWN: road_prospect(player, 1);
    break;
   case LEFT: road_prospect(player, 2);
    break;
   case RIGHT: road_prospect(player, 3);
    break;
   case ENTER:
   case 'r':
    if(road_build(player, pregame))
     return 1;
    break;
   default:
    if(! pregame) {
     build = 0;
     road_erase(player);
    }
    break;
  }
 }
}
