#include "Ship.h"
#include "Spritesheet.h"
#include "SolarSystem.h"



void Ship::die()
{
	//Play animations etc...
	isDead = true;
	Animation deathAnim = {0,8,0,15};
	sprite.setAnimation(deathAnim);
}

Ship::Ship(int id)
{
	this->id = id;
}

Ship::~Ship()
{
}

int Ship::getID()
{
	return id;
}

void Ship::Poll(std::optional<sf::Event>, sf::RenderWindow& window)
{
}

void Ship::Update(float dt)
{
	//A bit of movement logic
	if (mpLeft <= 0) {
		doMovement = false;
	}

	//Check if path goal reached
	if (doMovement && !path.empty()) {
		sf::Vector2<float> a = sprite.getPosition();
		sf::Vector2<float> b = path.back();
		float theta = std::atan2f(b.y - a.y, b.x - a.x);
		int deg = (int)utl::radToDeg(theta);
		deg = (deg + 360) % 360;

		//Check if subgoal reached
		if (utl::dist(a.x, a.y, b.x, b.y) <= 0.1) {
			mpLeft -= 1;
			isMoving = false;
			path.pop_back();
		}
		else {
			//# Continue to subgoal
			//Movement
			float x = std::cosf(theta) * pathingSpeed * dt;
			float y = std::sinf(theta) * pathingSpeed * dt;
			sprite.move({ x, y });
			isMoving = true;

			//Direction
			sprite.setRotation(sf::degrees(deg + 90.f));
		}
	}

	sprite.update(dt);
}

void Ship::onTurnBegin()
{
	//Refresh move points
	mpLeft = mp;	
}

void Ship::onTurnEnd()
{
	doMovement = true;
}

void Ship::setTexture(sf::Texture* newTexture)
{
	texture = newTexture;
	sprite.setTexture(texture);
	sprite.setFrameSize(textureSize, textureSize);
	sprite.setSize(size);
	sprite.setOrigin({ sprite.getLocalBounds().size.x / 2, sprite.getLocalBounds().size.y / 2 });
}

void Ship::draw(sf::RenderWindow& win) {
	//Draw Path
	if (doDrawPath && !path.empty()) {
		//Draw beginning
		sf::Vertex a{ path.back(), sf::Color::Red};
		sf::Vertex b{ sprite.getPosition(), sf::Color::Red};
		sf::Vertex line[2] = { a,b };
		win.draw(line, 2, sf::PrimitiveType::Lines);

		//Draw middle
		for (int i = path.size()-1; i > 0; i--) {	
			sf::Vertex a{ path.at(i), sf::Color::Red};
			sf::Vertex b{ path.at(i-1), sf::Color::Red};
			sf::Vertex line[2] = {a,b};
			win.draw(line, 2, sf::PrimitiveType::Lines);
		}

		//Draw end
		sf::CircleShape end(5.f,3);
		end.setOrigin({ end.getLocalBounds().size.x / 2, end.getLocalBounds().size.y / 2 });
		end.setFillColor(sf::Color::Red);
		end.setPosition(path.front());
		win.draw(end);
	}

	

	//Draw Ship
	sprite.draw(win);
}

void Ship::setTextureSize(unsigned int newTextureSize)
{
	textureSize = newTextureSize;
	sprite.setFrameSize(textureSize, textureSize);
}

void Ship::setSize(sf::Vector2<float> newSize) {
	size = newSize;
	sprite.setSize(newSize);
}

void Ship::setPosition(sf::Vector2<float> pos) {
	sprite.setPosition(pos);
}

void Ship::pathTo(std::vector<sf::Vector2<float>> newPath)
{
	path = newPath;
	doMovement = true;
}

void Ship::facePoint(sf::Vector2<float> point)
{
	sf::Vector2<float> a = sprite.getPosition();
	float theta = std::atan2f(point.y - a.y, point.x - a.x);
	int deg = (int)utl::radToDeg(theta);
	deg = (deg + 360) % 360;
	sprite.setRotation(sf::degrees(deg+90));
}

