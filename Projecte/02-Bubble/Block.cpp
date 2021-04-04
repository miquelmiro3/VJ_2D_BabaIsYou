#include <cmath>
#include <iostream>
#include <GL/glew.h>
#include <GL/glut.h>
#include "Block.h"
#include "Game.h"

enum BlockMoves
{
	BSTAND, BSTAND_LEFT, BSTAND_RIGHT, BSTAND_UP, BSTAND_DOWN, BMOVE_LEFT, BMOVE_RIGHT, BMOVE_UP, BMOVE_DOWN
};

enum PlayerAnims
{
	STAND_RIGHT, STAND_LEFT, STAND_UP, STAND_DOWN, MOVE_LEFT, MOVE_RIGHT, MOVE_UP, MOVE_DOWN
};

Block::~Block()
{
	if (sprite != NULL)
		delete sprite;
}

void Block::init(const glm::ivec2 &tileMapPos, ShaderProgram &shaderProgram, const int &tex, Texture* spritesheet)
{
	id = tex;
	sprite = Sprite::createSprite(glm::ivec2(24, 24), glm::vec2(1.f / 32.f, 1.f / 66.f), spritesheet, &shaderProgram);
	sprite->setNumberAnimations(8);

		sprite->setAnimationSpeed(STAND_RIGHT, 2);
			sprite->addKeyframe(STAND_RIGHT, glm::vec2(float((tex - 1) % 32) * (1.f / 32.f), float((tex - 1) / 32) * (1.f / 66.f)));
			sprite->addKeyframe(STAND_RIGHT, glm::vec2(float((tex - 1) % 32) * (1.f / 32.f), float((tex - 1) / 32 + 1) * (1.f / 66.f)));
			sprite->addKeyframe(STAND_RIGHT, glm::vec2(float((tex - 1) % 32) * (1.f / 32.f), float((tex - 1) / 32 + 2) * (1.f / 66.f)));

		sprite->setAnimationSpeed(STAND_LEFT, 2);
			sprite->addKeyframe(STAND_LEFT, glm::vec2(10 * (1.f / 32.f), 0 * (1.f / 66.f)));
			sprite->addKeyframe(STAND_LEFT, glm::vec2(10 * (1.f / 32.f), 1 * (1.f / 66.f)));
			sprite->addKeyframe(STAND_LEFT, glm::vec2(10 * (1.f / 32.f), 2 * (1.f / 66.f)));

		sprite->setAnimationSpeed(STAND_UP, 2);
			sprite->addKeyframe(STAND_UP, glm::vec2(5 * (1.f / 32.f), 0 * (1.f / 66.f)));
			sprite->addKeyframe(STAND_UP, glm::vec2(5 * (1.f / 32.f), 1 * (1.f / 66.f)));
			sprite->addKeyframe(STAND_UP, glm::vec2(5 * (1.f / 32.f), 2 * (1.f / 66.f)));

		sprite->setAnimationSpeed(STAND_DOWN, 2);
			sprite->addKeyframe(STAND_DOWN, glm::vec2(15 * (1.f / 32.f), 0 * (1.f / 66.f)));
			sprite->addKeyframe(STAND_DOWN, glm::vec2(15 * (1.f / 32.f), 1 * (1.f / 66.f)));
			sprite->addKeyframe(STAND_DOWN, glm::vec2(15 * (1.f / 32.f), 2 * (1.f / 66.f)));

		sprite->setAnimationSpeed(MOVE_LEFT, 8);
			sprite->addKeyframe(MOVE_LEFT, glm::vec2(11 * (1.f / 32.f), 0 * (1.f / 66.f)));
			sprite->addKeyframe(MOVE_LEFT, glm::vec2(12 * (1.f / 32.f), 0 * (1.f / 66.f)));
			sprite->addKeyframe(MOVE_LEFT, glm::vec2(13 * (1.f / 32.f), 0 * (1.f / 66.f)));
			sprite->addKeyframe(MOVE_LEFT, glm::vec2(14 * (1.f / 32.f), 0 * (1.f / 66.f)));

		sprite->setAnimationSpeed(MOVE_RIGHT, 8);
			sprite->addKeyframe(MOVE_RIGHT, glm::vec2(1 * (1.f / 32.f), 0 * (1.f / 66.f)));
			sprite->addKeyframe(MOVE_RIGHT, glm::vec2(2 * (1.f / 32.f), 0 * (1.f / 66.f)));
			sprite->addKeyframe(MOVE_RIGHT, glm::vec2(3 * (1.f / 32.f), 0 * (1.f / 66.f)));
			sprite->addKeyframe(MOVE_RIGHT, glm::vec2(4 * (1.f / 32.f), 0 * (1.f / 66.f)));

		sprite->setAnimationSpeed(MOVE_UP, 8);
			sprite->addKeyframe(MOVE_UP, glm::vec2(6 * (1.f / 32.f), 0 * (1.f / 66.f)));
			sprite->addKeyframe(MOVE_UP, glm::vec2(7 * (1.f / 32.f), 0 * (1.f / 66.f)));
			sprite->addKeyframe(MOVE_UP, glm::vec2(8 * (1.f / 32.f), 0 * (1.f / 66.f)));
			sprite->addKeyframe(MOVE_UP, glm::vec2(9 * (1.f / 32.f), 0 * (1.f / 66.f)));

		sprite->setAnimationSpeed(MOVE_DOWN, 8);
			sprite->addKeyframe(MOVE_DOWN, glm::vec2(16 * (1.f / 32.f), 0 * (1.f / 66.f)));
			sprite->addKeyframe(MOVE_DOWN, glm::vec2(17 * (1.f / 32.f), 0 * (1.f / 66.f)));
			sprite->addKeyframe(MOVE_DOWN, glm::vec2(18 * (1.f / 32.f), 0 * (1.f / 66.f)));
			sprite->addKeyframe(MOVE_DOWN, glm::vec2(19 * (1.f / 32.f), 0 * (1.f / 66.f)));

	sprite->changeAnimation(0);
	tileMapDispl = tileMapPos;
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posBlock.x), float(tileMapDispl.y + posBlock.y)));
}

