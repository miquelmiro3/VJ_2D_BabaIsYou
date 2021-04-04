#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "TileMap.h"

using namespace std;

enum BlockMoves
{
	BSTAND, BSTAND_LEFT, BSTAND_RIGHT, BSTAND_UP, BSTAND_DOWN, BMOVE_LEFT, BMOVE_RIGHT, BMOVE_UP, BMOVE_DOWN
};

TileMap *TileMap::createTileMap(const string &levelFile, const glm::vec2 &minCoords, ShaderProgram &program)
{
	TileMap *map = new TileMap(levelFile, minCoords, program);
	
	return map;
}


TileMap::TileMap(const string &levelFile, const glm::vec2 &minCoords, ShaderProgram &program)
{	
	initProp();

	effects = createIrrKlangDevice();
	effects->setSoundVolume(0.4);
	pushSound = createIrrKlangDevice();
	pushSound->setSoundVolume(0.4);
	destroySound = createIrrKlangDevice();
	destroySound->setSoundVolume(0.4);
	movement = createIrrKlangDevice();
	movement->setSoundVolume(0.6);
	oneSound = false;
	onePush = false;
	oneDestroy = false;
	superActive = false;
	loadLevel(levelFile);
	prepareArrays(minCoords, program);
}

TileMap::~TileMap()
{
	if(map != NULL)
		delete map;
	
	for (int j = 0; j < mapSize.y; j++) {
		for (int i = 0; i < mapSize.x; i++) {
			if (blocks[j * mapSize.x + i].size() != 0) {
				for (int e = 0; e < blocks[j * mapSize.x + i].size(); e++)
					delete blocks[j * mapSize.x + i][e];
				blocks[j * mapSize.x + i].clear();
			}
		}
	}
	for (int j = 0; j < mapSize.y; j++) {
		for (int i = 0; i < mapSize.x; i++) {
			if (texts[j * mapSize.x + i] != NULL) delete texts[j * mapSize.x + i];
		}
	}

	if (texts != NULL)
		delete texts;
	prop.clear();
	blocks.clear();
}


void TileMap::render() const
{
	glEnable(GL_TEXTURE_2D);
	tilesheet.use();
	glBindVertexArray(vao);
	glEnableVertexAttribArray(posLocation);
	glEnableVertexAttribArray(texCoordLocation);
	glDrawArrays(GL_TRIANGLES, 0, 6 * mapSize.x * mapSize.y);
	glDisable(GL_TEXTURE_2D);
	
	for (int j = 0; j < mapSize.y; j++)	{
		for (int i = 0; i < mapSize.x; i++)	{
			if (blocks[j * mapSize.x + i].size() != 0) {
				for (int e = 0; e < blocks[j * mapSize.x + i].size(); e++)
					blocks[j * mapSize.x + i][e]->render();
			}
		}
	}
	for (int i = 0; i < movingBlocks.size(); i++) movingBlocks[i]->render();
	for (int j = 0; j < mapSize.y; j++) {
		for (int i = 0; i < mapSize.x; i++) {
			if (texts[j * mapSize.x + i] != NULL) texts[j * mapSize.x + i]->render();
		}
	}
}

void TileMap::free()
{
	glDeleteBuffers(1, &vbo);
}

bool TileMap::loadLevel(const string &levelFile)
{
	ifstream fin;
	string line, tilesheetFile;
	stringstream sstream;
	char tile;
	
	fin.open(levelFile.c_str());
	if(!fin.is_open())
		return false;
	getline(fin, line);
	if(line.compare(0, 7, "TILEMAP") != 0)
		return false;
	getline(fin, line);
	sstream.str(line);
	sstream >> mapSize.x >> mapSize.y;
	getline(fin, line);
	sstream.str(line);
	sstream >> tileSize >> blockSize;
	getline(fin, line);
	sstream.str(line);
	sstream >> tilesheetFile;
	tilesheet.loadFromFile(tilesheetFile, TEXTURE_PIXEL_FORMAT_RGBA);
	tilesheet.setWrapS(GL_CLAMP_TO_EDGE);
	tilesheet.setWrapT(GL_CLAMP_TO_EDGE);
	tilesheet.setMinFilter(GL_NEAREST);
	tilesheet.setMagFilter(GL_NEAREST);
	getline(fin, line);
	sstream.str(line);
	sstream >> tilesheetSize.x >> tilesheetSize.y;
	tileTexSize = glm::vec2(1.f / tilesheetSize.x, 1.f / tilesheetSize.y);
	
	blocks.resize(mapSize.y * mapSize.x, vector<Block*>(0));
	movingBlocks.resize(0);
	texts = new Block*[mapSize.x * mapSize.y];
	map = new int[mapSize.x * mapSize.y];
	for (int j = 0; j<mapSize.y; j++) {
		for (int i = 0; i < mapSize.x; i++) {
			fin.get(tile);
			string aux = "";
			while (tile != ',') {
				aux += tile;
				fin.get(tile);
			}
			map[j * mapSize.x + i] = stoi(aux);
			texts[j * mapSize.x + i] = NULL;
		}
		fin.get(tile);
#ifndef _WIN32
		fin.get(tile);
#endif
	}
	fin.close();
	
	return true;
}

