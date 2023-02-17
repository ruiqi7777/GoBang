#define _CRT_SECURE_NO_WARNINGS
#include<graphics.h>
#include<Windows.h>
#include<conio.h>
#include <math.h>
#include<time.h>
#include<mmsystem.h>
#include<easyx.h>
#include<iostream>
#include<stdlib.h>

IMAGE Img1;
IMAGE Img2;
IMAGE Img3;
IMAGE Img4;

#define E 10                            
#define V 10                             
#define MIN 5                            
#define MAX 20                           
#define SPACE 50
#define BLOCK_SIZE 50

const int POS_OFFSET = BLOCK_SIZE * 0.4;

int ii = 0, jj = 0, xx = 0, yy = 0;     //记录上一步棋位置
int num = 0;                            //对弈次数
long double A = -99999999999999999;
long double B = 99999999999999999;

void ChessBoard();	                                                                                //绘制棋盘
void Init(struct point p[25][25]);                                                                  //棋盘初始化

void Regret(struct point p[25][25]);		                                                        //悔棋
void Begin(struct point p[25][25]);  	                                                            //开始
void GameOver(int x);		                                                                        //结束	
int CheckWin(struct point p[25][25]);                                                               //返回胜负代数
bool ClickBoard(MOUSEMSG msg, struct point p[25][25]);                                              //检验是否有效落子


void Assess_v(struct point p[25][25]);                                                              //某点价值分评定
double Assess_a(struct point p[25][25]);                                                            //局面优势评估
double deduction(struct point p[25][25], int sex, int depth, long double a, long double b);         //递归推演 

void Go_chess(struct point p[25][25], int x, int y);                                                //电脑下棋
int AI_Go(struct point p[25][25]);		                                                            //电脑执棋


struct point {
	int state;                //位置的状态    玩家-1,   空位置0,   电脑1
	long double value;        //该点价值分
	long double n_value;      //下一步棋下这点时棋盘的总优势分
};


void Init(struct point p[25][25])
{
	int i, j;
	num = 0;
	for (i = 0; i < 25; i++)
	{
		for (j = 0; j < 25; j++)
		{
			p[i][j].state = 0;  
			p[i][j].value = 0;
			p[i][j].n_value = 0;
		}
	}
}

void ChessBoard()
{
	initgraph(950, 800);
	loadimage(&Img1, _T("D:/源代码/GoBang/gobang.jpg"), 950, 800);
	putimage(0, 0, &Img1);
	setlinecolor(BLACK);
	for (int i = 50; i <= 750; i += 50)
	{
		line(i, 50, i, 750);
		line(50, i, 750, i);
	}

	setlinecolor(BLACK);
	settextstyle(40, 0, _T("幼圆"));
	outtextxy(800, 200, _T("悔 棋"));
	outtextxy(800, 300, _T("重 开"));
	outtextxy(800, 400, _T("后 手"));
	outtextxy(800, 500, _T("退 出"));
}

void Regret(struct point p[25][25])
{
	ChessBoard();
	setcolor(BLACK);
	p[ii][jj].state = 0;                     //将上一步棋清空
	p[xx][yy].state = 0;
	for (int i = MIN; i < MAX; i++) {        //重新绘制棋盘
		for (int j = MIN; j < MAX; j++) {
			if (p[i][j].state == -1) {
				setfillcolor(BLACK);
				fillcircle((i + 1 - MIN) * 50, (j + 1 - MIN) * 50, 20);
			}
			if (p[i][j].state == 1) {
				setfillcolor(WHITE);
				fillcircle((i + 1 - MIN) * 50, (j + 1 - MIN) * 50, 20);
			}
		}
	}
}

bool ClickBoard(MOUSEMSG msg, struct point p[25][25])
{
	int x = msg.x;
	int y = msg.y;

	int col = (x - SPACE) / BLOCK_SIZE;
	int row = (y - SPACE) / BLOCK_SIZE;

	int lefttopx = SPACE + BLOCK_SIZE * col;
	int lefttopy = SPACE + BLOCK_SIZE * row;

	int len, selectpos = false;

	do
	{
		//点击选项
		if ((x <= 900 && y < 250 && x >= 800 && y > 200) || (x <= 900 && y < 350 && x >= 800 && y > 300) ||
			(x <= 900 && y < 450 && x >= 800 && y > 400) || (x <= 900 && y < 550 && x >= 800 && y > 500))
		{
			selectpos = true;
			break;
		}

		//左上
		len = sqrt((x - lefttopx) * (x - lefttopx) + (y - lefttopy) * (y - lefttopy));
		if (len < POS_OFFSET)
		{
			if (p[row][col].state == 0)
			{
				selectpos = true;
			}
			break;
		}

		//右上
		len = sqrt((x - lefttopx - BLOCK_SIZE) * (x - lefttopx - BLOCK_SIZE) + (y - lefttopy) * (y - lefttopy));
		if (len < POS_OFFSET)
		{
			if (p[row][col + 1].state == 0)
			{
				selectpos = true;
			}
			break;
		}

		//左下
		len = sqrt((x - lefttopx) * (x - lefttopx) + (y - lefttopy - BLOCK_SIZE) * (y - lefttopy - BLOCK_SIZE));
		if (len < POS_OFFSET)
		{
			if (p[row + 1][col].state == 0)
			{
				selectpos = true;
			}
			break;
		}

		//右下
		len = sqrt((x - lefttopx - BLOCK_SIZE) * (x - lefttopx - BLOCK_SIZE) + (y - lefttopy - BLOCK_SIZE) * (y - lefttopy - BLOCK_SIZE));
		if (len < POS_OFFSET)
		{
			if (p[row + 1][col + 1].state == 0)
			{
				selectpos = true;
			}
			break;
		}

	} while (0);

	return selectpos;
}