bool Block::update(int deltaTime, const bool& babaisyou)
{
	sprite->update(deltaTime);
	bool finish = false;
	if (move == BMOVE_LEFT) {
		if (posBlock.x % 24 != 0) posBlock.x -= 2;
		if (posBlock.x % 24 == 0) {
			move = BSTAND_LEFT;
			finish = true;
		}
	}
	else if (move == BMOVE_RIGHT) {
		if (posBlock.x % 24 != 0) posBlock.x += 2;
		if (posBlock.x % 24 == 0) {
			move = BSTAND_RIGHT;
			finish = true;
		}
	}
	else if (move == BMOVE_UP) {
		if (posBlock.y % 24 != 0) posBlock.y -= 2;
		if (posBlock.y % 24 == 0) {
			move = BSTAND_UP;
			finish = true;
		}
	}
	else if (move == BMOVE_DOWN) {
		if (posBlock.y % 24 != 0) posBlock.y += 2;
		if (posBlock.y % 24 == 0) {
			move = BSTAND_DOWN;
			finish = true;
		}
	}
	else if (move == BSTAND_LEFT || move == BSTAND_RIGHT || move == BSTAND_UP || move == BSTAND_DOWN) {
		if (id == 1) {
			if (move == BSTAND_LEFT && babaisyou) sprite->changeAnimation(STAND_LEFT);
			else if (move == BSTAND_RIGHT && babaisyou) sprite->changeAnimation(STAND_RIGHT);
			else if (move == BSTAND_UP && babaisyou) sprite->changeAnimation(STAND_UP);
			else if (move == BSTAND_DOWN && babaisyou) sprite->changeAnimation(STAND_DOWN);
		}
		move = BSTAND;
	}
	if (id == 1) {
		if (!babaisyou && sprite->animation() == MOVE_LEFT) sprite->changeAnimation(STAND_LEFT);
		else if (!babaisyou && sprite->animation() == MOVE_RIGHT) sprite->changeAnimation(STAND_RIGHT);
		else if (!babaisyou && sprite->animation() == MOVE_UP) sprite->changeAnimation(STAND_UP);
		else if (!babaisyou && sprite->animation() == MOVE_DOWN) sprite->changeAnimation(STAND_DOWN);
	}
	else {
		if (sprite->animation() != STAND_RIGHT) sprite->changeAnimation(STAND_RIGHT);
	}

	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posBlock.x), float(tileMapDispl.y + posBlock.y)));
	return finish;
}

void Block::render()
{
	sprite->render();
}

void Block::setPosition(const glm::vec2 &pos)
{
	posBlock = pos;
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posBlock.x), float(tileMapDispl.y + posBlock.y)));
}

int Block::getId() 
{
	return id;
}

glm::vec2 Block::getPos()
{
	return posBlock;
}

void Block::changeMove(const int & mov, const bool& babaisyou)
{
	if (mov == BMOVE_LEFT) {
		posBlock.x -= 2;
		if (id == 1 && babaisyou && move != BSTAND_LEFT) sprite->changeAnimation(MOVE_LEFT);
		sprite->setPosition(glm::vec2(float(tileMapDispl.x + posBlock.x), float(tileMapDispl.y + posBlock.y)));
	}
	else if (mov == BMOVE_RIGHT) {
		posBlock.x += 2;
		if (id == 1 && babaisyou && move != BSTAND_RIGHT) sprite->changeAnimation(MOVE_RIGHT);
		sprite->setPosition(glm::vec2(float(tileMapDispl.x + posBlock.x), float(tileMapDispl.y + posBlock.y)));
	}
	else if (mov == BMOVE_UP) {
		posBlock.y -= 2;
		if (id == 1 && babaisyou && move != BSTAND_UP) sprite->changeAnimation(MOVE_UP);
		sprite->setPosition(glm::vec2(float(tileMapDispl.x + posBlock.x), float(tileMapDispl.y + posBlock.y)));
	}		
	else if (mov == BMOVE_DOWN) {
		posBlock.y += 2;
		if (id == 1 && babaisyou && move != BSTAND_DOWN) sprite->changeAnimation(MOVE_DOWN);
		sprite->setPosition(glm::vec2(float(tileMapDispl.x + posBlock.x), float(tileMapDispl.y + posBlock.y)));
	}
	move = mov;
}

void Block::changeAnimationFrames(const int& tex)
{
	id = tex;
	sprite->removeAnimationFrames(0);
	sprite->addKeyframe(0, glm::vec2(float((tex - 1) % 32) * (1.f / 32.f), float((tex - 1) / 32) * (1.f / 66.f)));
	sprite->addKeyframe(0, glm::vec2(float((tex - 1) % 32) * (1.f / 32.f), float((tex - 1) / 32 + 1) * (1.f / 66.f)));
	sprite->addKeyframe(0, glm::vec2(float((tex - 1) % 32) * (1.f / 32.f), float((tex - 1) / 32 + 2) * (1.f / 66.f)));
	sprite->changeAnimation(0);
}