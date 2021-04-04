#ifndef _TILE_MAP_INCLUDE
#define _TILE_MAP_INCLUDE


#include <glm/glm.hpp>
#include "Texture.h"
#include "ShaderProgram.h"


#include "Block.h"
#include <irrKlang.h>

using namespace irrklang;

#pragma comment(lib, "irrKlang.lib")

// Class Tilemap is capable of loading a tile map from a text file in a very
// simple format (see level01.txt for an example). With this information
// it builds a single VBO that contains all tiles. As a result the render
// method draws the whole map independently of what is visible.


class TileMap
{

public:
	// Tile maps can only be created inside an OpenGL context
	static TileMap *createTileMap(const string &levelFile, const glm::vec2 &minCoords, ShaderProgram &program);

	TileMap(const string &levelFile, const glm::vec2 &minCoords, ShaderProgram &program);
	~TileMap();

	void render() const;
	void free();
	
	int getTileSize() const { return tileSize; }
	glm::ivec2 getMapSize() const { return mapSize; }

	bool pCollisionMoveLeft();
	bool pCollisionMoveRight();
	bool pCollisionMoveUp();
	bool pCollisionMoveDown();

	bool collisionMoveLeft(const glm::ivec2 &pos, const glm::ivec2 &size, bool s);
	bool collisionMoveRight(const glm::ivec2 &pos, const glm::ivec2 &size, bool s);
	bool collisionMoveUp(const glm::ivec2 &pos, const glm::ivec2 &size, bool s);
	bool collisionMoveDown(const glm::ivec2 &pos, const glm::ivec2 &size, bool s);

	bool isText(const int& i) const;
	bool isBlock(const int& i) const;
	bool isNoun(const int& i) const;
	bool isProp(const int& i) const;
	bool didIWin();
	void didILose();
	void update(int deltaTime);
	void initProp();
	void resetProp();
	block_propieties* blockPropieties(const bool& block, const int& id);
	void updateProp();
	void subUpdateProp(vector<int> left, vector<int> right);
	void removeElement(vector<Block*>& vec, const int& e);
	void clearMovingBlocks();
	void soundSuper();

private:
	bool loadLevel(const string &levelFile);
	void prepareArrays(const glm::vec2 &minCoords, ShaderProgram &program);

	void lookUp(vector<int>& up, bool and, int j, int i);
	void lookLeft(vector<int>& left, bool and, int j, int i);
	void lookRight(vector<int>& right, bool and, int j, int i);
	void lookDown(vector<int>& down, bool and, int j, int i);

private:
	GLuint vao;
	GLuint vbo;
	GLint posLocation, texCoordLocation;
	glm::ivec2 position, mapSize, tilesheetSize;
	int tileSize, blockSize;
	Texture tilesheet;
	glm::vec2 tileTexSize;
	int *map;

	vector<vector<Block*>> blocks;
	vector<Block*> movingBlocks;
	Block* *texts;
	
	vector<block_propieties> prop;
	bool babaisyou;

	//sounds
	ISoundEngine* movement;
	ISoundEngine* effects;
	ISoundEngine* pushSound;
	ISoundEngine* destroySound;
	bool oneSound;
	bool onePush;
	bool oneDestroy;
	bool superActive;
	vector<int> supers;

};


#endif // _TILE_MAP_INCLUDE


