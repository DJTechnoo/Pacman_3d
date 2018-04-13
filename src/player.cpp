#include "player.h"

Player::Player() {};

Player::Player(int startDir, bool ghost, float spd, glm::vec3 startPos)
{
	speed = spd;
	pos = startPos;
	direction = startDir;
	isGhost = ghost;

}


Player::Player(int startDir, bool ghost, float spd, float stX, float stY, float stZ)
{
	speed = spd;
	pos.x = stX;
	pos.y = stY;
	pos.z = stZ;
	direction = startDir;
	isGhost = ghost;
}

Player::Player(float spd, glm::vec3 startPos)
{
	speed = spd;
	pos = startPos; 
	direction = -1;
	isGhost = false;
	
}


Player::Player(float spd,float stX, float stY, float stZ)
{
	speed = spd;
	pos.x = stX;
	pos.y = stY;
	pos.z = stZ;
	direction = -1;
	isGhost = false;
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

	switch (direction) {
	case 0: lookahead.y = pos.y + speed * dt; lookahead.x = pos.x;  break;		// UP
	case 1: lookahead.x = pos.x - speed * dt; lookahead.y = pos.y; break;			// LEFT
	case 2: lookahead.y = pos.y - speed * dt; lookahead.x = pos.x; break;			// DOWN
	case 3: lookahead.x = pos.x + speed * dt; lookahead.y = pos.y; break;			// RIGHT
	default: lookahead = pos; break;
	}
}

void Player::setPos(float d)
{
	switch (direction)
	{
	case 0: pos.y += d; break;
	case 1: pos.x -= d; break;
	case 2: pos.y -= d; break;
	case 3: pos.x += d; break;
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




glm::vec3 Player::getLookPos()
{
	return lookahead;
}

float Player::lookX()
{
	return lookahead.x;
}
float Player::lookY()
{
	return lookahead.y;
}
float Player::lookZ()
{
	return lookahead.z;
}

