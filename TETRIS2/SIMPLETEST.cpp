/**
 * Tetris2 ��������
 * 20171027���£���trans����ĵڶ�ά���ȴ�4�Ӵ�6����лkczno1�û���ָ����
 * https://wiki.botzone.org/index.php?title=Tetris2
 */
// ע�⣺x�ķ�Χ��1~MAPWIDTH��y�ķ�Χ��1~MAPHEIGHT
// ���������У�y�����У�c��
// ����ϵ��ԭ�������½�
#include <iostream>
#include <string>
#include <cmath>
#include <algorithm>
#include <cstdlib>
#include <vector>
#include <cassert>
#include <ctime>
#include <bitset>
#include "jsoncpp/json.h"
using namespace std;
 
#define MAPWIDTH 10
#define MAPHEIGHT 20
 
// �����ڶ������ɫ��0Ϊ�죬1Ϊ��������ʾ���飬�����Ⱥ�
int currBotColor;
int enemyColor;
 
// ��y��x����¼��ͼ״̬��0Ϊ�գ�1Ϊ��ǰ���ã�2Ϊ�ոշ��ã�����ΪԽ��
// ��2���������к����һ���������͸��Է���
int gridInfo[2][MAPHEIGHT + 2][MAPWIDTH + 2] = { 0 };
 
// ����ֱ���Է�ת�Ƶ���
int trans[2][6][MAPWIDTH + 2] = { 0 };
 
// ת������
int transCount[2] = { 0 };
 
// ����eliminate��ĵ�ǰ�߶�
int maxHeight[2] = { 0 };
 
// ����ȥ�����ķ���֮��
int elimTotal[2] = { 0 };

// �������غϷ�������ȥ��
int elimCombo[2] = { 0 };

// �������ߵĻ�ɫ������������ɫ�� ** ʹ��ע�⣺ע��ÿ�θı��Ҫ���ԭֵ�� �ı�ĺ�����Util::eliminate && Util::transfer
int yellowCount[2] = { 0 };
 
// һ������ȥ������Ӧ����
const int elimBonus[] = { 0, 1, 3, 5, 7 };
 
// ����Ӧ��ҵĸ�������Ŀ�ܼ�
int typeCountForColor[2][7] = { 0 };
 
const int blockShape[7][4][8] = {
	{ { 0,0,1,0,-1,0,-1,-1 },{ 0,0,0,1,0,-1,1,-1 },{ 0,0,-1,0,1,0,1,1 },{ 0,0,0,-1,0,1,-1,1 } },
	{ { 0,0,-1,0,1,0,1,-1 },{ 0,0,0,-1,0,1,1,1 },{ 0,0,1,0,-1,0,-1,1 },{ 0,0,0,1,0,-1,-1,-1 } },
	{ { 0,0,1,0,0,-1,-1,-1 },{ 0,0,0,1,1,0,1,-1 },{ 0,0,-1,0,0,1,1,1 },{ 0,0,0,-1,-1,0,-1,1 } },
	{ { 0,0,-1,0,0,-1,1,-1 },{ 0,0,0,-1,1,0,1,1 },{ 0,0,1,0,0,1,-1,1 },{ 0,0,0,1,-1,0,-1,-1 } },
	{ { 0,0,-1,0,0,1,1,0 },{ 0,0,0,-1,-1,0,0,1 },{ 0,0,1,0,0,-1,-1,0 },{ 0,0,0,1,1,0,0,-1 } },
	{ { 0,0,0,-1,0,1,0,2 },{ 0,0,1,0,-1,0,-2,0 },{ 0,0,0,1,0,-1,0,-2 },{ 0,0,-1,0,1,0,2,0 } },
	{ { 0,0,0,1,-1,0,-1,1 },{ 0,0,-1,0,0,-1,-1,-1 },{ 0,0,0,-1,1,-0,1,-1 },{ 0,0,1,0,0,1,1,1 } }
}; // 7����״(��L| ��L| ��z| ��z| T| ֱһ| ���)��4�ֳ���(��������)��8:ÿ���ڵ������ֱ�Ϊx��y

const int rotateBlank[7][4][10] = {
	{ { 1,1,0,0 },{ -1,1,0,0 },{ -1,-1,0,0 },{ 1,-1,0,0 } },
	{ { -1,-1,0,0 },{ 1,-1,0,0 },{ 1,1,0,0 },{ -1,1,0,0 } },
	{ { 1,1,0,0 },{ -1,1,0,0 },{ -1,-1,0,0 },{ 1,-1,0,0 } },
	{ { -1,-1,0,0 },{ 1,-1,0,0 },{ 1,1,0,0 },{ -1,1,0,0 } },
	{ { -1,-1,-1,1,1,1,0,0 },{ -1,-1,-1,1,1,-1,0,0 },{ -1,-1,1,1,1,-1,0,0 },{ -1,1,1,1,1,-1,0,0 } },
	{ { 1,-1,-1,1,-2,1,-1,2,-2,2 } ,{ 1,1,-1,-1,-2,-1,-1,-2,-2,-2 } ,{ -1,1,1,-1,2,-1,1,-2,2,-2 } ,{ -1,-1,1,1,2,1,1,2,2,2 } },
	{ { 0,0 },{ 0,0 } ,{ 0,0 } ,{ 0,0 } }
}; // ��ת��ʱ����ҪΪ�յĿ��������ת���ĵ�����
 