void TileMap::prepareArrays(const glm::vec2 &minCoords, ShaderProgram &program)
{
	int tile, nTiles = 0;
	glm::vec2 posTile, texCoordTile[2], halfTexel;
	vector<float> vertices;
	
	halfTexel = glm::vec2(0.5f / tilesheet.width(), 0.5f / tilesheet.height());
	for(int j=0; j<mapSize.y; j++)
	{
		for(int i=0; i<mapSize.x; i++)
		{
			tile = map[j * mapSize.x + i];
			if (tile == 0) {}
			else if (isBlock(tile)) {
				Block* b = new Block();
				b->init(minCoords, program, tile, &tilesheet);
				b->setPosition(glm::vec2(i * tileSize, j * tileSize));
				b->render();
				if (isText((tile))) texts[j * mapSize.x + i] = b;
				else blocks[j * mapSize.x + i].push_back(b);
				map[j * mapSize.x + i] = 0;
			}
			else {
				// Non-empty tile
				nTiles++;
				posTile = glm::vec2(minCoords.x + i * tileSize, minCoords.y + j * tileSize);
				texCoordTile[0] = glm::vec2(float((tile - 1) % 32) / tilesheetSize.x, float((tile - 1) / 32) / tilesheetSize.y);
				texCoordTile[1] = texCoordTile[0] + tileTexSize;
				//texCoordTile[0] += halfTexel;
				//texCoordTile[1] -= halfTexel;
				// First triangle
				vertices.push_back(posTile.x); vertices.push_back(posTile.y);
				vertices.push_back(texCoordTile[0].x); vertices.push_back(texCoordTile[0].y);
				vertices.push_back(posTile.x + blockSize); vertices.push_back(posTile.y);
				vertices.push_back(texCoordTile[1].x); vertices.push_back(texCoordTile[0].y);
				vertices.push_back(posTile.x + blockSize); vertices.push_back(posTile.y + blockSize);
				vertices.push_back(texCoordTile[1].x); vertices.push_back(texCoordTile[1].y);
				// Second triangle
				vertices.push_back(posTile.x); vertices.push_back(posTile.y);
				vertices.push_back(texCoordTile[0].x); vertices.push_back(texCoordTile[0].y);
				vertices.push_back(posTile.x + blockSize); vertices.push_back(posTile.y + blockSize);
				vertices.push_back(texCoordTile[1].x); vertices.push_back(texCoordTile[1].y);
				vertices.push_back(posTile.x); vertices.push_back(posTile.y + blockSize);
				vertices.push_back(texCoordTile[0].x); vertices.push_back(texCoordTile[1].y);
			}
		}
	}
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, 24 * nTiles * sizeof(float), &vertices[0], GL_STATIC_DRAW);
	posLocation = program.bindVertexAttribute("position", 2, 4*sizeof(float), 0);
	texCoordLocation = program.bindVertexAttribute("texCoord", 2, 4*sizeof(float), (void *)(2*sizeof(float)));

	updateProp();
}

// Collision tests for axis aligned bounding boxes.
// Method collisionMoveDown also corrects Y coordinate if the box is
// already intersecting a tile below.

