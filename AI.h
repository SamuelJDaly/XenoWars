#pragma once
#include <iostream>
#include <vector>
#include <unordered_map>
#include "Ship.h"
#include "Map.h"
#include "Curves.hpp"


enum class enActionType {
	MOVE, ATTACK
};

struct Goal;

class Action {
protected:
	//Common Data
	enActionType type;
	float baseScore = 1.f;
	float score = 1.f;

public:
	//Common Functions
	inline enActionType  getType() { return type; }
	inline float getBaseScore() { return baseScore; }
	inline float getScore() { return score; }

	//Abstract Functions
	virtual void execute() = 0;
	virtual void print() = 0;
	virtual void calcScore(Goal g) = 0;
};

class Act_Idle : public Action {
private:
	//Data
	Ship* actor;

public:
	//Constructor and Destructor
	Act_Idle(Ship* initActor);
	~Act_Idle();

	//Primary Functions
	void execute();
	void print();
	void calcScore(Goal g);
};

class Act_Move : public Action {
private:
	//Data
	Ship* actor;
	Ship* strongestNeighbor;
	Map* env;
	sf::Vector2<float> target;
	float cowardice = 1.f;

public:
	//Constructor and Destructor
	Act_Move(Ship* inActor, Map* inEnv, Ship* inStrongestNeigbor,sf::Vector2<float> inTarget);
	~Act_Move();
	
	//Primary Functions
	void execute();
	void print();
	void calcScore(Goal g);
};

class Act_Attack : public Action {
private:
	//Data
	Ship* actor;
	Map* env;
	Ship* target;
	int range = 1;

public:
	//Constructor and Destructor
	Act_Attack(Ship* initActor, Map* initEnv, Ship* initTarget, int initRange);
	~Act_Attack();

	//Primary Functions
	void execute();
	void print();
	void calcScore(Goal g);
};

struct Goal {
	std::string name = "default";
	float agression = 1.f;
	float cohesion = 1.f;
	float idle = 0.75f;
};


class AI_Tactical {
private:
	//## Data
	//General
	int ownerID = -1;
	std::unordered_map<int, Ship*> aiShips;
	std::unordered_map<int, Ship*> playerShips;
	Map* map;
	Goal strategicGoal = {"default", 1.5f, 1.2f};
	std::vector<Action*> chosenActions;

	//## Util
	

public:
	//Constructors and Destructor
	AI_Tactical(std::vector<Ship*> initShips, Map* initMap, int initOwnerID);
	~AI_Tactical();

	//Primary Functions
	void getActions();
	void executeTurn();

	void update();
};