class Tetris
{
public:
	const int blockType;   // ��Ƿ������͵���� 0~6
	int blockX;            // ��ת���ĵ�x������
	int blockY;            // ��ת���ĵ�y������
	int orientation;       // ��Ƿ���ĳ��� 0~3
	const int(*shape)[8]; // ��ǰ���ͷ������״����
 
	int color;
 
	Tetris(int t, int color) : blockType(t), shape(blockShape[t]), color(color)
	{ }
 
	inline Tetris &set(int x = -1, int y = -1, int o = -1)
	{
		blockX = x == -1 ? blockX : x;
		blockY = y == -1 ? blockY : y;
		orientation = o == -1 ? orientation : o;
		return *this;
	}
 
	// �жϵ�ǰλ���Ƿ�Ϸ�
	inline bool isValid(int x = -1, int y = -1, int o = -1)
	{
		x = x == -1 ? blockX : x;
		y = y == -1 ? blockY : y;
		o = o == -1 ? orientation : o;
		if (o < 0 || o > 3)
			return false;
 
		int i, tmpX, tmpY;
		for (i = 0; i < 4; i++)
		{
			tmpX = x + shape[o][2 * i];
			tmpY = y + shape[o][2 * i + 1];
			if (tmpX < 1 || tmpX > MAPWIDTH ||
				tmpY < 1 || tmpY > MAPHEIGHT ||
				gridInfo[color][tmpY][tmpX] != 0)
				return false;
		}
		return true;
	}
 
	// �ж��Ƿ����
	inline bool onGround()
	{
		if (isValid() && !isValid(-1, blockY - 1))
			return true;
		return false;
	}
 
	// ����������ڳ�����
	inline bool place()
	{
		if (!onGround())
			return false;
 
		int i, tmpX, tmpY;
		for (i = 0; i < 4; i++)
		{
			tmpX = blockX + shape[orientation][2 * i];
			tmpY = blockY + shape[orientation][2 * i + 1];
			gridInfo[color][tmpY][tmpX] = 2;
		}
		return true;
	}
 
	// ����ܷ���ʱ����ת�Լ���o
	inline bool rotation(int o)
	{
		if (o < 0 || o > 3)
			return false;
 
		if (orientation == o)
			return true;
 
		int fromO = orientation;
		int i, blankX, blankY;
		while (true)
		{
			if (!isValid(-1, -1, fromO))
				return false;
 
			if (fromO == o)
				break;
	            
	        // �����ת��ײ
	        for (i = 0; i < 5; i++) {
	            blankX = blockX + rotateBlank[blockType][fromO][2 * i];
	            blankY = blockY + rotateBlank[blockType][fromO][2 * i + 1];
	            if (blankX == blockX && blankY == blockY)
	                break;
	            if (gridInfo[color][blankY][blankX] != 0)
	                return false;
	        }
 
			fromO = (fromO + 1) % 4;
		}
		return true;
	}
};
 
// ΧһȦ���Ǻ�
void init()
{
	int i;
	for (i = 0; i < MAPHEIGHT + 2; i++)
	{
		gridInfo[1][i][0] = gridInfo[1][i][MAPWIDTH + 1] = -2;
		gridInfo[0][i][0] = gridInfo[0][i][MAPWIDTH + 1] = -2;
	}
	for (i = 0; i < MAPWIDTH + 2; i++)
	{
		gridInfo[1][0][i] = gridInfo[1][MAPHEIGHT + 1][i] = -2;
		gridInfo[0][0][i] = gridInfo[0][MAPHEIGHT + 1][i] = -2;
	}
}
 
namespace Util
{
 
	// ����ܷ�ӳ��ض���ֱ���䵽��ǰλ��
	inline bool checkDirectDropTo(int color, int blockType, int x, int y, int o)
	{
		auto &def = blockShape[blockType][o];
		for (; y <= MAPHEIGHT; y++)
			for (int i = 0; i < 4; i++)
			{
				int _x = def[i * 2] + x, _y = def[i * 2 + 1] + y;
				if (_y > MAPHEIGHT)
					continue;
				if (_y < 1 || _x < 1 || _x > MAPWIDTH || gridInfo[color][_y][_x])
					return false;
			}
		return true;
	}
 
	// ��ȥ��
	void eliminate(int color)
	{
		int &count = transCount[color] = 0;
		int i, j, emptyFlag, fullFlag, firstFull = 1, hasBonus = 0;
		maxHeight[color] = MAPHEIGHT;

		int yellowFull = 0;

		for (i = 1; i <= MAPHEIGHT; i++)
		{
			emptyFlag = 1;
			fullFlag = 1;
			for (j = 1; j <= MAPWIDTH; j++)
			{
				if (gridInfo[color][i][j] == 0)
					fullFlag = 0;
				else
					emptyFlag = 0;
			}
			if (fullFlag)
			{
				if (i <= yellowCount[color])
					yellowFull++;

				if (firstFull && ++elimCombo[color] >= 3)
				{
					// ������
					for (j = 1; j <= MAPWIDTH; j++)
						trans[color][count][j] = gridInfo[color][i][j] == 1 ? 1 : 0;
					count++;
					hasBonus = 1;
				}
				firstFull = 0;
				for (j = 1; j <= MAPWIDTH; j++)
				{
					// ע������ֻת����ǰ�Ŀ飬���������һ�����µĿ飨���������һ������
					trans[color][count][j] = gridInfo[color][i][j] == 1 ? 1 : 0;
					gridInfo[color][i][j] = 0;
				}
				count++;
			}
			else if (emptyFlag)
			{
				maxHeight[color] = i - 1;
				break;
			}
			else
				for (j = 1; j <= MAPWIDTH; j++)
				{
					gridInfo[color][i - count + hasBonus][j] =
						gridInfo[color][i][j] > 0 ? 1 : gridInfo[color][i][j];
					if (count)
						gridInfo[color][i][j] = 0;
				}
		}
		if (count == 0)
			elimCombo[color] = 0;
		maxHeight[color] -= count - hasBonus;
		elimTotal[color] += elimBonus[count];

		yellowCount[color] -= yellowFull;
	} 