bool TileMap::pCollisionMoveLeft() {
	bool aux = false;
	bool super = false;
	for (int j = 0; j < mapSize.y; j++) {
		for (int i = 0; i < mapSize.x; i++) {
			if (blocks[j * mapSize.x + i].size() != 0) {
				vector<int> you(0);
				super = false;
				for (int e = 0; e < blocks[j * mapSize.x + i].size(); e++) {
					block_propieties* block = blockPropieties(true, blocks[j * mapSize.x + i][e]->getId());
					if (block->you) {
						you.push_back(e);
						if (block->super) super = true;
					}
				}
				if (you.size() != 0) {
					glm::ivec2 pos = blocks[j * mapSize.x + i][you[0]]->getPos();
					if (!collisionMoveLeft(glm::ivec2(pos.x - tileSize, pos.y), glm::ivec2(tileSize, tileSize), super)) {
						aux = true;
						for (int e = you.size() - 1; e >= 0; e--) {
							Block* b = blocks[j * mapSize.x + i][you[e]];
							b->changeMove(BMOVE_LEFT, babaisyou);
							blocks[j * mapSize.x + (i - 1)].push_back(b);
							movingBlocks.push_back(b);
							removeElement(blocks[j * mapSize.x + i], you[e]);
						}
					}
				}
			}
		}
	}
	if (aux) {
		movement->play2D("sounds/move1.ogg");
		movement->play2D("sounds/move2.ogg");
	}
	onePush = false;
	oneDestroy = false;
	return aux;
}

bool TileMap::collisionMoveLeft(const glm::ivec2 &pos, const glm::ivec2 &size, bool s)
{
	int x, y0, y1;
	bool super = false;
	
	x = pos.x / tileSize;
	y0 = pos.y / tileSize;
	y1 = (pos.y + size.y - 1) / tileSize;
	for(int y=y0; y<=y1; y++)
	{
		if (map[y * mapSize.x + x] != 0) return true;
		else {
			vector<int> push(0);
			vector<int> destroy(0);
			if (blocks[y * mapSize.x + x].size() != 0) {
				for (int e = 0; e < blocks[y * mapSize.x + x].size(); e++) {
					int id = blocks[y * mapSize.x + x][e]->getId();
					block_propieties* block = blockPropieties(true, id);
					if (s && !block->super) destroy.push_back(e);
					else {
						if (!block->stop) {
							if (block->push) {
								push.push_back(e);
								super = block->super;
							}
						}
						else return true;
					}
				}
			}
			if (push.size() != 0 || texts[y * mapSize.x + x] != NULL) {
				if (!collisionMoveLeft(glm::ivec2(pos.x - size.x, pos.y), glm::ivec2(24, 24), super)) {
					int i = push.size() - 1;
					int j = destroy.size() - 1;
					while (i >= 0 || j >= 0) {
						if (i >= 0 && (j < 0 || push[i] > destroy[j])) {
							Block* b = blocks[y * mapSize.x + x][push[i]];
							b->changeMove(BMOVE_LEFT, babaisyou);
							blocks[y * mapSize.x + (x - 1)].push_back(b);
							movingBlocks.push_back(b);
							removeElement(blocks[y * mapSize.x + x], push[i]);
							if (!onePush) {
								pushSound->play2D("sounds/push.ogg");
								onePush = true;
							}
							--i;
						}
						else {
							if (!oneDestroy) {
								destroySound->play2D("sounds/break.mp3");
								oneDestroy = true;
							}
							removeElement(blocks[y * mapSize.x + x], destroy[j]);
							--j;
						}
					}
					if (texts[y * mapSize.x + x] != NULL) {
						Block* b = texts[y * mapSize.x + x];
						b->changeMove(BMOVE_LEFT, babaisyou);
						texts[y * mapSize.x + (x - 1)] = b;
						texts[y * mapSize.x + x] = NULL;
						if (!onePush) {
							pushSound->play2D("sounds/push.ogg");
							onePush = true;
						}
					}
					return false;
				}
				else return true;
			}
			else {
				if (destroy.size() > 0 && !oneDestroy) {
					destroySound->play2D("sounds/break.mp3");
					oneDestroy = true;
				}
				for (int i = destroy.size() - 1; i >= 0; i--) removeElement(blocks[y * mapSize.x + x], destroy[i]);
			}
			return false;
		}
	}
	return false;
}

