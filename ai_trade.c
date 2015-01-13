#include "settlers.h"
int ai_cansettle(int player);
int ai_canupgrade(int player);
int* ai_canroad(int player, int free);
int* ai_rateres(int player);
int data_getresource(int p, int res);
int get_trade(int p, int res);
void data_addresource(int p, int card, int num);
void doTrade(int *trade, int player1, int player2);
int* accepttrade(int *trade, int player, int playerTrading);
int data_vps(int p);

int rate_hand(int *hand, int player)
{
 int sum = 0;
 if(hand[WHEAT] >= 2 && hand[STONE] >= 3 && ai_canupgrade(player))
  sum += 5;
 if(hand[WOOD] && hand[WHEAT] && hand[SHEEP] && hand[BRICK] && ai_cansettle(player))
  sum += 4;
 else if(hand[WOOD] && hand[BRICK] && ai_canroad(player, 0)[0] > 0)
  sum += 3;
 if(hand[WHEAT] && hand[SHEEP] && hand[STONE])
  sum += 2;
 int i;
 int diversity = 0;
 int *rates = ai_rateres(player);
 for(i = 0; i < 5; i++) {
 sum += hand[i] * rates[i]; //numbers of resources weighted by their availability
  if(hand[i]) diversity++;
  if(hand[i] < 0) sum = -1000; //Failsafe to not trade away resources you don't have
 }
 sum += diversity;
 return sum;
}

#define POOLSIZE 10000
static int handpool[POOLSIZE];
static int *poolp = handpool;

int* gethand(int player)
{
 int *hand = poolp;
 poolp += 5;
 if(poolp >= handpool + POOLSIZE) poolp = handpool;
 int i;
 for(i = 0; i < 5; i++)
  *(hand+i) = data_getresource(player, i);
 return hand;
}

int ai_accepttrade(int *trade, int player)
{
 if(data_vps(player) >= 7) return 0;
 int *hand = gethand(player);
 int rating = rate_hand(hand, player);
 int i;
 for(i = 0; i < 5; i++)
  hand[i] += trade[i];
if(rate_hand(hand, player) > rating)
  return 1;
 else
  return 0;
}

int ai_trade(int player)
{
 int *hand;
 int rating = rate_hand(hand, player);
 int traded = 0;
 //start with port trades
 int i, j, max, maxi, maxj, n;
 while(1) {
  hand = gethand(player);
  max = maxi = maxj = 0;
  for(i = 0; i < 5; i++) {
   if(hand[i] >= get_trade(player, i)) {
    hand[i] -= get_trade(player, i);
    for(j = 0; j < 5; j++) {
     hand[j]++;
     n = rate_hand(hand, player);
     if(n > max && n > rating) {
      max = n;
      maxi = i;
      maxj = j;
     }
     hand[j]--;
    }
    hand[i] += get_trade(player, i);
   }
  }
  if(max) {
   data_addresource(player, maxi, get_trade(player, maxi) * -1);
   data_addresource(player, maxj, 1);
   traded = 1;
  } else break;
 }
 //Now player trades
 int k, p, trade[5];
 for(i = 0; i < 5; i++) trade[i] = 0;
 hand = gethand(player);
 rating = rate_hand(hand, player);
 for(k = 1; i <= 2; k++) {
  for(i = 0; i < 5; i++) {
   if(hand[i] >= k) {
    hand[i] -= k;
    for(j = 0; j < 5; j++) {
     hand[j]++;
     n = rate_hand(hand, player);
     if(n > rating + 3) {//3 chosen arbitrarily
      trade[j] = -1;
      trade[i] = k;
      for(p = 1; p < 4; p++) {
       if(accepttrade(trade, (player+p)%4, player)[5] == 1) {
        doTrade(trade, player, (player+p)%4);
        traded = 1;
        break;
       }
      }
      trade[i] = trade[j] = 0;
     }
     hand = gethand(player);
    }
    hand = gethand(player);
   }
  }
 }

 return traded;
}
