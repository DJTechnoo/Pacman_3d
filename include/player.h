#ifndef  __PLAYER_H
#define __PLAYER_H

//#include <GL/glew.h>
//#include <GLFW/glfw3.h>
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>


class Player {
private:
	glm::vec3 pos;
public:
	float speed;
	glm::vec3 lookahead;
	int direction;

	Player();
	Player(float spd, glm::vec3 startPos);
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
