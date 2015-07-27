#include <string.h>

#include "settlers.h"

//static int t[6]; //The 5 resources and trade type
static int tCursor; //The selected resource
static int tCredits;
static int tType;
static int trading;

int tCursor_move(int direction) {
	if ((tCursor += direction) < 0)
		tCursor += 6;
	return tCursor %= 6;
}

void trade_down(int player) {
	int i;
	for (i = 0; i < 5; i++)
		data_addresource(player, i, trade[i] * -1);
	for (i = 0; i < TRADECOLS; i++)
		trade[i] = 0;
	data_refresh(player);
}

void trade_refresh() {
	trade[5] = tCursor;
	switch (tType) {
	case 0:
		strcpy(&trade[6], "player");
		break;
	case 1:
		strcpy(&trade[6], "port");
		break;
	case 2:
		strcpy(&trade[6], cat("discard ", itoa(tCredits * -1)));
		break;
	case 3:
		strcpy(&trade[6], cat("take ", itoa(tCredits)));
		break;
	case 4:
		strcpy(&trade[6], "select one");
		break;
	case 5:
		strcpy(&trade[6], "accept or counter");
		break;
	default:
		strcpy(&trade[6], "error");
		break;
	}
}

void trade_up(int credits, int type) {
	tType = type;
	tCursor = -1;
	int i;
	for (i = 0; i < TRADECOLS; i++)
		trade[i] = 0;
	tCredits = credits;
}

void trade_change_enact(int change, int player) {
	trade[tCursor] += change;
	data_addresource(player, tCursor, change);
	if (change > 0)
		tCredits--;
	else
		tCredits++;
}

void trade_change(int change, int player) {
	int rate;
	if (tCursor == 5) {
		if (tType == 0 || tType == 1) { //standard trade
			trade_down(player); //wipe out previous trades
			trade_up(0, (tType - 1) * -1);
		}
	} else {
		switch (tType) {
		case 0: //player trade
			if (change == 1 || data_getresource(player, tCursor) > 0)
				trade_change_enact(change, player);
			break;
		case 1: //port trade
			rate = get_trade(player, tCursor);
			if (change == 1 && trade[tCursor] < 0)
				trade_change_enact(rate, player);
			else if (trade[tCursor] > 0 || (trade[tCursor] == 0 && change == 1))
				trade_change_enact(change, player);
			else if (change == -1 && data_getresource(player, tCursor) >= rate)
				trade_change_enact(rate * -1, player);
			break;
		case 2: //robber discard
			if (tCredits < 0 && change == -1
					&& data_getresource(player, tCursor) > 0)
				trade_change_enact(change, player);
			if (change == 1 && trade[tCursor] < 0)
				trade_change_enact(change, player);
			break;
		case 3: //year of plenty
			if (change == 1 && tCredits > 0)
				trade_change_enact(change, player);
			if (change == -1 && tCredits < 2 && trade[tCursor] > 0)
				trade_change_enact(change, player);
			break;
		case 4: //monopoly
			trading = 0;
			devcards_domonopoly(player, tCursor);
			break;
		case 5: //incoming trade
			if (change == 1 || data_getresource(player, tCursor) > 0)
				trade_change_enact(change, player);
			break;
		default:
			break;
		}
	}
}

int perform_trade(int player) {
	int i;
	if (tType == 0) {
		int tr[5];
		for (i = 0; i < 5; i++)
			tr[i] = trade[i] * -1;
		trade_down(player);
		return player_trade(player, tr);
	}
	if (tCredits == 0) {
		for (i = 0; i < 5; i++)
			trade[i] = 0;
		return 1;
	}
	if (tCredits < 0)
		map_setmessage("Trade requires more resources");
	else
		map_setmessage("Trade requires fewer resources");
	return 0;
}

//Set trade to start
void setup(int *start, int player) {
	for (tCursor = 0; tCursor < 5; tCursor++)
		trade_change_enact(start[tCursor], player);
	tCursor = -1;
}

int* trade_routine_other(int player, int credits, int type) {
	int start[5];
	if(type == 5) {
		for(int i = 0; i < 5; i++)
			start[i] = i;
	}
	return trade_routine(player, credits, type, start);
}

/*
 type is:
 0: 1: outgoing player/port trade
 2: Robber discard
 3: Year of plenty take 2
 4: Monopoly selection
 5: Incoming trade
 */
int* trade_routine(int player, int credits, int type, int *start) {
	trade_up(credits, type);
	static int ret[5];
	if (type == 5) {
		setup(start, player);
	}
	trading = 1;
	while (trading) {
		data_refresh(player);
		trade_refresh();
		io_printmap(1);
		io_printtrade();
		switch (io_getkey()) {
		case LEFT:
			tCursor_move(-1);
			break;
		case RIGHT:
			tCursor_move(1);
			break;
		case UP:
			if (tCursor != -1)
				trade_change(-1, player);
			break;
		case DOWN:
			if (tCursor != -1)
				trade_change(1, player);
			break;
		case ENTER:
		case 't':
			if (type == 4)
				devcards_domonopoly(player, tCursor);
			if (type == 5) {
				int i;
				for (i = 0; i < 5; i++)
					ret[i] = trade[i];
				trade_down(player);
				return ret;
			}
			if (type == 4 || perform_trade(player)) {
				trading = 0;
				trade_down(player);
			}
			break;
		case 'c':
			trade_down(player);
			int i;
			for (i = 0; i < 5; ret[i++] = 0)
				;
			return ret;
		default:
			map_setmessage("Undefined key.");
			break;
		}
	}
	return ret;
}
