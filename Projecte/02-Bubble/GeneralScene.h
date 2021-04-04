#ifndef _GENERALSCENE_INCLUDE
#define _GENERALSCENE_INCLUDE


#include <glm/glm.hpp>
#include <string>
#include "ShaderProgram.h"
#include "TileMap.h"
#include "Player.h"


// Scene contains all the entities of our game.
// It is responsible for updating and render them.


class GeneralScene
{

public:
	GeneralScene();
	~GeneralScene();

	void init();
	bool update(int deltaTime);
	void render();

	void prepareMenu();
	void prepareInstructions();
	void prepareCredits();

	int click(int x, int y, int width, int height);
	void print();

private:
	void initShaders();

private:
	ShaderProgram texProgram;
	float currentTime;
	glm::mat4 projection;

	int scenes;

	GLuint vao;
	GLuint vbo;
	GLint posLocation, texCoordLocation;
	Texture tilesheet;

	int quads;
	vector<glm::vec2> positions;
	vector<glm::vec2> blockSize;
	vector<glm::vec2> texCoordMin;
	vector<glm::vec2> texCoordMax;

};


#endif // _GENERALSCENE_INCLUDE

