#include "AI.h"

//##################################################################################################################################
//		ACTIONS
//##################################################################################################################################


//#####################################	IDLE #########################################################
Act_Idle::Act_Idle(Ship* initActor)
{
	actor = initActor;
}

Act_Idle::~Act_Idle()
{
}

void Act_Idle::calcScore(Goal g) {
	score *= g.idle;
}

void Act_Idle::execute() {
	//Do Nothing
}

void Act_Idle::print() {
	std::cout << "[Type: IDLE | Score: " << score << " | Actor: " << actor->getID() << std::endl;
}

//#####################################	MOVE #########################################################
Act_Move::Act_Move(Ship* inActor, Map* inEnv, Ship* inStrongestNeigbor, sf::Vector2<float> inTarget)
{
	type = enActionType::MOVE;
	actor = inActor;
	env = inEnv;
	target = inTarget;
	baseScore = 1;
	strongestNeighbor = inStrongestNeigbor;
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

	sf::Vector2<int> og = env->posToTileIdx(actor->getPosition());
	sf::Vector2<int> targ = env->posToTileIdx(target);
	std::vector<sf::Vector2<float>> path = env->pathfind(og, targ);
	actor->pathTo(path);

	return;
}

void Act_Move::print() {
	std::cout << "[Type: MOVE | Score: " << score << " | Cowardice: " << cowardice << " | Actor: "<< actor->getID() << " | Target : " << target.x << ", " << target.y << std::endl;
}

void Act_Move::calcScore(Goal g) {
	//
	if (!actor) {
		score = -1;
		return;
	}

	score = baseScore;
	
	sf::Vector2<int> targ = env->posToTileIdx(target);
	auto t = env->tileAtIdx(targ.x, targ.y);

	//Next to friendly ship
	if (t->ownerID == actor->getOwner() && t->shipID != actor->getID()) {
		score *= g.cohesion;
	}

	//Next to enemy ship
	float closingWeight = 1.f;
	if (t->shipID > 0 && t->ownerID != actor->getOwner()) {
		score *= g.agression;
	}

	//Get preservation weight; at higher preservation running away should score higher
	float runDesire = 1.f - crv::quadratic(actor->getHpLeft() / actor->getBaseHp(), 1.f, 1.4f); // [0,1)

	if (strongestNeighbor) {
		//Get distance from strongest neighbor
		sf::Vector2<float> posA = strongestNeighbor->getPosition();
		sf::Vector2<float> posB = target;
		float distToStrongest = utl::dist(posA.x, posA.y, posB.x,posB.y);

		//Get max distance from strongest neighbor
		float maxDist = -1;
		auto pts = env->tileIdxInRange(actor->getMpLeft(), env->posToTileIdx(actor->getPosition()));
		for (auto p : pts) {
			auto pos = env->tileIdxToPos(p);
			float chk = utl::dist(strongestNeighbor->getPosition().x, strongestNeighbor->getPosition().y, pos.x, pos.y);
			if (chk > maxDist) {
				maxDist = chk;
			}
		}

		cowardice = 1 + (runDesire * (distToStrongest / maxDist)); //[1,n]
		//std::cout << "Cowardice at (" << target.x << "," << target.y << ")" << std::endl;
		//std::cout << "Run Desire: " << runDesire << " | Distance: " << distToStrongest / maxDist << " | Cowardice: " << cowardice << std::endl;
	}
	


	

	float randomFactor = (float)utl::randRange(9900, 10000) / 10000.f; //Between 1 and 0.99
	score *= randomFactor * cowardice;
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
	//
	if (!actor || !target) {
		return;
	}

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
	//
	if (!actor || !target) {
		score = -1;
		return;
	}

	score = baseScore;

	float attackDesire = g.agression * (actor->getStr() / target->getStr());
	float preservation = 1.f - crv::quadratic(actor->getHpLeft() / actor->getBaseHp(), 1.f, 1.4f); //[0,1)
	float attackWeight = attackDesire - preservation;


	float randomFactor = (float)utl::randRange(9900, 10000) / 10000.f; //Between 1 and 0.99

	score *= attackWeight * randomFactor;
}




//##################################################################################################################################
//		TACTICAL AI
//##################################################################################################################################4

AI_Tactical::AI_Tactical(std::vector<Ship*> initShips, Map* initMap, int initOwnerID)
{
	ownerID = initOwnerID;

	for (auto s : initShips) {
		if (s->getOwner() != ownerID) {
			playerShips.insert({ s->getID(), s });
		}
		else {
			aiShips.insert({s->getID(), s});
		}
	}

	map = initMap;
}

