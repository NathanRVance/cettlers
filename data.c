#include "settlers.h"
#include "color.h"
#include <string.h>
#include <stdio.h>

char* cat(char a[], char b[]);
char* itoa(int i);
void data_refresh(int p);
void map_setmessage(char s[]);
void robber_discard(int player);
int roadlength_main(int roads[], int player);
int io_getkey(void);
void io_printmap(int printsdat);
void io_printwin(int player, int cards);
void halt(void);
int* ai_surroundinghexes(int vert);

//4 playes, 5 things to keep track of, 30 integers to keep track of each.
static int player[4][5][30];
//We have 5 resources, 5 settlements, 4 cities, 15 roads, and devel cards / knights played / largest army / longest road
#define RESOURCES     0
#define SETTLEMENTS   1
#define CITIES        2
#define ROADS         3
#define MISC          4
#define KNIGHTSPLAYED 5
#define LARGESTARMY   6
#define LONGESTROAD   7
#define ROADLENGTH    8
#define ISAI          9

static int ports[6][8]; //the 6 types of ports and 8 vertecies for
                        //each type
void data_init()
{
 int i, j;
 for(i = 0; i < DATROWS; i++)
  strcpy(dat[i], "");
 data_refresh(0);
 ports[0][0] = 12;
 ports[0][1] = 13;
 ports[1][0] = 23;
 ports[1][1] = 29;
 ports[2][0] = 57;
 ports[2][1] = 58;
 ports[3][0] = 2;
 ports[3][1] = 3;
 ports[4][0] = 42;
 ports[4][1] = 48;
 ports[5][0] = 16;
 ports[5][1] = 17;
 ports[5][2] = 24;
 ports[5][3] = 30;
 ports[5][4] = 55;
 ports[5][5] = 56;
 ports[5][6] = 47;
 ports[5][7] = 53;
 
}

int data_getresource(int p, int res)
{
 return player[p][RESOURCES][res];
}

void data_addresource(int p, int res, int num)
{
 player[p][RESOURCES][res] += num;
}

int data_totresources(int p)
{
 int sum = 0;
 int i;
 for(i = 0; i < 5; i++)
  sum += data_getresource(p, i);
 return sum;
}

void data_addcards(int p, int card, int num)
{
 player[p][MISC][card] += num;
}

int data_getcards(int p, int card)
{
 return player[p][MISC][card];
}

void data_playknight(int p)
{
 int i;
 player[p][MISC][KNIGHTSPLAYED] += 1;
 if(player[p][MISC][KNIGHTSPLAYED] >= 3) {
  player[p][MISC][LARGESTARMY] = 1;
  for(i = p+1; i < p+4; i++)
   if(player[p][MISC][KNIGHTSPLAYED] <= player[i%4][MISC][KNIGHTSPLAYED])
    player[p][MISC][LARGESTARMY] = 0;
   else
    player[i%4][MISC][LARGESTARMY] = 0;
 }
}

int data_numcards(int p)
{
 int sum = 0;
 int i;
 for(i = 0; i < 5; i++)
  sum += data_getcards(p, i);
 return sum;
}

int data_isai(int p)
{
 return player[p][MISC][ISAI];
}

void data_setai(int p, int set)
{
 player[p][MISC][ISAI] = set;
}

//Checks what's at the vertex. Returns:
// -1: nothing (built by player)
// 0: road
// 1: settlement
// 2: city
int data_atvertex(int p, int vertex)
{
 int i;
 for(i = 0; i < 5; i++)
  if(player[p][SETTLEMENTS][i] == vertex) return 1;
 for(i = 0; i < 4; i++)
  if(player[p][CITIES][i] == vertex) return 2;
 for(i = 0; i < 30; i++)
  if(player[p][ROADS][i] == vertex) return 0;
 return -1;
}

int data_playeratvertex(int vertex)
{
 int p;
 for(p = 0; p < 4; p++)
  if(data_atvertex(p, vertex) > 0) return p;
 return -1;
}

int* data_getroad(int p, int road)
{
 static int ret[2];
 ret[0] = player[p][ROADS][road/2];
 ret[1] = player[p][ROADS][road/2+1];
 return ret;
}

int* data_getsettlements(int p)
{
 return player[p][SETTLEMENTS];
}

int getupperleft(int hex);

