#pragma once
#pragma once
#include <vector>
#include <stack>
#include <algorithm>
#include <SFML/Graphics.hpp>

#include "imgui.h"
#include "imgui-SFML.h"
#include "TextureRegistry.h"
#include "Spritesheet.h"
#include "Map.h"
#include "Ship.h"
#include "SolarSystem.h"
#include "AI.h"


/*
Project: XenoWars, Gamestate System
Created: 26 MAY 2025
Updated: 9 APR 2026

Description:
	This file contains the Gamestate System. This is what actually ties the various systems together and manages their interactions.
	These states have some common program loop functions that are called by the engine class once per frame:
		Poll:
			This is where less time dependant event logic happens, like window closures, button clicks, etc...

		Update:
			This is where per-frame logic happens. Events that need to happen once per frame for smooth movement, etc are called here. This includes things
			like collision, movement, etc...

		Draw:
			This is where any draw calls happen.
*/

//##########################	STATE ENUMS	#########################################
enum class En_Gamestate {
	MENU, GAME, EDITOR, TEST, END
};

//##########################	BASE CLASS	#########################################
class Gamestate
{
protected:
	//Common Data
	TextureRegistry* texReg = nullptr;
	sf::RenderWindow* win;
	bool isFinished = false;
	En_Gamestate nextState = En_Gamestate::END;

public:
	//Common Functions
	bool getFinished();
	En_Gamestate getNextState();

	//Virtual Functions
	virtual void update(float dt) = 0;
	virtual void poll(sf::RenderWindow& win, std::optional<sf::Event> event) = 0;
	virtual void draw(sf::RenderWindow& win) = 0;
};

//##########################	GAME STATE	#########################################
class State_Game : public Gamestate {
private:
	//## Data
	std::vector<Ship*> ships;
	std::vector<unsigned int> shipsToRemove;
	std::vector<SolarSystem*> solarSystems;
	std::vector<Spritesheet> effects;
	std::unordered_map<int, Infrastructure> infCatalog;
	std::unordered_map<int, Ship> shipCatalog;
	sf::Texture defaultTexture;
	
	//Gui
	bool isMenuShown = false; //Only enable map interaction when no menu shown
	bool showDemo = false;
	unsigned int windowWidth = 0;
	unsigned int windowHeight= 0;
	unsigned int topBarHeight = 0;
	unsigned int topBarBottom = 0;
	unsigned int systemPreviewSize = 100;
	sf::Sprite systemPreview = sf::Sprite(defaultTexture);
	

	//Player player;
	Map* map;
	float testTimer = 0.f;
	int tileSize = 32;
	int turn = 0;

	//AI
	AI_Tactical* enemyAI;

	//Camera
	sf::View mapView;
	sf::View guiView;
	float panSpeed = 850.f;
	float maxZoom = 1.5f;
	float minZoom = 0.1f;
	float zoomSpeed = .06f;
	float zoomStep = .1f;
	float currZoom = 1.f;
	sf::Rect<float> cameraConstraints = { {-1.f,-1.f},{1.f,1.f} };
	bool isMousePan = false;
	sf::Vector2<float> panStart = { 0,0 };
	sf::Vector2<float> panDelta = {0,0};
	sf::Vector2<float> cameraOffset = {0,0};

	//Background
	sf::Sprite background = sf::Sprite(defaultTexture);

	//Controls
	int shipSel = -1;
	sf::Sprite cursor = sf::Sprite(defaultTexture);
	sf::Sprite targetCursor = sf::Sprite(defaultTexture);
	int systemSel = -1;
	sf::Vector2<int> cursorPos = { -1,-1 };


	//Util
	void initTest();
	void initGui();
	void updateCamera(float dt);
	void constrainCamera();
	void moveCamera(sf::Vector2<float> offset);
	void zoomCamera(float zoom);
	void selectShip(int i);
	void selectSolarSystem(int i);

	void showGui_TopBar();
	void showGui_shipSel();
	void showGui_systemSel();
	void showGui_SystemSel_Empty();
	void showGui_SystemSel_Enemy();
	void showGui_debugPanel();

	void pollMove(Ship* ship, sf::Vector2<float> pos);
	void pollAttack(Ship* attacker, Ship* defender, int range);

	void spawnShip(int owner, Ship type, sf::Vector2<float> pos);
	void updateShipPositions();

public:
	//Constructors and Destructors
	State_Game(TextureRegistry* textureRegistry, sf::RenderWindow* window);
	~State_Game();

	//Primary Functions
	void endTurn();
	void beginTurn();
	void update(float dt);
	void poll(sf::RenderWindow &win, std::optional<sf::Event> event);
	void draw(sf::RenderWindow &win);

};