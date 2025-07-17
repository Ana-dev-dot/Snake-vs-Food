#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include <vector>
#include <deque>

#include "Bullet.h"

#include "Fruit.h"

namespace sfSnake
{
    typedef sf::Vector2f Direction;
    typedef sf::Vector2f SnakePathNode;

    class Snake
    {
    public:
        Snake();

        void handleInput(sf::RenderWindow &window);
        void update(sf::Time delta);
        void render(sf::RenderWindow &window);

        // 子弹相关
        void fireBullet();
        const std::vector<Bullet>& getBullets() const { return bullets_; }

        void checkFruitCollisions(std::deque<Fruit> &fruits);

        bool hitSelf() const;

        unsigned getScore() const;

        void printhead() const;
        unsigned int getHp() const;
        void reduceHp(unsigned int damage) { if (hp_ > damage) hp_ -= damage; else hp_ = 0; }
        float getNodeRadius() const { return nodeRadius_; }
        sf::Vector2f getHeadPosition() const { return toWindow(path_.front()); }
    public:
        void grow(int score);

    private:
        void initNodes();

        // void grow(int score); // 移除私有声明
        void move();

        void checkOutOfWindow();
        void checkSelfCollisions();

        template <typename T>
        void renderNode(sf::Vector2f &nowPosition, T &shape, sf::RenderWindow &window, int offset);

        SnakePathNode toWindow(SnakePathNode node) const;
        bool hitSelf_;
        bool speedup_;

        Direction direction_;
        float nodeRadius_;
        std::deque<SnakePathNode> path_;
        unsigned tailOverlap_;

        sf::CircleShape nodeShape;
        sf::RectangleShape nodeMiddle;
        sf::Texture headTexture;
        sf::Sprite headSprite;

        static const int InitialSize;
        unsigned int score_;
        // 新增：血量属性，初始血量与长度挂钩
        unsigned int hp_;        

        sf::SoundBuffer pickupBuffer_;
        sf::Sound pickupSound_;

        sf::SoundBuffer dieBuffer_;
        sf::Sound dieSound_;

        // 子弹相关
        std::vector<Bullet> bullets_;
        float bulletTimer_ = 0.f;
    };
}