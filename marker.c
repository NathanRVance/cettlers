#include "settlers.h"

static int position;
static char buffer;
static int marker_left(int pos);
static int marker_right(int pos);
static int marker_down(int pos);
static int marker_up(int pos);

void marker_setposition(int pos) {
	marker_hide();
	position = pos;
	marker_place(pos);
}

void marker_hide() {
	marker_erase(position);
}

void marker_show() {
	marker_hide();
	marker_place(position);
}

void marker_move(int direction) {
	marker_erase(position);
	switch (direction) {
	case LEFT:
		position = marker_left(position);
		break;
	case RIGHT:
		position = marker_right(position);
		break;
	case UP:
		position = marker_up(position);
		break;
	case DOWN:
		position = marker_down(position);
		break;
	default:
		break;
	}
	marker_place(position);
}

void marker_erase(int pos) {
	char str[] = { buffer, '\0' };
	util_strreplace(map[map_getrowmapvert(pos)], "*", str, 1);
}

void marker_place(int pos) {
	buffer = util_strreplace(map[map_getrowmapvert(pos)], VERTEX ".",
			VERTEX "*", map_getcolmapvert(pos) + 1);
}

static int marker_left(int pos) {
	do {
		pos -= 1;
		if (pos < 0)
			pos = 63;
	} while (!map_islegalvert(pos));
	return pos;
}

static int marker_right(int pos) {
	do {
		pos += 1;
		if (pos > 63)
			pos = 2;
	} while (!map_islegalvert(pos));
	return pos;
}

static int marker_down(int pos) {
	do {
		pos += 6;
		if (pos > 63)
			pos -= 60;
	} while (!map_islegalvert(pos));
	return pos;
}

static int marker_up(int pos) {
	do {
		pos -= 6;
		if (pos < 0)
			pos += 60;
	} while (!map_islegalvert(pos));
	return pos;
}

int marker_getposition() {
	return position;
}