void Begin(struct point p[25][25])
{
	MOUSEMSG m; 
	int  x, y, i, j;
	ChessBoard();   
	setlinecolor(BLACK);

	while (1)
	{
		m = GetMouseMsg();
		x = m.x % 50;
		y = m.y % 50;
		if (m.uMsg == WM_LBUTTONDOWN && ClickBoard(m, p))		
		{
			if (m.x <= 900 && m.y < 250 && m.x >= 800 && m.y > 200)  //悔棋
			{
				Regret(p);
				continue;
			}
			if (m.x <= 900 && m.y < 350 && m.x >= 800 && m.y > 300)	//重开
			{
				Init(p);   
				Begin(p);
			}
			if (m.x <= 900 && m.y < 450 && m.x >= 800 && m.y > 400)  //后手
			{
				if (num < 1)
				{
					AI_Go(p);  
					num++;
				}
			}
			if (m.x <= 900 && m.y < 550 && m.x >= 800 && m.y > 500)  //退出
			{
				closegraph();
				exit(0);   
			}

			if (x >= 25)     
				m.x = m.x - x + 50;
			else
				m.x = m.x - x;
			if (y >= 25)
				m.y = m.y - y + 50;
			else
				m.y = m.y - y;
			i = m.x / 50 - 1 + MIN; 
			j = m.y / 50 - 1 + MIN;
			if (i < 20 && j < 20)
			{
				if (p[i][j].state == -1)
					break;
				if (m.x <= 750 && m.y <= 750 && m.x >= 50 && m.y >= 50)
				{
					setfillcolor(BLACK);
					fillcircle(m.x, m.y, 20); 
					ii = i;            //为悔棋坐标做记录
					jj = j;
					p[i][j].state = -1;//修改该位置的状态
				}
				if (CheckWin(p) != 0)
				{
					Sleep(1000);
					GameOver(CheckWin(p));
				}
				AI_Go(p);   
				if (CheckWin(p) != 0)
				{
					Sleep(1000);
					GameOver(CheckWin(p));
				}
				num++;    //对弈次数
			}
		}
	}
}

int CheckWin(struct point p[25][25])
{
	int i, j, k = 0, x = 0, y = 0, b = 0, q = 0, e = 0;
	for (i = MIN; i < MAX; i++)
	{
		for (j = MIN; j < MAX; j++)
		{
			if (p[i][j].state == 0)
			{
				k++;
				if (k == 0)
				{
					e = 1;//平局
				}
			}
			if (p[i][j].state)
			{
				for (x = -2; p[i][j].state == p[i][j + x].state && x < 4; x++)     //横
				{
				}
				for (y = -2; p[i][j].state == p[i + y][j].state && y < 4; y++)      //纵
				{
				}
				for (b = -2; p[i][j].state == p[i + b][j + b].state && b < 4; b++)    //斜下
				{
				}
				for (q = -2; p[i][j].state == p[i - q][j + q].state && q < 4; q++)    //斜上
				{
				}
				if (x == 3 || y == 3 || b == 3 || q == 3)
				{
					if (p[i][j].state == 1)
					{
						e = 2; //玩家输
					}
					if (p[i][j].state == -1)
					{
						e = 3; //玩家赢
					}
				}
			}
		}
	}
	return e;
}


