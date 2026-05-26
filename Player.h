#pragma once
#include <iostream>
#include <vector>


class Player
{
private:
	//## Data
	//Stats
	int id = -1;
	float income = 0;
	float maintainance = 0;
	float netIncome = 0.f;
	float researchPoints = 0.f;
	float population = 0.f;

	float credits = 0.f;
	float researchProgress = 0.f;
	int researchID = -1;

	

	//Util


public:
	//Constructors and Destructors
	Player();
	~Player();

	//Primary Functions
	inline void setID(int newID) { id = newID; }
	inline int getID() { return id; }

	inline void setIncome(float newIncome) { income = newIncome; netIncome = income - maintainance; }
	inline void modIncome(float amt) { income += amt; netIncome = income - maintainance; }
	inline void setMaint(float newMaint) { maintainance = newMaint; netIncome = income - maintainance; }
	inline void modMaint(float amt) { maintainance += amt; netIncome = income - maintainance; }
	inline void setResearch(float newResearch) { researchPoints = newResearch; }
	inline void modResearch(float amt) { researchPoints += amt; }
	inline void setPop(float newPop) { population = newPop; }
	inline void modPop(float amt) { population += amt; }
	inline float getPop() { return population; }
	
	inline float getIncome() { return income; }
	inline float getMaint() { return maintainance; }
	inline float getNetIncome() { return netIncome; }
	inline float getResearch() { return researchPoints; }

	inline void setCredits(float amt) { credits = amt; }
	inline void modCredits(float amt) { credits += amt; }
	inline float getCredits() { return credits; }
	inline void setResearchProgress(float amt) { researchProgress = amt; }
	inline void modResearchProgress(float amt) { researchProgress += amt; }
	inline float getResearchProgress() { return researchProgress; }


	
	void endTurn();
};

