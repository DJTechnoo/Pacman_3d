#ifndef __USER_H
#define __USER_H


class User {
public:
	int lives, score;
	bool gameOver;
	User();
	User(int lv);
	void updateScore();
	void loseLive();
	void display();
};


#endif