	// ת��˫����ȥ���У�����-1��ʾ���������򷵻�����
	int transfer()
	{
		int color1 = 0, color2 = 1;
		
		yellowCount[color2] += transCount[color1];
		yellowCount[color1] += transCount[color2];

		if (transCount[color1] == 0 && transCount[color2] == 0)
			return -1;
		if (transCount[color1] == 0 || transCount[color2] == 0)
		{
			if (transCount[color1] == 0 && transCount[color2] > 0)
				swap(color1, color2);
			int h2;
			maxHeight[color2] = h2 = maxHeight[color2] + transCount[color1];
			if (h2 > MAPHEIGHT)
				return color2;
			int i, j;
 
			for (i = h2; i > transCount[color1]; i--)
				for (j = 1; j <= MAPWIDTH; j++)
					gridInfo[color2][i][j] = gridInfo[color2][i - transCount[color1]][j];
 
			for (i = transCount[color1]; i > 0; i--)
				for (j = 1; j <= MAPWIDTH; j++)
					gridInfo[color2][i][j] = trans[color1][i - 1][j];
			return -1;
		}
		else
		{
			int h1, h2;
			maxHeight[color1] = h1 = maxHeight[color1] + transCount[color2];//��color1���ƶ�count1ȥcolor2
			maxHeight[color2] = h2 = maxHeight[color2] + transCount[color1];
 
			if (h1 > MAPHEIGHT) return color1;
			if (h2 > MAPHEIGHT) return color2;
 
			int i, j;
			for (i = h2; i > transCount[color1]; i--)
				for (j = 1; j <= MAPWIDTH; j++)
					gridInfo[color2][i][j] = gridInfo[color2][i - transCount[color1]][j];
 
			for (i = transCount[color1]; i > 0; i--)
				for (j = 1; j <= MAPWIDTH; j++)
					gridInfo[color2][i][j] = trans[color1][i - 1][j];
 
			for (i = h1; i > transCount[color2]; i--)
				for (j = 1; j <= MAPWIDTH; j++)
					gridInfo[color1][i][j] = gridInfo[color1][i - transCount[color2]][j];
 
			for (i = transCount[color2]; i > 0; i--)
				for (j = 1; j <= MAPWIDTH; j++)
					gridInfo[color1][i][j] = trans[color2][i - 1][j];
 
			return -1;
		}
	}
 
	// ��ɫ�����ܷ������Ϸ
	inline bool canPut(int color, int blockType)
	{
		Tetris t(blockType, color);
		for (int y = MAPHEIGHT; y >= 1; y--)
			for (int x = 1; x <= MAPWIDTH; x++)
				for (int o = 0; o < 4; o++)
				{
					t.set(x, y, o);
					if (t.isValid() && checkDirectDropTo(color, blockType, x, y, o))
						return true;
				}
		return false;
	}
 
	// ��ӡ�������ڵ���
	inline void printField()
	{
#ifndef _BOTZONE_ONLINE
		static const char *i2s[] = {
			"~~",
			"~~",
			"  ",
			"[]",
			"##"
		};
		cout << "~~��ǽ��[]���飬##���¿�" << endl;
		for (int y = MAPHEIGHT + 1; y >= 0; y--)
		{
			for (int x = 0; x <= MAPWIDTH + 1; x++)
				cout << i2s[gridInfo[0][y][x] + 2];
			for (int x = 0; x <= MAPWIDTH + 1; x++)
				cout << i2s[gridInfo[1][y][x] + 2];
			cout << endl;
		}
#endif
	}
}

vector<int> succ[10];

 int now[30][30][30], h[30], A[30][30], tmp[8];
int qx[1010], qy[1010], qo[1010], qf, qr;
int v[30][30][5], rd = 0;

const int dx[] = {-1, 1, 0, 0};
const int dy[] = {0, 0, -1, 1};

inline int idex(int x, int y, int o) {
	return o * 200 + (x - 1) * 20 + y - 1;
}

