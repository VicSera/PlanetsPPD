//
// Created by vsera on 1/5/2022.
//

#include "physics.h"

Planet::Planet(float x, float y, float radius, float density, float speedX, float speedY)
        : x(x), y(y), radius(radius), oldX(x), oldY(y), speed({speedX, speedY})
{
    this->mass = PI * radius * radius * density;
}

Planet::Planet()
        : x(0), y(0), radius(0), oldX(0), oldY(0), speed({0, 0})
{
    this->mass = 0;
}

void Planet::setNewPos(float newX, float newY)
{
    oldX = x;
    oldY = y;
    x = newX;
    y = newY;
}

Force::Force(float dirX, float dirY, float magnitude) : dirX(dirX), dirY(dirY), magnitude(magnitude) {}

Force::Force(): dirX(0), dirY(0), magnitude(0) {}

Vector2 computeAcceleration(const Planet& p, const Force& f)
{
    const auto a = f.magnitude / p.mass;
    return {f.dirX * a, f.dirY * a};
}

void applyForce(Planet& p, const Force& f, float seconds, int horizontalBorder, int verticalBorder)
{
    auto acc = computeAcceleration(p, f);
    p.speed = {p.speed.x + acc.x * seconds, p.speed.y + acc.y * seconds};
    float newX = p.x + p.speed.x, newY = p.y + p.speed.y;
    if (newX - p.radius <= 0 || newX + p.radius >= (float)horizontalBorder)
    {
        p.speed.x *= -0.5f;
        newX = p.x + p.speed.x;
    }
    if (newY - p.radius <= 0 || newY + p.radius >= (float)verticalBorder)
    {
        p.speed.y *= -0.5f;
        newY = p.y + p.speed.y;
    }
    p.setNewPos(newX, newY);
}

// p1 attracts p2, so the force is directed towards p1 from p2
Force computeAttraction(const Planet& p1, const Planet& p2)
{
    const auto deltaX = p1.x - p2.x;
    const auto deltaY = p1.y - p2.y;

    const auto rSquared = deltaX * deltaX + deltaY * deltaY;
    const auto magnitude = G * p1.mass * p2.mass / std::max(rSquared, p1.radius + p2.radius);

    return {deltaX / sqrtf(rSquared), deltaY / sqrtf(rSquared), magnitude};
}

Force addForces(const std::vector<Force>& forces)
{
    float x = .0f, y = .0f;

    // add up forces
    for (auto force : forces)
    {
        x += force.dirX * force.magnitude;
        y += force.dirY * force.magnitude;
    }

    // normalize force
    const auto magnitude = sqrtf(x * x + y * y);
    x /= magnitude;
    y /= magnitude;

    return {x, y, magnitude};
}

Force computeGravity(const std::vector<Planet>& planets, int planetNumber)
{
    std::vector<Force> forces;
    forces.reserve(planets.size() - 1);
    for (auto i = 0; i < planets.size(); ++i)
        if (i != planetNumber)
            forces.push_back(computeAttraction(planets[i], planets[planetNumber]));

    return addForces(forces);
}

bool Force::check(const Force& other) const
{
    return magnitude == other.magnitude && dirX == other.dirX && dirY == other.dirY;
}

void Force::print() const
{
    std::cout << "(" << dirX << ", " << dirY << ", " << magnitude << ")";
}