void GameOver(int x)
{
	point pp[25][25];     
	Init(pp);     
	initgraph(800, 800);
	cleardevice();     
	settextstyle(50, 24, _T("楷体"));
	settextcolor(RED);    
	if (x == 1)
	{
		outtextxy(80, 350, _T("平局！恭喜你！你与AI不相上下"));
	}
	else if (x == 2)
	{
		loadimage(&Img2, _T("D:/源代码/GoBang/false.jpg"), 800, 800);
		putimage(0, 0, &Img2);
	}
	else if (x == 3)
	{
		loadimage(&Img3, _T("D:/源代码/GoBang/win.jpg"), 800, 800);
		putimage(0, 0, &Img3);
	}
	settextcolor(WHITE);
	outtextxy(260, 670, _T("返回游戏界面"));
	outtextxy(300, 750, _T("结束游戏"));


	while (true)
	{
		MOUSEMSG n;
		n = GetMouseMsg();
		switch (n.uMsg) {
		case WM_LBUTTONDOWN:
			if (n.x <= 700 && n.x >= 260 && n.y <= 720 && n.y >= 670)
				Begin(pp);
			if (n.x <= 490 && n.x >= 300 && n.y <= 800 && n.y >= 750)
			{
				closegraph();
				exit(0);
			}
		}
	}
}

double Assess_a(struct point p[25][25])
{
	int i, j;
	Assess_v(p);                      //某点价值评定
	long double s_value = 0;
	for (i = MIN; i < MAX; i++)
	{
		for (j = MIN; j < MAX; j++)
		{
			s_value += p[i][j].value;//棋盘总优势分为所有点分数的和
		}
	}
	return s_value;
}

void Assess_v(struct point p[25][25])
{
	int i, j, k, m, n, x, y;
	for (i = MIN; i < MAX; i++)    
	{
		for (j = MIN; j < MAX; j++)
		{
			if (p[i][j].state == 0)
			{
				p[i][j].value = 0;      //空位价值为零 
			}
			else                        
			{                              
				long double v1 = V, v2 = V, v3 = V, v4 = V, v5 = V, v6 = V, v7 = V, v8 = V;
				
				for (x = 0, n = j; p[i][j].state != -p[i][n + 1].state && n + 1 < 20 && x < 4; x++, n++)       //右
				{
				}
				if (x < 4)
				{
					v1 = 0;    
				}
				else {
					for (k = 0, n = j; p[i][j].state == p[i][n + 1].state && k < 5; k++, n++)       //右
					{
						v1 *= V;     
					}
				}
				for (y = 0, n = j; p[i][j].state != -p[i][n - 1].state && n - 1 > 4 && y < 4; y++, n--)      //左
				{
				}
				if (y < 4)
				{
					v2 = 0;    
				}
				else {
					for (k = 0, n = j; p[i][j].state == p[i][n - 1].state && k < 5; k++, n--)      //左
					{
						v2 *= V;     
					}
				}
				
				for (x = 0, n = i; p[i][j].state != -p[n + 1][j].state && n + 1 < 20 && x < 5; x++, n++)       //上
				{
				}
				if (x < 4)
				{
					v3 = 0;    
				}
				else {
					for (k = 0, n = i; p[i][j].state == p[n + 1][j].state && k < 5; k++, n++)       //上
					{
						v3 *= V;   
					}
				}
				for (y = 0, n = i; p[i][j].state != -p[n - 1][j].state && n - 1 > 4 && y < 5; y++, n--)       //下
				{
				}
				if (y < 4)
				{
					v4 = 0;   
				}
				else {
					for (k = 0, n = i; p[i][j].state == p[n - 1][j].state && k < 5; k++, n--)       //下
					{
						v4 *= V;   
					}
				}
				
				for (x = 0, m = i, n = j; p[i][j].state != -p[m - 1][n + 1].state && n + 1 < 20 && m - 1 > 4 && x < 5; x++, m--, n++) //右上
				{
				}
				if (x < 4)
				{
					v5 = 0;   
				}
				else {
					for (k = 0, m = i, n = j; p[i][j].state == p[m - 1][n + 1].state && k < 5; k++, m--, n++)       //右上
					{
						v5 *= V;    
					}
				}
				for (y = 0, m = i, n = j; p[i][j].state != -p[m + 1][n - 1].state && m + 1 < 20 && n - 1 > 4 && y < 5; y++, m++, n--) //左下
				{
				}
				if (y < 4)
				{
					v6 = 0;    
				}
				else {
					for (k = 0, m = i, n = j; p[i][j].state == p[m + 1][n - 1].state && k < 5; k++, m++, n--)       //左下
					{
						v6 *= V;    
					}
				}
				
				for (x = 0, m = i, n = j; p[i][j].state != -p[m - 1][n - 1].state && n - 1 > 4 && m - 1 > 4 && x < 5; x++, m--, n--)  //左上
				{
				}
				if (x < 4)
				{
					v7 = 0;    
				}
				else {
					for (k = 0, m = i, n = j; p[i][j].state == p[m - 1][n - 1].state && k < 5; k++, m--, n--)       //左上
					{
						v7 *= V;   
					}
				}
				for (y = 0, m = i, n = j; p[i][j].state != -p[m + 1][n + 1].state && n + 1 < 20 && m + 1 < 20 && y < 5; y++, m++, n++) //右下
				{
				}
				if (y < 4)
				{
					v8 = 0;    
				}
				else {
					for (k = 0, m = i, n = j; p[i][j].state == p[m + 1][n + 1].state && k < 5; k++, m++, n++)       //右下
					{
						v8 *= V;   
					}
				}
				
				p[i][j].value = v1 + v2 + v3 + v4 + v5 + v6 + v7 + v8 + 0.1 * (15 - abs(i - 12) - abs(j - 12)); 
				if (p[i][j].state == -1)    
				{
					p[i][j].value = -p[i][j].value * E;       //E为偏向防守程度
				}
			}
		}
	}
}

