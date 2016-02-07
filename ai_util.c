#include "settlers.h"

int* ai_surroundinghexes(int vert) {
	static int ret[3];
	int vnum = vert % 12;

	//find the hex above
	if (vnum < 6)
		ret[0] = vnum / 2 - 5 + 5 * (vert / 12);
	else if (vnum > 6 && vnum < 11)
		ret[0] = (vnum - 1) / 2 - 5 + 5 * (vert / 12);
	else
		ret[0] = -100;

	//hex below
	if (ret[0] != -100)
		ret[1] = ret[0] + 5;
	else
		ret[1] = -100;

	//hex to the side
	if (vnum < 5 && vnum > 0)
		ret[2] = (vnum - 1) / 2 - 2 + 5 * (vert / 12);
	else if (vnum > 5)
		ret[2] = vnum / 2 - 3 + 5 * (vert / 12);
	else
		ret[2] = -100;

	return ret;
}

int abs(int i) {
	if (i < 0)
		i *= -1;
	return i;
}

/*
 rate[resource] is its weight, with rarer resources getting a larger number. Rates are approx 1-5
 */
int* ai_rateres(int player) {
	static int res[5];
	int weight[5];
	int mapweight[5];
	int i;
	for (i = 0; i < 5; i++)
		weight[i] = 0;
	for (i = 0; i < 5; i++)
		mapweight[i] = 0;
	for (i = 0; i < 22; i++) {
		if (map_getdat(i, 0) != 5 && map_getdat(i, 0) != -1) {
			weight[map_getdat(i, 0)] += data_onhex(player, i)
					* (abs(map_getdat(i, 1) - 7) * -1 + 6);
			mapweight[map_getdat(i, 0)] += (abs(map_getdat(i, 1) - 7) * -1 + 6);
		}
	}

	int max = 0;
	int mapmax = 0;
	for (i = 0; i < 5; i++) {
		if (max < weight[i])
			max = weight[i];
		if (mapmax < mapweight[i])
			mapmax = mapweight[i];
	}
	for (i = 0; i < 5; i++) {
		res[i] = 4 - mapweight[i] * 3 / mapmax;
	}
	if (max == 0) {
		return res;
	}
	for (i = 0; i < 5; i++) {
		res[i] += 5 - weight[i] * 4 / max;
	}
	return res;
}

//Weighs a hex based on its resource and number
int ai_weight(int player, int hex) {
	int num = map_getdat(hex, 1);
	if (num == 0)
		return 0;
	if (num > 7)
		num = num * -1 + 14;
	if (num > 0)
		num--;
	num += ai_rateres(player)[map_getdat(hex, 0)];
	return num;
}

//weighs the vert based on surrounding hexes
int ai_vertweight(int player, int vert) {
	if (!map_islegalvert(vert))
		return 0;
	int *hexes = ai_surroundinghexes(vert);
	int weight = 0;
	int i;
	for (i = 0; i < 3; i++) {
		if (hexes[i] > 0 && hexes[i] < 22 && hexes[i] != 2 && hexes[i] != 20)
			weight += ai_weight(player, hexes[i]);
	}
	int port = data_getport(vert);
	if (port == -1)
		return weight;
	if (port == 5 && !data_hasanyport(player))
		weight += 4;
	else {
		weight += ai_rateres(player)[port] * -1 + 5;
	}
	return weight;
}

int map_getrowvert(int vert);
int map_getcolvert(int vert);

int* ai_surroundingverts(int vert, int sur[3]) {
	sur[0] = sur[1] = sur[2] = 0;
	if (map_getrowvert(vert) % 2 == map_getcolvert(vert) % 2) {
		if ((vert - 11) % 12 != 0) {
			sur[0] = vert + 1;
		}
	} else if ((vert + 6) % 12 != 0) {
		sur[0] = vert - 1;
	}
	sur[1] = vert + 6;
	sur[2] = vert - 6;
	return sur;
}

int getupperleft(int hex); //returns upper left vertex

int* ai_vertsonhex(int hex, int sur[6]) {
	sur[0] = getupperleft(hex);
	sur[1] = sur[0] + 1;
	sur[2] = sur[0] + 6;
	sur[3] = sur[0] + 7;
	sur[4] = sur[0] + 12;
	sur[5] = sur[0] + 13;
	return sur;
}

//returns destination vertex and its weight
int* ai_roadfromvert(int player, int vert, int ret[2]) {
	int i, j, weight, w;
	int maxweight = 0;
	int dest = 0;
	int sur[3];
	ai_surroundingverts(vert, sur);
	int handicap = 0;
	int roadlen, newroadlen, nextlongest;
	nextlongest = 0;
	for (j = 1; j < 4; j++) {
		if (data_getroadlen((player + j) % 4) > nextlongest) {
			nextlongest = data_getroadlen((player + j) % 4);
		}
	}
	if (data_poslegal(vert)) {
		handicap = ai_vertweight(player, vert);
		handicap *= handicap; //square it!
	}
	for (i = 0; i < 3; i++) {
		weight = handicap * -1;
		if (road_freeedge(vert, sur[i])) {
			roadlen = data_hypotheticalroadlength(player, ret, 0);
			ret[0] = vert;
			ret[1] = sur[i];
			newroadlen = data_hypotheticalroadlength(player, ret, 1);
			if (newroadlen >= 5 && newroadlen > nextlongest && newroadlen > roadlen) {
				weight += 5;
			}
			if (data_poslegal(sur[i]))
				weight += ai_vertweight(player, sur[i])
						* ai_vertweight(player, sur[i]) * 2; //we won't count this one again in the following loop
			int sur2[3];
			ai_surroundingverts(sur[i], sur2);
			w = 0;
			for (j = 0; j < 3; j++) {
				if (road_freeedge(sur[i], sur2[j]) && data_poslegal(sur2[j]))
					if (ai_vertweight(player, sur2[j]) > w)
						w = ai_vertweight(player, sur2[j]);
			}
			weight += w * w; //square it!
			if (weight > maxweight) {
				maxweight = weight;
				dest = sur[i];
			}
		}
	}
	ret[0] = dest;
	ret[1] = maxweight;
	return ret;
}
