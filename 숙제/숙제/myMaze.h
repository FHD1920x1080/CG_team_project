#include "stdafx.h"
#include "myRobot.h"

class MazeMap3D {
public:
	MyBox** Map;
	int h, w;
	float max_height = 0;
	float min_height = 0;
	float down_maze_intensity = 0;
	float low_height_intensity = 0;
	bool low_state = 0;
	void init(int ih, int iw) {
		h = ih;
		w = iw;
		Map = new MyBox*[h];
		for (int i = 0; i < h; i++) {
			Map[i] = new MyBox[w];
		}
	}
	void delete_Map() {
		for (int i = 0; i < h; i++) {
			delete[] Map[i];
		}
		delete[] Map;
	}
};

class Room {
public:
	int y;//세로 인덱스-> 위치
	int x;//가로 인덱스-> 위치
	int visit = 0;//방문 여부
	Room* prev = NULL;//전에 있던 방
	int dir[4][2]; //4가지 방향
	int dir_index = 0;
	void init(int iy, int ix) {
		y = iy;
		x = ix;
		int rand_dir[4][2];
		rand_dir[0][0] = iy + 1; //아래
		rand_dir[0][1] = ix;
		rand_dir[1][0] = iy; //오른쪽
		rand_dir[1][1] = ix + 1;
		rand_dir[2][0] = iy - 1; // 위
		rand_dir[2][1] = ix;
		rand_dir[3][0] = iy; //왼쪽
		rand_dir[3][1] = ix - 1;
		int num[4] = { -1, -1, -1, -1 };
		int r;
		bool insert_able;
		for (int i = 0; i < 4; i++) {
			while (1) {
				insert_able = true;
				r = rand() % 4;
				for (int j = 0; j < i; j++) {
					if (r == num[j]) {
						insert_able = false;
						break;
					}
				}
				if (insert_able == true) {
					num[i] = r;
					break;
				}
			}
		}
		//이제 num[] 에는 0,1,2,3 랜덤 순서로 들어가 있음.
		for (int i = 0; i < 4; i++) {
			dir[i][0] = rand_dir[num[i]][0];
			dir[i][1] = rand_dir[num[i]][1];
		}
		//이제 dir에 4방향이 랜덤 순서로 있음.
	}
};

class MazeMap {
public:
	bool** Map;
	int h, w;
	int start_y;
	int start_x;
	int end_y;
	int end_x;
	void init(int ih, int iw) {
		h = ih;
		w = iw;
		Map = new bool* [h];
		for (int i = 0; i < h; i++) {
			Map[i] = new bool[w];
			for (int j = 0; j < w; j++) {
				Map[i][j] = 1;
			}
		}
		if (h < 3 || w < 3) {
			printf("미로를 생성할만한 기둥 수가 안됨\n");
			return;
		}
		start_y = 0;
		if (w % 2 != 0) {//홀수 15, 12, 6, 0~12, 1~13
			start_x = (rand() % ((w - 1) / 2)) * 2 + 1;
		}
		else
			start_x = (rand() % ((w - 2) / 2)) * 2 + 1;
		make_maze(this);
	}
	void break_wall(MazeMap* maze, Room* prev, Room* room, Room** room_matrix, int room_h, int room_w) {
		room->prev = prev;
		int y, x;
		if (room->prev == NULL) {
			maze->Map[maze->start_y][maze->start_x] = 0;// 여기 시작지점.
		}
		else {
			y = prev->y - room->y;
			x = prev->x - room->x;
			maze->Map[(room->y + 1) * 2 - 1 + y][(room->x + 1) * 2 - 1 + x] = 0;
			//printf("여기%d %d\n", y, x);
		}
		room->visit = 1;
		maze->Map[(room->y + 1) * 2 - 1][(room->x + 1) * 2 - 1] = 0;
		//printf("현재 좌표: %d %d \n", room->y, room->x);
		while (1) {
			if (room->dir_index > 3)
				break;
			int ny, nx;
			ny = room->dir[room->dir_index][0];
			nx = room->dir[room->dir_index][1];

			room->dir_index += 1;
			if (ny >= 0 && ny < room_h && nx >= 0 && nx < room_w) {
				if (room_matrix[ny][nx].visit != 1)
					break_wall(maze, room, &room_matrix[ny][nx], room_matrix, room_h, room_w);
				//else
					//printf("이미 방문\n");
			}
			//else
				//printf("범위 밖\n");
		}

	}
	void make_maze(MazeMap* maze) {
		int h = maze->h;
		int w = maze->w;
		if (h % 2 == 0) {
			h--;
			for (int i = 0; i < maze->w; i++) {
				maze->Map[h][i] = 0;
			}
		}
		if (w % 2 == 0) {
			w--;
			for (int i = 0; i < maze->h; i++) {
				maze->Map[i][w] = 0;
			}
		}
		int room_h = h / 2;
		int room_w = w / 2;
		Room** room_matrix = new Room * [room_h];
		for (int i = 0; i < room_h; i++) {
			room_matrix[i] = new Room[room_w];
			for (int j = 0; j < room_w; j++) {
				room_matrix[i][j].init(i, j);
			}
		}
		break_wall(maze, NULL, &room_matrix[0][(maze->start_x - 1) / 2], room_matrix, room_h, room_w);
		while (1) {//끝 지점
			int a = 1 + rand() % (w - 2);
			if (maze->Map[h - 2][a] == 0) {
				maze->Map[h - 1][a] = 0;
				maze->end_y = h - 1;
				maze->end_x = a;
				break;
			}
		}
		for (int i = 0; i < room_h; i++) {
			delete[] room_matrix[i];
		}
		delete[] room_matrix;
	}
	void delete_Map() {
		for (int i = 0; i < h; i++) {
			delete[] Map[i];
		}
		delete[] Map;
	}
};
