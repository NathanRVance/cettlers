#include "color.h"
#include "settlers.h"

/* The builder's job is to charge for the placement of the map element,
 check if it's even available, and produce an error message accordingly.
 It then updates the map with the built element. */

int builder_placesettlement(int pos, int player, int isPregame) {
	if (!data_poslegal(pos)) {
		map_setmessage(
				"It's illegal to build within one gridspace of another settlement or city.");
		return 0;
	}
	if (data_elementsremaining(player, 1) == 0) {
		map_setmessage("Error, maximum 5 settlements!");
		return 0;
	}
	if (!isPregame) {
		if (data_getresource(player, WOOD) > 0
				&& data_getresource(player, SHEEP) > 0
				&& data_getresource(player, BRICK) > 0
				&& data_getresource(player, WHEAT) > 0) {
			data_addresource(player, WOOD, -1);
			data_addresource(player, SHEEP, -1);
			data_addresource(player, BRICK, -1);
			data_addresource(player, WHEAT, -1);
		} else {
			map_setmessage("You don't have enough resources.");
			return 0;
		}
	}
	char* color = getcolor(player);
	int row = map_getrowvert(pos);
	int col = map_getcolvert(pos);
	//if col is even and row is even, build to the left.
	//if col is odd and row is odd, build to the left.
	//Otherwise, build to the right.

	if ((row % 2) == (col % 2)) {
		util_strreplace(map[map_getrowmapvert(pos)], VERTEX "*.",
				cat(color, "@" NRM VERTEX "*"), 1);
//  util_strreplace(map[map_getrowmapvert(pos)], cat(cat(color, color), "@" NRM VERTEX "*"), cat(cat(cat(color, "@" NRM), color), VERTEX "*"), 1);
		int i;
		char *color2;
		for (i = 0; i < 4; i++) {
			color2 = getcolor(i);
			util_strreplace(map[map_getrowmapvert(pos)],
					cat(cat(color2, color), "@" NRM VERTEX "*"),
					cat(cat(cat(color, "@" NRM), color2), VERTEX "*"), 1);
		}
	} else {
		util_strreplace(map[map_getrowmapvert(pos)], "." VERTEX "*",
				cat(cat(VERTEX "*", color), "@" NRM), 1);
	}
	data_place(player, pos);
	map_setmessage(
			cat(cat("Player ", itoa(player + 1)), " built a settlement"));
	return 1;
}

int builder_placecity(int pos, int player) {
	if (data_elementsremaining(player, 2) == 0) {
		map_setmessage("Error, maximum 4 cities!");
		return 0;
	}
	if (data_getresource(player, WHEAT) > 1
			&& data_getresource(player, STONE) > 2) {
		data_addresource(player, WHEAT, -2);
		data_addresource(player, STONE, -3);
	} else {
		map_setmessage("You don't have enough resources.");
		return 0;
	}
	char* color = getcolor(player);
	util_strreplace(map[map_getrowmapvert(pos)], "@" NRM VERTEX "*",
			"#" NRM VERTEX "*", 1);
	util_strreplace(map[map_getrowmapvert(pos)],
			cat(cat(VERTEX "*", color), "@"), cat(cat(VERTEX "*", color), "#"),
			1);
	util_strreplace(map[map_getrowmapvert(pos)],
			cat(cat("@" NRM, color), VERTEX "*"),
			cat(cat("#" NRM, color), VERTEX "*"), 1);
	data_upgrade(player, pos);
	map_setmessage(cat(cat("Player ", itoa(player + 1)), " built a city"));
	return 1;
}

int build_routine(int player, int ispregame) {
	int position = marker_getposition();
	int atVertex = data_atvertex(player, position);
	if(atVertex == -1 && ispregame) atVertex = 0;

	switch (atVertex) {
	case -1:
		map_setmessage("Must have road here first");
		break;
	case 0:
		return builder_placesettlement(position, player, ispregame);
		break;
	case 1:
		if (ispregame) {
			map_setmessage("Cannot build city in pregame!");
			break;
		}
		return builder_placecity(position, player);
		break;
	default:
		map_setmessage("City already built, cannot build further here.");
	}
	return 0;
}
