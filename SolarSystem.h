#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>

enum class enBuildType {
	NONE, INF, SHIP
};

enum class enInfTags {
	STD, SCI, PROD, POP, ECO
};

struct Infrastructure {
	int id = -1;
	int techNeeded = -1; //Tech ID needed to build this
	std::vector<enInfTags> tags;
	std::string name = "Unnamed";
	float rp = 0.f; //research pts
	float rpMod = 0.f;
	float creditCost = 0.0f;
	float income = 0.f;
	float incomeMod = 0.f;
	float maint = 0.f;
	float maintMod = 0.f;
	float laborCost = 0.f; //Cost to build
	float laborMaint = 0.f; //Cost to keep active
	float laborGen = 0.f; //Labor output
	float laborMod = 0.f;
	float popGrowth = 0.f;
	float popCapacity = 0.f;
	bool isActive = true;
};

struct Project {
	int buildID = -1;
	enBuildType type = enBuildType::NONE;
	std::string name = "none";
	float laborCost;
	float laborSpent;
	float creditCost = 0.f;
	std::pair<float, float> pos = { 0,0 };
	bool isDone = false;
	bool isCancelled = false;

	inline float advance(float labor) {
		laborSpent += labor;
		if (laborSpent >= laborCost) {
			isDone = true;
			return laborSpent - laborCost; //Return leftover labor
		}

		return labor;
	}
};


class SolarSystem
{
private:
	//## Data
	//General
	int id = -1;
	int ownerId = -1;
	std::string name = "unnamed";
	std::string textureName = "";
	std::pair<int, int> position = {0,0};
	std::vector<Infrastructure> improvements;
	std::vector<Project> buildQueue;


	//Stats
	float rp = 0.f; //research pts
	float rpMod = 1.f;
	float income = 0.f;
	float refund = 0.f;
	float incomeMod = 1.f;
	float maint = 0.f;
	float maintMod = 1.f;
	float taxRate = 0.1f;
	float popCapacity = 3.f; //How much pop can be supported
	float popCapMod = 1.f;
	float population = 0.f;
	float popGrowth = 0.f;
	float popGrowthMod = 1.f;
	float popWealth = 100.f;
	float laborPool = 0.f;
	float laborPoolMod = 1.f;
	float laborUsed = 0.f;
	float laberUseMod = 1.f;

	//Util
	
	

public:
	//Constructors and Destructor
	SolarSystem(int id);
	~SolarSystem();

	//Primary Functions
	void update(float dt);
	void onTurnEnd();
	void onTurnBegin();
	void updateStats();

	void setId(int newId);
	void setOwnerId(int newId);
	void setPosition(std::pair<int,int> newPosition);
	void setTextureName(std::string newName);
	void setName(std::string newName);
	void addInfrastructure(Infrastructure inf);
	void addProject(Project p);
	void removeDoneProjects();

	int getId();
	int getOwnerId();
	std::pair<int, int> getPosition();
	std::string getName();
	std::string getTextureName();

	inline void setPopulation(float newPop) { population = newPop; };
	inline void setPopGrowth(float newGrowth) { popGrowth = newGrowth; };
	inline void setPopGrowthMod(float mod) { popGrowthMod = mod; }
	inline void setPopCap(float newCap) { popCapacity = newCap; };
	inline void setPopCapMod(float mod) { popCapMod = mod; }
	inline void setIncome(float newInc) { income = newInc; };
	inline void setIncomeMod(float mod) { incomeMod = mod; }
	inline void setMaint(float newMaint) { maint = newMaint; };
	inline void setMaintMod(float mod) { maintMod = mod; }
	inline void setRP(float newRP) { rp = newRP; };
	inline void setRPMod(float mod) { rpMod = mod; }
	inline void setLaborUsed(float used) { laborUsed = used; };
	inline void setLaborUseMod(float mod) { laberUseMod = mod; }
	inline void setLaborPool(float pool) { laborPool = pool; };
	inline void setLaborPoolMod(float mod) { laborPoolMod = mod; }
	inline void setRefund(float amt) { refund = amt; }
	inline float getRefund() { return refund; }

	inline std::vector<Infrastructure> getImprovements() { return improvements; };
	inline float getPopulation() { return population; }
	inline float getPopulationGrowth() { return popGrowth; }
	inline float getPopulationCap() { return popCapacity;  }
	inline float getIncome() { return income; }
	inline float getMaint() { return maint; }
	inline float getRP() { return rp; }
	inline float getLaborUsed() { return laborUsed; }
	inline float getLaborPool() { return laborPool; }
	inline std::vector<Project>& getBuildQueue() { return buildQueue; };
};

