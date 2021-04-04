#ifndef _BLOCK_INCLUDE
#define _BLOCK_INCLUDE


#include "Sprite.h"

//BLOCKS
#define WALL_BLOCK 692
#define FLAG_BLOCK 679
#define ROCK_BLOCK 688
#define LAVA_BLOCK 1841
#define BABA_BLOCK 1

//TEXTS
#define WALL 1084
#define FLAG 962
#define BABA 871
#define ROCK 1068
#define LAVA 985
#define IS 979
#define AND 868
#define STOP 1357
#define WIN 1362
#define YOU 1365
#define PUSH 1347
#define DEFEAT 1254
#define SUPER 1256

// Player is basically a Sprite that represents the player. As such it has
// all properties it needs to track its movement, jumping, and collisions.


struct block_propieties {
	int idBlock;
	int idText;
	bool stop;
	bool win;
	bool you;
	bool push;
	bool defeat;
	bool super;
};


class Block
{

public:
	~Block();
	void init(const glm::ivec2& tileMapPos, ShaderProgram& shaderProgram, const int &tex, Texture* spritesheet);
	bool update(int deltaTime, const bool& babaisyou);
	void render();

	void setPosition(const glm::vec2& pos);

	int getId();
	glm::vec2 getPos();
	void changeMove(const int& mov, const bool& babaisyou);
	void changeAnimationFrames(const int& tex);

private:
	glm::ivec2 tileMapDispl, posBlock;
	Sprite* sprite;

	int id;
	int move;

};


#endif // _BLOCK_INCLUDE


