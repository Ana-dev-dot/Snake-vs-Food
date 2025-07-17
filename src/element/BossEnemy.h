#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "Bullet.h"
namespace sfSnake {
class BossEnemy {
public:
    BossEnemy(sf::Vector2f pos, float radius, int hp, float fireInterval, float barrageInterval);
    void update(float dt, std::vector<Bullet>& bullets, sf::Vector2f snakeHead, float snakeRadius, float& snakeHp, float& bossDamageTimer);
    void render(sf::RenderWindow& window);
    bool checkCollision(sf::Vector2f snakeHead, float snakeRadius) const;
    sf::Vector2f getPosition() const;
    float getRadius() const;
    void takeDamage(int dmg) { hp_ -= dmg; }
    bool isDead() const { return hp_ <= 0; }
private:
    sf::CircleShape shape_;
    int hp_;
    float fireInterval_;      // 单发弹幕间隔
    float barrageInterval_;   // 整体弹幕间隔
    float fireTimer_;
    float barrageTimer_;
};
}
