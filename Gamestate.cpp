#include "Gamestate.h"
#include "Ship.h"
#include "SolarSystem.h"

//################################################################################################################
//				COMMON FUNCTIONS
////##############################################################################################################

bool Gamestate::getFinished()
{
	return isFinished;
}

En_Gamestate Gamestate::getNextState()
{
	return nextState;
}

//################################################################################################################
//				GAME STATE FUNCTIONS
////##############################################################################################################


void State_Game::initTest()
{
	std::cout << "Initializing test for Game State" << std::endl;
	map = new Map(texReg, &mapView);
	map->loadFromFile("resource/map/map_0.txt");
	map->setGridColor({100,100,255,100});
	//map->addSpotter({5,5}, 2);
	//map->addSpotter({ 20,13 }, 1);

	//Reconfigure camera constraints
	cameraConstraints = { {-500.f,-500.f}, {(int)map->getSize().x + 500.f, (int)map->getSize().y + 500.f} };


	std::cout << "L: " << cameraConstraints.position.x << " T: " << cameraConstraints.position.y;
	std::cout << " W: " << cameraConstraints.size.x << " H:" << cameraConstraints.size.y << std::endl;

	Ship* s = new Ship(0);
	s->setOwner(0);
	s->setTextureSize(64);
	s->setTexture(texReg->lookup("ship_4"));
	s->setName("NCS Hammer");
	s->setLaborCost(2.f);
	s->setHp(10);
	s->addOffComp({0,true,5,0,1,0,3});
	s->addDefComp({0,true, 3, 3, 1, 0});
	ships.push_back(s);
	s->setMp(5);

	Ship* s2 = new Ship(1);
	s2->setOwner(0);
	s2->setTextureSize(64);
	s2->setTexture(texReg->lookup("ship_5"));
	s2->setPosition({160,160});
	s2->setName("NCS Anvil");
	s2->setHp(10);
	s2->addOffComp({ 0,true,5,0,1,0,1 });
	s2->addDefComp({ 0,true, 3, 3, 1, 0 });
	s2->setLaborCost(3.f);
	s2->setMp(5);
	ships.push_back(s2);

	Ship* s3 = new Ship(2);
	s3->setOwner(1);
	s3->setTexture(texReg->lookup("ship_3"));
	s3->setPosition({ 320,320 });
	s3->setName("Enemy Ship");
	s3->setLaborCost(3.f);
	s3->addOffComp({ 0,true,5,0,1,0,3 });
	s3->addDefComp({ 0,true, 3, 3, 1, 0 });
	s3->setHp(10);
	s3->setMp(5);
	ships.push_back(s3);

	//Set up Solar systems
	SolarSystem* system1 = new SolarSystem(0);
	system1->setPosition({2,2});
	system1->setTextureName("testSheet");
	system1->setName("Alpha Centauri");
	//system1->setLaborPool(1.f);
	//system1->setPopulation(1.f);
	map->tileAtIdx(2, 2)->isPassable = false;
	map->tileAtIdx(2, 2)->sprite.setFrame(1);
	

	SolarSystem* system2 = new SolarSystem(0);
	system2->setPosition({ 4,6 });
	system2->setOwnerId(0);
	system2->setTextureName("testSheet");
	system2->setName("Altair");
	system2->setLaborPool(2.f);
	system2->setPopulation(1.f);
	map->tileAtIdx(4, 6)->isPassable = false;
	map->tileAtIdx(4, 6)->sprite.setFrame(2);

	solarSystems.push_back(system1);
	solarSystems.push_back(system2);

	//Project Catalogs
	shipCatalog.insert({0,*s});
	shipCatalog.insert({ 1,*s2 });

	//ID, Tech, Tags, name, rp, rpMod, creditCost, income, incomeMod, maint, maintMod, laborCost, laborMaint, laborGen, laborMod, popGrowth, popCap, active
	Infrastructure basicFarm = { 0,-1,{enInfTags::STD, enInfTags::POP},"Basic Farm", 0.f, 0.f, 100.f, 0.f, 0.f, 5.f, 0.f, 10.f, 0.2f, 0.f, 0.f, 0.05f, 0.f, true };
	Infrastructure advancedFarm = { 1,-1,{enInfTags::STD, enInfTags::POP},"Advanced Farm", 0.f, 0.f, 100.f, 0.f, 0.f, 5.f, 0.f, 10.f, 0.2f, 0.f, 0.f, 0.06f, 1.f, true };
	Infrastructure basicFactory = { 2,-1,{enInfTags::STD, enInfTags::POP},"Basic Factory", 0.f, 0.f, 100.f, 0.f, 0.f, 5.f, 0.f, 10.f, 0.0f, 1.f, 0.f, 0.f, 0.f, true };
	infCatalog.insert({0, basicFarm });
	infCatalog.insert({ 1, advancedFarm });
	infCatalog.insert({ 2, basicFactory });
}

