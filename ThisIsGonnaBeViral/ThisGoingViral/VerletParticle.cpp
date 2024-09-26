#include "VerletParticle.h"

VerletParticle::VerletParticle(Vector2f pos_i, float radius_i, float mass_i, Color color_i, Vector2f vel_i, unsigned int id_i) : radius(radius_i), mass(mass_i), id(id_i)
{
	currentPos = pos_i;
	oldPos = pos_i - vel_i;

	// Initialize circleShape
	circleShape.setRadius(radius);
	circleShape.setOrigin(circleShape.getRadius(), circleShape.getRadius());
	circleShape.setPosition(currentPos);
	circleShape.setFillColor(color_i);
}

// Put things in particle class that doesn't have to do with interactions or global state of system
// also anything that is personal to a particle such as its variables and such.

void VerletParticle::update(float dt)
{
	acceleration /= mass;
	acceleration += Vector2f(0, 0.01);
	//std::cout << dt << std::endl;

	Vector2f newPos = (2.0f * currentPos - oldPos) + acceleration;

	oldPos = currentPos;
	currentPos = newPos;


	acceleration = Vector2f(0, 0);
}

void VerletParticle::display(RenderWindow& window)
{
	circleShape.setPosition(currentPos);
	window.draw(circleShape);
}

void VerletParticle::addForce(Vector2f force)
{
	acceleration += force;
}