void findSucc(bitset<205> &a, int block, int dep, int col) {
	for (int i = 0; i < 4; i++) {
		for (int j = 1; j <= MAPWIDTH; j++)
			for (int k = 1; k <= MAPHEIGHT; k++) {
				now[i][j][k] = -1;
				for (int ii = 0; ii < 4; ii++) {
					int px = j + blockShape[block][i][2 * ii];
					int py = k + blockShape[block][i][2 * ii + 1];
					if (px < 1 || px > MAPWIDTH || py < 1 || py > MAPHEIGHT || a[(px - 1) * 20 + py - 1]) now[i][j][k] = 0;
				}
				if (now[i][j][k] == -1) now[i][j][k] = 1;
			}
	}
	
	qf = 1, qr = 0, ++rd;
	for (int n = 20; n >= 18; --n)
		for (int i = 1; i <= 10; i++)
			for (int j = 0; j < 4; ++j)
				if (Util :: checkDirectDropTo(col, block, i, n, j))
					qx[++qr] = i, qy[qr] = n, qo[qr] = j, v[i][n][j] = rd;

	Tetris aaa(block, col);
	for ( ; qf <= qr; ) {
		int x = qx[qf], y = qy[qf], o = qo[qf++];
		for (int k = 0; k < 4; ++k) {
			int xx = x + dx[k], yy = y + dy[k];
			if (xx < 1 || xx > MAPWIDTH || yy < 1 || yy > MAPHEIGHT) continue;
			if (!now[o][xx][yy] || v[xx][yy][o] == rd) continue;
			qx[++qr] = xx, qy[qr] = yy, qo[qr] = o, v[xx][yy][o] = rd;
		}
		int nxt = (o + 1) % 4;
		if (!aaa.set(x, y, o).rotation(nxt) || !now[nxt][x][y] || v[x][y][nxt] == rd) continue;
		qx[++qr] = x, qy[qr] = y, qo[qr] = nxt, v[x][y][nxt] = rd;
	}

	succ[dep].clear();
	for (int i = 1; i <= qr; ++i) {
		int x = qx[i], y = qy[i], o = qo[i];
		if (block == 6 && o != 0) continue;
		if ((block == 5 || block == 2 || block == 3) && (o > 1)) continue;
		if (!aaa.set(x, y, o).onGround()) continue;
		succ[dep].push_back(idex(x, y, o));
	}
}

inline void decompress(int player, bitset <205> &cur) {
	for (int i = 1; i <= MAPWIDTH; ++i)
		for (int j = 1; j <= MAPHEIGHT; ++j) {
			gridInfo[player][j][i] = cur[(i - 1) * 20 + j - 1];
		}
}
	
inline void compress(int player, bitset <205> &cur) {
	for (int i = 1; i <= MAPWIDTH; ++i)
		for (int j = 1; j <= MAPHEIGHT; ++j) {
			cur[(i - 1) * 20 + j - 1] = gridInfo[player][j][i] > 0;
		}
}

int mH[7][4];

void prePutblock() {
	for (int block = 0; block < 7; ++block) {
		for (int o = 0; o < 4; ++o) {
			for (int i = 0; i < 4; ++i) {
				if (blockShape[block][o][2 * i + 1] > mH[block][o]) mH[block][o] = blockShape[block][o][2 * i + 1];
			}
		}
	}
}

int putBlock(bitset <205> &cur, int block, int info, int tp) {
	int y = info % 20 + 1, x = ((info - (y - 1)) % 200) / 20 + 1, o = (info - (x - 1) * 20 - (y - 1)) / 200;
	for (int i = 0; i < 4; ++i) {
		int xx = x + blockShape[block][o][2 * i], yy = y + blockShape[block][o][2 * i + 1];
		cur[(xx - 1) * 20 + yy - 1] = tp;
	}
	return y + mH[block][o];
}

const int defaultDep = 7;

const double FHeight = -6.500158825082766, FRowTransitions = -5.2178882868487753, FColTransitions = -9.348695305445199, FHoles = -9.899265427351652, FWells = -2.6855972247263626, FSubh = 1.5, Frealholes = -4.9, Fsumholes = -12.9, Fdelay = -9.9333333333;//, Fgourd = -4.00000000;
double Feliminated = 7.4181268101392694;

double subh = 0;
int f[1010], maxh[11] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

int fullline[21];
int originalFullline[2][21];

inline int find(int x) {
	return (f[x] == x ? x : f[x] = find(f[x]));
}	

