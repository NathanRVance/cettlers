#define INF 100
#define DELEN 50

int data_atvertex(int p, int vertex);

int deadends[DELEN];

static int get_key(int map[], int n, int value) {
	int i = 0;
	while (map[i++] != value && i < n)
		;
	i--;
	if (i == n && map[i] != value)
		return -1;
	return i;
}

static int* roadlength_prepare(int roads[], int edges, int player) {
	int i, j, k;
	for (i = 0; i < DELEN; i++)
		deadends[i] = 0;
	int map[edges * 2 + 1];
	for (i = 0; i < edges * 2 + 1; i++)
		map[i] = INF;
	for (i = 0;; i++) {
		for (j = 0; j < edges * 2; j++) {
			if ((i == 0 && map[i] > roads[j])
					|| (i != 0 && map[i - 1] < roads[j] && map[i] > roads[j]))
				map[i] = roads[j];
		}
		if (map[i] == INF) {
			for (; i < edges * 2 + 1; i++)
				map[i] = 0;
			break;
		}
	}
	static int graph[15 * 2 * 3]; //space to hold the maximum 2 verts per edge and the maximum 3 connected verts
	for (i = 0; i < 15 * 2 * 3; i++)
		graph[i] = INF;
	//For every x%3 = 0, vert x/3 is connected to graph[x], graph[x+1], and graph[x+2].
	i = j = k = 0;
	for (i = 0; i < edges * 2; i++) {
		for (j = 0; j < 3; j++) {
			if (graph[get_key(map, edges * 2, roads[i]) * 3 + j] == INF) {
				graph[get_key(map, edges * 2, roads[i]) * 3 + j] = get_key(map,
						edges * 2, roads[i + (i % 2) * -2 + 1]);
				break;
			}
		}
	}
	for (i = 0; map[i] != 0; i++) {
		for (j = 1; j < 4; j++) {
			if (data_atvertex((player + j) % 4, map[i]) >= 1)
				deadends[i] = 1;
		}
	}
	return graph;
}

int not_beento(int beento[], int n) {
	int i;
	for (i = 0; i < 30; i++)
		if (beento[i] == n)
			return 0;
	return 1;
}

int dfs(int start, int graph[], int beento[], int parent) {
	int k, i, max, len;
	max = len = 0;
	for (k = 0; k < 3; k++) {
		if (graph[start * 3 + k] != INF
				&& not_beento(beento, graph[start * 3 + k])) {
			for (i = 0; beento[i] != INF; i++)
				;
			beento[i] = start;
			if (deadends[graph[start * 3 + k]])
				len = 1;
			else
				len = dfs(graph[start * 3 + k], graph, beento, start);
			if (max < len)
				max = len;
			beento[i] = INF;
		}
		if (graph[start * 3 + k] != INF
				&& !not_beento(beento, graph[start * 3 + k])
				&& graph[start * 3 + k] != parent) {
			if (max == 0)
				max = 1;
		}
	}
	return max + 1; //Returns 1 even for a dead end, corrected later so recursion works
}

int roadlength_main(int roads[], int player) {
	int numroads = -1;
	while (roads[++numroads] != 0)
		;
	numroads /= 2;
	int max = 0;
	int i, len;
	int* graph = roadlength_prepare(roads, numroads, player);
	static int beento[30];
	for (i = 0; i < 30; i++)
		beento[i] = INF;
	for (i = 0; graph[i * 3] != INF; i++) {
		len = dfs(i, graph, beento, INF);
		if (len > max)
			max = len;
	}
	if (max == 0)
		max++;
	return max - 1; //See above note in return line for dfs
}
