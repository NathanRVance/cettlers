#include "settlers.h"

void ai_discard(int player, int num) {
	int hand[5];
	while(num > 0) {
		gethand(player, hand);
		int best = -100;
		int bestdec = -1;
		int res;
		for(res = 0; res < 5; res++) {
			if(hand[res] == 0)
				continue;
			hand[res]--;
			int rating = rate_hand(hand, player);
			if(rating > best) {
				best = rating;
				bestdec = res;
			}
			hand[res]++;
		}
		data_addresource(player, bestdec, -1);
		num--;
	}
}

void robber_erase(int hex);
void robber_place(int hex);
int robber_steal(int player);
int map_getrobberhex(void);
int data_playeratvertex(int vertex);
int data_atvertex(int p, int vertex);
int data_rank(int p); //what place is player p in
int map_getdat(int index, int pos);
int abs(int);
int* ai_vertsonhex(int hex, int array[6]);

static int ai_playerweight(int player, int p, int vert) //player is the one who rolled, p is the player on the vert
{
	if (p == -1)
		return 0;
	int num = 1;
	if (p == player)
		num *= -1;
	num *= data_atvertex(p, vert);
	if (data_rank(p) == 1)
		num *= 3;
	return num;
}

static int ai_robweight(int player, int hex) {
	int verts[6];
	ai_vertsonhex(hex, verts);
	int sum = 0;
	int i;
	for (i = 0; i < 6; i++)
		sum += ai_playerweight(player, data_playeratvertex(verts[i]), verts[i]);
	sum *= abs(map_getdat(hex, 1) - 7) * -1 + 6;
	return sum;
}

void ai_moverobber(int player) {
	int hexorig = map_getrobberhex();
	robber_erase(hexorig);
	int hexes[22], i;
	for (i = 0; i < 22; i++)
		hexes[i] = 0;
	hexes[0] = hexes[2] = hexes[20] = -1000;
	for (i = 1; i < 22; i++) {
		if (i == 2 || i == 20)
			continue;
		hexes[i] = ai_robweight(player, i);
	}
	int max, hex;
	hex = max = 0;
	for (i = 1; i < 22; i++) {
		if (hexes[i] > max && i != hexorig) {
			max = hexes[i];
			hex = i;
		}
	}
	robber_place(hex);
	int verts[6];
	ai_vertsonhex(hex, verts);
	max = 0;
	int maxp = 0;
	for (i = 0; i < 6; i++) {
		if (ai_playerweight(player, data_playeratvertex(verts[i]), verts[i])
				> max) {
			max = ai_playerweight(player, data_playeratvertex(verts[i]),
					verts[i]);
			maxp = data_playeratvertex(verts[i]);
		}
	}
	if (data_totresources(maxp) > 0)
		data_addresource(player, robber_steal(maxp), 1);
	data_refresh(player);
}