inline double value(int player) {
	for (int j = 1; j <= MAPWIDTH; ++j) {
		maxh[j] = 0;
 		for (int i = MAPHEIGHT; i >= 1; --i)
			if (gridInfo[player][i][j]) {maxh[j] = i; break;}
	}
	subh = 0;
	int _maxH = maxh[MAPWIDTH], no = 0;
	for (int j = 1; j < MAPWIDTH; ++j)
		subh += pow(abs(maxh[j + 1] - maxh[j]), 1.2), _maxH = max(_maxH, maxh[j]);
	if (_maxH == 20) no = -400;
	if (_maxH == 19) no = -100;
	int row = 0, col = 0;
	for (int i = 1; i <= MAPHEIGHT; ++i) { //Row Transitions
		for (int j = 1; j <= MAPWIDTH + 1; ++j)
			if ((gridInfo[player][i][j] != 0 && gridInfo[player][i][j - 1] == 0) || (gridInfo[player][i][j] == 0 && gridInfo[player][i][j - 1] != 0)) ++row;
	}
	for (int i = 1; i <= MAPWIDTH; ++i) { //Colomn Transitions
		for (int j = 1; j <= MAPHEIGHT; ++j)
			if ((gridInfo[player][j - 1][i] != 0 && gridInfo[player][j][i] == 0) || (gridInfo[player][j - 1][i] == 0 && gridInfo[player][j][i] != 0)) ++col;
		if (gridInfo[player][MAPHEIGHT][i]) ++col;
	}

	int holes = 0, maxH = 0, minH = 22;
	for (int i = 1; i <= MAPWIDTH; ++i) {
		int lid = yellowCount[player];
		for (int j = MAPHEIGHT; j >= 1; --j) {
			if (gridInfo[player][j][i]) {
				lid = max(lid, j);
				maxH = max(maxH, j), minH = min(minH, j);
				break;
			}
		}
		for (int k = lid; k >= 1; --k) if (!gridInfo[player][k][i]) ++holes;
	}
	for (int i = 1; i <= 10; i++)
		for (int j = 1; j <= 20; j++)
			f[(i - 1) * 20 + j] = (i - 1) * 20 + j;
	for (int i = 1; i <= 10; i++)
		for (int j = 1; j <= 20; j++) {
			if (gridInfo[player][j][i]) continue;
			if (i - 1 >= 1 && gridInfo[player][j][i - 1] == 0) f[find((i - 1) * 20 + j)] = find((i - 2) * 20 + j);
			if (j - 1 >= 1 && gridInfo[player][j - 1][i] == 0) f[find((i - 1) * 20 + j)] = find((i - 1) * 20 + j - 1);
		}
	f[1001] = 1001;
	for (int i = 1; i <= 10; i++)
		if (!gridInfo[player][20][i]) f[find(i * 20)] = 1001;
    
	int realholes = 0, sumholes = 0;
	for (int j = 1; j <= 19; j++) {
		int flag = 0;
		for (int i = 1; i <= 10; i++)
			if ( !gridInfo[player][j][i] && (find((i - 1) * 20 + j) != 1001 || j <= yellowCount[player]) ) // connected or yellow line
				++realholes, flag = 1;
		if (flag) sumholes++;
	}
	holes -= realholes;
	realholes -= sumholes;
	sumholes = sumholes * 2;

	// int realholes = 0, sumholes = 0, tholes = 0;
	// ++rdt;
	// for (int j = 1; j <= 19; j++) {
	// 	for (int i = 1; i <= 10; i++)
	// 		if (!gridInfo[player][j][i] && find((i - 1) * 20 + j) != 1001) {
	// 			++tholes;
	// 			int k = find((i - 1) * 20 + j);
	// 			if (visit[k] != rdt) visit[k] = rdt, lst[++realholes] = k, holeBlock[k] = 1;
	// 			else ++holeBlock[k];
	// 		}
	// }
	// holes -= tholes;
	// for (int i = 1; i <= realholes; ++i)
	// 	if (holeBlock[lst[i]] <= 7) ++sumholes;	
	// sumholes = sumholes * 2;

	for (int i = 1; i <= MAPHEIGHT; ++i) {
		fullline[i] = 0; int cntF = 0;
		for (int j = 1; j <= MAPWIDTH; ++j)
			if (gridInfo[player][i][j] > 0) ++cntF;
			else fullline[i] = j;
		if (cntF < MAPWIDTH - 1) fullline[i] = 0;
	}
	
	int wells = 0, delay = 0;
	for (int i = 1; i <= MAPWIDTH; ++i) {
		for (int j = 1; j <= MAPHEIGHT; ++j)
			if (!gridInfo[player][j][i] && gridInfo[player][j][i - 1] && gridInfo[player][j][i + 1]) {
				int k = j;
				bool flag = 1;
				for ( ; k <= MAPHEIGHT && !gridInfo[player][k][i] && gridInfo[player][k][i - 1] && gridInfo[player][k][i + 1]; ++k) if (!fullline[k]) flag = 0;
				wells += (k - j) * (k - j + 1) / 2;
				//if (j + 1 == k) continue;
				if (j < maxh[i] && flag) delay += (k - j) * (k - j + 1);
				j = k - 1;
			}
	}

	// int cover = 0;
	// for (int i = 1; i <= MAPHEIGHT; ++i) {
	// 	if (!fullline[i]) continue;
	// 	int j = i + 1;
	// 	for ( ; j <= MAPHEIGHT && gridInfo[player][j][fullline[i]]; ++j);
	// 	if (j - i - 1 > originalFullline[player][i] && j - i <= 4 && j - i > 1) cover += j - i - 1;
	// }
	
	double sum = FHeight * maxH + FRowTransitions * row + FColTransitions * col + FHoles * holes + sumholes * Fsumholes + Frealholes * realholes + FWells * wells - FSubh * subh + no;// + Fdelay * delay;// + cover * Fcover;// + Fgourd * gourd;
	return sum;
}

double maxValue(int player, bitset <205> &grid, int block, int combo) {
	int originalYelloCount = yellowCount[player];

	findSucc(grid, block, defaultDep, player);
	double res = -1e9;
	for (int i = 0, end = succ[defaultDep].size(); i < end; ++i) {
	    putBlock(grid, block, succ[defaultDep][i], 1);
		decompress(player, grid);
		Util :: eliminate(player);
		double cVal = value(player);
		int extra =  transCount[player] ? combo + 1 : 0;
		res = max(res, cVal + (transCount[player] + (extra >= 3)) * 2 * Feliminated);
		putBlock(grid, block, succ[defaultDep][i], 0);
		yellowCount[player] = originalYelloCount;
	}
	return res;
}

int maxMyDep, maxEnemyDep;
int significantMove; //possbility -> e^(x / FAC) wieghted average
const int infinity = (int)1e9;

struct node {
	int board, eli;
	double evalue;
};

bool operator < (const node &a, const node &b) { return a.evalue > b.evalue; }

int finalAction;

double givenBlock[7];
bool canGive[7];

double temp[22];

