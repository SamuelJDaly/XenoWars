#pragma once
#include <iostream>
#include <vector>
#include <unordered_map>
#include "Ship.h"
#include "Map.h"


enum class enActionType {
	MOVE, ATTACK
};

struct Goal;

class Action {
protected:
	//Common Data
	enActionType type;
	float baseScore = 1;
	float score = 1;

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


class Act_Move : public Action {
private:
	//Data
	Ship* actor;
	Map* env;
	sf::Vector2<float> target;

public:
	//Constructor and Destructor
	Act_Move(Ship* initActor, Map* initEnv,sf::Vector2<float> initTarget);
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
};


class AI_Tactical {
private:
	//## Data
	//General
	int ownerID = -1;
	std::vector<Action*> actions;
	std::unordered_map<int, Ship*> aiShips;
	std::unordered_map<int, Ship*> playerShips;
	Map* map;
	Goal strategicGoal = {"default", 1.5f, 1.2f};


	//## Util
	void clearActions();

public:
	//Constructors and Destructor
	AI_Tactical(std::vector<Ship*> initShips, Map* initMap, int initOwnerID);
	~AI_Tactical();

	//Primary Functions
	void getActions();
	void scoreActions();
	void executeTurn();

};