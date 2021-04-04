#ifndef _GAME_INCLUDE
#define _GAME_INCLUDE


#include "Scene.h"
#include "GeneralScene.h"

#include <irrKlang.h>
using namespace irrklang;
#pragma comment(lib, "irrKlang.lib")


#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

// Game is a singleton (a class with a single instance) that represents our whole application

class Game
{

public:
	Game() {}
	
	
	static Game &instance()
	{
		static Game G;
	
		return G;
	}
	
	void init();
	bool update(int deltaTime);
	void render();
	
	// Input callback methods
	void keyPressed(int key);
	void keyReleased(int key);
	void specialKeyPressed(int key);
	void specialKeyReleased(int key);
	void mouseMove(int x, int y);
	void mousePress(int button, int x, int y);
	void mouseRelease(int button);
	
	bool getKey(int key) const;
	bool getSpecialKey(int key) const;

private:
	void changeScenes(bool x);

	bool bPlay;                       // Continue to play game?
	Scene* scene;                      // Scene to render
	bool keys[256], specialKeys[256]; // Store key states so that 
	                                  // we can have access at any time
	bool reset, victory;
	int level;

	ISoundEngine* engine;
	ISoundEngine* effects;

	glm::vec2 mouse;
	GeneralScene* generalScene;

	int scenes;//0-menu 1-jugando 2-instruciones 3-creditos
	int click;//-1-nada 0-iramenu 1-jugar 2-verinstruciones 3-vercreditos 4-salir
	
};


#endif // _GAME_INCLUDE


