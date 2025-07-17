#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "Bullet.h"
namespace sfSnake {
class RangedEnemy {
public:
    RangedEnemy(sf::Vector2f pos, float radius, int hp, int damage, float speed, float fireInterval);
    void update(sf::Vector2f target, float dt, std::vector<Bullet>& bullets);
    void render(sf::RenderWindow& window);
    bool checkCollision(sf::Vector2f snakeHead, float snakeRadius);
    bool isDead() const;
    void takeDamage(int dmg);
    sf::Vector2f getPosition() const;
    int getDamage() const { return damage_; }
    float getRadius() const { return shape_.getRadius(); }
private:
    sf::CircleShape shape_;
    int hp_;
    int damage_;
    float speed_;
    float fireInterval_;
    float fireTimer_;
};
}
