#include "user.h"
#include <iostream>

User::User()
{
	lives = 4;
	gameOver = won = false;
	score = 0;
	MAXSCORE = 100;
}


User::User(unsigned int lv, unsigned int maxScore)
{
	lives = lv;
	gameOver = won = false;
	score = 0;
	MAXSCORE = maxScore;
}
void User::updateScore()
{
	if (!won) {
		score++;
		if (score >= MAXSCORE) won = true;
	}
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
	if (!won) {
		std::cout << "scr " << score << '\n';
		std::cout << "lvs " << lives << '\n';
	}
	else if (gameOver) std::cout << "LOSER \n";
	else std::cout << "WINNER \n";
	
}