double deduction(struct point p[25][25], int sex, int depth, long double a, long double b)
{
	int m, n, i, j, t = 0, br = 0;
	long double va = 0;
	if (depth == 0 || CheckWin(p) != 0)   //达到深度或棋局结束
	{
		return Assess_a(p);              //递归出口，返回分数
	}
	else if (sex % 2 == 1)               //玩家回合
	{
		for (m = MIN; m < MAX; m++)
		{
			if (br == 1)
			{   
				break;
			}
			for (n = MIN; n < MAX; n++)
			{
				if (p[m][n].state == 0)  
				{
					if (num < 80)
					{
						t = 0;
						for (i = -1; i < 2; i++) 
						{
							for (j = -1; j < 2; j++) 
							{
								if (p[i + m][j + n].state != 0) 
								{
									t++;     
								}
							}
						}
					}
					else 
					{
						t = 1;
					}
					if (t == 0)   
					{
						va = B;
					}
					else 
					{
						p[m][n].state = -1;    
						va = deduction(p, sex + 1, depth - 1, a, b);
						p[m][n].state = 0;    
					}
					if (va < b)   
					{
						b = va;
					}
					if (b < a)
					{
						break;    
						br = 1;
					}
				}
			}
		}
		return b; //返回分数
	}
	else if (sex % 2 == 0)     //电脑回合
	{
		for (m = MIN; m < MAX; m++)
		{
			if (br == 1)
			{   
				break;
			}
			for (n = MIN; n < MAX; n++)
			{
				if (p[m][n].state == 0)    
				{
					if (num < 80) 
					{
						t = 0;
						for (i = -1; i < 2; i++)
						{
							for (j = -1; j < 2; j++)
							{
								if (p[i + m][j + n].state != 0)
								{
									t++;     
								}
							}
						}
					}
					else
					{
						t = 1;
					}
					if (t == 0)   
					{
						va = A;
					}
					else
					{
						p[m][n].state = 1;     
						va = deduction(p, sex + 1, depth - 1, a, b);
						p[m][n].state = 0;     
					}
					if (va > a)    
					{
						a = va;
					}
					if (b < a)
					{
						break;  
						br = 1;
					}
				}
			}
		}
		return a;//返回分数
	}
	return 0;
}

void Go_chess(struct point p[25][25], int x, int y)
{
	setfillcolor(WHITE);
	fillcircle((x + 1 - MIN) * 50, (y + 1 - MIN) * 50, 20);
	xx = x;
	yy = y;                //为悔棋坐标做记录
	p[x][y].state = 1;     //修改状态
}

int AI_Go(struct point p[25][25])
{
	int i, j, x = 12, y = 12, m, n, t = 0;
	long double v = -999999999999999999;    //10^18
	for (i = MIN; i < MAX; i++)        //寻找最佳落子点
	{
		for (j = MIN; j < MAX; j++)
		{
			if (p[i][j].state == 0)      
			{
				t = 0;
				for (m = -1; m < 2; m++) 
				{
					for (n = -1; n < 2; n++) 
					{
						if (p[i + m][j + n].state != 0)
						{
							t++;     //偏僻点剪枝
						}
					}
				}
				if (t != 0)   //不偏僻
				{
					p[i][j].state = 1;    //假设电脑下在这里
					if (CheckWin(p) == 2)
					{
						Go_chess(p, i, j);    //下棋
						return 0;
					}
					p[i][j].n_value = deduction(p, 1, 2, A, B);
					p[i][j].state = 0;    //撤回
					if (p[i][j].n_value > v)
					{
						v = p[i][j].n_value;    //找最大值
						x = i;
						y = j;     //获取坐标
					}
				}
			}
		}
	}

	Sleep(500);
	Go_chess(p, x, y);    
	return 0;
}

int main()
{
	point p[25][25];       //  棋盘大小15*15	边界为5

	initgraph(950, 800);
	setbkcolor(BLACK);
	cleardevice();
	loadimage(&Img4, _T("D:/源代码/GoBang/begin.png"), 950, 700);
	putimage(0, 50, &Img4);
	_getch();

	Init(p);      
	Begin(p);  
	return 0;
}
