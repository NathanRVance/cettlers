#include "color.h"
#include "settlers.h"
#include <string.h>
#include <time.h>
#include <stdio.h>

extern void util_strreplace(char a[], char x[], char y[], int n);
extern char* betterstrcat(char a[], char b[], char c[]);
int map_getrowhex(int hex);
int map_getcolhex(int hex);
static void map_distribute(int n[], int res[], int pos);
int map_getdat(int index, int pos);
void map_setdat(int index, int pos, int value);
char* map_getresname(int res);
char* map_itoa(int num);
void robber_place(int hex);
int map_getrobberhex(void);

//map matrix for storing map data
//row,column,data. Data is resource, num, robber?
int mapdat[9][5][3];

int illegalHexes[] = {0, 2, 20, 22};
int illegalVerts[] = {0, 1, 4, 5, 6, 11, 54, 59, 60, 61, 64, 65};

/*  Before we get started, Here's the map with vertecies and hexes numbered:

Hex Col     0      1      2      3      4
Row
 Vert Col 0     1     2      3      4       5  
   Row                                              
    0     0----1        2___3         4---5  
         |     |       /     \       |     |   
 0  1   6   0   |7___8/   1   \9___10   2   11        
        |       /     \       /     \       |   
 1  2    12___13   3   14___15   4   16___17  
         /     \       /     \       /     \
 2  3  18   5   \19__20   6   21___22   7   23 
        \       /     \       /     \       /
 3  4    24___25   8   26___27   9   28___29
         /     \       /     \       /     \
 4  5  30  10   31___32  11   33___34  12   35
        \       /     \       /     \       /
 5  6    36___37  13   38___39  14   40___41
         /     \       /     \       /     \
 6  7  42  15   43___44  16   45___46  17   47
        \       /     \       /     \       /
 7  8    48___49  18   50___51  19   52___53
         |     \       /     \       /      |
 8  9   54  20  55___56  21   57___58  22   59
        |       |     \       /     |        |
 9  10   60----61      62___63       64----65

These rows/columns and vertex/hex numbers will be used and converted
between in map logic. Refer to this diagram for reference.
Note that hexes 0, 2, 20 and 22 are included only to keep the pattern.
*/

