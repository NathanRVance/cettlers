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

int ROLLED;

//Here we place all declarations:
int ai_road(int player, int free);
int rate_hand(int *hand, int player);
int* gethand(int player, int hand[5]);
int* stats_getlast5rolls(int array[5]);
int* ai_vertsonhex(int hex, int array[6]);
int data_playeratvertex(int vertex);
int data_totresources(int p);
int builder_placecity(int pos, int player);
void devcards_buy(int player);
int devcards_remaining(void);
int data_elementsplaced(int p, int item);
int* data_getsettlements(int p);
void marker_setposition(int pos);
int ai_trade(int player);
int* ai_roadfromvert(int player, int vert, int array[2]);
int ai_playcard(int player);
int road_vertbuild(int player, int origin, int dest, int pregame);
int road_legalpos(int player, int pos);
int builder_placesettlement(int pos, int player, int isPregame);
int ai_cansettle(int player);
int ai_canupgrade(int player);
int* ai_canroad(int player, int free);
int* ai_rateres(int player);
int get_trade(int p, int res);
void doTrade(int *trade, int player1, int player2);
int* accepttrade(int *trade, int player, int playerTrading, int array[6]);
int data_vps(int p);
int map_getdat(int index, int pos);
int data_onhex(int p, int hex);
int ai_vertweight(int player, int vert);
int data_hypotheticalroadlength(int p, int* roads, int n);
int data_getroadlen(int p);
int data_getport(int pos);
int data_hasanyport(int player);
int road_freeedge(int pos1, int pos2);
void io_print(int x, int y, char* s, int color);
int millasleep(int ms);
int abs(int a);
int map_getrowvert(int vert);
int map_getcolvert(int vert);
int map_getrowmapvert(int vert);
char util_strreplace(char a[], char x[], char y[], int n);
void data_place(int p, int position);
void data_upgrade(int p, int position);
int data_poslegal(int pos);
void robber_discard(int player);
int roadlength_main(int roads[], int player);
void io_printwin(int player, int cards);
void halt(void);
int* ai_surroundinghexes(int vert);
void stats_roll(int roll);
void animate(int resource, int hex, int player);
void devcards_distribute(int num, int card);
void io_printcards(int c, int s, int r, int m, int p, int v, int pos);
void data_addcards(int p, int card, int num);
int data_getcards(int p, int card);
void data_playknight(int p);
int road_routine(int player, int pregame);
void io_printsetup(int cursor, int *set);
void data_setai(int p, int set);
void data_setsleeptime(int ds);
void setanimate(int set);
void io_printdat(int printsdat);
void io_printrolled(void);
int stats_numrolls(void);
void io_init(void);
void io_printhelp(void);
void main_routine(int p);
int pregame_routine(void);
void devcards_init(void);
void data_init(void);
int build_routine(int player, int ispregame);
void roll(int player);
int devcards_routine(int player, int hascarded);
void ai_pregame(int player);
void data_pregameres(int p, int vert);
void ai_routine(int player, int hascarded);
int setup_routine(void);
void map_create(int randomnums);
int devcards_knightoption(int player);
void io_printgeneric(char *s);
void devcards_newturn(void);
int data_sleep(void);
int map_getcolhex(int hex);
char* map_getresname(int res);
void robber_place(int hex);
int map_islegalvert(int pos);
int map_getcolmapvert(int vert);
void marker_erase(int pos);
void marker_place(int pos);
int ai_accepttrade(int *trade, int player, int playerTrading);
int* trade_routine(int player, int credits, int type, int *start);
int* trade_routine_other(int player, int credits, int type);
void io_selectTrade(int accepted[3], int cursor, int player);
void trade_up(int credits, int type);
void setup(int *start, int player);
void trade_refresh(void);
void trade_down(int player);
int marker_getposition(void);
void marker_show(void);
int map_getroadnum(int pos, int direction);
void data_road(int p, int pos1, int pos2);
int data_elementsremaining(int p, int item);
int* data_getroad(int player, int road);
int* ai_surroundingverts(int vert, int *array);
int map_getrowhex(int hex);
char* map_itoa(int num);
void map_setdat(int index, int pos, int value);
int map_getrobberhex(void);
int data_totresources(int p);
int data_isai(int p);
int data_atvertex(int p, int vertex);
void ai_discard(int player, int num);
int getupperleft(int hex);
void marker_hide(void);
void ai_moverobber(int player);
void pass_between_humans(int to);
int data_multihumans(void);
char* itoa(int i);
char* cat(char a[], char b[]);
int io_getkey(void);
void io_printmap(int printsdat);
void io_printtrade(void);
int data_getresource(int p, int res);
int data_addresource(int p, int res, int num);
void data_refresh(int p);
void map_setmessage(char *s);
void devcards_domonopoly(int player, int res);
int player_trade(int player, int* tr);
void robber_routine(int player);
void marker_move(int direction);
int ai_playknight(int player);
int devcards_knight(int player);
int decasleep(int ds);