void State_Game::initGui()
{
	topBarHeight = 25;
}

void State_Game::updateCamera(float dt)
{
	//Pan
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) {
		this->moveCamera({ 0, -1.f * panSpeed * currZoom * dt });
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) {
		this->moveCamera({ 0, panSpeed * currZoom * dt });
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
		this->moveCamera({ -1.f * panSpeed * currZoom * dt, 0 });
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
		this->moveCamera({ panSpeed * currZoom * dt, 0 });
	}

}

void State_Game::constrainCamera()
{
	//Width
	if (cameraOffset.x < cameraConstraints.position.x) {
		this->moveCamera({cameraConstraints.position.x - cameraOffset.x, 0});
		cameraOffset.x = cameraConstraints.position.x;
	}

	//Height
	if (cameraOffset.y < cameraConstraints.position.y) {
		this->moveCamera({ 0, cameraConstraints.position.y - cameraOffset.y});
		cameraOffset.y = cameraConstraints.position.y;
	}

	//Right
	if (cameraOffset.x > cameraConstraints.position.x + cameraConstraints.size.x) {
		this->moveCamera({ cameraConstraints.position.x + cameraConstraints.size.x - cameraOffset.x, 0 });
		cameraOffset.x = cameraConstraints.position.x + cameraConstraints.size.x;
	}

	//Bottom
	if (cameraOffset.y > cameraConstraints.position.y + cameraConstraints.size.y) {
		this->moveCamera({ 0, cameraConstraints.position.y + cameraConstraints.size.y - cameraOffset.y });
		cameraOffset.y = cameraConstraints.position.y + cameraConstraints.size.y;
	}
}

void State_Game::moveCamera(sf::Vector2<float> offset)
{
	cameraOffset += offset;
	mapView.move(offset);
	//std::cout << "Cam offset: " << cameraOffset.x << ", " << cameraOffset.y << std::endl;
}

void State_Game::zoomCamera(float zoom)
{
	if (zoom < maxZoom && zoom >= minZoom) {

		mapView.zoom(1 / currZoom);
		currZoom = zoom;
		mapView.zoom(currZoom);
	}
}

void State_Game::selectShip(int i)
{
	shipSel = i;
	cursor.setPosition(ships.at(i)->getPosition());
	/*gui_shipSel->setVisible(true);

	label_shipName->setText(ships.at(i)->getName());
	sf::Vector2<int> p = map->tileAtPos(ships.at(i)->getPosition());
	label_shipPos->setText(std::to_string(p.x) + ", " + std::to_string(p.y));
	label_shipMp->setText("Movement: " + std::to_string(ships.at(i)->getM.position.x()));*/
}

void State_Game::selectSolarSystem(int i)
{
	systemSel = i;
	sf::Vector2<float> pos = map->tileAtIdx(solarSystems.at(i)->getPosition())->sprite.getPosition();
	cursor.setPosition(pos);
}

