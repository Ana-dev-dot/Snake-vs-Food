#include <SFML/Graphics.hpp>

#include "Fruit.h"
#include "Game.h"

using namespace sfSnake;

Fruit::Fruit(sf::Vector2f position, sf::Color color, int score, int type)
    : score_(score), ftype_(type)
{
    shape_.setRadius(Game::GlobalVideoMode.width / 128.0f);
    setOriginMiddle(shape_);
    shape_.setPosition(position);
    shape_.setFillColor(color);
}

void Fruit::render(sf::RenderWindow &window)
{
    window.draw(shape_);
}

int Fruit::getftype()
{
    return ftype_;
}