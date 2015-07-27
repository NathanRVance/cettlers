#include "color.h"
#include "settlers.h"

static int position;

int road_legalpos(int player, int pos) {
	if (data_atvertex(player, pos) >= 0) {
		int i;
		for (i = 0; i < 3; i++)
			if (data_atvertex(++player % 4, pos) > 0)
				return 0;
		return 1;
	}
	return 0;
}

int map_islegalvert(int pos);

//checks if pos1 and pos2 are adjacent
int road_areadjacent(int pos1, int pos2) {
	if (!map_islegalvert(pos1) || !map_islegalvert(pos2))
		return 0;
	int surrounding[3];
	ai_surroundingverts(pos1, surrounding);
	int i;
	for (i = 0; i < 3; i++) {
		if (surrounding[i] == pos2)
			return 1;
	}
	return 0;
}

int road_freeedge(int pos1, int pos2) {
	if (pos1 == pos2)
		return 0;
	int pos = (pos1 < pos2) ? pos1 : pos2;
	pos2 = (pos1 > pos2) ? pos1 : pos2;
	pos1 = pos;
	if (!road_areadjacent(pos1, pos2))
		return 0;
	int i, j;
	int *road;
	for (i = 0; i < 4; i++)
		for (j = 0; j < 15; j++) {
			road = data_getroad(i, j);
			if ((road[0] == pos1 && road[1] == pos2)
					|| (road[1] == pos1 && road[0] == pos2))
				return 0;
		}
	return 1;
}

void road_erase(int player) {
	int i;
	for (i = 0; i < MAPROWS; i++)
		util_strreplace(map[i], cat(getcolor(player), BLINK), "", 0);
	position = 0;
}

int road_build(int player, int pregame) {
	if (!position || !road_freeedge(marker_getposition(), position))
		return 0;
	int i;
	for (i = 0; i < MAPROWS; i++)
		util_strreplace(map[i], cat(getcolor(player), BLINK), getcolor(player),
				0);
	if (!pregame) {
		data_addresource(player, WOOD, -1);
		data_addresource(player, BRICK, -1);
	}
	data_road(player, marker_getposition(), position);
	if (!pregame)
		map_setmessage(cat(cat("Player ", itoa(player + 1)), " built a road"));
	return 1;
}

int road_left_prospect(int player) {
	int pos = marker_getposition();
	int prospos = pos - 1;
	road_erase(player);
	if (!road_freeedge(pos, prospos)
			|| (pos % 12 == 0 || pos % 12 == 2 || pos % 12 == 4 || pos % 12 == 6
					|| pos % 12 == 7 || pos % 12 == 9 || pos % 12 == 11))
		return 0;
	else {
		int i = map_getrowmapvert(pos);
		util_strreplace(map[i], VERTEX "____" VERTEX "*",
				cat(getcolor(player), BLINK VERTEX "____" VERTEX "*"), 1);
		util_strreplace(map[i], VERTEX "___" VERTEX "*",
				cat(getcolor(player), BLINK VERTEX "___" VERTEX "*"), 1);
		util_strreplace(map[i], VERTEX "^___" VERTEX "*",
				cat(getcolor(player), BLINK VERTEX "^___" VERTEX "*"), 1);
		util_strreplace(map[i], VERTEX "^__" VERTEX "*",
				cat(getcolor(player), BLINK VERTEX "^__" VERTEX "*"), 1);
		position = prospos;
		return 1;
	}
}

int road_right_prospect(int player) {
	int pos = marker_getposition();
	int prospos = pos + 1;
	road_erase(player);
	if (!road_freeedge(pos, prospos)
			|| (pos % 12 == 1 || pos % 12 == 3 || pos % 12 == 5 || pos % 12 == 6
					|| pos % 12 == 8 || pos % 12 == 10 || pos % 12 == 11))
		return 0;
	else {
		int i = map_getrowmapvert(marker_getposition());
		util_strreplace(map[i], VERTEX "*",
				cat(cat(getcolor(player), BLINK), VERTEX "*"), 1);
		position = prospos;
		return 1;
	}
}

int road_up_prospect(int player) {
	int pos = marker_getposition();
	int prospos = pos - 6;
	road_erase(player);
	if (!road_freeedge(pos, prospos)
			|| (pos == 2 || pos == 3 || pos == 7 || pos == 10 || pos == 12
					|| pos == 17))
		return 0;
	else {
		int i = map_getrowmapvert(marker_getposition());
		while (i >= map_getrowmapvert(marker_getposition()) - 1)
			util_strreplace(map[i--], ROAD, cat(getcolor(player), BLINK ROAD),
					map_getroadnum(marker_getposition(), 0) + 1);
		position = prospos;
		return 1;
	}
}

int road_down_prospect(int player) {
	int pos = marker_getposition();
	int prospos = pos + 6;
	road_erase(player);
	if (!road_freeedge(pos, prospos)
			|| (pos == 48 || pos == 53 || pos == 55 || pos == 58 || pos == 62
					|| pos == 63))
		return 0;
	else {
		int i = map_getrowmapvert(marker_getposition()) + 1;
		while (i <= map_getrowmapvert(marker_getposition()) + 2)
			util_strreplace(map[i++], ROAD, cat(getcolor(player), BLINK ROAD),
					map_getroadnum(marker_getposition(), 1) + 1);
		position = prospos;
		return 1;
	}
}

/* Direction (dir) is:
 0: up
 1: down
 2: left
 3: right
 */
int road_prospect(int player, int dir) {
	//if the direction doesn't work for the position, we erase.
	marker_show();
	switch (dir) {
	case 0:
		return road_up_prospect(player);
		break;
	case 1:
		return road_down_prospect(player);
		break;
	case 2:
		return road_left_prospect(player);
		break;
	case 3:
		return road_right_prospect(player);
		break;
	default:
		return 0;
		break;
	}
}

int road_routine(int player, int pregame) {
	int build = 1;
	position = 0;
	if (!road_legalpos(player, marker_getposition())) {
		map_setmessage("Error, can't build here.");
		return 0;
	}
	if (!pregame) {
		if (data_elementsremaining(player, 3) < 1) {
			map_setmessage("Error, out of roads!");
			return 0;
		} else if (!(data_getresource(player, WOOD) >= 1)
				|| !(data_getresource(player, BRICK) >= 1)) {
			map_setmessage("Error, not enough resources.");
			return 0;
		}
	}
	while (build) {
		io_printmap(!data_isai(player) && !pregame);
		switch (io_getkey()) {
		case UP:
			road_prospect(player, 0);
			break;
		case DOWN:
			road_prospect(player, 1);
			break;
		case LEFT:
			road_prospect(player, 2);
			break;
		case RIGHT:
			road_prospect(player, 3);
			break;
		case ENTER:
		case 'r':
			if (road_build(player, pregame))
				return 1;
			break;
		default:
			if (!pregame) {
				build = 0;
				road_erase(player);
			}
			break;
		}
	}
	return 1;
}

int road_vertbuild(int player, int origin, int dest, int pregame) {
	marker_setposition(origin);
	int dir = 0;
	if (dest == origin - 1)
		dir = 2; //left
	else if (dest == origin + 1)
		dir = 3; //right
	else if (dest < origin)
		dir = 0; //up
	else if (dest > origin)
		dir = 1; //down
	int ret = road_prospect(player, dir);
	if (ret)
		road_build(player, pregame);
	return ret;
}