AI_Tactical::~AI_Tactical()
{
	
}

void AI_Tactical::getActions()
{
	//std::cout << "Getting AI Actions:" << std::endl;
	//#### Clear existing actions
	for (auto a : chosenActions) {
		delete a;
	}

	chosenActions.clear();

	//#### Find every possible action for every ship and score them
	std::vector<Action*> possibleActions;

	//### Loop Through Each Ship Controlled by AI
	for (auto s : aiShips) {
		//## Check for death
		if (s.second->getDead()) {
			continue;
		}

		//## Add Default Idle Action
		possibleActions.push_back(new Act_Idle(s.second));

		//## Loop through every tile in move range and get move actions
		std::vector<sf::Vector2<int>> tiles;
		sf::Vector2<int> shipPos = map->posToTileIdx(s.second->getPosition());
		tiles = map->tileIdxInRange(s.second->getMpLeft(), shipPos);

		for (auto t : tiles) {
			bool isPassable = map->tileAtIdx({t.x,t.y})->isPassable;
			if (isPassable) {
				//Only move to empty tiles
				
				//Get Strongest Neighbor
				Ship* strongest = nullptr;
				for (auto n : map->tileIdxInRange(s.second->getSensorRange(), shipPos)) {
					auto tile = map->tileAtIdx({n.x,n.y});
					if (tile->ownerID != this->ownerID && tile->shipID >= 0) {
						Ship* candidate = playerShips.at(tile->shipID);
						if (!strongest || strongest->getStr() < candidate->getStr()) {
							strongest = candidate;
						}
					}
				}
				possibleActions.push_back(new Act_Move(s.second, map, strongest ,map->tileAtIdx({ t.x,t.y })->sprite.getPosition()));
			}
		}

		//## Loop through every tile in attack range and get attack actions (can be combined with move loop for optimization purposes)
		tiles.clear();
		tiles = map->tileIdxInRange(s.second->getMaxRange(), map->posToTileIdx(s.second->getPosition()));
		for (auto t : tiles) {
			int shipID = map->tileAtIdx({ t.x,t.y })->shipID;
			int shipOwner = map->tileAtIdx({ t.x,t.y })->ownerID;
			if (playerShips.find(shipID) != playerShips.end()) {
				//If the tile contains an enemy ship in range, push attack action
				sf::Vector2<int> posA = map->posToTileIdx(s.second->getPosition());
				sf::Vector2<int> posB = map->posToTileIdx(playerShips.at(shipID)->getPosition());
				int dist = (int)std::floor(std::sqrt(std::pow(posB.x - posA.x, 2) + std::pow(posB.y - posA.y, 2)));

				if (dist <= s.second->getMaxRange() && !playerShips.at(shipID)->getDead()) {
					//Push back attack action
					possibleActions.push_back(new Act_Attack(s.second, map, playerShips.at(shipID), dist));
				}
			}
		}

		//## Score the actions
		for (auto a : possibleActions) {
			a->calcScore(strategicGoal);
			a->print();
		}

		//## Sort the actions (Ascending)
		std::sort(possibleActions.begin(), possibleActions.end(), [](Action* a, Action* b)->bool {return a->getScore() < b->getScore(); });

		//## Pick an action (Just the highest score action for now)
		if (possibleActions.size() >= 1) {
			chosenActions.push_back(possibleActions.back());
			possibleActions.pop_back();
		}

		//## Clean up unused actions
		for(auto a : possibleActions){
			delete a;
		}

		possibleActions.clear();
	}

	//std::cout << "----------------------------------------------" << std::endl;
}

void AI_Tactical::executeTurn()
{
	//## Loop through each chosen action and execute them
	for (auto a : chosenActions) {
		a->execute();
	}

	//## Clean up the actions
	for (auto a : chosenActions) {
		delete a;
	}

	chosenActions.clear();
}

void AI_Tactical::update() {
	//## Remove dead ships
	//AI
	for (auto it = aiShips.begin(); it != aiShips.end();)
	{

		if ((*it).second->getDead()) {
			it = aiShips.erase(it);
		}
		else {
			++it;
		}
	}

	//Player
	for (auto it = playerShips.begin(); it != playerShips.end();)
	{

		if ((*it).second->getDead()) {
			it = playerShips.erase(it);
		}
		else {
			++it;
		}
	}
}
