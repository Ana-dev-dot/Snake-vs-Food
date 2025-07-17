#pragma once
#include <SFML/Graphics.hpp>
namespace sfSnake {
    class Enemy {
    public:
        Enemy(sf::Vector2f pos, float radius, int hp, int damage, float speed);
        void update(sf::Vector2f target); // 向蛇头移动
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
    };
}