
#define ROLLSLEN 100
static int rolls[ROLLSLEN];
static int *rollsp = rolls;

static int numrolls = 0;

//rollsp points to next empty slot
void stats_roll(int roll) {
	*rollsp++ = roll;
	if (rollsp > rolls + ROLLSLEN)
		rollsp = rolls;
	numrolls++;
}

int* stats_getlast5rolls(int ret[5]) {
	int *start = rollsp - 5;
	if (start < rolls)
		start += ROLLSLEN;
	int i;
	for (i = 0; i < 5; i++, start++) {
		if (start > rolls + ROLLSLEN)
			start -= ROLLSLEN;
		ret[i] = *start;
	}
	return ret;
}

int stats_numrolls(void) {
	return numrolls;
}
