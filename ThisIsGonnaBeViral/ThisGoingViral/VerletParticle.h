#pragma once

#include <iostream>
#include <SFML/Graphics.hpp>

using namespace sf;

struct VerletParticle {
	unsigned int id;
	float mass;
	float radius;
	Vector2f pos;

	Vector2f currentPos;
	Vector2f oldPos;
	Vector2f acceleration;

	CircleShape circleShape;

	VerletParticle(Vector2f pos_i, float radius_i, float mass_i, Color color_i, Vector2f vel_i, unsigned int id_i);

	void update(float dt);

	void display(RenderWindow& window);

	void addForce(Vector2f force);
};