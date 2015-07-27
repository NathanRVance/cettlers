#include "settlers.h"

int rate_hand(int *hand, int player) {
	int sum = 0;
	if (hand[WHEAT] >= 2 && hand[STONE] >= 3 && ai_canupgrade(player))
		sum += 10;
	if (hand[WOOD] && hand[WHEAT] && hand[SHEEP] && hand[BRICK]
			&& ai_cansettle(player))
		sum += 9;
	else if (hand[WOOD] && hand[BRICK] && ai_canroad(player, 0)[0] > 0)
		sum += 8;
	if (hand[WHEAT] && hand[SHEEP] && hand[STONE])
		sum += 0; //Don't trade to get devcards
	int i;
	int diversity = 0;
	int *rates = ai_rateres(player);
	for (i = 0; i < 5; i++) {
		sum += hand[i] * rates[i]; //numbers of resources weighted by their availability
		if (hand[i])
			diversity++;
		if (hand[i] < 0)
			sum = -1000; //Failsafe to not trade away resources you don't have
	}
	sum += diversity;
	return sum;
}

int* gethand(int player, int array[5]) {
	int i;
	for (i = 0; i < 5; i++)
		*(array + i) = data_getresource(player, i);
	return array;
}

int ai_accepttrade(int *trade, int player, int playerTrading) {
	if (data_vps(playerTrading) >= 7)
		return 0;
	int hand[5];
	gethand(player, hand);
	int rating = rate_hand(hand, player);
	int i;
	for (i = 0; i < 5; i++)
		hand[i] += trade[i];
	if (rate_hand(hand, player) > rating)
		return 1;
	else
		return 0;
}

int ai_playertrade(int player, int *trade, int minRating) {
	static int trades[4][6];
	int p, i;
	for (i = 0; i < 6; i++)
		trades[player][i] = 0;
	for (p = 1; p < 4; p++) {
		if (data_vps((player + p) % 4) < 7) {
			int t[6];
			accepttrade(trade, (player + p) % 4, player, t);
			for (i = 0; i < 6; i++)
				trades[(player + p) % 4][i] = t[i];
		} else {
			trades[(player + p) % 4][5] = 0; //we won't even consider those close to winning
		}
	}
	int max, maxp;
	int hand[5];
	max = minRating;
	maxp = player;
	for (p = 1; p < 4; p++) {
		if (trades[(player + p) % 4][5] > 0) {
			gethand(player, hand);
			for (i = 0; i < 5; i++)
				hand[i] -= trades[(player + p) % 4][i];
			if (rate_hand(hand, player) > max) {
				max = rate_hand(hand, player);
				maxp = (player + p) % 4;
			}
		}
	}
	doTrade(trades[maxp], player, maxp);
	return (maxp != player);
}

int ai_trade(int player) {
	int hand[5];
	gethand(player, hand);
	int rating = rate_hand(hand, player);
	int traded = 0;
	//start with port trades
	int i, j, max, maxi, maxj, n;
	while (1) {
		gethand(player, hand);
		max = maxi = maxj = 0;
		for (i = 0; i < 5; i++) {
			if (hand[i] >= get_trade(player, i)) {
				hand[i] -= get_trade(player, i);
				for (j = 0; j < 5; j++) {
					hand[j]++;
					n = rate_hand(hand, player);
					if (n > max && n > rating) {
						max = n;
						maxi = i;
						maxj = j;
					}
					hand[j]--;
				}
				hand[i] += get_trade(player, i);
			}
		}
		if (max) {
			data_addresource(player, maxi, get_trade(player, maxi) * -1);
			data_addresource(player, maxj, 1);
			traded += 1;
		} else
			break;
	}
	if (traded)
		return 1;
	//Now player trades
	int k, l, trade[5];
	gethand(player, hand);
	rating = rate_hand(hand, player);
	for (k = 1; k <= 2; k++) {
		for (i = 0; i < 5; i++) {
			if (hand[i] >= k) {
				for (j = 0; j < 5; j++) {
					gethand(player, hand);
					hand[j]++;
					hand[i] -= k;
					for (l = 0; l < 5; l++)
						trade[l] = 0;
					trade[j] = -1;
					trade[i] = k;
					n = rate_hand(hand, player);
					if (n > rating + 5) { //5 chosen arbitrarily
						if (ai_playertrade(player, trade, rating)) {
							int tmp[5];
							rating = rate_hand(gethand(player, tmp), player);
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