double myMCTS(int dep, bitset <205> grid, int block, int combo) {
	if (block == -1) {
		int originalYelloCount = yellowCount[currBotColor];

		decompress(currBotColor, grid);
		Util :: eliminate(currBotColor);
		bitset <205> nextGrid;
		compress(currBotColor, nextGrid);
		
		int maxC = -1, minC = 101;
		for (int i = 0; i < 7; ++i) {
			if (typeCountForColor[1 - enemyColor][i] > maxC) maxC = typeCountForColor[1 - enemyColor][i];
			if (typeCountForColor[1 - enemyColor][i] < minC) minC = typeCountForColor[1 - enemyColor][i];
		}
		
		vector <node> expand;
		for (int i = 0; i < 7; ++i) {
			if (maxC - minC == 2 && typeCountForColor[1 - enemyColor][i] == maxC) continue;
			decompress(currBotColor, nextGrid);
			expand.push_back((node){i, 0, maxValue(currBotColor, nextGrid, i, combo)});
		}

		int sz = expand.size();
		sort(expand.begin(), expand.end());
		
		if (sz == 1) return expand[0].evalue;
		
		for (int i = sz - 1, end = max(0, sz - significantMove - 1); i >= end; --i) {
			++typeCountForColor[1 - enemyColor][expand[i].board];
			expand[i].evalue = myMCTS(dep, nextGrid, expand[i].board, combo);
			//expand[i].evalue = myMCTS(dep, nextGrid, expand[i].board);
			--typeCountForColor[1 - enemyColor][expand[i].board];			
		}

		double resW = 0, sumW = 0, FAC = 1e9;
		for (int i = sz - 1, end = max(0, sz - significantMove - 1); i >= end; --i) temp[i] = -expand[i].evalue;
		for (int i = sz - 1, end = max(0, sz - significantMove - 1); i >= end; --i) FAC = min(FAC, temp[i]);
		for (int i = sz - 1, end = max(0, sz - significantMove - 1); i >= end; --i) temp[i] -= FAC;

		for (int i = sz - 1, end = max(0, sz - significantMove - 1); i >= end; --i) sumW += temp[i];
		for (int i = sz - 1, end = max(0, sz - significantMove - 1); i >= end; --i) {
			double fac = temp[i] / sumW;
			resW += fac * expand[i].evalue;
		}

		yellowCount[currBotColor] = originalYelloCount;
		return resW;
	}
	else {
		int originalYelloCount = yellowCount[currBotColor]; // whb д���

		bitset <205> nextGrid = grid;
		decompress(currBotColor, grid);
		findSucc(grid, block, dep, currBotColor);
		vector <node> expand;
		for (int i = 0, end = (int)succ[dep].size(); i < end; ++i) {
			putBlock(nextGrid, block, succ[dep][i], 1);
			decompress(currBotColor, nextGrid);
			Util :: eliminate(currBotColor);
			double cVal = value(currBotColor);
			int extra = transCount[currBotColor] ? combo + 1 : 0;
			expand.push_back((node){succ[dep][i], transCount[currBotColor], cVal + (transCount[currBotColor] + (extra >= 3)) * 2 * Feliminated});
			putBlock(nextGrid, block, succ[dep][i], 0);
			yellowCount[currBotColor] = originalYelloCount;
		}

		int sz = expand.size();
		sort(expand.begin(), expand.end());
		if (!sz) return -1e9;
		
		int res = -1;
		double resW = 0;
		for (int i = 0, end = min(sz, significantMove); i < end; ++i) {
			if (dep < maxMyDep) {
				putBlock(nextGrid, block, expand[i].board, 1);
				int extra = expand[i].eli ? combo + 1 : 0;
				expand[i].evalue = myMCTS(dep + 1, nextGrid, -1, extra) + Feliminated * (expand[i].eli + (extra >= 3)) * 2;
				//expand[i].evalue = myMCTS(dep + 1, nextGrid, -1);
				putBlock(nextGrid, block, expand[i].board, 0);
			}
			if (res == -1 || expand[i].evalue > expand[res].evalue) res = i;
		}
		resW = expand[res].evalue;
		return finalAction = expand[res].board, resW;
	}
}

//double enemyVal;

int maxGap;

