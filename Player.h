#pragma once
#include <iostream>
#include <vector>
#include "Ship.h"

class Player
{
private:
	//## Data
	//Stats
	float income = 0;
	float maintainance = 0;
	int researchPoints = 0;


	//Assets
	std::vector<Ship>* ships;

	//Util

public:
	//Constructors and Destructors
	Player();
	~Player();

	//Primary Functions
	void update();

	std::vector<Ship> getShips();
};

