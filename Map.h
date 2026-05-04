#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <utility>
#include <SFML/Graphics.hpp>

#include "TextureRegistry.h"
#include "Spritesheet.h"
#include "Utility.h"

struct Tile {
	sf::Vector2<unsigned int> pos = {0,0};
	Spritesheet sprite;

	int type = -1;
	bool isPassable = true;
	bool isSpotted = false;
	float visibility = 0.f;
	int ownerID = -1;
	int shipID = -1; //ID of the ship there
	bool fow = true;
	bool isHighlight = false;

	inline bool contains(sf::Vector2<float> pos) {
		if (sprite.contains(pos)) {
			return true;
		}

		return false;
	}

	inline void Poll(sf::RenderWindow &win, std::optional<sf::Event> event) {

	}

	inline void update() {
		uint8_t a = 255;

		if (!fow) {
			return;
		}
		if (isSpotted) {
			visibility = 1.f;
		}
		else if (visibility > 0.f) {
			visibility -= .1f;
		}

		a *= visibility;
		sprite.setColor({a,a,a,255});
	}

	inline void draw(sf::RenderWindow& win) {
		sprite.draw(win);
	}

	inline void print() {
		std::cout << "[t:" << type << " p:" << isPassable << " s:" << isSpotted << " v:" << visibility << " o:" << ownerID;
	}
};

struct Spotter {
	sf::Vector2<unsigned int> pos;
	int range = 1;
	bool hasChanged = true;
	std::vector<sf::Vector2<unsigned int>> tiles;

	inline void getTiles() {
		//Clear tiles
		tiles.clear();

		int minX = pos.x - range;
		int maxX = pos.x + range;
		int minY = pos.y - range;
		int maxY = pos.y + range;

		if (minX < 0) { minX = 0; }
		if (minY < 0) { minY = 0; }

		for (int i = minY; i <= maxY; i++) {
			for (int j = minX; j <= maxX; j++) {
				float dist = std::sqrtf(std::powf((int)pos.x - j, 2) + std::powf((int)pos.y - i, 2));
				//float comp = i * std::sqrt(2);
				//float comp = range * std::sqrt(2);
				if (dist < (float)range + .2) {
					tiles.push_back({(unsigned int)j,(unsigned int)i});
				}
			}
		}
	}
};



class Map
{
private:
	//Data
	sf::Vector2<unsigned int> dimensions = { 0,0 };
	std::vector<std::vector<Tile*>*> tiles;
	TextureRegistry* texReg;
	//std::vector<sf::Vertex> grid;
	std::vector<Spotter> spotters;
	sf::Vector2<float> offset = { 0,0 };
	int tileSize = 32;
	sf::View* view;
	bool drawGrid = true;
	sf::Color gridColor = sf::Color::Green;
	std::vector<sf::Vertex> grid;

	//Util
	void align();
	void generateGrid();

public:
	//Constructors and Destructor
	Map(TextureRegistry* textureRegistry, sf::View* view);
	~Map();

	//Primary Functions
	void addSpotter(sf::Vector2<unsigned int> pos, int range);
	inline void setGridColor(sf::Color col) { gridColor = col; this->generateGrid(); }

	std::vector<sf::Vector2<float>> pathfind(sf::Vector2<int> origin, sf::Vector2<int> target);
	void setOffset(sf::Vector2<float> newOffset);
	sf::Vector2<int> tileAtPos(sf::Vector2<float> pos);

	sf::Vector2<float> getSize();

	int loadFromFile(std::string filename);
	sf::Vector2<float> getOffset();

	Tile* tileAtIdx(size_t x, size_t y);
	Tile* tileAtIdx(std::pair<int,int> idx);

	std::vector<sf::Vector2<int>> tileIdxInRange(int range, sf::Vector2<int> og, bool includeOG = false);

	void clearShipPositions();

	void Poll(sf::RenderWindow &win, std::optional<sf::Event> event);
	void Update();
	void Draw(sf::RenderWindow &win);
};