map_create(int randomnums)
{
 strcpy(map[0], "Press 'h' for help");
 strcpy(map[1], "                   ~   | 2:1 |   ~   |>       ~");
 strcpy(map[2], "                       |brick|     __|__           ~");
 strcpy(map[3], "   ~           ~        ^___^     ~{___}~   ~");
 strcpy(map[4], " ~      ~        ~     /     \\   ~      ~     ~");
 strcpy(map[5], "   |>    | 2:1 | =___=/ thing \\=___= | 3:1 |        ~");
 strcpy(map[6], " __|__   |wood |/     \\  ##   /     \\| any |   ~");
 strcpy(map[7], " {___}    ^___^/ thing \\=___=/ thing \\^___^  ~");
 strcpy(map[8], "  ~   ~  /     \\  ##   /     \\  ##   /     \\      ~");
 strcpy(map[9], "~      =/ thing \\=___=/ thing \\=___=/ thing \\= ");
 strcpy(map[10],"    ~   \\  ##   /     \\  ##   /     \\  ##   />| 2:1 |");
 strcpy(map[11],"         \\=___=/ thing \\=___=/ thing \\=___=/> |wheat|");
 strcpy(map[12],"| 3:1 | </     \\  ##   /     \\  ##   /     \\  ~");
 strcpy(map[13],"| any |=/ thing \\=___=/ thing \\=___=/ thing \\=     ~");
 strcpy(map[14],"   ~    \\  ##   /     \\  ##   /     \\  ##   /   |>");
 strcpy(map[15],"  ~      \\=___=/ thing \\=___=/ thing \\=___=/  __|__");
 strcpy(map[16],"      ~  /     \\  ##   /     \\  ##   /     \\  {___}");
 strcpy(map[17],"       =/ thing \\=___=/ thing \\=___=/ thing \\=  ~");
 strcpy(map[18],"| 2:1 |<\\  ##   /     \\  ##   /     \\  ##   />| 3:1 |");
 strcpy(map[19],"|stone| <\\=___=/ thing \\=___=/ thing \\=___=/> | any |");
 strcpy(map[20],"               \\  ##   /     \\  ##   /");
 strcpy(map[21],"  ~  |>   ~     \\=___=/ thing \\=___=/            ~");
 strcpy(map[22],"   __|__         v   v\\  ##   />| 2:1 | ~     ~");
 strcpy(map[23],"~  {___}        | 3:1 |\\=___=/> |sheep|      ~");
 strcpy(map[24],"           ~    | any |        ~                ~");
 
 //Now add color and such
 int i, j;
 for(i = 1; i < MAPROWS; i++) {
  util_strreplace(map[i], "\\", NRM ROAD "\\" NRM, 0 );
  util_strreplace(map[i], "/", NRM ROAD "/" NRM, 0);
  util_strreplace(map[i], "~", BLU "~" NRM, 0);
  util_strreplace(map[i], "{___}", BRN "\\___/" NRM, 0);
  util_strreplace(map[i], "__|__", BRN "__|__" NRM, 0);
  util_strreplace(map[i], "|>", BRN "|" RED ">" NRM, 0);
  util_strreplace(map[i], "=_", VERTEX "__", 0);
  util_strreplace(map[i], "_=", "_" VERTEX "_", 0);
  util_strreplace(map[i], "=", VERTEX " ", 0);
  util_strreplace(map[i], "_^", "_" VERTEX "^" NRM, 0);
  util_strreplace(map[i], "^_", VERTEX "^_", 0);
  util_strreplace(map[i], " ##", ROBBER " ##", 0);
  util_strreplace(map[i], "| 3", NRM "| 3", 0); //added so road coloring doesn't run too far
 }

 //Distribute resources
 srand(time(NULL)); //initialize random seed (we don't want every game to be the same ;)
 int n[11];
 n[0] = n[1] = n[2] = 4;
 n[3] = n[4] = 3;
 n[5] = 1;
 int res[11];
 res[0] = WOOD;
 res[1] = WHEAT;
 res[2] = SHEEP;
 res[3] = BRICK;
 res[4] = STONE;
 res[5] = DESERT;
 map_distribute(n, res, 0);

//Distribute numbers
 if(randomnums) {
  n[0] = n[9] = n[10] = 1;
  for(i = 1; i < 9; i++)
   n[i] = 2;
  for(i = 0; i < 5; i++)
   res[i] = i+2;
  for(i = 5; i < 10; i++)
   res[i] = i+3;
  res[10] = 0;
  map_distribute(n, res, 1);
 } else {
  int nums[] = {5, 2, 6, 3, 8, 10, 9, 12, 11, 4, 8, 10, 9, 4, 5, 6, 3, 11};
  int order[] = {1, 3, 5, 10, 15, 18, 21, 19, 17, 12, 7, 4, 6, 8, 13, 16, 14, 9, 11};
  int desert;
  for(desert = 0; map_getdat(desert, 0) != 5; desert++);
  for(i = 0, j = 0; i < 19; i++) {
   if(order[i] != desert) map_setdat(order[i], 1, nums[j++]);
  }
 }

//unset robber for all (will be fixed when we fix the desert)
for(i = 0; i < 23; i++)
 map_setdat(i, 2, 0);

//These indecies are not to be used, set their data to -1
 for(i = 0; i < 3; i++)
  for(j = 0; j < 4; j++)
   map_setdat(illegalHexes[j], i, -1);

//fix the location of the desert and the zero dice roll
 for(i = 0; map_getdat(i, 0) != 5; i++); //i is the index of the desert
 for(j = 0; map_getdat(j, 1) != 0; j++); //j is the index of zero
 map_setdat(j, 1, map_getdat(i, 1));
 map_setdat(i, 1, 0);
 map_setdat(i, 2, 1); //place the robber in the desert

// print the dat status 
// for(i = 0; i < 23; i++)
//  printf("mapdat[%d][%d][0] = %d\n", map_getrowhex(i), map_getcolhex(i), mapdat[map_getrowhex(i)][map_getcolhex(i)][0]);
// for(i = 0; i < 23; i++)
//  printf("mapdat[%d][%d][1] = %d\n", map_getrowhex(i), map_getcolhex(i), mapdat[map_getrowhex(i)][map_getcolhex(i)][1]);

//Update the map to match map_data
//First do resources
 i = 1;
 util_strreplace(map[map_getrowhex(i)*2+5], "thing", map_getresname(map_getdat(i, 0)), 1);
 for(i = 3; i < 20; i++)
  util_strreplace(map[map_getrowhex(i)*2+5], "thing", map_getresname(map_getdat(i, 0)), 1);
 i = 21;
 util_strreplace(map[map_getrowhex(i)*2+5], "thing", map_getresname(map_getdat(i, 0)), 1);
//Next, roll numbers
 i = 1;
 util_strreplace(map[map_getrowhex(i)*2+6], "##", map_itoa(map_getdat(i, 1)), 1);
 for(i = 3; i < 20; i++)
  util_strreplace(map[map_getrowhex(i)*2+6], "##", map_itoa(map_getdat(i, 1)), 1);
 i = 21;
 util_strreplace(map[map_getrowhex(i)*2+6], "##", map_itoa(map_getdat(i, 1)), 1);
 robber_place(map_getrobberhex()); //Lastly, initial robber position
}