double enemyMCTS(int dep, bitset <205> grid, int block, double WG, int combo) { //������depҪ�������Ǹ�dep�ĳ�ʼֵ��1
	if (block == -1) {
		int originalYelloCount = yellowCount[enemyColor];

		decompress(enemyColor, grid);
		Util :: eliminate(enemyColor);
		bitset <205> nextGrid;
		compress(enemyColor, nextGrid);

		int maxC = -1, minC = 101;
		for (int i = 0; i < 7; ++i) {
			if (typeCountForColor[enemyColor][i] > maxC) maxC = typeCountForColor[enemyColor][i];
			if (typeCountForColor[enemyColor][i] < minC) minC = typeCountForColor[enemyColor][i];
		}

		vector <node> expand;
		for (int i = 0; i < 7; ++i) {
			if (maxC - minC == maxGap && typeCountForColor[enemyColor][i] == maxC) continue;
			decompress(enemyColor, nextGrid);
			expand.push_back((node){i, 0, maxValue(enemyColor, nextGrid, i, combo)});
		}

		int sz = expand.size();
		sort(expand.begin(), expand.end());
		
		double resW = 1e9;
		for (int i = sz - 1, end = 0; i >= end; --i) {
			++typeCountForColor[enemyColor][expand[i].board];
			expand[i].evalue = enemyMCTS(dep, nextGrid, expand[i].board, WG, combo);
			//expand[i].evalue = enemyMCTS(dep, nextGrid, expand[i].board, WG);
			--typeCountForColor[enemyColor][expand[i].board];
			if (dep == 1) givenBlock[expand[i].board] += WG * expand[i].evalue;
			if (expand[i].evalue < resW) resW = expand[i].evalue;
		}

		yellowCount[enemyColor] = originalYelloCount;
		return resW;
	}
	else {
		int originalYelloCount = yellowCount[enemyColor];

		bitset <205> nextGrid = grid;
		decompress(enemyColor, grid);
		findSucc(grid, block, dep, enemyColor);
		vector <node> expand;
		for (int i = 0, end = (int)succ[dep].size(); i < end; ++i) {
			putBlock(nextGrid, block, succ[dep][i], 1);
			decompress(enemyColor, nextGrid);
			Util :: eliminate(enemyColor);
			double cVal = value(enemyColor);
			int extra = transCount[enemyColor] ? combo + 1 : 0;
			expand.push_back((node){succ[dep][i], transCount[enemyColor], cVal + (transCount[enemyColor] + (extra >= 3)) * 2 * Feliminated});
			putBlock(nextGrid, block, succ[dep][i], 0);

			yellowCount[enemyColor] = originalYelloCount;
		}

		int sz = expand.size();
		sort(expand.begin(), expand.end());
		if (!sz) return -1e9;
		
		for (int i = 0, end = min(sz, significantMove + 1); i < end; ++i) {
			if (dep < maxEnemyDep) {
				putBlock(nextGrid, block, expand[i].board, 1);
				int extra = expand[i].eli ? combo + 1 : 0;
				expand[i].evalue = enemyMCTS(dep + 1, nextGrid, -1, WG, extra) + Feliminated * (expand[i].eli + (extra >= 3)) * 2;
				//expand[i].evalue = enemyMCTS(dep + 1, nextGrid, -1, WG);
				putBlock(nextGrid, block, expand[i].board, 0);
			}
		}
		
		double resW = 0, sumW = 0, FAC = 1e9;
		for (int i = 0, end = min(sz, significantMove + 1); i < end; ++i) FAC = min(FAC, expand[i].evalue);		
		for (int i = 0, end = min(sz, significantMove + 1); i < end; ++i) temp[i] = expand[i].evalue - FAC;
		for (int i = 0, end = min(sz, significantMove + 1); i < end; ++i) sumW += temp[i];
		for (int i = 0, end = min(sz, significantMove + 1); i < end; ++i) {
			double fac = temp[i] / sumW;
			resW += fac * expand[i].evalue;
		}		
		return resW;
	}
}

int etop;

void getFullline() {
	for (int player = 0; player <= 1; ++player) {
		for (int i = 1; i <= MAPHEIGHT; ++i) {
			int cntF = 0;
			for (int j = 1; j <= MAPWIDTH; ++j) cntF += (gridInfo[player][i][j] > 0);
			if (cntF < MAPWIDTH - 1) originalFullline[player][i] = 0;
			else {
				for (int k = 1; k <= MAPWIDTH; ++k)
					if (!gridInfo[player][i][k]) {
						int j = i + 1;
						for ( ; j <= MAPHEIGHT && gridInfo[player][j][k]; ++j);
						if (j - i - 1 > 0) originalFullline[player][i] = j - i - 1;
						else originalFullline[player][i] = 0;
					}
			}			
		}		
	}
}

int getMyOperations(int turnID, int block, int combo) {
	bitset <205> start;
	compress(currBotColor, start);
    maxMyDep = 2, significantMove = 3;
	finalAction = -1;
	myMCTS(0, start, block, combo);
	decompress(currBotColor, start);
	assert(finalAction != -1);
	return finalAction;
}

