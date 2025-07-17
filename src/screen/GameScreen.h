#pragma once

#include <SFML/Graphics.hpp>
#include <deque>
#include <vector>

#include "screen/Screen.h"
#include "element/Snake.h"
#include "element/Fruit.h"
#include "element/Grid.h"
#include "element/Button.h"
#include "element/Enemy.h" // Include Enemy header
#include "element/RangedEnemy.h"
#include "element/Bullet.h"
#include "element/BossEnemy.h"

namespace sfSnake
{
    class GameScreen : public Screen
    {
    public:
        GameScreen();

        void handleInput(sf::RenderWindow &window) override;
        void update(sf::Time delta) override;
        void render(sf::RenderWindow &window) override;

        void generateFruit();

    private:
        Snake snake_;
        std::deque<Fruit> fruit_;
        Grid grid_;
        Button pauseButton_;
        sf::Text score_;
        sf::Text hp_;
        std::vector<Enemy> enemies_;
        float enemySpawnTimer_ = 0.0f;
        // 远程小怪和子弹
        std::vector<RangedEnemy> rangedEnemies_;
        float rangedEnemySpawnTimer_ = 0.0f;
        std::vector<Bullet> bullets_;
        // Boss相关
        std::unique_ptr<BossEnemy> boss_;
        float bossSpawnTimer_ = 0.0f;
        float bossDamageTimer_ = 0.0f;
    };
}