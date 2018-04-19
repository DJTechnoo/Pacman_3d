#include "user.h"
#include <iostream>

User::User()
{
	lives = 4;
	gameOver = false;
	score = 0;
}


User::User(int lv)
{
	lives = lv;
	gameOver = false;
	score = 0;
}
void User::updateScore()
{
	score++;
}


void User::loseLive()
{
	lives--;
	if (lives == 0) {
		gameOver = true;
	}
}

void User::display()
{
	std::cout << "scr " << score << '\n';
	std::cout << "lvs " << lives << '\n';
}