int getEnemyOperations(int turnID, int block, int combo) {
	getFullline();
	bitset <205> start;
	compress(enemyColor, start);
	maxEnemyDep = 2, significantMove = 3;
	maxGap = 2;
	//maxGap = 2;
	
	for (int i = 0; i < 7; ++i) givenBlock[i] = 0, canGive[i] = 0;
	int maxC = -1, minC = 101;
	for (int i = 0; i < 7; ++i) {
		if (typeCountForColor[enemyColor][i] > maxC) maxC = typeCountForColor[enemyColor][i];
		if (typeCountForColor[enemyColor][i] < minC) minC = typeCountForColor[enemyColor][i];
	}
	
	for (int i = 0; i < 7; ++i)
		if (maxC - minC < maxGap || typeCountForColor[enemyColor][i] < maxC) canGive[i] = 1;

	int originalYelloCount = yellowCount[enemyColor];
	
	vector <node> expand;
	decompress(enemyColor, start);
	findSucc(start, block, defaultDep, enemyColor);
	for (int i = 0, end = (int)succ[defaultDep].size(); i < end; ++i) {
		putBlock(start, block, succ[defaultDep][i], 1);
		decompress(enemyColor, start);
		Util :: eliminate(enemyColor);
		double cVal = value(enemyColor);
		int extra = transCount[enemyColor] ? combo + 1 : 0;
		expand.push_back((node){succ[defaultDep][i], transCount[enemyColor], cVal + (transCount[enemyColor] + (extra >= 3)) * 2 * Feliminated});
		putBlock(start, block, succ[defaultDep][i], 0);

		yellowCount[enemyColor] = originalYelloCount;
	}

	int sz = expand.size();
	sort(expand.begin(), expand.end());
	
	for (int i = 0, end = min(sz - 1, significantMove); i < end; ++i) expand[i].evalue -= expand[end].evalue;//, printf("%d\n", expand[i].eli);
	double sumW = 0;
	for (int i = 0, end = min(sz, significantMove); i < end; ++i) {
		sumW += expand[i].evalue;
	}
	for (int i = 0, end = min(sz, significantMove); i < end; ++i) {		
		putBlock(start, block, expand[i].board, 1);
		int extra = expand[i].eli ? combo + 1 : 0;
		//printf("%.9lf\n", expand[i].evalue / sumW);
		enemyMCTS(1, start, -1, expand[i].evalue / sumW, extra);
		putBlock(start, block, expand[i].board, 0);
	}

	expand.clear(), sumW = 0;
	for (int i = 0; i < 7; ++i) {
		if (canGive[i]) expand.push_back((node){i, 0, -givenBlock[i]});
	}

	decompress(enemyColor, start);
	yellowCount[enemyColor] = originalYelloCount;

	sort(expand.begin(), expand.end());
	//if (turnID > 30 || turnID % 10) return expand[0].board;
	return expand[0].board;
	/*for ( ; expand.size() > 3; expand.pop_back()); //�������
	FAC = maxV(expand);
	for (int i = 0, end = expand.size(); i < end; ++i) expand[i].evalue = exp(expand[i].evalue / FAC), sumW += expand[i].evalue;

	double res = sumW * (rand() / (double)RAND_MAX), cur = 0;
	for (int i = 0, end = expand.size(); i < end; ++i)
		if (res >= cur && res <= cur + expand[i].evalue) return expand[i].board;
		else cur += expand[i].evalue;
	
		return sz > 0 ? expand[0].board : -1;*/
}

int main()
{
	// ��������
#ifndef _BOTZONE_ONLINE
	freopen("data.in", "r", stdin);
	// freopen("main.out", "w", stdout);
#endif
	
	istream::sync_with_stdio(false);
	srand(time(NULL));
	init();
 
	int turnID, blockType;
	int nextTypeForColor[2];
	cin >> turnID;
 
	// �ȶ����һ�غϣ��õ��Լ�����ɫ
	// ˫���ĵ�һ��϶���һ����
	cin >> blockType >> currBotColor;
	enemyColor = 1 - currBotColor;
	nextTypeForColor[0] = blockType;
	nextTypeForColor[1] = blockType;
	typeCountForColor[0][blockType]++;
	typeCountForColor[1][blockType]++;
 
	// Ȼ�������ǰÿ�غϵ�������������ָ�״̬
	// ѭ���У�color ��ʾ��ǰ��һ���� color ����Ϊ
	// ƽ̨��֤�������붼�ǺϷ�����
	for (int i = 1; i < turnID; i++)
	{
		int currTypeForColor[2] = { nextTypeForColor[0], nextTypeForColor[1] };
		int x, y, o;
		// ������Щ��������𽥻ָ�״̬����ǰ�غ�
 
		// �ȶ��Լ��������Ҳ�����Լ�����Ϊ
		// �Լ���������Լ������һ��
		// Ȼ��ģ�����һ�����ÿ�
		cin >> blockType >> x >> y >> o;
 
		// �ҵ�ʱ����һ���䵽�� x y o��
		Tetris myBlock(currTypeForColor[currBotColor], currBotColor);
		myBlock.set(x, y, o).place();
 
		// �Ҹ��Է�ʲô�����ţ�
		typeCountForColor[enemyColor][blockType]++;
		nextTypeForColor[enemyColor] = blockType;
 
		// Ȼ����Լ������룬Ҳ���ǶԷ�����Ϊ
		// ���и��Լ��������ǶԷ������һ��
		cin >> blockType >> x >> y >> o;
 
		// �Է���ʱ����һ���䵽�� x y o��
		Tetris enemyBlock(currTypeForColor[enemyColor], enemyColor);
		enemyBlock.set(x, y, o).place();
 
		// �Է�����ʲô�����ţ�
		typeCountForColor[currBotColor][blockType]++;
		nextTypeForColor[currBotColor] = blockType;
 
		// �����ȥ
		Util::eliminate(0);
		Util::eliminate(1);
 
		// ����ת��
		Util::transfer();
	}
 
 
	int blockForEnemy, finalX, finalY, finalO;
 
	// �������ߣ���ֻ���޸����²��֣�
 	prePutblock();
	//myBlock = 21;
	blockForEnemy = getEnemyOperations(turnID, nextTypeForColor[enemyColor], elimCombo[enemyColor]);
	int myBlock = getMyOperations(turnID, nextTypeForColor[currBotColor], elimCombo[currBotColor]);
		
	int maxC = -1, minC = 101;
	for (int i = 0; i < 7; ++i) {
		if (typeCountForColor[enemyColor][i] > maxC) maxC = typeCountForColor[enemyColor][i];
		if (typeCountForColor[enemyColor][i] < minC) minC = typeCountForColor[enemyColor][i];
	}
	
	finalY = myBlock % 20 + 1;
	finalX = (myBlock - (finalY - 1)) % 200 / 20 + 1;
	finalO = (myBlock - (finalX - 1) * 20 - (finalY - 1)) / 200;

	// ���²����������ƽ̨�ϱ��벻�������
	Util::printField();
	cout << blockForEnemy << " " << finalX << " " << finalY << " " << finalO;
 
	return 0;
}