bool TileMap::pCollisionMoveRight() {
	bool aux = false;
	bool super = false;
	for (int j = 0; j < mapSize.y; j++) {
		for (int i = mapSize.x-1; i >= 0; i--) {
			if (blocks[j * mapSize.x + i].size() != 0) {
				vector<int> you(0);
				super = false;
				for (int e = 0; e < blocks[j * mapSize.x + i].size(); e++) {
					block_propieties* block = blockPropieties(true, blocks[j * mapSize.x + i][e]->getId());
					if (block->you) {
						you.push_back(e);
						if (block->super) super = true;
					}
				}
				if (you.size() != 0) {
					glm::ivec2 pos = blocks[j * mapSize.x + i][you[0]]->getPos();
					if (!collisionMoveRight(glm::ivec2(pos.x + tileSize, pos.y), glm::ivec2(tileSize, tileSize), super)) {
						aux = true;
						for (int e = you.size() - 1; e >= 0; e--) {
							Block* b = blocks[j * mapSize.x + i][you[e]];
							b->changeMove(BMOVE_RIGHT, babaisyou);
							blocks[j * mapSize.x + (i + 1)].push_back(b);
							movingBlocks.push_back(b);
							removeElement(blocks[j * mapSize.x + i], you[e]);
						}
					}
				}
			}
		}
	}
	if (aux) {
		movement->play2D("sounds/move1.ogg");
		movement->play2D("sounds/move2.ogg");
	}
	onePush = false;
	oneDestroy = false;
	return aux;
}

bool TileMap::collisionMoveRight(const glm::ivec2 &pos, const glm::ivec2 &size, bool s)
{
	int x, y0, y1;
	bool super = false;

	x = (pos.x + size.x - 1) / tileSize;
	y0 = pos.y / tileSize;
	y1 = (pos.y + size.y - 1) / tileSize;
	for(int y=y0; y<=y1; y++)
	{
		if (map[y * mapSize.x + x] != 0) return true;
		else {
			vector<int> push(0);
			vector<int> destroy(0);
			if (blocks[y * mapSize.x + x].size() != 0) {
				for (int e = 0; e < blocks[y * mapSize.x + x].size(); e++) {
					int id = blocks[y * mapSize.x + x][e]->getId();
					block_propieties* block = blockPropieties(true, id);
					if (s && !block->super) destroy.push_back(e);
					else {
						if (!block->stop) {
							if (block->push) {
								push.push_back(e);
								super = block->super;
							}
						}
						else return true;
					}
				}
			}
			if (push.size() != 0 || texts[y * mapSize.x + x] != NULL) {
				if (!collisionMoveRight(glm::ivec2(pos.x + size.x, pos.y), glm::ivec2(24, 24), super)) {
					int i = push.size() - 1;
					int j = destroy.size() - 1;
					while (i >= 0 || j >= 0) {
						if (i >= 0 && (j < 0 || push[i] > destroy[j])) {
							Block* b = blocks[y * mapSize.x + x][push[i]];
							b->changeMove(BMOVE_RIGHT, babaisyou);
							blocks[y * mapSize.x + (x + 1)].push_back(b);
							movingBlocks.push_back(b);
							removeElement(blocks[y * mapSize.x + x], push[i]);
							if (!onePush) {
								pushSound->play2D("sounds/push.ogg");
								onePush = true;
							}
							--i;
						}
						else {
							if (!oneDestroy) {
								destroySound->play2D("sounds/break.mp3");
								oneDestroy = true;
							}
							removeElement(blocks[y * mapSize.x + x], destroy[j]);
							--j;
						}
					}
					if (texts[y * mapSize.x + x] != NULL) {
						Block* b = texts[y * mapSize.x + x];
						b->changeMove(BMOVE_RIGHT, babaisyou);
						texts[y * mapSize.x + (x + 1)] = b;
						texts[y * mapSize.x + x] = NULL;
						if (!onePush) {
							pushSound->play2D("sounds/push.ogg");
							onePush = true;
						}
					}
					return false;
				}
				else return true;
			}
			else {
				if (destroy.size() > 0 && !oneDestroy) {
					destroySound->play2D("sounds/break.mp3");
					oneDestroy = true;
				}
				for (int i = destroy.size() - 1; i >= 0; i--) removeElement(blocks[y * mapSize.x + x], destroy[i]);
			}
			return false;
		}
	}
	
	return false;
}

