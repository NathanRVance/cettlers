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
int* get_intarray(int size);

int rate_hand(int *hand, int player)
{
 int sum = 0;
 if(hand[WHEAT] >= 2 && hand[STONE] >= 3 && ai_canupgrade(player))
  sum += 10;
 if(hand[WOOD] && hand[WHEAT] && hand[SHEEP] && hand[BRICK] && ai_cansettle(player))
  sum += 9;
 else if(hand[WOOD] && hand[BRICK] && ai_canroad(player, 0)[0] > 0)
  sum += 8;
 if(hand[WHEAT] && hand[SHEEP] && hand[STONE])
  sum += 7;
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

int* gethand(int player)
{
 int *hand = get_intarray(5);
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

int ai_playertrade(int player, int *trade, int minRating)
{
 static int trades[4][6];
 int p, i;
 for(i = 0; i < 6; i++) trades[player][i] = 0;
 for(p = 1; p < 4; p++) {
  if(data_vps((player+p)%4) < 7) {
   int *t = accepttrade(trade, (player+p)%4, player);
   for(i = 0; i < 6; i++) trades[(player+p)%4][i] = t[i];
  } else {
   trades[(player+p)%4][5] = 0; //we won't even consider those close to winning
  }
 }
 int max, maxp, *hand;
 max = minRating;
 maxp = player;
 for(p = 1; p < 4; p++) {
  if(trades[(player+p)%4][5] > 0) {
   hand = gethand(player);
   for(i = 0; i < 5; i++) hand[i] -= trades[(player+p)%4][i];
   if(rate_hand(hand, player) > max) {
    max = rate_hand(hand, player);
    maxp = (player+p)%4;
   }
  }
 }
 doTrade(trades[maxp], player, maxp);
 return (maxp != player);
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
   traded += 1;
  } else break;
 }
 if(traded) return 1;
 //Now player trades
 int k, p, l, trade[5];
 hand = gethand(player);
 rating = rate_hand(hand, player);
 for(k = 1; k <= 2; k++) {
  for(i = 0; i < 5; i++) {
   if(hand[i] >= k) {
    for(j = 0; j < 5; j++) {
     hand = gethand(player);
     hand[j]++;
     hand[i] -= k;
     for(l = 0; l < 5; l++) trade[l] = 0;
     trade[j] = -1;
     trade[i] = k;
     n = rate_hand(hand, player);
     if(n > rating + 5) {//5 chosen arbitrarily
      if(ai_playertrade(player, trade, rating)) {
       rating = rate_hand(gethand(player), player);
       traded++;
       break;
      }
     }
    }
   }
  }
 }
 return traded;
}
