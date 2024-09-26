#pragma once

#include <iostream>
#include <vector>
#include <SFML/Graphics.hpp>
#include "VerletParticle.h"
#include "ErrorUtil.h"
#include "math.h"

struct Solver
{
	// Vars
	std::vector<VerletParticle> particleArray;
	unsigned int numOfParticles = 0;

	// Simulation State Vars
	float timeDilation = 0.1f;
	int substeps = 2;
	float borderCollisionDamping = 0.1;
	Vector2f gravity = Vector2f(0, 1);

	float fixedTimeStep = 1.0f / 200.0f;
	float accumulator = 0;

	// Variables from main.cpp
	float& deltaTime;
	RenderWindow& window;
	unsigned int& winWidth;
	unsigned int& winHeight;

	// Radial Constraint
	CircleShape radialConstraintShape;
	float radialConstraintRadius = 400;

	Vector2f radialContraintPos = Vector2f(960, 540);
	bool radialContraintActive = false;
	
	Solver(RenderWindow& windowRef, float& deltaTimeRef, unsigned int& winWidthRef, unsigned int& winHeightRef);

	void update();

	void borderCollision(VerletParticle& ptc);

	void resolvePtcOverlap(VerletParticle& p1, VerletParticle& p2);

	void addParticle(Vector2f pos, float radius, float mass, Color color_i, Vector2f vel_i);
};