void State_Game::showGui_TopBar()
{

	ImGuiWindowFlags windowFlags = 0;
	windowFlags |= ImGuiWindowFlags_NoCollapse;
	windowFlags |= ImGuiWindowFlags_NoTitleBar;
	windowFlags |= ImGuiWindowFlags_NoResize;
	windowFlags |= ImGuiWindowFlags_NoMove;

	bool pOpen = false;

	ImGui::Begin("topBar", &pOpen, windowFlags);
	ImGui::SetWindowPos(ImVec2(0, 0));
	ImGui::SetWindowSize(ImVec2(windowWidth, topBarHeight));
	topBarBottom = (unsigned int)ImGui::GetWindowHeight();
	ImGui::Text("Turn: %d", turn);
	ImGui::End();
}

void State_Game::showGui_shipSel()
{
	ImGuiWindowFlags windowFlags = 0;
	windowFlags |= ImGuiWindowFlags_NoCollapse;
	windowFlags |= ImGuiWindowFlags_NoTitleBar;
	windowFlags |= ImGuiWindowFlags_NoResize;
	windowFlags |= ImGuiWindowFlags_NoMove;

	bool pOpen = false;

	//ImGui::Text("Underlying float value: %f", drag_f);

	Ship& sel = *ships.at(shipSel);

	ImGui::Begin("shipsel",&pOpen,windowFlags);
	ImGui::SetWindowPos(ImVec2(0,topBarBottom));
	ImGui::SetWindowSize(ImVec2(300, 150));
	ImGui::Text("%s", sel.getName().c_str());
	ImGui::Separator();
	sf::Vector2<int> pos = map->tileAtPos(sel.getPosition());
	ImGui::Text("(%d, %d)", pos.x, pos.y);
	ImGui::NewLine();
	ImGui::Text("HP: %d", sel.getHpLeft());
	ImGui::Text("Movement Left: %d", sel.getMpLeft());
	ImGui::End();
}

