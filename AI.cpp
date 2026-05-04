#include "AI.h"

//##################################################################################################################################
//		ACTIONS
//##################################################################################################################################

//#####################################	MOVE #########################################################
Act_Move::Act_Move(Ship* initActor, Map* initEnv, sf::Vector2<float> initTarget)
{
	type = enActionType::MOVE;
	actor = initActor;
	env = initEnv;
	target = initTarget;
	baseScore = 1;
}

Act_Move::~Act_Move()
{
}

void Act_Move::execute()
{
	if (!actor) {
		std::cout << "Cannot execute move action, invalid actor!" << std::endl;
		return;
	}

	if (!env) {
		std::cout << "Cannot execute move action, invalid map!" << std::endl;
		return;
	}

	sf::Vector2<int> og = env->tileAtPos(actor->getPosition());
	sf::Vector2<int> targ = env->tileAtPos(target);
	std::vector<sf::Vector2<float>> path = env->pathfind(og, targ);
	actor->pathTo(path);

	return;
}

void Act_Move::print() {
	std::cout << "[Type: MOVE | Score: " << score << " | Actor: "<< actor->getID() << " | Target : " << target.x << ", " << target.y << std::endl;
}

void Act_Move::calcScore(Goal g) {
	score = baseScore;
	
	sf::Vector2<int> targ = env->tileAtPos(target);
	auto t = env->tileAtIdx(targ.x, targ.y);

	//Next to friendly ship
	if (t->ownerID == actor->getOwner() && t->shipID != actor->getID()) {
		score += g.cohesion;
	}

	//Next to enemy
	if (t->ownerID > 0 && t->ownerID != actor->getOwner()) {
		score += g.agression * .5;
	}

}

//#####################################	ATTACK #########################################################

Act_Attack::Act_Attack(Ship* initActor, Map* initEnv, Ship* initTarget, int initRange)
{
	actor = initActor;
	env = initEnv;
	target = initTarget;
	type = enActionType::ATTACK;
	range = initRange;
}

Act_Attack::~Act_Attack()
{
}

void Act_Attack::execute() {
	//Face Each other
	actor->facePoint(target->getPosition());
	target->facePoint(actor->getPosition());
	
	//Play Animations
	actor->doAttackAnimation();
	target->doDamageAnimation();

	//Do Damage
	target->takeDmg(actor->getEffectiveDamage(*target, range));
}

void Act_Attack::print() {
	std::cout << "[Type: ATTACK | Score: " << score << " | Actor: " << actor->getID() << " | Target: " << target->getID() << std::endl;
}

void Act_Attack::calcScore(Goal g) {
	score = baseScore;

	//Self Preservation
	float preservation = (float)actor->getHpLeft() / (float)actor->getBaseHp();
	std::cout << "Preservation calculation for ID: " << actor->getID() << std::endl;
	std::cout << "HP Left: " << actor->getHpLeft() << " - " << (float)actor->getHpLeft() / (float)actor->getBaseHp() * 100.f << "%" << std::endl;
	std::cout << "Preservation Weight: " << preservation << std::endl;

	//Strength ratio (logistic curve)
	float strWeight = 1.f;
	int strDiff = actor->getEffectiveDamage(*target, range) - target->getEffectiveDamage(*actor, range);
	strWeight = 2.f / (1.f + std::powf(E, (-1.f * strDiff)));
	std::cout << "Strength Ratio Calculation: " << strWeight << std::endl;
	

	score *= g.agression * preservation * strWeight;
}

void AI_Tactical::clearActions()
{
	for (auto a : actions) {
		delete a;
	}

	actions.clear();
}

//##################################################################################################################################
//		TACTICAL AI
//##################################################################################################################################
AI_Tactical::AI_Tactical(std::vector<Ship*> initShips, Map* initMap, int initOwnerID)
{
	ownerID = initOwnerID;

	for (auto s : initShips) {
		if (s->getOwner() != ownerID) {
			playerShips.insert({ s->getID(), s });
		}
		else {
			aiShips.insert({ s->getID(), s });
		}
	}

	map = initMap;
}

AI_Tactical::~AI_Tactical()
{
	for (auto a : actions) {
		delete a;
	}

	actions.clear();
}

void AI_Tactical::getActions()
{
	//Reset Action List
	this->clearActions();

	//For every ship
	for (auto s : aiShips) {

		//For every tile in move range
		std::vector<sf::Vector2<int>> tiles = map->tileIdxInRange(s.second->getMpLeft(), map->tileAtPos(s.second->getPosition()));

		for (auto t : tiles) {
			std::cout << "Considering Tile: (" << t.x << ", " << t.y << ")" << std::endl;
			int shipID = map->tileAtIdx({ t.x,t.y })->shipID;
			int shipOwner = map->tileAtIdx({t.x,t.y})->ownerID;
			if (shipID >= 0 || shipOwner != ownerID) { std::cout << "Enemy ship present with ID: " << shipID << std::endl; }
			if (shipID >= 0 || shipOwner == ownerID) { std::cout << "Friendly ship present with ID: " << shipID << std::endl; }
			if (shipID < 0) { std::cout << "No ship present" << std::endl; }
			if (playerShips.find(shipID) != playerShips.end()) {
				//If the tile contains an enemy ship in range, push attack action
				sf::Vector2<int> posA = map->tileAtPos(s.second->getPosition());
				sf::Vector2<int> posB = map->tileAtPos(playerShips.at(shipID)->getPosition());
				int dist = (int)std::floor(std::sqrt(std::pow(posB.x - posA.x, 2) + std::pow(posB.y - posA.y, 2)));
				std::cout << "Range to enemy ship: " << dist << " ... ";
				if (dist <= s.second->getMaxRange()) {
					//Push back attack action
					std::cout << "In range" << std::endl;
					Act_Attack* act = new Act_Attack(s.second, map, playerShips.at(shipID), dist);
					actions.push_back(act);
				}
				else {
					std::cout << " Out of range" << std::endl;
				}
			
			}
			else {
				//If the tile does not contain enemy then push move action
				Act_Move* act = new Act_Move(s.second, map, map->tileAtIdx({t.x,t.y})->sprite.getPosition());
				actions.push_back(act);
			}

			std::cout << "---------------------" << std::endl;
		}
	}

	
}

void AI_Tactical::scoreActions()
{
	for (auto a : actions) {
		a->calcScore(strategicGoal);
	}

	std::sort(actions.begin(), actions.end(), [](Action* a, Action* b)->bool {return a->getScore() > b->getScore(); });

	for (auto a : actions) {
		a->print();
	}
}

void AI_Tactical::executeTurn()
{
	//For now get the higest scoring action
	Action* act = nullptr;

	for (auto a : actions) {
		if (!act ||a->getScore() > act->getScore()) {
			act = a;
		}
	}

	if (act) {
		act->execute();
	}
}