bool TileMap::pCollisionMoveUp() {
	bool aux = false;
	bool super = false;
	for (int j = 0; j < mapSize.y; j++) {
		for (int i = 0; i < mapSize.x; i++) {
			if (blocks[j * mapSize.x + i].size() != 0) {
				vector<int> you(0);
				super = false;
				for (int e = 0; e < blocks[j * mapSize.x + i].size(); e++) {
					block_propieties* block = blockPropieties(true, blocks[j * mapSize.x + i][e]->getId());
					if (block->you) {
						you.push_back(e);
						if (block->super) super = true;
					}
				}
				if (you.size() != 0) {
					glm::ivec2 pos = blocks[j * mapSize.x + i][you[0]]->getPos();
					if (!collisionMoveUp(glm::ivec2(pos.x, pos.y - tileSize), glm::ivec2(tileSize, tileSize), super)) {
						aux = true;
						for (int e = you.size() - 1; e >= 0; e--) {
							Block* b = blocks[j * mapSize.x + i][you[e]];
							b->changeMove(BMOVE_UP, babaisyou);
							blocks[(j - 1) * mapSize.x + i].push_back(b);
							movingBlocks.push_back(b);
							removeElement(blocks[j * mapSize.x + i], you[e]);
						}
					}
				}
			}
		}
	}
	if (aux) {
		movement->play2D("sounds/move1.ogg");
		movement->play2D("sounds/move2.ogg");
	}
	onePush = false;
	oneDestroy = false;
	return aux;
}

bool TileMap::collisionMoveUp(const glm::ivec2 &pos, const glm::ivec2 &size, bool s)
{
	int x0, x1, y;
	bool super = false;

	x0 = pos.x / tileSize;
	x1 = (pos.x + size.x - 1) / tileSize;
	y = pos.y / tileSize;
	for (int x = x0; x <= x1; x++)
	{
		if (map[y * mapSize.x + x] != 0) return true;
		else {
			vector<int> push(0);
			vector<int> destroy(0);
			if (blocks[y * mapSize.x + x].size() != 0) {
				for (int e = 0; e < blocks[y * mapSize.x + x].size(); e++) {
					int id = blocks[y * mapSize.x + x][e]->getId();
					block_propieties* block = blockPropieties(true, id);
					if (s && !block->super) destroy.push_back(e);
					else {
						if (!block->stop) {
							if (block->push) {
								push.push_back(e);
								super = block->super;
							}
						}
						else return true;
					}
				}
			}
			if (push.size() != 0 || texts[y * mapSize.x + x] != NULL) {
				if (!collisionMoveUp(glm::ivec2(pos.x, pos.y - size.y), glm::ivec2(24, 24), super)) {
					int i = push.size() - 1;
					int j = destroy.size() - 1;
					while (i >= 0 || j >= 0) {
						if (i >= 0 && (j < 0 || push[i] > destroy[j])) {
							Block* b = blocks[y * mapSize.x + x][push[i]];
							b->changeMove(BMOVE_UP, babaisyou);
							blocks[(y - 1) * mapSize.x + x].push_back(b);
							movingBlocks.push_back(b);
							removeElement(blocks[y * mapSize.x + x], push[i]);
							if (!onePush) {
								pushSound->play2D("sounds/push.ogg");
								onePush = true;
							}
							--i;
						}
						else {
							if (!oneDestroy) {
								destroySound->play2D("sounds/break.mp3");
								oneDestroy = true;
							}
							removeElement(blocks[y * mapSize.x + x], destroy[j]);
							--j;
						}
					}
					if (texts[y * mapSize.x + x] != NULL) {
						Block* b = texts[y * mapSize.x + x];
						b->changeMove(BMOVE_UP, babaisyou);
						texts[(y - 1) * mapSize.x + x] = b;
						texts[y * mapSize.x + x] = NULL;
						if (!onePush) {
							pushSound->play2D("sounds/push.ogg");
							onePush = true;
						}
					}
					return false;
				}
				else return true;
			}
			else {
				if (destroy.size() > 0 && !oneDestroy) {
					destroySound->play2D("sounds/break.mp3");
					oneDestroy = true;
				}
				for (int i = destroy.size() - 1; i >= 0; i--) removeElement(blocks[y * mapSize.x + x], destroy[i]);
			}
			return false;
		}
	}
	return false;
}

