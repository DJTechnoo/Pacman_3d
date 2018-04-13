#ifndef  __PLAYER_H
#define __PLAYER_H

//#include <GL/glew.h>
//#include <GLFW/glfw3.h>
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>


class Player {
private:
public:
	glm::vec3 pos;
	float speed;
	glm::vec3 lookahead;
	int direction;
	bool isGhost;

	Player();
	Player(int startDir, bool ghost, float spd, glm::vec3 startPos);
	Player(float spd, glm::vec3 startPos);
	Player(int startDir, bool ghost, float spd, float stX, float stY, float stZ);
	Player(float spd, float stX, float stY, float stZ);
	void update(float dt);
	void setPos(float d);
	glm::vec3 getPlayerPos();
	float getX();
	float getY();
	float getZ();

	glm::vec3 getLookPos();
	float lookX();
	float lookY();
	float lookZ();
};







#endif 
