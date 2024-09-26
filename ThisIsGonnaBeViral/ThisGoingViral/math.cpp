#include "math.h"
#include <cmath>

sf::Vector2f addVect(sf::Vector2f v1, sf::Vector2f v2)
{
    float x = v1.x + v2.x;
    float y = v1.y + v2.y;

    return sf::Vector2f(x, y);
}

sf::Vector2f multVectConst(sf::Vector2f vec, float multiplier)
{
    float a = vec.x;
    float b = vec.y;

    a *= multiplier;
    b *= multiplier;

    return sf::Vector2f(a, b);
}

float mag(sf::Vector2f vec)
{
    float a = vec.x;
    float b = vec.y;

    a *= a;
    b *= b;

    return sqrtf(a + b);
}



sf::Vector2f normalize(sf::Vector2f vec)
{
    float vec_mag = mag(vec);
    float vec_x = vec.x / vec_mag;
    float vec_y = vec.y / vec_mag;
    sf::Vector2f finalVec = sf::Vector2f(vec_x, vec_y);
    return finalVec;
}

float dot(sf::Vector2f v1, sf::Vector2f v2)
{
    return v1.x * v2.x + v1.y * v2.y;
}

// void print(auto var)
// {
//    std::cout << var << std::endl;
// }

sf::Vector2f rotateVector(sf::Vector2f vec, float amt)
{
    float x = vec.x * cos(amt) - vec.y * sin(amt);
    float y = vec.x * sin(amt) + vec.y * cos(amt);
    sf::Vector2f rotatedVector = sf::Vector2f(x, y);
    return rotatedVector;
}
