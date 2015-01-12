#define NRM  "\x1B[0m"
#define RED  "\x1B[31m"
#define GRN  "\x1B[32m"
#define YEL  "\x1B[33m"
#define BLU  "\x1B[34m"
#define MAG  "\x1B[35m"
#define CYN  "\x1B[36m"
#define WHT  "\x1B[37m"
#define BRN  "\x1B[38;5;94m"
#define COLOR_BROWN 8
#define BLINK "\x1B[5m"

#define P1     RED
#define P2     GRN
#define P3     YEL
#define P4     BLU

#define IOP1   31
#define IOP2   32
#define IOP3   33
#define IOP4   34

static char* getcolor(int player)
{
 switch(player) {
  case 0: return P1;
  case 1: return P2;
  case 2: return P3;
  case 3: return P4;
  default: return "error";
 }
}
