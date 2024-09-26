#include "Solver.h"

Solver::Solver(RenderWindow& windowRef, float& deltaTimeRef, unsigned int& winWidthRef, unsigned int& winHeightRef) : window(windowRef), deltaTime(deltaTimeRef), winWidth(winWidthRef), winHeight(winHeightRef)
{
	// Initialize Circular Contraint Shape
	radialConstraintShape.setRadius(radialConstraintRadius);
	radialConstraintShape.setPosition(radialContraintPos - Vector2f(radialConstraintRadius, radialConstraintRadius));
	radialConstraintShape.setPointCount(60);
	radialConstraintShape.setFillColor(Color(30, 30, 30));
}

void Solver::update()
{
	accumulator += deltaTime;
	//while (accumulator > fixedTimeStep) 
	//{
		// Update Physics
		for (int s = 0; s < substeps; s++)
		{
			float deltaTimeSub = deltaTime / substeps;
			for (VerletParticle& ptc : particleArray)
			{
				//ptc.addForce(Vector2f(0.0, 0.1));
				borderCollision(ptc);
				ptc.update(deltaTime * timeDilation);
			}

			for (int i = 0; i < numOfParticles; i++)
			{
				for (int j = 0; j < numOfParticles; j++)
				{
					if (i != j)
						resolvePtcOverlap(particleArray[i], particleArray[j]);
				}
			}
		}
		//accumulator -= fixedTimeStep;
	//}

	// Rendering
	if (radialContraintActive)
	{
		window.draw(radialConstraintShape);
		radialConstraintRadius += -0.0001;
		radialConstraintShape.setRadius(radialConstraintRadius);
		radialConstraintShape.setPosition(radialContraintPos - Vector2f(radialConstraintRadius, radialConstraintRadius));
	}
	for (VerletParticle& ptc : particleArray)
	{
		ptc.display(window);
	}
	// TODO: add springs
	// TODO: Add collisions with sparse grid
	// TODO: Figure out how to implement multithreading
	// TODO: Draw Particles in one call
}

void Solver::resolvePtcOverlap(VerletParticle& p1, VerletParticle& p2)
{
	Vector2f diffVector = p1.currentPos - p2.currentPos;
	float correctDist = p1.radius + p2.radius;
	float currentDist = mag(diffVector);

	if (currentDist < correctDist)
	{
		float overlapAmount = currentDist - correctDist;
		float resolveDist = (overlapAmount / correctDist) / 2;
		float sumOfMass = p1.mass + p2.mass;
		float mass_ratio = p1.mass / p2.mass;

		p1.currentPos += ((diffVector / currentDist) * (overlapAmount / mass_ratio) * 0.5f) * -1.0f;
		p2.currentPos += ((diffVector / currentDist) * (overlapAmount * mass_ratio) * 0.5f) * 1.0f;
	}
}

void Solver::borderCollision(VerletParticle& ptc)
{
	float vel_x = ptc.currentPos.x - ptc.oldPos.x;
	float vel_y = ptc.currentPos.y - ptc.oldPos.y;
	if (ptc.currentPos.y > winHeight - ptc.radius)
	{
		ptc.currentPos.y = winHeight - ptc.radius;
		ptc.oldPos.y = ptc.currentPos.y + vel_y * borderCollisionDamping;
	}
	else if (ptc.currentPos.y < ptc.radius)
	{
		ptc.currentPos.y = ptc.radius;
		ptc.oldPos.y = ptc.currentPos.y + vel_y * borderCollisionDamping;
	}
	if (ptc.currentPos.x > winWidth - ptc.radius)
	{
		ptc.currentPos.x = winWidth - ptc.radius;
		ptc.oldPos.x = ptc.currentPos.x + vel_x * borderCollisionDamping;
	}
	else if (ptc.currentPos.x < ptc.radius)
	{
		ptc.currentPos.x = ptc.radius;
		ptc.oldPos.x = ptc.currentPos.x + vel_x * borderCollisionDamping;
	}

	// Radial Constraint
	if (radialContraintActive)
	{
		Vector2f diffVec = ptc.currentPos - radialContraintPos;
		if (mag(diffVec) > radialConstraintRadius - ptc.radius)
		{
			float ratio = (radialConstraintRadius - ptc.radius) / mag(diffVec);
			ptc.currentPos = radialContraintPos + (diffVec * ratio);
		}
	}
}

void Solver::addParticle(Vector2f pos_i, float radius_i, float mass_i, Color color_i, Vector2f vel_i)
{
	particleArray.push_back(VerletParticle(pos_i, radius_i, mass_i, color_i, vel_i, numOfParticles));
	numOfParticles++;
}