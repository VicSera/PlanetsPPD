#ifndef PLANETS_PHYSICS_H
#define PLANETS_PHYSICS_H

#include <windows.h>
#include <cmath>
#include <vector>
#include <iostream>

#define PI 3.14f
#define G 0.0001f

struct Vector2
{
    float x, y;
};

struct Planet
{
    float x, y;
    float oldX, oldY;
    float radius;
    float mass;
    Vector2 speed;

    Planet(float x, float y, float radius, float density, float speedX, float speedY);
    Planet();

    void setNewPos(float newX, float newY);
};

struct Force
{
    float magnitude, dirX, dirY;

    Force(float dirX, float dirY, float magnitude);

    Force();

    [[nodiscard]] bool check(const Force& other) const;

    void print() const;
};

Vector2 computeAcceleration(const Planet& p, const Force& f);

void applyForce(Planet& p, const Force& f, float seconds, int horizontalBorder, int verticalBorder);

Force computeAttraction(const Planet& p1, const Planet& p2);

Force addForces(const std::vector<Force>& forces);

Force computeGravity(const std::vector<Planet>& planets, int planetNumber);

#endif //PLANETS_PHYSICS_H