//returns the number of resources the player would get
//if this hex were rolled
int data_onhex(int p, int hex)
{
 int vert = getupperleft(hex);
 //vert, vert+6, vert+12, vert+1, vert+7, and vert+13 are all adjacent
 //index [p][1] is the 5 settlements and [p][2] is the 4 cities. 
 int i, j, n;
 int ans = 0;
 for(i = 1; i <= 2; i++) {
  for(j = 0; j < 5; j++){
   n = player[p][i][j]; //n is just for code readability
   if(n == vert || n == vert + 1 || n == vert + 6 || n == vert + 7 || n == vert + 12 || n == vert + 13)
    ans += i;
  }
 }
 return ans;
}

int getupperleft(int hex)
{
 int i, j, k, vert;
 vert = 0;
 //hex is 5i + j + k
 for(i = 0; i < 6; i++) {
  for(j = 0, k = 0; j < 2; j++) {
   if(hex == 5*i + j + k)
    return vert;
   vert += 2;
  }
  if(hex == 5*i + j + k)
   return vert;
  vert += 3;
  for(k = 1; k < 2; k++) {
   if(hex == 5*i + j + k)
    return vert;
   vert += 2;
  }
  if(hex == 5*i + j + k)
   return vert;
  vert += 3;
 }
}

void roll(int player)
{
 int roll = rand() % 6 + rand() % 6 + 2;
 if(roll == 7) {
  robber_discard(player);
  map_setmessage(cat(cat("Player ", itoa(player+1)), ", please move the robber."));
  io_printmap(! data_isai(player));
  robber_routine(player);
 }
 map_setmessage(cat("Rolled ", itoa(roll)));
 int i, p;
 for(i = 1; i <= 21; i++)
  if(map_getdat(i, 1) == roll && map_getdat(i, 2) != 1) //is not robber
   for(p = 0; p < 4; p++)
    data_addresource(p, map_getdat(i, 0), data_onhex(p, i));
}

//item: 1 is settlement, 2 is city, 3 is road
int data_elementsplaced(int p, int item)
{
 int i = -1;
 while(player[p][item][++i]);
 return i;
}

//This checks if we have enough game pieces to
//place something.
int data_elementsremaining(int p, int item)
{
 int i = data_elementsplaced(p, item);
 if(item == 1) return 5 - i;
 if(item == 2) return 4 - i;
 if(item == 3) return (30 - i) / 2;
}

void data_pregameres(int p, int vert)
{
 int *hexes = ai_surroundinghexes(vert);
 int i;
 for(i = 0; i < 3; i++) {
  if(hexes[i] > 0 && hexes[i] < 22 && hexes[i] != 2 && hexes[i] != 20) {
   player[p][RESOURCES][map_getdat(hexes[i], 0)]++;
  }
 }
}

//Checks for anything within one vertex
int data_poslegal(int pos)
{
 if(pos <= 1 || pos == 4 || pos == 5 || pos == 60 || pos == 61 || pos >= 64)
  return 0;
 //Check all other settlements/cities for a row +- 1 or
  // if even column, a column - 1,
  // if odd column, column + 1
 //Also check the vertex we are currently on!
 int odd = map_getcolvert(pos) % 2;
 int checkhoriz = 0;
 int i, j, k;
 if(!odd && (pos + 6) % 12) checkhoriz = 1;
 if(odd && (pos + 1) % 12) checkhoriz = -1;
 if(map_getrowvert(pos) % 2 == 1) checkhoriz *= -1; //fix if odd row
 for(i = 0; i < 4; i++)
  for(j = 1; j <= 2; j++)
   for(k = 0; player[i][j][k] != 0; k++) {
    if(checkhoriz != 0)
     if(pos + checkhoriz == player[i][j][k]) return 0;
    if(player[i][j][k] + 6 == pos || player[i][j][k] - 6 == pos) return 0;
    if(player[i][j][k] == pos) return 0;
   }
 return 1;
}

//Places settlement
void data_place(int p, int position)
{
 int i = -1;
 while(player[p][SETTLEMENTS][++i] != 0);
 player[p][SETTLEMENTS][i] = position;
}

//Upgrades settlement to city
void data_upgrade(int p, int position)
{
 int i = -1;
 while(player[p][SETTLEMENTS][++i] != position);
 int j = i + 1;
 while((player[p][SETTLEMENTS][i++] = player[p][SETTLEMENTS][j++]) != 0);
 i = -1;
 while(player[p][CITIES][++i] != 0);
 player[p][CITIES][i] = position;
}

