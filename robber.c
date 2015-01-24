#include "settlers.h"

int map_getrowhex(int hex);
int map_getcolhex(int hex);
char util_strreplace(char a[], char x[], char y[], int n);
char* cat(char a[], char b[]);
char* map_itoa(int num);
int map_getdat(int index, int pos);
void map_setdat(int index, int pos, int value);
int map_getrobberhex(void);
int io_getkey();
void io_printmap(int printsdat);
void map_setmessage(char s[]);
char* itoa(int i);
int data_totresources(int p);
int data_isai(int p);
int data_atvertex(int p, int vertex);
void data_refresh(int p);
void ai_discard(int player, int num);
void trade_routine(int player, int credits, int type);
int getupperleft(int hex);
void marker_hide(void);
void ai_moverobber(int player);
void pass_between_humans(int to);
int data_multihumans(void);

//Returns how many hexes in the robber is
static int getoffset(int hex)
{
 int offset = 1;
 if(hex == 1 || hex == 21) return offset;
 if(hex % 5 == 0 || hex % 5 == 3) return offset;
 if(hex % 5 == 2 && map_getdat(hex-2, 1) == map_getdat(hex, 1)) offset++;
 if(map_getdat(hex-1, 1) == map_getdat(hex, 1)) offset++;
 return offset;
}

void robber_erase(int hex)
{
 map_setdat(hex, 2, 0);
 int row = map_getrowhex(hex)*2+6;
 util_strreplace(map[row], "robber", cat(cat(" ", map_itoa(map_getdat(hex,1))), "   "), 1);
}

void robber_place(int hex)
{
 map_setdat(hex, 2, 1);
 int row = map_getrowhex(hex)*2+6;
 util_strreplace(map[row], cat(cat(cat(ROBBER, " "), map_itoa(map_getdat(hex, 1))), "   "), ROBBER "robber", getoffset(hex));
}

int robber_up(int hex)
{
 robber_erase(hex);
 hex -= 5;
 if(hex == -4) hex = 21;
 if(hex == 0 || hex == 2) hex += 15;
 if(hex < 1) hex += 20;
 robber_place(hex);
 return hex;
}

int robber_down(int hex)
{
 robber_erase(hex);
 hex += 5;
 if(hex == 26) hex = 1;
 if(hex == 20 || hex == 22) hex -= 15;
 if(hex > 21) hex -= 20;
 robber_place(hex);
 return hex;
}

int robber_left(int hex)
{
 robber_erase(hex);
 hex--;
 if(hex == 2 || hex == 20) hex--;
 if(hex == 0) hex = 21;
 robber_place(hex);
 return hex;
}

int robber_right(int hex)
{
 robber_erase(hex);
 hex++;
 if(hex == 2 || hex == 20) hex++;
 if(hex == 22) hex = 1;
 robber_place(hex);
 return hex;
}

int robber_steal(int player)
{
 int k = rand() % data_totresources(player);
 int i;
 for(i = 0; k >= 0; i++)
  k -= data_getresource(player, i);
 data_addresource(player, --i, -1);
 return i;
}

int robber_atvert(int player, int vert)
{
 int i;
 for(i = 1; i < 4; i++)
  if(data_atvertex((player+i) % 4, vert) > 0)
   return (player+i) % 4;
 return -1;
}

void robber_stealroutine(int player, int hex)
{
 data_refresh(player);
 int verts[3], i;
 for(i = 0; i < 3; i++)
  verts[i] = 0;
 int vert = getupperleft(map_getrobberhex());
 i = -1;
 if(robber_atvert(player, vert) >= 0) verts[++i] = vert;
 if(robber_atvert(player, vert+1) >= 0) verts[++i] = vert+1;
 if(robber_atvert(player, vert+6) >= 0) verts[++i] = vert+6;
 if(robber_atvert(player, vert+7) >= 0) verts[++i] = vert+7;
 if(robber_atvert(player, vert+12) >= 0) verts[++i] = vert+12;
 if(robber_atvert(player, vert+13) >= 0) verts[++i] = vert+13;
 if(i == -1) return;
 int v1, v2, v3;
 v1 = v2 = v3 = robber_atvert(player, verts[0]);
 if(i >= 1) v2 = robber_atvert(player, verts[1]);
 if(i == 2) v3 = robber_atvert(player, verts[2]);
 marker_setposition(vert = verts[0]);
 if(v1 != v2 || v1 != v3 || v2 != v3)
 {
  map_setmessage("Please select a victim to steal from");
  int rob = 1;
  while(rob) {
   io_printmap(1);
   switch(io_getkey()) {
    case UP: marker_setposition(vert = verts[0]);
     break;
    case DOWN: marker_setposition(vert = verts[i]);
     break;
    case LEFT: marker_setposition(vert = verts[i/2]);
     break;
    case RIGHT: marker_setposition(vert = verts[(i+1)/2]); 
     break;
    case ENTER:
    case 'r': rob = 0;
     break;
    default:
     break;
   }
  }
 }
 int robbed = robber_atvert(player, vert);
 if(data_totresources(robbed) > 0)
  data_addresource(player, robber_steal(robbed), 1);
 data_refresh(player);
}

void robber_routine(int player)
{
 if(data_isai(player)) {
  ai_moverobber(player);
  return;
 }
 marker_hide();
 int hex = map_getrobberhex();
 int hexorig = hex;
 int rob = 1;
 while(rob) {
  data_refresh(player);
  io_printmap(1);
  switch(io_getkey()) {
   case UP: hex = robber_up(hex);
    break;
   case DOWN: hex = robber_down(hex);
    break;
   case LEFT: hex = robber_left(hex);
    break;
   case RIGHT: hex = robber_right(hex);
    break;
   case ENTER:
   case 'r': 
    if(hex != hexorig)
     rob = 0;
    else
     map_setmessage("Cannot rob same hex twice in a row.");
    break;
   default:
    break;
  }
 }
 robber_stealroutine(player, hex);
}

void robber_discardhandler(int player, int num)
{
 if(data_isai(player)) ai_discard(player, num);
 else {
  trade_routine(player, num * -1, 2);
 }
}

void robber_discard(int player)
{
 int i;
 for(i = 0; i < 4; i++) {
  if(data_totresources(player) > 7) {
   if(! data_isai(player) && data_multihumans())
    pass_between_humans(player);
   robber_discardhandler(player, data_totresources(player) / 2);
  }
  player = (player+1)%4;
 }
}