bool TileMap::pCollisionMoveDown() {
	bool aux = false;
	bool super = false;
	for (int j = mapSize.y-1; j >= 0; j--) {
		for (int i = 0; i < mapSize.x; i++) {
			if (blocks[j * mapSize.x + i].size() != 0) {
				vector<int> you(0);
				super = false;
				for (int e = 0; e < blocks[j * mapSize.x + i].size(); e++) {
					block_propieties* block = blockPropieties(true, blocks[j * mapSize.x + i][e]->getId());
					if (block->you) {
						you.push_back(e);
						if (block->super) super = true;
					}
				}
				if (you.size() != 0) {
					glm::ivec2 pos = blocks[j * mapSize.x + i][you[0]]->getPos();
					if (!collisionMoveDown(glm::ivec2(pos.x, pos.y + tileSize), glm::ivec2(tileSize, tileSize), super)) {
						aux = true;
						for (int e = you.size() - 1; e >= 0; e--) {
							Block* b = blocks[j * mapSize.x + i][you[e]];
							b->changeMove(BMOVE_DOWN, babaisyou);
							blocks[(j + 1) * mapSize.x + i].push_back(b);
							movingBlocks.push_back(b);
							removeElement(blocks[j * mapSize.x + i], you[e]);
						}
					}
				}
			}
		}
	}
	if (aux) {
		movement->play2D("sounds/move1.ogg");
		movement->play2D("sounds/move2.ogg");
	}
	onePush = false;
	oneDestroy = false;
	return aux;
}

bool TileMap::collisionMoveDown(const glm::ivec2 &pos, const glm::ivec2 &size, bool s)
{
	int x0, x1, y;
	bool super = false;

	x0 = pos.x / tileSize;
	x1 = (pos.x + size.x - 1) / tileSize;
	y = (pos.y + size.y - 1) / tileSize;
	for (int x = x0; x <= x1; x++)
	{
		if (map[y * mapSize.x + x] != 0) return true;
		else {
			vector<int> push(0);
			vector<int> destroy(0);
			if (blocks[y * mapSize.x + x].size() != 0) {
				for (int e = 0; e < blocks[y * mapSize.x + x].size(); e++) {
					int id = blocks[y * mapSize.x + x][e]->getId();
					block_propieties* block = blockPropieties(true, id);
					if (s && !block->super) destroy.push_back(e);
					else {
						if (!block->stop) {
							if (block->push) {
								push.push_back(e);
								super = block->super;
							}
						}
						else return true;
					}
				}
			}
			if (push.size() != 0 || texts[y * mapSize.x + x] != NULL) {
				if (!collisionMoveDown(glm::ivec2(pos.x, pos.y + size.y), glm::ivec2(24, 24), super)) {
					int i = push.size() - 1;
					int j = destroy.size() - 1;
					while (i >= 0 || j >= 0) {
						if (i >= 0 && (j < 0 || push[i] > destroy[j])) {
							Block* b = blocks[y * mapSize.x + x][push[i]];
							b->changeMove(BMOVE_DOWN, babaisyou);
							blocks[(y + 1) * mapSize.x + x].push_back(b);
							movingBlocks.push_back(b);
							removeElement(blocks[y * mapSize.x + x], push[i]);
							if (!onePush) {
								pushSound->play2D("sounds/push.ogg");
								onePush = true;
							}
							--i;
						}
						else {
							if (!oneDestroy) {
								destroySound->play2D("sounds/break.mp3");
								oneDestroy = true;
							}
							removeElement(blocks[y * mapSize.x + x], destroy[j]);
							--j;
						}
					}
					if (texts[y * mapSize.x + x] != NULL) {
						Block* b = texts[y * mapSize.x + x];
						b->changeMove(BMOVE_DOWN, babaisyou);
						texts[(y + 1) * mapSize.x + x] = b;
						texts[y * mapSize.x + x] = NULL;
						if (!onePush) {
							pushSound->play2D("sounds/push.ogg");
							onePush = true;
						}
					}
					return false;
				}
				else return true;
			}
			else {
				if (destroy.size() > 0 && !oneDestroy) {
					destroySound->play2D("sounds/break.mp3");
					oneDestroy = true;
				}
				for (int i = destroy.size() - 1; i >= 0; i--) removeElement(blocks[y * mapSize.x + x], destroy[i]);
			}
			return false;
		}
	}

	return false;
}

void TileMap::update(int deltaTime)
{
	for (int j = 0; j < mapSize.y; j++) {
		for (int i = 0; i < mapSize.x; i++) {
			if (blocks[j * mapSize.x + i].size() != 0) {
				for (int e = 0; e < blocks[j * mapSize.x + i].size(); e++)
					blocks[j * mapSize.x + i][e]->update(deltaTime, babaisyou);
			}
			if (texts[j * mapSize.x + i] != NULL) texts[j * mapSize.x + i]->update(deltaTime, babaisyou);
		}
	}
}

