#pragma once
#include <iostream>
#include <vector>
#include <SFML/Graphics.hpp>

#include "Spritesheet.h"
#include "Utility.h"

struct Cmp_Offensive {
	int id = -1;
	bool isActive = true;
	int physicalDmg = 0;
	int energyDmg = 0;
	int capCost = 1;
	int maintCost = 0;
	int range = 1;
};

struct Cmp_Defensive {
	int id = -1;
	int isActive = true;
	int armor = 0;
	int sheild = 0;
	int capCost = 1;
	int maintCost = 0;
};

//Engine, hull, and reactor components? Could be fun...


struct Persona {
	//Contains certain AI weights
	float baseAgression =  1.f;
	float agression = 1.f;
	float rangedPref = 1.f;
};

class Ship
{
private:
	//## Data
	int id = -1;
	int ownerID = -1;
	int hp = 1;
	int hpLeft = 1;
	int armor = 0;
	int shield = 0;
	int maxEnergyDmg = 0;
	int maxPhysDmg = 0;
	int maxDmg = 0;
	int maintCost = 0;
	int ap = 1; //action pts
	int maxRange = 1;
	int sensorRange = 2;
	bool isColonyShip = false;

	std::string name = "Unnamed";

	Spritesheet sprite;
	sf::Texture* texture = nullptr;
	sf::Texture* attackEffectTexture = nullptr;
	unsigned int textureSize = 32;
	sf::Vector2<float> size = {32,32};
	bool isDead = false;
	std::vector<Spritesheet> effects;

	Persona personality;

	//Movement
	int mp = 1; //Move pts
	int mpLeft = 1;
	std::vector<sf::Vector2<float>> path;
	float pathingSpeed = 100.f;
	bool doMovement = false;
	bool doDrawPath = true;
	bool isMoving = false;
	
	//Other
	float laborCost = 0.f;
	float creditCost = 0.f;


	//Components
	unsigned int offCapacity = 1;
	unsigned int defCapacity = 1;
	unsigned int offUsed = 0;
	unsigned int defUsed = 0;

	std::vector<Cmp_Offensive> components_off;
	std::vector<Cmp_Defensive> components_def;


	//## Util
	void die(bool doAnim = true);

public:
	//Constructor and Destructor
	Ship(int id);
	~Ship();

	int getID();

	//# Primary Functions
	//Updates
	void Poll(std::optional<sf::Event> event, sf::RenderWindow &window);
	void Update(float dt);
	void onTurnBegin();
	void onTurnEnd();

	//Graphics
	void setTexture(sf::Texture* newTexture);
	inline void setAttackEffectTexture(sf::Texture* newTexture) { attackEffectTexture = newTexture; }
	void draw(sf::RenderWindow& win);
	inline void setAnimation(Animation anm) { sprite.setAnimation(anm); }
	void setTextureSize(unsigned int newTextureSize);
	void setSize(sf::Vector2<float> newSize);

	//Positioning
	void setPosition(sf::Vector2<float> pos);
	void pathTo(std::vector<sf::Vector2<float>> newPath);
	void facePoint(sf::Vector2<float> point);
	sf::Vector2<float> getPosition();
	inline sf::Angle getRotation() { return sprite.getRotation(); }
	inline sf::Rect<float> getLocalBounds() { return sprite.getLocalBounds(); }

	//General
	bool contains(sf::Vector2<float> pos);
	bool getMoving(); //If the ship is currently in motion
	bool hasGoal(); //If the ship has a non empty path
	sf::Vector2<float> getPathTarget();

	void setName(std::string newName);
	std::string getName();

	inline void setOwner(int newOwner) { ownerID = newOwner; };
	inline int getOwner() { return ownerID; };

	//Stats
	void takeDmg(int amt); //Raw Hull Damage
	int getPhysDmgRng();
	int getEnergyDmgRng();
	int getPhysDmg(int range);
	int getEnergyDmg(int range);
	inline int getShield() { return shield; };
	inline int getArmor() { return armor; };
	void processTurn(); //pretty much just refresh move points
	void setHp(int newHp);
	inline int getBaseHp() { return hp; }
	int getHpLeft();
	void setMp(int newMp);
	int getMpLeft();
	void kill(bool doAnim = true);
	bool getDead();
	inline int getAp() { return ap; };
	inline int getMaxRange() { return maxRange; }
	inline bool isAnimated() { return sprite.getAnimated(); }
	inline void setSensorRange(int newSensorRange) { sensorRange = newSensorRange; }
	inline int getSensorRange() { return sensorRange; }
	int getEffectiveDamage(const Ship& target, int range);
	inline void setPersona(Persona newPersona) { personality = newPersona; }
	inline Persona* getPersona() { return &personality; }
	inline void setIsColonyShip(bool state) { isColonyShip = state; }
	inline bool getIsColonyShip() { return isColonyShip; }
	
	float getStr();

	void doAttackAnimation();
	void doDamageAnimation();
	void doDeathAnimation();

	inline void setLaborCost(float cost) { laborCost = cost; };
	inline void setCreditCost(float cost) { creditCost = cost; };

	inline float getLaborCost() { return laborCost; }
	inline float getCreditCost() { return creditCost; }

	//Components
	int addOffComp(Cmp_Offensive component);
	int addDefComp(Cmp_Defensive component);
	int remOffComponent(int id);
	int remDefComponent(int id);

	//Serialization (NOT YET IMPLEMENTED)
	//void loadFromDB(std::string filename);
	//void saveToDB(std::string filename);
};

