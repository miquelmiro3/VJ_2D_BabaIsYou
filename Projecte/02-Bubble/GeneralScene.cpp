#include <iostream>
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>
#include "GeneralScene.h"
#include "Game.h"


#define SCREEN_X 80
#define SCREEN_Y 0


GeneralScene::GeneralScene()
{
}

GeneralScene::~GeneralScene()
{
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
	glDisableVertexArrayAttrib(GL_TEXTURE_2D, posLocation);
	glDisableVertexArrayAttrib(GL_TEXTURE_2D, texCoordLocation);
	positions.clear();
	blockSize.clear();
	texCoordMin.clear();
	texCoordMax.clear();
}


void GeneralScene::init()
{
	quads = 0;
	initShaders();

	projection = glm::ortho(0.f, float(SCREEN_WIDTH - 1), float(SCREEN_HEIGHT - 1), 0.f);
	currentTime = 0.0f;
}

bool GeneralScene::update(int deltaTime)
{
	currentTime += deltaTime;
	return false;
}

void GeneralScene::render()
{
	glm::mat4 modelview;

	texProgram.use();
	texProgram.setUniformMatrix4f("projection", projection);
	texProgram.setUniform4f("color", 1.0f, 1.0f, 1.0f, 1.0f);
	modelview = glm::mat4(1.0f);
	texProgram.setUniformMatrix4f("modelview", modelview);
	texProgram.setUniform2f("texCoordDispl", 0.f, 0.f);

	glEnable(GL_TEXTURE_2D);
	tilesheet.use();
	glBindVertexArray(vao);
	glEnableVertexAttribArray(posLocation);
	glEnableVertexAttribArray(texCoordLocation);
	glDrawArrays(GL_TRIANGLES, 0, 6 * quads);
	glDisable(GL_TEXTURE_2D);
}

void GeneralScene::initShaders()
{
	Shader vShader, fShader;

	vShader.initFromFile(VERTEX_SHADER, "shaders/texture.vert");
	if(!vShader.isCompiled())
	{
		cout << "Vertex Shader Error" << endl;
		cout << "" << vShader.log() << endl << endl;
	}
	fShader.initFromFile(FRAGMENT_SHADER, "shaders/texture.frag");
	if(!fShader.isCompiled())
	{
		cout << "Fragment Shader Error" << endl;
		cout << "" << fShader.log() << endl << endl;
	}
	texProgram.init();
	texProgram.addShader(vShader);
	texProgram.addShader(fShader);
	texProgram.link();
	if(!texProgram.isLinked())
	{
		cout << "Shader Linking Error" << endl;
		cout << "" << texProgram.log() << endl << endl;
	}
	texProgram.bindFragmentOutput("outColor");
	vShader.free();
	fShader.free();
}

void GeneralScene::prepareMenu()
{
	quads = 5;
	scenes = 0;
	tilesheet.loadFromFile("images/menu.png", TEXTURE_PIXEL_FORMAT_RGBA);
	tilesheet.setWrapS(GL_CLAMP_TO_EDGE);
	tilesheet.setWrapT(GL_CLAMP_TO_EDGE);
	tilesheet.setMinFilter(GL_NEAREST);
	tilesheet.setMagFilter(GL_NEAREST);

	positions = {
		glm::vec2(80,20), glm::vec2(200,160), glm::vec2(200,240), glm::vec2(200,320), glm::vec2(240,400)
	};
	blockSize = {
		glm::vec2(480,120), glm::vec2(240,60), glm::vec2(240,60), glm::vec2(240,60), glm::vec2(160,40)
	};
	texCoordMin = {
		glm::vec2(0,0), glm::vec2(0,0.5), glm::vec2(0.5,0.5), glm::vec2(0,0.75), glm::vec2(0.5,0.75)
	};
	texCoordMax = {
		glm::vec2(1,0.5), glm::vec2(0.5,0.75), glm::vec2(1,0.75), glm::vec2(0.5,1), glm::vec2(0.75,0.875)
	};
	/*positions = {
		glm::vec2(0,0), glm::vec2(80,20), glm::vec2(200,160), glm::vec2(200,240), glm::vec2(200,320), glm::vec2(240,400)
	};
	blockSize = {
		glm::vec2(640,480), glm::vec2(480,120), glm::vec2(240,60), glm::vec2(240,60), glm::vec2(240,60), glm::vec2(160,40)
	};
	texCoordMin = {
		glm::vec2(0,0), glm::vec2(0.5,0), glm::vec2(0.5,0.25), glm::vec2(0.75,0.25), glm::vec2(0.5,0.375), glm::vec2(0.75,0.375)
	};
	texCoordMax = {
		glm::vec2(0.5,1), glm::vec2(1,0.25), glm::vec2(0.75,0.375), glm::vec2(1,0.375), glm::vec2(0.75,0.5), glm::vec2(0.875,0.4375)
	};*/
	
	print();
}