bool TileMap::isText(const int& i) const
{
	if (i == WALL || i == FLAG || i == BABA || i == ROCK || i == LAVA || i == IS || i == AND || i == STOP || i == WIN || i == YOU || i == PUSH || i == DEFEAT || i == SUPER)
		return true;
	else
		return false;
}

bool TileMap::isBlock(const int& i) const
{
	if (i == WALL_BLOCK || i == FLAG_BLOCK || i == ROCK_BLOCK || i == LAVA_BLOCK || i == BABA_BLOCK || isText(i))
		return true;
	else
		return false;
}

bool TileMap::isNoun(const int& i) const
{
	if (i == WALL || i == FLAG || i == BABA || i == ROCK || i == LAVA)
		return true;
	else
		return false;
}

bool TileMap::isProp(const int& i) const
{
	if (i == STOP || i == WIN || i == YOU || i == PUSH || i == DEFEAT)
		return true;
	else
		return false;
}

void TileMap::initProp()
{
	prop = vector<block_propieties>(5);
	prop[0].idBlock = WALL_BLOCK;
	prop[0].idText = WALL;
	prop[1].idBlock = FLAG_BLOCK;
	prop[1].idText = FLAG;
	prop[2].idBlock = ROCK_BLOCK;
	prop[2].idText = ROCK;
	prop[3].idBlock = LAVA_BLOCK;
	prop[3].idText = LAVA;
	prop[4].idBlock = BABA_BLOCK;
	prop[4].idText = BABA;
	resetProp();
}

void TileMap::resetProp()
{
	for (int i = 0; i < 5; i++) {
		prop[i].stop = false;
		prop[i].win = false;
		prop[i].you = false;
		prop[i].push = false;
		prop[i].defeat = false;
		prop[i].super = false;
	}
	babaisyou = false;
}

block_propieties* TileMap::blockPropieties(const bool& block, const int& id)
{
	for (int i = 0; i < 5; i++) {
		if (block && prop[i].idBlock == id) return &prop[i];
		if (!block && prop[i].idText == id) return &prop[i];
	}
}

//////////////////////////////////////////////////////LECTURA DE PROPIEDADES//////////////////////////////////////////////////////
void TileMap::updateProp() 
{
	supers = vector<int>(0);
	resetProp();
	for (int j = 0; j < mapSize.y; j++) {
		for (int i = 0; i < mapSize.x; i++) {
			if (texts[j * mapSize.x + i] != NULL && texts[j * mapSize.x + i]->getId() == IS) {
				vector<int> up, left, right, down;

				lookUp(up, false, j - 1, i);
				lookLeft(left, false, j, i - 1);
				lookRight(right, false, j, i + 1);
				lookDown(down, false, j + 1, i);

				subUpdateProp(up, down);
				subUpdateProp(left, right);
			}
		}
	}
	soundSuper();
}

void TileMap::lookUp(vector<int>& up, bool and, int j, int i) {
	if (j > 0 && texts[j * mapSize.x + i] != NULL) {
		int id = texts[j * mapSize.x + i]->getId();
		if (and && id == AND) lookUp(up, false, j - 1, i);
		else if (!and && id != AND && id != IS) {
			up.push_back(id);
			lookUp(up, true, j - 1, i);
		}
	}
}

void TileMap::lookLeft(vector<int>& left, bool and, int j, int i) {
	if (i > 0 && texts[j * mapSize.x + i] != NULL) {
		int id = texts[j * mapSize.x + i]->getId();
		if (and && id == AND) lookLeft(left, false, j, i - 1);
		else if (!and && id != AND && id != IS) {
			left.push_back(id);
			lookLeft(left, true, j, i - 1);
		}
	}
}

void TileMap::lookRight(vector<int>& right, bool and, int j, int i) {
	if (i < mapSize.x && texts[j * mapSize.x + i] != NULL) {
		int id = texts[j * mapSize.x + i]->getId();
		if (and && id == AND) lookRight(right, false, j, i + 1);
		else if (!and && id != AND && id != IS) {
			right.push_back(id);
			lookRight(right, true, j, i + 1);
		}
	}
}

void TileMap::lookDown(vector<int>& down, bool and, int j, int i) {
	if (j > 0 && texts[j * mapSize.x + i] != NULL) {
		int id = texts[j * mapSize.x + i]->getId();
		if (and && id == AND) lookDown(down, false, j + 1, i);
		else if (!and && id != AND && id != IS) {
			down.push_back(id);
			lookDown(down, true, j + 1, i);
		}
	}
}

