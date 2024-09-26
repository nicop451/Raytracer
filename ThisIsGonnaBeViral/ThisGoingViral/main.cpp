#include <iostream>
#include <SFML/Graphics.hpp>
#include "ErrorUtil.h"
#include "Solver.h"
#include "math.h"

using namespace sf;

int main()
{
	unsigned int winWidth = 1920;
	unsigned int winHeight = 1080;

	unsigned int winCenterX = winWidth / 2;
	unsigned int winCenterY = winHeight / 2;

	Font font;
	if (!font.loadFromFile("Assets/static/Inconsolata-Regular.ttf")) { LOG_ERROR("Couldn't load font"); }

	Text fpsText;
	fpsText.setFont(font);
	fpsText.setPosition(winWidth * 0.9, 50);
	fpsText.setCharacterSize(60);
	fpsText.setFillColor(Color(255, 255, 255));

	Clock frameRateUpdateClock;
	int framerate = 0;


	Clock frameClock;
	Clock elapsedTimeClock;
	float deltaTime;

	sf::RenderWindow window(sf::VideoMode(winWidth, winHeight), "Sarah is my habib", Style::Fullscreen);

	Solver* verletSolver = new Solver(window, deltaTime, winWidth, winHeight);
	
	sf::Event event;
	while (window.isOpen())
	{
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				window.close();
			}
			if (event.type == Event::MouseButtonPressed)
			{
				verletSolver->addParticle(Vector2f(452, 450), 20, 2, Color(20, 160, 240), Vector2f(-0.0, 0.0));
			}
		}
		float elapsedTime = elapsedTimeClock.getElapsedTime().asSeconds();
		if (elapsedTime > 0.04)
		{
			//+ ((rand() % 10) - 5), 50 + ((rand() % 10) - 5)
			verletSolver->addParticle(Vector2f(1800, 100), 8, 2, Color(rand() % 255, rand() % 255, rand() % 255), Vector2f(-1.5, 0.0));
			verletSolver->addParticle(Vector2f(1800, 118), 8, 2, Color(rand() % 255, rand() % 255, rand() % 255), Vector2f(-1.5, 0.0));
			verletSolver->addParticle(Vector2f(1800, 136), 8, 2, Color(rand() % 255, rand() % 255, rand() % 255), Vector2f(-1.5, 0.0));
			verletSolver->addParticle(Vector2f(1800, 152), 8, 2, Color(rand() % 255, rand() % 255, rand() % 255), Vector2f(-1.5, 0.0));
			elapsedTimeClock.restart();
		}

		deltaTime = frameClock.restart().asSeconds();
		framerate = 1 / deltaTime;
		//deltaTime = deltaTime > 0.1 ? 0.1 : deltaTime;

		window.clear(Color(5, 5, 10));
		verletSolver->update();

		if (frameRateUpdateClock.getElapsedTime().asSeconds() > 0.2)
		{
			std::cout << framerate << std::endl;
			fpsText.setString(std::to_string(framerate));
			frameRateUpdateClock.restart();
		}
		window.draw(fpsText);


		window.display();
	}
	return 0;
}