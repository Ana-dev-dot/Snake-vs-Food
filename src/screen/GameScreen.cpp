#include <SFML/Graphics.hpp>

#include <random>
#include <memory>
#include <iostream>

#include "screen/GameScreen.h"
#include "screen/GameOverScreen.h"
#include "screen/PauseScreen.h"
#include "screen/HelpScreen.h"
#include "element/enemy.h"
#include "element/RangedEnemy.h"
#include "element/Bullet.h"

using namespace sfSnake;

GameScreen::GameScreen()
    : snake_(), grid_(), pauseButton_(), enemySpawnTimer_(0.0f)
{
    pauseButton_.update("assets/image/pauseUI.png", 1 / 16.0f);
    pauseButton_.setPosition(
        Game::GlobalVideoMode.width / 15.0 * 14.0,
        Game::GlobalVideoMode.width / 15.0);

    score_.setFont(Game::GlobalFont);
    score_.setString(sf::String(L"分数:") + std::to_string(snake_.getScore()));
    score_.setCharacterSize(Game::GlobalVideoMode.width / 25.0f);
    score_.setFillColor(Game::Color::Yellow);
    setOriginMiddle(score_);
    score_.setPosition(
        Game::GlobalVideoMode.width / 2.0f,
        Game::GlobalVideoMode.width * 0.05f);

    hp_.setFont(Game::GlobalFont);
    hp_.setCharacterSize(Game::GlobalVideoMode.width / 25.0f);
    hp_.setFillColor(Game::Color::Red);
    setOriginMiddle(hp_);
    hp_.setPosition(
        Game::GlobalVideoMode.width / 2.0f,
        score_.getPosition().y + score_.getCharacterSize() + 5.0f
    );    
}

void GameScreen::handleInput(sf::RenderWindow &window)
{
    snake_.handleInput(window);

    auto mousePosition = sf::Mouse::getPosition(window);

    pauseButton_.focused(false);

    if (pauseButton_.contain(mousePosition))
    {
        pauseButton_.focused(true);
        if (
            !Game::mouseButtonLocked &&
            sf::Mouse::isButtonPressed(sf::Mouse::Left))
        {
            Game::mouseButtonCDtime = sf::Time::Zero;
            Game::mouseButtonLocked = true;
            Game::TmpGameScreen = Game::MainScreen;
            Game::MainScreen = std::make_shared<PauseScreen>();
            return;
        }
    }
}

