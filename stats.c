#include "settlers.h"

int* get_intarray(int size);

#define ROLLSLEN 100
static int rolls[ROLLSLEN];
static int *rollsp = rolls;

static int turns = 0;

//rollsp points to next empty slot
void stats_roll(int roll)
{
 *rollsp++ = roll;
 if(rollsp > rolls+ROLLSLEN) rollsp = rolls;
 turns++;
}

int* stats_getlast5rolls(void)
{
 int *ret = get_intarray(5);
 int *start = rollsp - 5;
 if(start < rolls) start += ROLLSLEN;
 int i;
 for(i = 0; i < 5; i++, start++) {
  if(start > rolls+ROLLSLEN) start -= ROLLSLEN;
  ret[i] = *start;
 }
 return ret;
}

int stats_numturns(void)
{
 return turns;
}
