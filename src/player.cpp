#include "player.h"

Player::Player() {};

Player::Player(float spd, glm::vec3 startPos)
{
	speed = spd;
	pos = startPos; 
	direction = -1;
}


Player::Player(float spd,float stX, float stY, float stZ)
{
	speed = spd;
	pos.x = stX;
	pos.y = stY;
	pos.z = stZ;
	direction = -1;
}

void Player::update(float dt)
{
	switch (direction) {
	case 0: pos.y += speed * dt;  break;		// UP
	case 1: pos.x -= speed * dt; break;			// LEFT
	case 2: pos.y -= speed * dt; break;			// DOWN
	case 3: pos.x += speed * dt; break;			// RIGHT
	default: break;
	}
}

glm::vec3 Player::getPlayerPos()
{
	return pos;
}

float Player::getX()
{
	return pos.x;
}
float Player::getY()
{
	return pos.y;
}
float Player::getZ()
{
	return pos.z;
}