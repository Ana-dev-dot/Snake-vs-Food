#pragma once
#include <SFML/Graphics.hpp>
namespace sfSnake {
enum class BulletType { Player, Enemy };

class Bullet {
public:
    Bullet(sf::Vector2f pos, sf::Vector2f dir, float speed, int damage, BulletType type = BulletType::Player);
    void update(float dt);
    void render(sf::RenderWindow& window);
    bool checkCollision(sf::Vector2f target, float radius) const;
    bool isOutOfBounds(float width, float height) const;
    int getDamage() const { return damage_; }
    BulletType getType() const { return type_; }
    sf::Vector2f getPosition() const { return shape_.getPosition(); }
private:
    sf::CircleShape shape_;
    sf::Vector2f direction_;
    float speed_;
    int damage_;
    BulletType type_;
};
}
