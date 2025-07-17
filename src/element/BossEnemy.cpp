#include "BossEnemy.h"
#include <cmath>
// 修正M_PI未定义
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
using namespace sfSnake;
BossEnemy::BossEnemy(sf::Vector2f pos, float radius, int hp, float fireInterval, float barrageInterval)
    : hp_(hp), fireInterval_(fireInterval), barrageInterval_(barrageInterval), fireTimer_(0.0f), barrageTimer_(0.0f)
{
    shape_.setRadius(radius);
    shape_.setFillColor(sf::Color(0x8e44adff)); // 紫色
    shape_.setOrigin(radius, radius);
    shape_.setPosition(pos);
}
void BossEnemy::update(float dt, std::vector<Bullet>& bullets, sf::Vector2f snakeHead, float snakeRadius, float& snakeHp, float& bossDamageTimer) {
    // 持续碰撞掉血逻辑由GameScreen外部处理
    // 弹幕发射
    barrageTimer_ += dt;
    if (barrageTimer_ >= barrageInterval_) {
        barrageTimer_ = 0.0f;
        // 环形弹幕
        int bulletCount = 16;
        float angleStep = 2 * M_PI / bulletCount;
        float speed = 220.0f;
        for (int i = 0; i < bulletCount; ++i) {
            float angle = i * angleStep;
            sf::Vector2f dir(std::cos(angle), std::sin(angle));
            bullets.emplace_back(shape_.getPosition(), dir, speed, 3, BulletType::Enemy);
        }
    }
}
void BossEnemy::render(sf::RenderWindow& window) { window.draw(shape_); }
bool BossEnemy::checkCollision(sf::Vector2f snakeHead, float snakeRadius) const {
    float dist = std::sqrt(
        std::pow(shape_.getPosition().x - snakeHead.x, 2) +
        std::pow(shape_.getPosition().y - snakeHead.y, 2));
    return dist < (shape_.getRadius() + snakeRadius);
}
sf::Vector2f BossEnemy::getPosition() const { return shape_.getPosition(); }
float BossEnemy::getRadius() const { return shape_.getRadius(); }
