#ifndef MATH_HPP
#define MATH_HPP

#include <SFML/Graphics.hpp>

sf::Vector2f addVect(sf::Vector2f v1, sf::Vector2f v2);
sf::Vector2f multVectConst(sf::Vector2f vec, float multiplier);
float dot(sf::Vector2f v1, sf::Vector2f v2);
float mag(sf::Vector2f vec);
sf::Vector2f normalize(sf::Vector2f vec);
sf::Vector2f rotateVector(sf::Vector2f vec, float amt);
// void print(auto var);

#endif // MATH_HPP