void State_Game::showGui_systemSel()
{
	ImGuiWindowFlags windowFlags = 0;
	windowFlags |= ImGuiWindowFlags_NoCollapse;
	windowFlags |= ImGuiWindowFlags_NoTitleBar;
	windowFlags |= ImGuiWindowFlags_NoResize;
	windowFlags |= ImGuiWindowFlags_NoMove;

	bool pOpen = false;
	SolarSystem &sel = *solarSystems.at(systemSel);

	ImGui::Begin("systemSel", &pOpen, windowFlags);
	ImGui::Text("%s", sel.getName().c_str());
	ImGui::SetWindowPos(ImVec2(.1 * windowWidth, .1 * windowHeight));
	ImGui::SetWindowSize(ImVec2(.8 * windowWidth, .8 * windowHeight));

	sf::Texture* tex = map->tileAtIdx(sel.getPosition())->sprite.getTexture(); //This is a little cursed, but whatever
	sf::Rect<int> rec = map->tileAtIdx(sel.getPosition())->sprite.getTextureRect();
	if (tex) {
		systemPreview.setTexture(*tex);
		systemPreview.setTextureRect(rec);
	}
	else {
		systemPreview.setTexture(*texReg->lookup("default"));
	}

	float scaleX = systemPreviewSize / systemPreview.getLocalBounds().size.x;
	float scaleY = systemPreviewSize / systemPreview.getLocalBounds().size.y;
	systemPreview.setScale({scaleX,scaleY});

	//## Top Pane (Stat Block)
	ImGui::BeginChild("top pane", ImVec2(ImGui::GetWindowWidth(), ImGui::GetWindowHeight() / 3), 0);
	ImGui::Image(systemPreview);
	ImGui::Text("Population: %0.2f / %0.1f", sel.getPopulation(), sel.getPopulationCap()); //Population
	ImGui::Text("Population Growth: %0.2f", sel.getPopulationGrowth());

	ImGui::Text("Income: +%2.f / -%2.f / %2.f", sel.getIncome(), sel.getMaint(), sel.getIncome() - sel.getMaint()); //Income and Maint
	ImGui::Text("Research Pts: %2.f", sel.getRP()); //Research
	ImGui::Text("Labor: %2.f / %2.f", sel.getLaborUsed(), sel.getLaborPool()); //Labor
	ImGui::EndChild();
	ImGui::Separator();

	//## Bottom half
	ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;

	if (ImGui::BeginTabBar("SystemTabBar", tab_bar_flags))
	{
		if (ImGui::BeginTabItem("Projects"))
		{
			//# Improvement Catalog
			ImGui::BeginChild("improvementCatalog", ImVec2(ImGui::GetWindowWidth() / 3, ImGui::GetWindowHeight() * (2 / 3)), ImGuiChildFlags_Borders);
			ImGui::Text("Build Improvements");
			for (int i = 0; i < infCatalog.size(); i++) {
				if (ImGui::Button(infCatalog.at(i).name.c_str())) {
					Infrastructure inf = infCatalog.at(i);

					sel.addProject({ i,enBuildType::INF,inf.name, inf.laborCost, 0.f, {0.f, 0.f}, false });
				}

			}
			ImGui::EndChild();

			//# Ship Catalog
			ImGui::SameLine();
			ImGui::BeginChild("shipCatalog", ImVec2(ImGui::GetWindowWidth() / 3, ImGui::GetWindowHeight() * (2 / 3)), ImGuiChildFlags_Borders);
			ImGui::Text("Build Ships");
			for (int i = 0; i < shipCatalog.size(); i++) {
				if (ImGui::Button(shipCatalog.at(i).getName().c_str())) {
					Ship shp = shipCatalog.at(i);
					sf::Vector2<float> spawnPos = map->tileAtIdx({ sel.getPosition().first + 1, sel.getPosition().second })->sprite.getPosition();

					sel.addProject({ i, enBuildType::SHIP, shp.getName(), shp.getLaborCost(), 0.f, {spawnPos.x, spawnPos.y}, false });
				}
					
			}
			ImGui::EndChild();

			//#Project Queue
			ImGui::SameLine();
			ImGui::BeginChild("buildQueue", ImVec2(ImGui::GetWindowWidth() / 3, ImGui::GetWindowHeight() * (2 / 3)), ImGuiChildFlags_Borders);
			ImGui::Text("Project Queue");
			ImGui::PushItemFlag(ImGuiItemFlags_AllowDuplicateId, true);
			std::vector<Project>& items = sel.getBuildQueue();
			for (size_t i = 0; i < items.size(); i++)
			{
				Project item = items.at(i);
				
				std::string str = item.name + " Labor:" + utl::toStrP(item.laborSpent, 2) + " / " + utl::toStrP(item.laborCost, 2);

				ImGui::Selectable(str.c_str());

				if (ImGui::IsItemActive() && !ImGui::IsItemHovered())
				{
					auto next = i + (ImGui::GetMouseDragDelta(0).y < 0.f ? -1 : 1);
					if (next >= 0 && next < items.size())
					{
						items.at(i) = items.at(next);
						items.at(next) = item;
						ImGui::ResetMouseDragDelta();
					}
				}
			}

			ImGui::PopItemFlag();
			ImGui::EndChild();
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("View Improvements"))
		{
			//# Improvement Viewer
			ImGui::BeginChild("improvementViewer", ImVec2(ImGui::GetWindowWidth(), ImGui::GetWindowHeight() * (2 / 3)), ImGuiChildFlags_Borders);
			ImGui::Text("Current Improvements");
			for (size_t i = 0; i < sel.getImprovements().size(); i++) {
				Infrastructure imp = sel.getImprovements().at(i);
				ImGui::Text(imp.name.c_str());
			}
			ImGui::EndChild();
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
	
	ImGui::End();
}

void State_Game::showGui_SystemSel_Empty()
{
	ImGuiWindowFlags windowFlags = 0;
	windowFlags |= ImGuiWindowFlags_NoCollapse;
	windowFlags |= ImGuiWindowFlags_NoTitleBar;
	windowFlags |= ImGuiWindowFlags_NoResize;
	windowFlags |= ImGuiWindowFlags_NoMove;
	windowFlags |= ImGuiWindowFlags_NoScrollbar;

	bool pOpen = false;

	SolarSystem& sel = *solarSystems.at(systemSel);

	ImGui::Begin("systemSelEmpty", &pOpen, windowFlags);
	
	float h = .2 * windowHeight;
	ImGui::SetWindowPos(ImVec2(.3 * windowWidth, .4 * windowHeight));
	ImGui::SetWindowSize(ImVec2(.4 * windowWidth, h));


	sf::Texture* tex = map->tileAtIdx(sel.getPosition())->sprite.getTexture(); //This is a little cursed, but whatever
	sf::Rect<int> rec = map->tileAtIdx(sel.getPosition())->sprite.getTextureRect();
	if (tex) {
		systemPreview.setTexture(*tex);
		systemPreview.setTextureRect(rec);
	}
	else {
		systemPreview.setTexture(*texReg->lookup("default"));
	}

	float scaleX = (h - 20) / systemPreview.getLocalBounds().size.x;
	float scaleY = (h - 20) / systemPreview.getLocalBounds().size.y;
	systemPreview.setScale({ scaleX,scaleY });

	ImGui::BeginChild("left", ImVec2(h + 20, ImGui::GetWindowHeight()), 0);
	ImGui::Image(systemPreview);
	ImGui::EndChild();
	ImGui::SameLine();
	ImGui::BeginChild("middle", ImVec2(ImGui::GetWindowWidth() - (h+20), ImGui::GetWindowHeight()), 0);
	ImGui::Text("%s", sel.getName().c_str());
	ImGui::Separator();
	ImGui::Text("An uncolonized system");
	ImGui::EndChild();
	
	ImGui::End();
}

void State_Game::showGui_SystemSel_Enemy()
{
}

void State_Game::pollMove(Ship* ship, sf::Vector2<float> pos)
{
	sf::Vector2<int> t = map->tileAtPos(pos);
	std::cout << "Tile: " << t.x << ", " << t.y << std::endl;

	//Ship movment
	if (shipSel >= 0 && shipSel < ships.size()) {
		//Check for valid position
		sf::Vector2<int> og = map->tileAtPos(ships.at(shipSel)->getPosition());
		sf::Vector2<int> target = map->tileAtPos(pos);
		std::vector<sf::Vector2<float>> path = map->pathfind(og, target);


		if (path.size() >= 1) {
			ships.at(shipSel)->pathTo(path);
		}
	}
}

void State_Game::pollAttack(Ship* attacker, Ship* defender, int range)
{
	//Damage = physDmg - armor + energyDmg - sheild | Temporary method, this does not make good use of seperate dmg types
	int physDmg = attacker->getPhysDmg(range) - defender->getArmor();
	int energyDmg = attacker->getEnergyDmg(range) - defender->getShield();

	if (physDmg < 0) { physDmg = 0; }
	if (energyDmg < 0) { energyDmg = 0; }

	int damage = physDmg + energyDmg;

	if (damage < 0) {
		damage = 0;
	}

	Spritesheet effect;

	Animation anm = {0,0,0,3,false,false,-1,2};
	attacker->facePoint(defender->getPosition());
	defender->facePoint(attacker->getPosition());
	float rad = attacker->getRotation().asRadians() + utl::degToRad(90.f);
	sf::Vector2<float> pos = { attacker->getPosition().x - ((float)(tileSize/2.f) * std::cosf(rad)) , attacker->getPosition().y - ((float)(tileSize/2.f) * std::sinf(rad))};

	effect.setTexture(texReg->lookup("beam"));
	effect.setFrameSize(32,32);
	effect.setAnimation(anm);
	effect.setOrigin({ effect.getLocalBounds().size.x / 2.f, effect.getLocalBounds().size.y / 2.f });
	effect.setPosition(pos);
	effect.setRotation(attacker->getRotation());
	effects.push_back(effect);

	

	defender->takeDmg(damage);
}

void State_Game::spawnShip(int owner, Ship type, sf::Vector2<float> pos)
{
	Ship* s = new Ship(type);
	s->setOwner(owner);
	s->setPosition(pos);
	ships.push_back(s);
}

State_Game::State_Game(TextureRegistry* textureRegistry, sf::RenderWindow* window)
{
	this->texReg = textureRegistry;
	this->win = window;

	windowWidth = window->getSize().x;
	windowHeight = window->getSize().y;

	if (!window) {
		std::cout << "Invalid window!!!" << std::endl;
		abort();
	}

	cameraConstraints.position.x = win->getSize().x * -0.1; //10% offset.position.x
	cameraConstraints.size.x = win->getSize().x + (-1 * cameraConstraints.position.x); // 10% offset right
	cameraConstraints.position.y = win->getSize().y * -0.1; //10% offset up
	cameraConstraints.size.y = win->getSize().y + (-1 * cameraConstraints.position.y); // 10% offset down


	//Set up views
	mapView.setViewport({ {0.f,0.f},{1.f,1.f} }); //Where it appears in window
	mapView.setSize({ (float)win->getSize().x, (float)win->getSize().y }); //Ratio to window size
	guiView = win->getDefaultView();

	//Set up Cursors
	sf::Texture* temp = texReg->lookup("cursor_select");
	cursor.setTexture(*temp, true);
	cursor.setScale({ (float)tileSize / temp->getSize().x, (float)tileSize / temp->getSize().y });
	cursor.setOrigin({ cursor.getLocalBounds().size.x / 2, cursor.getLocalBounds().size.y / 2 });

    temp = texReg->lookup("cursor_target");
	targetCursor.setTexture(*temp, true);
	targetCursor.setScale({ (float)tileSize / temp->getSize().x, (float)tileSize / temp->getSize().y });
	targetCursor.setOrigin({ targetCursor.getLocalBounds().size.x / 2, targetCursor.getLocalBounds().size.y / 2 });

	//Set up background sprite
	background.setTexture(*texReg->lookup("background_1"), true);
	sf::Vector2<float> size = { background.getLocalBounds().size.x, background.getLocalBounds().size.y};
	float scaleX = win->getSize().x / size.x;
	float scaleY = win->getSize().y / size.y;
	
	background.setScale({ scaleX, scaleY });

	//Call initializers
	this->initGui();
	this->initTest();
}

State_Game::~State_Game()
{
	for (auto s : ships) {
		delete s;
	}

	ships.clear();

	for (auto s : solarSystems) {
		delete s;
	}

	solarSystems.clear();
}

void State_Game::endTurn()
{
	for (auto s : ships) {
		s->onTurnEnd();
	}

	for (auto a : solarSystems) {
		a->onTurnEnd();

		//Check for project completion
		auto q = a->getBuildQueue();
		for (auto p : q) {
			if (p.isDone) {
				if (p.type == enBuildType::INF) {
					a->addInfrastructure(infCatalog.at(p.buildID));
				}
				else if (p.type == enBuildType::SHIP) {
					this->spawnShip(a->getOwnerId(), shipCatalog.at(p.buildID), { p.pos.first, p.pos.second});
				}

				a->removeDoneProjects();
			}
		}
	}
	

	turn++;
}

void State_Game::beginTurn()
{

	for (auto s : ships) {
		s->onTurnBegin();
	}
}


//###################################################################################
//	UPDATE, POLL, DRAW (GAMELOOP FUNCTIONS)
//###################################################################################

void State_Game::update(float dt)
{
	this->updateCamera(dt);
	this->constrainCamera();

	//GUI
	if (showDemo) {
		ImGui::ShowDemoWindow();
	}
	showGui_TopBar();

	if (shipSel >= 0) {
		systemSel = -1;
		showGui_shipSel();
	}

	if (systemSel >= 0) {
		shipSel = -1;
		if (solarSystems.at(systemSel)->getOwnerId() == 0) {
			//Player System
			isMenuShown = true;
			showGui_systemSel();
		}
		else if (solarSystems.at(systemSel)->getOwnerId() < 0) {
			//Uninhabited system
			isMenuShown = true;
			showGui_SystemSel_Empty();
		}
		else {
			//Enemy System
			// isMenuShown = true;
			//showGui_SystemSel_Enemy();
		}
	}

	map->clearShipPositions(); //May be better to move this logic to not fire every frame


	for (auto it = ships.begin(); it != ships.end(); /* NOTHING */)
	{

		if (!(*it)->isAnimated() && (*it)->getDead()) {
			//Adjust ship selection
			if (shipSel >= 0 && std::distance(it,ships.begin() + shipSel) >= 0) {
				//Then Selected ship is after deleted ship, decrement shipSel
				if (ships.size() > 1 && ships.at(shipSel)->getID() != (*it)->getID()) {
					shipSel--;
				}
				else {
					shipSel = -1;
				}
			}

			delete (*it);
			it = ships.erase(it);
			std::cout << "Ship sel: " << shipSel << std::endl;
			std::cout << "Ships Size: " << ships.size() << std::endl;
		}else {
			(*it)->Update(dt);
			sf::Vector2<int> pos = map->tileAtPos((*it)->getPosition());
			map->tileAtIdx({ pos.x, pos.y })->shipID = (*it)->getID();
			
			++it;
		}
	}

	for(auto i = 0; i < effects.size(); i++) {
		effects.at(i).update(dt);
		if (!effects.at(i).getAnimated()) {
			//Remove from list
			effects.erase(effects.begin() + i);
		}
	}


	//testTimer += dt;
	if (testTimer >= 1.f) {
		map->Update();
		testTimer = 0;
	}
}

void State_Game::poll(sf::RenderWindow& win, std::optional<sf::Event> event)
{
	//## Mouse press
	if (const auto* mouseButton = event->getIf<sf::Event::MouseButtonPressed>()) {
		sf::Vector2<int> mousePos = sf::Mouse::getPosition(win);

		if (!isMenuShown) {
			//Left Button
			if (mouseButton->button == sf::Mouse::Button::Left) {
				//TODO: Add a check to see if mouse is in map viewport
				isMousePan = true;
				panStart = { (float)mousePos.x, (float)mousePos.y };
				panDelta = {0.f,0.f};
			}
		}
	}

	//## Mouse release
	if (const auto* mouseButton = event->getIf<sf::Event::MouseButtonReleased>()) {
		//Get position
		sf::Vector2<int> rawPos = sf::Mouse::getPosition(win);
		win.setView(mapView);
		sf::Vector2<float> mapViewMousePos = win.mapPixelToCoords(rawPos);

		if (isMousePan) {
			isMousePan = false;
		}

		if (!isMenuShown) {
			//Left
			if (mouseButton->button == sf::Mouse::Button::Left) {
			
				if (panDelta.length() < 2.f) {
					shipSel = -1;
				}

				//Ship selection logic
				for (int i = 0; i < ships.size(); i++) {
					if (ships.at(i)->contains(mapViewMousePos)) {
						this->selectShip(i);
						break;
					}
				}

				for (int i = 0; i < solarSystems.size(); i++) {
					if (map->tileAtIdx(solarSystems.at(i)->getPosition())->contains(mapViewMousePos)) {
						this->selectSolarSystem(i);
					}
				}
			}


			//Right
			if (mouseButton->button == sf::Mouse::Button::Right) {
				if (shipSel >= 0) {
					//Check for attack
					bool didAttack = false;

					for (auto s : ships) {
						if (s->contains(mapViewMousePos)) {
							
							if (s->getOwner() != ships.at(shipSel)->getOwner()) {
								sf::Vector2<int> posA = map->tileAtPos(ships.at(shipSel)->getPosition());
								sf::Vector2<int> posB = map->tileAtPos(s->getPosition());
								int dist = (int)std::floor(std::sqrt(std::pow(posB.x - posA.x, 2) + std::pow(posB.y - posA.y, 2)));
								if (dist <= ships.at(shipSel)->getMaxRange()) {
									this->pollAttack(ships.at(shipSel), s, dist);
								}
								didAttack = true;
							}
						}
					}

					if (!didAttack) {
						this->pollMove(ships.at(shipSel), mapViewMousePos);
					}
				}
			}
		}
	
	}

	//## Mouse Move
	if (const auto* mouseMoved = event->getIf<sf::Event::MouseMoved>()) {
		sf::Vector2<int> mousePos = sf::Mouse::getPosition(win);
		if (isMousePan) {
			float panSpeedMult = 1.2;
			panDelta = {panStart.x - (float)mousePos.x, panStart.y - (float)mousePos.y};
			panDelta *= (currZoom * panSpeedMult);
			this->moveCamera(panDelta);

			panStart = { (float)mousePos.x, (float)mousePos.y };
		}
	}

	//Scroll Wheel
	if (const auto* mouseScrolled = event->getIf<sf::Event::MouseWheelScrolled>()) {
		if (!isMenuShown) {
			float zoom = currZoom - (zoomSpeed * mouseScrolled->delta);
			zoomCamera(zoom);
		}

	}

	//Key Release
	if (const auto* keyR = event->getIf<sf::Event::KeyReleased>()) {
		if (keyR->code == sf::Keyboard::Key::Escape) {
			shipSel = -1;
			systemSel = -1;
			isMenuShown = false;
		}

		if (keyR->code == sf::Keyboard::Key::Enter) {
			this->endTurn();
			this->beginTurn();
		}

		if (keyR->code == sf::Keyboard::Key::Grave) {
			showDemo = !showDemo;
			isMenuShown = showDemo;
		}

		if (!isMenuShown) {
			if (keyR->code == sf::Keyboard::Key::Delete) {
				if (shipSel >= 0) {
					ships.at(shipSel)->kill();
				}
			}

			if (keyR->code == sf::Keyboard::Key::Hyphen) {
				float zoom = currZoom + zoomStep;
				zoomCamera(zoom);
			}

			if (keyR->code == sf::Keyboard::Key::Equal) {
				float zoom = currZoom - zoomStep;
				zoomCamera(zoom);
			}
		}
	}

	for (auto s : ships) {
		s->Poll(event, win);
	}
	map->Poll(win, event);
}

void State_Game::draw(sf::RenderWindow& win)
{
	win.setView(win.getDefaultView());
	win.draw(background);

	//Draw map
	win.setView(mapView);
	map->Draw(win);

	
	for (auto s : ships) {
		s->draw(win);
	}

	for (auto e : effects) {
		e.draw(win);
	}

	//Draw cursors for selected ship
	if (shipSel >= 0) {
		if (!ships.at(shipSel)->getMoving()) {
			//Then draw the selection cursor
			cursor.setPosition(ships.at(shipSel)->getPosition());
			win.draw(cursor);
		}

		if (ships.at(shipSel)->hasGoal()) {
			//The draw the target cursor
			targetCursor.setPosition(ships.at(shipSel)->getPathTarget());
			win.draw(targetCursor);
		}
	}

	//Draw cursor for selected solar system
	if (systemSel >= 0) {
		win.draw(cursor);
	}
}