void GameScreen::update(sf::Time delta)
{
    while (fruit_.size() < 10)
        generateFruit();

    snake_.update(delta);
    snake_.checkFruitCollisions(fruit_);

    // 敌人移动与碰撞检测
    for (auto& enemy : enemies_)
        enemy.update(snake_.getHeadPosition());
    for (auto it = enemies_.begin(); it != enemies_.end(); ) {
        if (it->checkCollision(snake_.getHeadPosition(), snake_.getNodeRadius())) {
            snake_.reduceHp(it->getDamage());
            it = enemies_.erase(it);
        } else if (it->isDead()) {
            snake_.grow(3); // 击杀敌人增加10分
            it = enemies_.erase(it);
        } else {
            ++it;
        }
    }

    // 远程小怪移动、发射子弹
    for (auto& renemy : rangedEnemies_)
        renemy.update(snake_.getHeadPosition(), delta.asSeconds(), bullets_);
    for (auto it = rangedEnemies_.begin(); it != rangedEnemies_.end(); ) {
        if (it->checkCollision(snake_.getHeadPosition(), snake_.getNodeRadius())) {
            snake_.reduceHp(it->getDamage());
            it = rangedEnemies_.erase(it);
        } else if (it->isDead()) {
            snake_.grow(3);        
            it = rangedEnemies_.erase(it);
        } else {
            ++it;
        }
    }

    // 处理敌人子弹移动与碰撞
    for (auto it = bullets_.begin(); it != bullets_.end(); ) {
        it->update(delta.asSeconds());
        bool erased = false;
        if (it->getType() == BulletType::Enemy && it->checkCollision(snake_.getHeadPosition(), snake_.getNodeRadius())) {
            snake_.reduceHp(it->getDamage());
            it = bullets_.erase(it);
            erased = true;
        }
        if (!erased && it->isOutOfBounds(Game::GlobalVideoMode.width, Game::GlobalVideoMode.height)) {
            it = bullets_.erase(it);
        } else if (!erased) {
            ++it;
        }
    }

    // 处理蛇子弹移动与碰撞
    auto& playerBullets = const_cast<std::vector<Bullet>&>(snake_.getBullets());
    for (auto it = playerBullets.begin(); it != playerBullets.end(); ) {
        it->update(delta.asSeconds());
        bool erased = false;

        for (auto enemyIt = enemies_.begin(); enemyIt != enemies_.end(); ++enemyIt) {
            if (it->checkCollision(enemyIt->getPosition(), enemyIt->getRadius())) {
                enemyIt->takeDamage(it->getDamage());
                it = playerBullets.erase(it);
                erased = true;
                break;
            }
        }
        if (!erased) {
            for (auto renemyIt = rangedEnemies_.begin(); renemyIt != rangedEnemies_.end(); ++renemyIt) {
                if (it->checkCollision(renemyIt->getPosition(), renemyIt->getRadius())) {
                    renemyIt->takeDamage(it->getDamage());
                    it = playerBullets.erase(it);
                    erased = true;
                    break;
                }
            }
        }
        if (!erased && boss_) {
            if (it->checkCollision(boss_->getPosition(), boss_->getRadius())) {
                boss_->takeDamage(it->getDamage());
                it = playerBullets.erase(it);
                erased = true;
            }
        }
        // Boss死亡判定
    if (boss_ && boss_->isDead()) {
        snake_.grow(20);
        boss_.reset();
    }
        if (!erased) {
            ++it;
        }
    }

    // Boss生成与更新
    bossSpawnTimer_ += delta.asSeconds();
    if (!boss_ && bossSpawnTimer_ >= 60.0f) {
        bossSpawnTimer_ = 0.0f;
        float radius = snake_.getNodeRadius() * 4.0f;
        int hp = 100; // Boss初始血量 
        float fireInterval = 0.0f; // 单发弹幕间隔（未用）
        float barrageInterval = 2.5f; // 每2.5秒发射一次弹幕
        float x = Game::GlobalVideoMode.width / 2.0f;
        float y = Game::GlobalVideoMode.height / 2.0f;
        boss_ = std::make_unique<BossEnemy>(sf::Vector2f(x, y), radius, hp, fireInterval, barrageInterval);
        bossDamageTimer_ = 0.0f;
    }
    if (boss_) {
        boss_->update(delta.asSeconds(), bullets_, snake_.getHeadPosition(), snake_.getNodeRadius(), /*snakeHp*/bossDamageTimer_, bossDamageTimer_);
        // 蛇头碰撞boss持续掉血
        if (boss_->checkCollision(snake_.getHeadPosition(), snake_.getNodeRadius())) {
            bossDamageTimer_ += delta.asSeconds();
            if (bossDamageTimer_ >= 1.0f) {
                snake_.reduceHp(5);
                bossDamageTimer_ = 0.0f;
            }
        } else {
            bossDamageTimer_ = 0.0f;
        }
    }
    // 近战敌人定时生成逻辑
    enemySpawnTimer_ += delta.asSeconds();
    if (enemySpawnTimer_ >= 10.0f && enemies_.size() < 2) {
        enemySpawnTimer_ = 0.0f;
        float radius = snake_.getNodeRadius() * 2.0f;
        int hp = 1; // 调低敌人血量为1
        int damage = 5;
        float speed = 2.0f;
        float x = static_cast<float>(rand()) / RAND_MAX * Game::GlobalVideoMode.width;
        float y = static_cast<float>(rand()) / RAND_MAX * Game::GlobalVideoMode.height;
        enemies_.push_back(Enemy(sf::Vector2f(x, y), radius, hp, damage, speed));
    }
    // 远程敌人定时生成逻辑
    rangedEnemySpawnTimer_ += delta.asSeconds();
    if (rangedEnemySpawnTimer_ >= 10.0f && rangedEnemies_.size() < 1) {
        rangedEnemySpawnTimer_ = 0.0f;
        float radius = snake_.getNodeRadius() * 2.0f;
        int hp = 5;
        int damage = 2;
        float speed = 1.2f;
        float fireInterval = 1.0f;
        float x = static_cast<float>(rand()) / RAND_MAX * Game::GlobalVideoMode.width;
        float y = static_cast<float>(rand()) / RAND_MAX * Game::GlobalVideoMode.height;
        rangedEnemies_.push_back(RangedEnemy(sf::Vector2f(x, y), radius, hp, damage, speed, fireInterval));
    }

    if (snake_.hitSelf())
    {
        Game::MainScreen = std::make_shared<GameOverScreen>(snake_.getScore());
        return;
    }
        if (snake_.getHp() == 0)
    {
        Game::MainScreen = std::make_shared<GameOverScreen>(snake_.getScore());
        return;
    }

    score_.setString(sf::String(L"分数:\t") + std::to_string(snake_.getScore()));
    hp_.setString(sf::String(L"血量:") + std::to_string(snake_.getHp()));    
}

