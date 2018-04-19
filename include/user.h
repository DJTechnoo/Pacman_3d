#ifndef __USER_H
#define __USER_H


class User {
private:
	unsigned int MAXSCORE;
	unsigned int lives, score;
	bool gameOver, won;
public:
	User();
	User(unsigned int lv, unsigned int maxScore);
	void updateScore();
	void loseLive();
	void display();
};


#endif