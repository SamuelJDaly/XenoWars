#include "SolarSystem.h"

void SolarSystem::updateStats()
{
	rp = 0.f; //research pts
	income = 0.f;
	maint = 0.f;
	popCapacity = 3.f; //How much pop can be supported
	//population = 0.f;
	popGrowth = 0.f;
	laborPool = population;
	laborUsed = 0.f;


	//Raw
	for (auto i : improvements) {
		rp += i.rp;
		income += i.income;
		maint += i.maint;
		popCapacity += i.popCapacity;
		popGrowth += i.popGrowth;
		laborPool += i.laborGen;
		laborUsed += i.laborMaint;
	}

	//Modifiers
}

SolarSystem::SolarSystem(int id) {
	this->id = id;
}

SolarSystem::~SolarSystem() {

}

void SolarSystem::update(float dt) {

}

void SolarSystem::onTurnBegin() {

}

void SolarSystem::onTurnEnd() {
	//Advance build queue
	float laborAvailable = laborPool - laborUsed;
	

	for (auto i = 0; i < buildQueue.size(); i++) {
		if (laborAvailable < 0.f) {
			return;
		}

		laborAvailable = buildQueue.front().advance(laborAvailable);
	}

	//Pop Growth
	population += (popGrowth * population);
	if (population >= popCapacity) {
		population = popCapacity;
	}

	this->updateStats();
}

void SolarSystem::setId(int newId) {
	id = newId;
}

void SolarSystem::setOwnerId(int newId) {
	ownerId = newId;
}

void SolarSystem::setPosition(std::pair<int, int> newPosition) {
	position = newPosition;
}

void SolarSystem::setTextureName(std::string newName)
{
	textureName = newName;
}

void SolarSystem::setName(std::string newName)
{
	name = newName;
}

void SolarSystem::addInfrastructure(Infrastructure inf)
{
	improvements.push_back(inf);
	this->updateStats();
}

void SolarSystem::addProject(Project p)
{
	buildQueue.push_back(p);
	this->updateStats();
}

void SolarSystem::removeDoneProjects()
{
	for (auto i = 0; i < buildQueue.size(); i++) {
		if (buildQueue.at(i).isDone) {
			buildQueue.erase(buildQueue.begin()+i);
		}
	}

	this->updateStats();
}

int SolarSystem::getId() {
	return id;
}

int SolarSystem::getOwnerId() {
	return ownerId;
}

std::pair<int,int> SolarSystem::getPosition() {
	return position;
}

std::string SolarSystem::getTextureName()
{
	return textureName;
}

std::string SolarSystem::getName()
{
	return name;
}