void data_road(int p, int pos1, int pos2)
{
 int i;
 for(i = 0; player[p][ROADS][i] != 0; i += 2);
 player[p][ROADS][i] = (pos1 < pos2)? pos1 : pos2;
 player[p][ROADS][i+1] = (pos1 > pos2)? pos1 : pos2;
 player[p][MISC][ROADLENGTH] = roadlength_main(player[p][ROADS], p);
 if(player[p][MISC][ROADLENGTH] >= 5 && player[p][MISC][LONGESTROAD] == 0) {
  int longest = 1;
  for(i = 1; i < 4; i++) {
   if(player[(p+i)%4][MISC][ROADLENGTH] >= player[p][MISC][ROADLENGTH]) longest = 0;
   else player[(p+i)%4][MISC][LONGESTROAD] = 0;
  }
  player[p][MISC][LONGESTROAD] = longest;
 }
}

int get_trade(int p, int res)
{
 int i;
 if(data_atvertex(p, ports[res][0]) > 0 ||
    data_atvertex(p, ports[res][1]) > 0)
  return 2;
 for(i = 0; i < 8; i++)
  if(data_atvertex(p, ports[5][i]) > 0)
   return 3;
 return 4;
}

int data_hasanyport(int p)
{
 int i;
 for(i = 0; i < 8; i++)
  if(data_atvertex(p, ports[5][i]) > 0)
   return 1;
 return 0;
}

int data_getport(int vert)
{
 int i, j;
 for(i = 0; i < 5; i++)
  for(j = 0; j < 2; j++)
   if(ports[i][j] == vert) return i;
 for(j = 0; j < 8; j++)
  if(ports[5][j] == vert) return 5;
 return -1;
}

int data_vps(int p) //returns vps as seen by other players (without cards)
{
 int vps = 0;
 int i = -1;
 while(player[p][SETTLEMENTS][++i] != 0);
 vps += i;
 i = -1;
 while(player[p][CITIES][++i] != 0);
 vps += i*2;
 if(player[p][MISC][LARGESTARMY]) vps += 2;
 if(player[p][MISC][LONGESTROAD]) vps += 2;
 return vps;
}

int data_rank(int p) //what place is player p in
{
 int rank, i, maxes[5];
 maxes[0] = 100;
 for(i = 1; i < 5; i++)
  maxes[i] = -1;
 for(rank = 1; rank < 5; rank++) {
  for(i = 0; i < 4; i++) {
   if(data_vps(i) > maxes[rank] && data_vps(i) < maxes[rank-1]) {
    maxes[rank] = data_vps(i);
   }
  }
 }
 for(rank = 1; ; rank++)
  if(data_vps(p) == maxes[rank]) return rank;
}

void data_refresh(int p)
{
 char chp[2];
 chp[0] = p+'1';
 chp[1] = '\0';
 strcpy(dat[0], cat(cat(cat(getcolor(p), "Player "), chp) , BRN  "  wood   " YEL "wheat" GRN "   sheep" RED "   brick" NRM "   stone"));
 int i;
 //dat[1] holds numbers of resources for the player
 for(i = 0; i < 5; i++)
  if(! data_isai(p))
   dat[1][i] = player[p][RESOURCES][i];
  else
   dat[1][i] = 0;
 //dat[2] holds data that is seen by all players
 for(i = 0; i < 4; i++) {
  dat[2][7*i] = data_totresources(i);
  dat[2][7*i+1] = data_numcards(i);
  dat[2][7*i+2] = player[i][MISC][LARGESTARMY];
  dat[2][7*i+3] = player[i][MISC][KNIGHTSPLAYED];
  dat[2][7*i+4] = player[i][MISC][LONGESTROAD];
  dat[2][7*i+5] = player[i][MISC][ROADLENGTH];
  dat[2][7*i+6] = data_vps(i);
  if(data_vps(i) + data_getcards(i, VP) >= 10) {
   io_printmap(0);
   io_printwin(i, data_getcards(i, VP));
   while(io_getkey() != 'q');
   endwin();
   printf("Player %d won with %d VP card%s!\n", i+1, data_getcards(i, VP), (data_getcards(i, VP) == 1)? "" : "s");
   halt();
  }
 }
}

