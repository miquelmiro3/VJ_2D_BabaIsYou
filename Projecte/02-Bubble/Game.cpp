#include <GL/glew.h>
#include <GL/glut.h>
#include "Game.h"


void Game::init()
{
	scenes = 0;
	click = -1;
	engine = createIrrKlangDevice();
	effects = createIrrKlangDevice();
	engine->play2D("sounds/flower_garden.mp3", true);
	engine->setSoundVolume(0.1);
	bPlay = true;
	reset = false;
	victory = false;
	level = 1;
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	generalScene = new GeneralScene();
	generalScene->init();
	generalScene->prepareMenu();
}

void Game::changeScenes(bool x)
{
}

bool Game::update(int deltaTime)
{
	if (scenes == 0) {
		generalScene->update(deltaTime);
		if (click == 1) {
			scenes = 1;
			delete generalScene;
			scene = new Scene();
			scene->init(level);
		}
		else if (click == 2) {
			scenes = 2;
			delete generalScene;
			generalScene = new GeneralScene();
			generalScene->init();
			generalScene->prepareInstructions();
		}
		else if (click == 3) {
			scenes = 3;
			delete generalScene;
			generalScene = new GeneralScene();
			generalScene->init();
			generalScene->prepareCredits();
		}
		else if (click == 4) return false;
		click = -1;
	}
	else if (scenes == 1) {
		if (click == 0) {
			scenes = 0;
			delete scene;
			generalScene = new GeneralScene();
			generalScene->init();
			generalScene->prepareMenu();
			click = -1;
		}
		else {
			click = -1;
			if (reset) {
				delete scene;
				scene = new Scene();
				scene->init(level);
				reset = false;
			}
			bool b = scene->update(deltaTime);
			if (b) {
				++level;
				effects->play2D("sounds/win.ogg");
				if (level > 7) {
					victory = true;
					level = 1;
					engine->removeAllSoundSources();
					engine->setSoundVolume(0.2);
					engine->play2D("sounds/vamo_a_baila.mp3", true);
					scenes = 3;
					delete scene;
					generalScene = new GeneralScene();
					generalScene->init();
					generalScene->prepareCredits();
				}
				else {
					delete scene;
					scene = new Scene();
					scene->init(level);
				}
			}
		}
	}
	else if (scenes == 2) {
		if (click == 0) {
			scenes = 0;
			delete generalScene;
			generalScene = new GeneralScene();
			generalScene->init();
			generalScene->prepareMenu();
			click = -1;
		}
	}
	else if (scenes == 3) {
		if (click == 0) {
			if (victory) {
				victory = false;
				engine->removeAllSoundSources();
				engine->setSoundVolume(0.1);
				engine->play2D("sounds/flower_garden.mp3", true);
			}
			scenes = 0;
			delete generalScene;
			generalScene = new GeneralScene();
			generalScene->init();
			generalScene->prepareMenu();
			click = -1;
		}
	}

	if (!bPlay) {
		engine->drop();
		effects->drop();
	}
	return bPlay;
}

void Game::render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if (scenes == 1)
		scene->render();
	else
		generalScene->render();
}

void Game::keyPressed(int key)
{
	if(key == 27) {// Escape code
		if (scenes == 0) bPlay = false;
		else click = 0;
	}
	if(scenes == 1 && (key == 'r' || key == 'R'))
		effects->play2D("sounds/reset.ogg");
		reset = true;
	keys[key] = true;
}

void Game::keyReleased(int key)
{
	keys[key] = false;
}

void Game::specialKeyPressed(int key)
{
	specialKeys[key] = true;
}

void Game::specialKeyReleased(int key)
{
	specialKeys[key] = false;
}

void Game::mouseMove(int x, int y)
{
}

void Game::mousePress(int button, int x, int y)//MOUSE
{
	if (scenes == 0 && button == 0) {
		click = generalScene->click(x, y, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
	}
}

void Game::mouseRelease(int button)
{
}

bool Game::getKey(int key) const
{
	return keys[key];
}

bool Game::getSpecialKey(int key) const
{
	return specialKeys[key];
}





