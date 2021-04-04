#include <cmath>
#include <iostream>
#include <GL/glew.h>
#include <GL/glut.h>
#include "Player.h"
#include "Game.h"

enum PlayerAnims
{
	STAND_LEFT, STAND_RIGHT, STAND_UP, STAND_DOWN, MOVE_LEFT, MOVE_RIGHT, MOVE_UP, MOVE_DOWN
};

void Player::init()
{
	bMoving = false;
}

void Player::update(int deltaTime)
{
	if (!bMoving) {
		bool aux = false;
		if (Game::instance().getSpecialKey(GLUT_KEY_LEFT)) aux = map->pCollisionMoveLeft();
		else if (Game::instance().getSpecialKey(GLUT_KEY_RIGHT)) aux = map->pCollisionMoveRight();
		else if (Game::instance().getSpecialKey(GLUT_KEY_UP)) aux = map->pCollisionMoveUp();
		else if (Game::instance().getSpecialKey(GLUT_KEY_DOWN)) aux = map->pCollisionMoveDown();
		if (aux) {
			bMoving = true;
			cont = 20;
		}
	}
	else {
		cont -= 2;
		if (cont == 0) {
			bMoving = false;
			map->updateProp();
			map->didILose();
			map->clearMovingBlocks();
		}
	}
}

bool Player::didYouWin() {
	return !bMoving && map->didIWin();
}

void Player::setTileMap(TileMap *tileMap)
{
	map = tileMap;
}