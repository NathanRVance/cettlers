#define MAPROWS 25
#define MAPCOLS 300
#define ROAD   "\1"
#define VERTEX "\2"
#define ROBBER "\3"
#define WOOD   0
#define WHEAT  1
#define SHEEP  2
#define BRICK  3
#define STONE  4
#define DESERT 5
#define LEFT   0
#define RIGHT  1
#define UP     2
#define DOWN   3
#define ENTER  4

char map[MAPROWS][MAPCOLS];

#define DATROWS 3
#define DATCOLS 100
char dat[DATROWS][DATCOLS];

#define TRADECOLS 20
char trade[TRADECOLS];
//cols 0-4 are the resources,
//col 5 is cursor position,
//cols 6-end is trade type (string)

#define CARDROWS 4
#define CARDCOLS 50
char cardDisplay[CARDROWS][100];

#define DEVCARDS    25
#define KNIGHT      0 //14
#define BUILDROAD   1 //2
#define MONOPOLY    2 //2
#define YEAROPLENTY 3 //2
#define VP          4 //5