sf::Vector2<float> Ship::getPosition() {
	return sprite.getPosition();
}

bool Ship::contains(sf::Vector2<float> pos) {
	if (sprite.getGlobalBounds().contains(pos)) {
		return true;
	}

	return false;
}

bool Ship::getMoving()
{
	return isMoving;
}

bool Ship::hasGoal()
{
	if (!path.empty()) {
		return true;
	}

	return false;
}

sf::Vector2<float> Ship::getPathTarget()
{
	if (path.empty()) {
		return sprite.getPosition();
	}

	return path.front();
}

void Ship::setName(std::string newName)
{
	name = newName;
}

std::string Ship::getName()
{
	return name;
}

void Ship::takeDmg(int amt)
{
	hpLeft -= amt;

	Animation anm = {9,12,9,9};
	sprite.setAnimation(anm);

	if (hpLeft <= 0) {
		this->die();
	}
}

int Ship::getPhysDmgRng()
{
	return physicalDmg_rng;
}

int Ship::getEnergyDmgRng()
{
	return energyDmg_rng;
}

int Ship::getPhysDmg(int range)
{
	int dmg = 0;

	for (auto c : components_off) {
		if (c.range >= range) {
			dmg += c.physicalDmg;
		}
	}


	return dmg;
}

int Ship::getEnergyDmg(int range)
{
	int dmg = 0;

	for (auto c : components_off) {
		if (c.range >= range) {
			dmg += c.energyDmg;
		}
	}


	return dmg;
}

void Ship::processTurn()
{
	mpLeft = mp;
}

void Ship::setHp(int newHp)
{
	hp = newHp;
	hpLeft = hp;
}

int Ship::getHpLeft()
{
	return hpLeft;
}

void Ship::setMp(int newMp)
{
	mp = newMp;
	mpLeft = mp;
}

int Ship::getMpLeft()
{
	return mpLeft;
}

void Ship::kill()
{
	this->hpLeft = 0;
	this->die();
}

bool Ship::getDead()
{
	return isDead;
}

int Ship::addOffComp(Cmp_Offensive component)
{
	//#Check capacity constraint
	if (component.capCost + offUsed > offCapacity) {
		return -1;
	}

	//#Add to comp list and update stats
	//Check whether ranged or cq
	components_off.push_back(component);
	if (component.range > 1) {
		physicalDmg_rng += component.physicalDmg;
		energyDmg_rng += component.energyDmg;

		//Update max range
		if (component.range > maxRange) {
			maxRange = component.range;
		}
	}
	else {
		physicalDmg_cq += component.physicalDmg;
		energyDmg_cq += component.energyDmg;
	}

	maintCost += component.maintCost;
	offUsed += component.capCost;

	return 1;
}

int Ship::addDefComp(Cmp_Defensive component)
{
	//#Check capacity constraint
	if (component.capCost + defUsed > defCapacity) {
		return -1;
	}

	//#Add to comp list and update stats
	//Check whether ranged or cq
	components_def.push_back(component);
	
	armor += component.armor;
	shield += component.sheild;
	maintCost += component.maintCost;
	defUsed += component.capCost;

	return 1;
}

int Ship::remOffComponent(int id)
{
	for (auto i : components_off) {
		if (i.id == id) {
			if (i.range > 1) {
				physicalDmg_rng -= i.physicalDmg;
				energyDmg_rng -= i.energyDmg;
			}
			else {
				physicalDmg_cq -= i.physicalDmg;
				energyDmg_cq -= i.energyDmg;
			}
			maintCost -= i.maintCost;
			offUsed -= i.capCost;
			return 1;
		}
	}


	return -1;
}

int Ship::remDefComponent(int id)
{
	for (auto i : components_def) {
		if (i.id == id) {
			armor -= i.armor;
			shield -= i.sheild;
			maintCost -= i.maintCost;
			offUsed -= i.capCost;
			return 1;
		}
	}

	return -1;
}
