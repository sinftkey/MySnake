#include <ncurses.h>
#include <iostream>
#include <random>
#include <unistd.h>
using namespace std;
#define MAX_LENGTH 1500
#define DELAY 100000 // delay in microseconds


class Position
{
public:
	int x, y;
};

class Snake
{
public:
	Position body[MAX_LENGTH];
	int length;
	int direction;
};

class Food
{
public:
	Position pos;
	bool eaten;
};

int getRandomNumber(int max)
{
	random_device rd;
	mt19937 gen(rd());
	uniform_int_distribution<> dis(0, max);
	return dis(gen);
}

void initGame(Snake *snake, int max_x, int max_y) // 初始化游戏
{
	snake->length = 3;
	for (int i = 0; i < snake->length; i++)
	{
		snake->body[i].x = i + max_x / 2;
		snake->body[i].y = max_y / 2;
	}
}

void newFood(Snake *snake, Food *food, int max_x, int max_y) // 生成新的食物
{
	if (food->eaten)
	{
		bool collision;
		do
		{
			collision = false;
			food->pos.x = getRandomNumber(max_x);
			food->pos.y = getRandomNumber(max_y);
			for (int i = 0; i < snake->length; i++)
			{
				if (food->pos.x == snake->body[i].x && food->pos.y == snake->body[i].y)
				{
					collision = true;
					break;
				}
			}
		} while (collision);
	}
	food->eaten = false;
}

void moveSnake(Snake *snake)
{
	for (int i = snake->length - 1; i > 0; i--)
	{
		snake->body[i] = snake->body[i - 1];
	}
	switch (snake->direction)
	{
	case KEY_UP:
		snake->body[0].y--;
		break;
	case KEY_DOWN:
		snake->body[0].y++;
		break;
	case KEY_LEFT:
		snake->body[0].x--;
		break;
	case KEY_RIGHT:
		snake->body[0].x++;
		break;
	}
}

bool checkCollision(Snake *snake, int max_x, int max_y)
{
	if (snake->body[0].x >= max_x || snake->body[0].x <= 0 || snake->body[0].y >= max_y || snake->body[0].y <= 0)
	{
		return true;
	}
	for (int i = 1; i < snake->length; i++)
	{
		if (snake->body[0].x == snake->body[i].x &&
			snake->body[0].y == snake->body[i].y)
			return true;
	}

	return false;
}

int main()
{
	initscr();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);
	curs_set(0);
	timeout(0);

	srand(time(NULL));

	int max_y, max_x;
	getmaxyx(stdscr, max_y, max_x);

	Snake *snake = new Snake();
	Food *food = new Food();
	int score = 0;
	snake->direction = KEY_LEFT;
	food->eaten = true;
	initGame(snake, max_x, max_y);

	int ch;
	while (1)
	{
		// 处理输入
		if ((ch = getch()) != ERR)
		{
			switch (ch)
			{
			case KEY_UP:
				if (snake->direction != KEY_DOWN)
					snake->direction = KEY_UP;
				break;
			case KEY_DOWN:
				if (snake->direction != KEY_UP)
					snake->direction = KEY_DOWN;
				break;
			case KEY_LEFT:
				if (snake->direction != KEY_RIGHT)
					snake->direction = KEY_LEFT;
				break;
			case KEY_RIGHT:
				if (snake->direction != KEY_LEFT)
					snake->direction = KEY_RIGHT;
				break;
			case 'q':
				endwin();
				return 0;
			}
		}
		// 生成食物
		newFood(snake, food, max_x, max_y);

		// 移动蛇
		moveSnake(snake);

		// 检查碰撞
		if (checkCollision(snake, max_x, max_y))
		{
			break;
		}
		if (snake->body[0].x == food->pos.x && snake->body[0].y == food->pos.y)
		{
			snake->length++;
			score += 10;
			food->eaten = true;
			if (snake->length >= MAX_LENGTH)
			{
				break; // 蛇达到最大长度
			}
		}
		erase();

		// 绘制边框
		box(stdscr, 0, 0);

		// 绘制蛇
		for (int i = 0; i < snake->length; i++)
		{
			mvprintw(snake->body[i].y, snake->body[i].x, i == 0 ? "@" : "o");
		}

		// 绘制食物
		mvprintw(food->pos.y, food->pos.x, "*");

		// 显示分数
		mvprintw(0, 2, "Score: %d", score);

		// 刷新屏幕
		refresh();

		// 控制游戏速度
		usleep(DELAY);
	}
	// 游戏结束
	mvprintw(max_y / 2, max_x / 2 - 5, "GAME OVER!");
	mvprintw(max_y / 2 + 1, max_x / 2 - 8, "Final Score: %d", score);
	refresh();
	timeout(-1);
	getch();

	delete snake;
	delete food;
	endwin();
	return 0;
}