void GeneralScene::prepareInstructions()
{
	quads = 1;
	scenes = 1;

	tilesheet.loadFromFile("images/instructions.png", TEXTURE_PIXEL_FORMAT_RGBA);
	tilesheet.setWrapS(GL_CLAMP_TO_EDGE);
	tilesheet.setWrapT(GL_CLAMP_TO_EDGE);
	tilesheet.setMinFilter(GL_NEAREST);
	tilesheet.setMagFilter(GL_NEAREST);

	positions = { glm::vec2(0,0) };
	blockSize = { glm::vec2(640,480) };
	texCoordMin = { glm::vec2(0,0) };
	texCoordMax = { glm::vec2(1,1) };
	
	print();
}

void GeneralScene::prepareCredits()
{
	quads = 1;
	scenes = 1;
	/*text.init("fonts/OpenSans-Regular.ttf");
	texts = {
		"INSTRUCTIONS",
		"ARROWS: Move",
		"R : Restart",
		"Esc : Quit | Back to Menu",
		"You need to win with the rules in the map.You can change the rules and make the way to the victory as you want.This game will make you spent hours of fun!"
	};
	textPos = {	glm::vec2(220, 40), glm::vec2(260, 100), glm::vec2(260, 180), glm::vec2(220, 260), glm::vec2(40, 340) };
	textSize = { 48,28,28,28, 18 };*/

	tilesheet.loadFromFile("images/credits.png", TEXTURE_PIXEL_FORMAT_RGBA);
	tilesheet.setWrapS(GL_CLAMP_TO_EDGE);
	tilesheet.setWrapT(GL_CLAMP_TO_EDGE);
	tilesheet.setMinFilter(GL_NEAREST);
	tilesheet.setMagFilter(GL_NEAREST);

	positions = { glm::vec2(0,0) };
	blockSize = { glm::vec2(640,480) };
	texCoordMin = { glm::vec2(0,0) };
	texCoordMax = { glm::vec2(1,1) };

	print();
}

void GeneralScene::print() 
{
	int nTiles = 0;
	glm::vec2 texCoordTile[2];
	vector<float> vertices;
	for (int i = 0; i < positions.size(); i++) {
		// Non-empty tile
		nTiles++;
		texCoordTile[0] = texCoordMin[i];
		texCoordTile[1] = texCoordMax[i];
		// First triangle
		vertices.push_back(positions[i].x); vertices.push_back(positions[i].y);
		vertices.push_back(texCoordTile[0].x); vertices.push_back(texCoordTile[0].y);
		vertices.push_back(positions[i].x + blockSize[i].x); vertices.push_back(positions[i].y);
		vertices.push_back(texCoordTile[1].x); vertices.push_back(texCoordTile[0].y);
		vertices.push_back(positions[i].x + blockSize[i].x); vertices.push_back(positions[i].y + blockSize[i].y);
		vertices.push_back(texCoordTile[1].x); vertices.push_back(texCoordTile[1].y);
		// Second triangle
		vertices.push_back(positions[i].x); vertices.push_back(positions[i].y);
		vertices.push_back(texCoordTile[0].x); vertices.push_back(texCoordTile[0].y);
		vertices.push_back(positions[i].x + blockSize[i].x); vertices.push_back(positions[i].y + blockSize[i].y);
		vertices.push_back(texCoordTile[1].x); vertices.push_back(texCoordTile[1].y);
		vertices.push_back(positions[i].x); vertices.push_back(positions[i].y + blockSize[i].y);
		vertices.push_back(texCoordTile[0].x); vertices.push_back(texCoordTile[1].y);
	}

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, 24 * nTiles * sizeof(float), &vertices[0], GL_STATIC_DRAW);
	posLocation = texProgram.bindVertexAttribute("position", 2, 4 * sizeof(float), 0);
	texCoordLocation = texProgram.bindVertexAttribute("texCoord", 2, 4 * sizeof(float), (void*)(2 * sizeof(float)));
}

int GeneralScene::click(int x, int y, int width, int height)
{
	float widthFactor = (float)width / 640.0f;
	float heightFactor = (float)height / 480.0f;
	int block = -1;

	float xmin = (widthFactor * (float)positions[2].x),
		xmax = (widthFactor * (float)positions[2].x + widthFactor * (float)blockSize[2].x),
		ymin = (heightFactor * (float)positions[2].y),
		ymax = (heightFactor * (float)positions[2].y + heightFactor * (float)blockSize[2].y);

	for (int i = 1; i < positions.size(); i++) {
		xmin = (widthFactor * (float)positions[i].x);
		xmax = (widthFactor * (float)positions[i].x + widthFactor * (float)blockSize[i].x);
		ymin = (heightFactor * (float)positions[i].y);
		ymax = (heightFactor * (float)positions[i].y + heightFactor * (float)blockSize[i].y);
		if (x > xmin && x < xmax && y > ymin && y < ymax) block = i;
	}

	if (block == 0) return -1;
	else return block;

	/*if (block == 1) return -1;
	else return block - 1;*/
}