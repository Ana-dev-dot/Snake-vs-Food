#include "RangedEnemy.h"
#include <iostream>
#include <cmath>
using namespace sfSnake;
RangedEnemy::RangedEnemy(sf::Vector2f pos, float radius, int hp, int damage, float speed, float fireInterval)
    : hp_(hp), damage_(damage), speed_(speed), fireInterval_(fireInterval), fireTimer_(0.0f)
{
    shape_.setRadius(radius);
    shape_.setFillColor(sf::Color(0x4a90e2ff)); // 蓝色
    shape_.setOrigin(radius, radius);
    shape_.setPosition(pos);
}
void RangedEnemy::update(sf::Vector2f target, float dt, std::vector<Bullet>& bullets) {
    // 移动
    sf::Vector2f dir = target - shape_.getPosition();
    float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
    if (len > 1e-2) {
        dir /= len;
        shape_.move(dir * speed_ * dt);
    }
    // 发射子弹
    fireTimer_ += dt;
    if (fireTimer_ >= fireInterval_) {
        fireTimer_ = 0.0f;
        bullets.emplace_back(shape_.getPosition(), dir, 300.0f, damage_, BulletType::Enemy);
    }
}
void RangedEnemy::render(sf::RenderWindow& window) { window.draw(shape_); }
bool RangedEnemy::checkCollision(sf::Vector2f snakeHead, float snakeRadius) {
    float dist = std::sqrt(
        std::pow(shape_.getPosition().x - snakeHead.x, 2) +
        std::pow(shape_.getPosition().y - snakeHead.y, 2));
    return dist < (shape_.getRadius() + snakeRadius);
}
bool RangedEnemy::isDead() const { return hp_ <= 0; }
void RangedEnemy::takeDamage(int dmg) {
    hp_ -= dmg;
}
sf::Vector2f RangedEnemy::getPosition() const { return shape_.getPosition(); }
