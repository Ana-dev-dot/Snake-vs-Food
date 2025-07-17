#include "Enemy.h"
#include <cmath>
#include <iostream>

using namespace sfSnake;
Enemy::Enemy(sf::Vector2f pos, float radius, int hp, int damage, float speed)
    : hp_(hp), damage_(damage), speed_(speed)
{
    shape_.setRadius(radius);
    shape_.setFillColor(sf::Color(0xcf871dff)); // 橙色
    shape_.setOrigin(radius, radius);
    shape_.setPosition(pos);
}
void Enemy::update(sf::Vector2f target) {
    sf::Vector2f dir = target - shape_.getPosition();
    float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
    if (len > 1e-2) {
        dir /= len;
        shape_.move(dir * speed_);
    }
}
void Enemy::render(sf::RenderWindow& window) { window.draw(shape_); }
bool Enemy::checkCollision(sf::Vector2f snakeHead, float snakeRadius) {
    float dist = std::sqrt(
        std::pow(shape_.getPosition().x - snakeHead.x, 2) +
        std::pow(shape_.getPosition().y - snakeHead.y, 2));
    return dist < (shape_.getRadius() + snakeRadius);
}
bool Enemy::isDead() const { return hp_ <= 0; }
void Enemy::takeDamage(int dmg) {
    hp_ -= dmg;
}
sf::Vector2f Enemy::getPosition() const { return shape_.getPosition(); }