char* map_getresname(int res)
{
 switch(res) {
  case WOOD:   return BRN "wood " NRM;
  case WHEAT:  return YEL "wheat" NRM;
  case SHEEP:  return GRN "sheep" NRM;
  case BRICK:  return RED "brick" NRM;
  case STONE:  return "stone";
  case DESERT: return "     ";
  default:     return "error";
 }
}

char* map_itoa(int num)
{
 static char n[20];
 n[2] = '\0';
 if(num == 6) {
  strcpy(n, RED " 6" NRM);
 } else if(num == 8) {
  strcpy(n, RED " 8" NRM);
 } else {
  n[1] = num % 10 + '0';
  if(num - 10 >= 0)
   n[0] = 1 + '0';
  else {
   n[0] = ' ';
   if(num == 0)
    n[1] = ' ';
  }
 }
 return n;
}

//Distribute n resources (res) randomly into the given position in mapdat
static void map_distribute(int n[], int res[], int pos)
{
 int hexes[23];
 int i;
 for(i = 0; i < 23; i++)
  hexes[i] = 1;
 int m = 0; //the position in n and res
 hexes[0] = hexes[2] = hexes[20] = hexes[22] = 0;
 int j, k;
 for(i = 19; i > 0; i--) {
  k = rand() % i;
  j = -1;
  while(k >= 0) {
   j++;
   if(hexes[j] == 1)
    k--;
  }
  hexes[j] = 0;
  if(n[m] > 0)
   n[m] -= 1;
  else
   n[++m] -= 1;
  map_setdat(j, pos, res[m]);
 }
}

int map_getdat(int index, int pos)
{
 return mapdat[map_getrowhex(index)][map_getcolhex(index)][pos];
}

void map_setdat(int index, int pos, int value)
{
 mapdat[map_getrowhex(index)][map_getcolhex(index)][pos] = value;
}

int map_getrobberhex()
{
 int i;
 for(i = 0; i < 22; i++)
  if(mapdat[map_getrowhex(i)][map_getcolhex(i)][2] == 1)
   return i;
 return 0;
}

//The logic for the following get functions involves
//four extra hexes in the corners (draw it!) that are
//treated as valid hexes; it's up to the caller to
//account for them.
int map_getrowhex(int hex)
{
 int i, row;
 i = row = 0;
 while(row < 9) {
  i += 3;
  if(i > hex)
   return row;
  i += 2;
  row++;
  if(i > hex)
   return row;
  row++;
 }
// printf("Error in map_getrowhex, hex was %d\n", hex);
 return -1;
}

int map_getcolhex(int hex)
{
 //See above comment
 return ((hex % 5) * 2) % 5;
}

int map_getrowvert(int vert)
{
 return vert/6; //6 vertecies in a row, round down
}

//As opposed to the hex getrow method, this returns the row in the map[][] array
int map_getrowmapvert(int vert)
{
 return 2 * map_getrowvert(vert) + 3;
}

/* This one returns which number it is counting from the left,
   which forces us to enforce the hexagonal shape of the gameboard
   rather than abstracting it as a square
*/
int map_getcolmapvert(int vert)
{
 int i;
 for(i = 0; i < 12; i++)
  if(illegalVerts[i] == vert)
   return -1;
 if(vert == 2) return 0;
 if(vert == 3) return 1;
 if(vert >= 7 && vert <= 10) return vert - 7;
 if(vert >= 55 && vert <= 58) return vert - 55;
 if(vert == 62) return 0;
 if(vert == 63) return 1;
 return vert % 6;
}

int map_getcolvert(int vert)
{
 return vert % 6;
}

int map_islegalvert(int pos)
{
 int i;
 for(i = 0; i < 12; i++)
  if(illegalVerts[i] == pos)
   return 0;
 return 1;
}

//direction is 0 up or 1 down
int map_getroadnum(int pos, int direction)
{
 int ans = pos % 6;
 if(pos <= 16 && direction == 0) ans--;
 if(pos <= 10) ans--;
 if(pos <= 3) ans--;
 if(pos >= 49 && direction == 1) ans--;
 if(pos >= 55) ans--;
 if(pos >= 62) ans--;
 return ans;
}

void map_setmessage(char s[])
{
 int i;
 for(i = 0; (map[0][i] = s[i]) != '\0'; i++);
}
