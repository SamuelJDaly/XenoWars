#include "Player.h"

Player::Player() {

}

Player::~Player() {

}

void Player::endTurn() {
	credits += netIncome;
	
	if (researchID >= 0) {
		researchProgress += researchPoints;
	}
}