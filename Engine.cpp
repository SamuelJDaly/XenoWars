#include "Engine.h"

void Engine::initWindow()
{
	win = new sf::RenderWindow;
	win->create(sf::VideoMode({ 1280, 720 }), "Xeno Wars");
}

void Engine::initTextures()
{
	texReg = new TextureRegistry();

	texReg->addTexture("default", "resource/tex/nope.png");
	texReg->addTexture("transparent", "resource/tex/transparent.png");
	texReg->addTexture("testSheet", "resource/tex/testSheet.png");
	texReg->addTexture("testShip_0", "resource/tex/testShip_0.png");
	texReg->addTexture("cursor_select", "resource/tex/cursor_0.png");
	texReg->addTexture("cursor_target", "resource/tex/cursor_1.png");
	texReg->addTexture("tileset_0", "resource/tex/tileset_0.png");
	texReg->addTexture("background_0", "resource/tex/Background_space.png");
	texReg->addTexture("background_1", "resource/tex/bg_1.png");
	texReg->addTexture("ship_1", "resource/tex/ship_1.png");
	texReg->addTexture("ship_2", "resource/tex/ship_2.png");
	texReg->addTexture("panel_0","resource/tex/panel_0.png");
	texReg->addTexture("ship_3", "resource/tex/ship_3.png");
	texReg->addTexture("ship_4", "resource/tex/ship_4.png");
	texReg->addTexture("ship_5", "resource/tex/ship_5.png");
	texReg->addTexture("beam", "resource/tex/beam.png");
}

void Engine::initState()
{
	currState = new State_Game(texReg, this->win);
	//currState = new State_Editor(textureHandler, this->win);
	//currState = new State_Menu(textureHandler, this->win);
}

Engine::Engine()
{
	std::srand(std::time(0));
	this->initWindow();
	this->initTextures();
	this->initState();
	ImGui::SFML::Init(*win);
}

Engine::~Engine()
{
	ImGui::SFML::Shutdown();
	delete currState;
	delete texReg;
	delete win;
}

void Engine::poll()
{
	//# Handle Polled Events
	while (std::optional event = win->pollEvent()) {
		ImGui::SFML::ProcessEvent(*win, *event);
		//Window closure
		if (event->is<sf::Event::Closed>()) {
			win->close();
			isRunning = false;
		}

		if (auto* mouseButton = event->getIf<sf::Event::MouseButtonReleased>()) {
			if (mouseButton->button == sf::Mouse::Button::Left) {
				auto pos = sf::Mouse::getPosition(*win);

				//std::cout << "(" << pos.x << ", " << pos.y << ")" << std::endl;
			}
		}

		currState->poll(*win, event);
	}
}

void Engine::update()
{
	//## Update delta time
	deltaTime = mainClock.getElapsedTime().asSeconds();
	mainClock.restart();


	//Update Current State
	ImGui::SFML::Update(*win, sf::seconds(deltaTime));
	currState->update(deltaTime);

	//State Transition Logic
	if (currState->getFinished()) {
		//Transition to next state
		switch (currState->getNextState()) {
		case En_Gamestate::MENU:
			//De allocate last state
			delete currState;
			//Allocate new state
			//currState = new State_Menu(textureHandler, this->win);

			break;
		case En_Gamestate::GAME:
			//De allocate last state
			delete currState;
			//Allocate new state
			currState = new State_Game(texReg, this->win);

			break;
		case En_Gamestate::EDITOR:
			//De allocate last state
			delete currState;
			//Allocate new state
			//currState = new State_Editor(textureHandler, this->win);

			break;
		case En_Gamestate::END:
			win->close();
			isRunning = false;
			break;
		default:
			std::cerr << "Invalid State Transition" << std::endl;
			win->close();
			isRunning = false;
			break;
		}
	}
}

void Engine::draw()
{
	if (!currState) {
		isRunning = false;
		return;
	}

	win->clear();

	currState->draw(*win);

	win->setView(win->getDefaultView());
	ImGui::SFML::Render(*win);

	win->display();
}

bool Engine::getRunning()
{
	return isRunning;
}