void TileMap::subUpdateProp(vector<int> left, vector<int> right) 
{
	if (left.size() != 0 && right.size() != 0) {
		for (int i = 0; i < left.size(); i++) {
			if (isNoun(left[i])) {
				for (int j = 0; j < right.size(); j++) {
					if (isNoun(right[j])) {
						int leftBlockID = blockPropieties(false, left[i])->idBlock;
						int rightBlockID = blockPropieties(false, right[j])->idBlock;
						for (int j = 0; j < mapSize.y; j++) {
							for (int i = 0; i < mapSize.x; i++) {
								if (blocks[j * mapSize.x + i].size() != 0) {
									for (int e = 0; e < blocks[j * mapSize.x + i].size(); e++) {
										if (blocks[j * mapSize.x + i][e]->getId() == leftBlockID) {
											blocks[j * mapSize.x + i][e]->changeAnimationFrames(rightBlockID);
											blocks[j * mapSize.x + i][e]->render();
										}
									}
								}
							}
						}
					}
					else {
						if (right[j] == STOP) blockPropieties(false, left[i])->stop = true;
						else if (right[j] == WIN) blockPropieties(false, left[i])->win = true;
						else if (right[j] == YOU) {
							if (left[i] == BABA) babaisyou = true;
							blockPropieties(false, left[i])->you = true;
						}
						else if (right[j] == PUSH) blockPropieties(false, left[i])->push = true;
						else if (right[j] == DEFEAT) blockPropieties(false, left[i])->defeat = true;
						else if (right[j] == SUPER) {
							blockPropieties(false, left[i])->super = true;
							supers.push_back(left[i]);
						}
					}
				}
			}
		}
	}
}

void TileMap::soundSuper() {
	if (supers.size() > 0 && !superActive) {
		bool done = false;
		for (int i = 0; i < supers.size() && !done; ++i) {
			if (blockPropieties(false, supers[i])->you) {
				effects->play2D("sounds/super.mp3");
				done = true;
				superActive = true;
			}
		}
	}
	else if (supers.size() == 0) superActive = false;
	else {
		bool found = false;
		for (int i = 0; i < supers.size() && !found; ++i) {
			if (blockPropieties(false, supers[i])->you) found = true;
		}
		if (!found) superActive = false;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void TileMap::removeElement(vector<Block*> &vec, const int &e) {
	for (int i = e; i < vec.size() - 1; i++)
		vec[i] = vec[i + 1];
	vec.pop_back();
}

void TileMap::didILose() {
	for (int j = 0; j < mapSize.y; j++) {
		for (int i = 0; i < mapSize.x; i++) {
			vector<int> you(0);
			if (blocks[j * mapSize.x + i].size() != 0) {
				for (int e = 0; e < blocks[j * mapSize.x + i].size(); e++) {
					block_propieties* block = blockPropieties(true, blocks[j * mapSize.x + i][e]->getId());
					if (block->you) {
						bool found = false;
						for (int se = 0; se < blocks[j * mapSize.x + i].size() && !found; se++) {
							if (se != e) {
								block_propieties* block = blockPropieties(true, blocks[j * mapSize.x + i][se]->getId());
								if (block->defeat) {
									you.push_back(e);
									found = true;
									if (!oneSound) {
										effects->play2D("sounds/defeat.ogg");
										oneSound = true;
									}
								}
							}
						}
					}
				}
			}
			for (int e = you.size() - 1; e >= 0; e--) removeElement(blocks[j * mapSize.x + i], you[e]);
		}
	}
	oneSound = false;
}

bool TileMap::didIWin() {
	for (int j = 0; j < mapSize.y; j++) {
		for (int i = 0; i < mapSize.x; i++) {
			if (blocks[j * mapSize.x + i].size() != 0) {
				bool you = false;
				bool win = false;
				for (int e = 0; e < blocks[j * mapSize.x + i].size(); e++) {
					block_propieties* block = blockPropieties(true, blocks[j * mapSize.x + i][e]->getId());
					if (block->you) you = true;
					else if (block->win) win = true;
				}
				if (win && you) return true;
			}
		}
	}
	return false;
}

void TileMap::clearMovingBlocks() {
	movingBlocks.clear();
}