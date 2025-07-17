#include "Bullet.h"
#include <cmath>
using namespace sfSnake;
Bullet::Bullet(sf::Vector2f pos, sf::Vector2f dir, float speed, int damage, BulletType type)
    : direction_(dir), speed_(speed), damage_(damage), type_(type)
{
    shape_.setRadius(6.0f);
    shape_.setFillColor(sf::Color::Cyan);
    shape_.setOrigin(6.0f, 6.0f);
    shape_.setPosition(pos);
    float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
    if (len > 1e-2) direction_ /= len;
}
void Bullet::update(float dt) {
    shape_.move(direction_ * speed_ * dt);
}
void Bullet::render(sf::RenderWindow& window) {
    window.draw(shape_);
}
bool Bullet::checkCollision(sf::Vector2f target, float radius) const {
    float dx = shape_.getPosition().x - target.x;
    float dy = shape_.getPosition().y - target.y;
    float dist = std::sqrt(dx * dx + dy * dy);
    return dist < (shape_.getRadius() + radius);
}
bool Bullet::isOutOfBounds(float width, float height) const {
    auto p = shape_.getPosition();
    return p.x < 0 || p.x > width || p.y < 0 || p.y > height;
}