void GameScreen::render(sf::RenderWindow &window)
{
    if (Game::GridVisibility)
        grid_.render(window);
    snake_.render(window);
    for (auto fruit : fruit_)
        fruit.render(window);
    for (auto& enemy : enemies_)
        enemy.render(window);
    for (auto& renemy : rangedEnemies_)
        renemy.render(window);
    for (auto& bullet : bullets_)
        bullet.render(window);
    if (boss_)
        boss_->render(window);
    pauseButton_.render(window);
    window.draw(score_);
    window.draw(hp_);    
}

void GameScreen::generateFruit()
{
    static std::default_random_engine engine(time(NULL));
    static std::default_random_engine colorEngine(time(NULL));

    static std::uniform_int_distribution<int> xPos(
        Game::GlobalVideoMode.width / 15.0f,
        Game::GlobalVideoMode.width -
            Game::GlobalVideoMode.width / 10.0f);

    static std::uniform_int_distribution<int> yPos(
        Game::GlobalVideoMode.width / 10.0f,
        Game::GlobalVideoMode.height -
            Game::GlobalVideoMode.width / 15.0f);

    static std::uniform_int_distribution<int> fruitColor(0, 30);

    switch (fruitColor(colorEngine))
    {
    case 0: // black
        fruit_.push_back(Fruit(
            sf::Vector2f(xPos(engine), yPos(engine)),
            Game::Color::Fruit[0],
            1,4));
        break;
    case 1: // yellow
        fruit_.push_back(Fruit(
            sf::Vector2f(xPos(engine), yPos(engine)),
            Game::Color::Fruit[1],
            1,2));
        break;
    case 2:
    case 3: // red
        fruit_.push_back(Fruit(
            sf::Vector2f(xPos(engine), yPos(engine)),
            Game::Color::Fruit[2],
            1,0));
        break;
    case 4:
    case 5: // blue
        fruit_.push_back(Fruit(
            sf::Vector2f(xPos(engine), yPos(engine)),
            Game::Color::Fruit[3],
            1,1));
        break;
    case 6:
    case 7: // purple
        fruit_.push_back(Fruit(
            sf::Vector2f(xPos(engine), yPos(engine)),
            Game::Color::Fruit[4],
            1,3));
        break;
    default:
        fruit_.push_back(Fruit(
            sf::Vector2f(xPos(engine), yPos(engine)),
            Game::Color::Fruit[5],
            1,5));    
        break;
    }
}