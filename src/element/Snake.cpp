#include <SFML/Graphics.hpp>

#include <memory>
#include <iostream>

#include "element/Snake.h"
#include "Game.h"
#include "element/Fruit.h"
#include "element/Bullet.h"

#include "screen/GameOverScreen.h"

using namespace sfSnake;

// 子弹发射冷却时间（秒）
const float BulletCooldown = 0.8f;

const int Snake::InitialSize = 5;

Snake::Snake()
    : hitSelf_(false),
      speedup_(false),
      direction_(Direction(0, -1)),
      nodeRadius_(Game::GlobalVideoMode.width / 100.0f),
      tailOverlap_(0u),
      nodeShape(nodeRadius_),
      nodeMiddle(sf::Vector2f(nodeRadius_ * std::sqrt(3), nodeRadius_)),
      score_(InitialSize),
      hp_(InitialSize)
      , bulletTimer_(0.f)
{
    initNodes();


    nodeShape.setFillColor(sf::Color::White);
    nodeMiddle.setFillColor(sf::Color::White);

    setOriginMiddle(nodeShape);
    setOriginMiddle(nodeMiddle);

    headTexture.loadFromFile("assets/image/snakeHeadImage.png");
    headTexture.setSmooth(true);
    sf::Vector2u TextureSize = headTexture.getSize();
    float headScale = nodeRadius_ / TextureSize.y * 2.6;
    headSprite.setTexture(headTexture);
    headSprite.setScale(headScale, headScale);

    setOriginMiddle(headSprite);

    pickupBuffer_.loadFromFile("assets/sounds/pickup.wav");
    pickupSound_.setBuffer(pickupBuffer_);
    pickupSound_.setVolume(30);

    dieBuffer_.loadFromFile("assets/sounds/die.wav");
    dieSound_.setBuffer(dieBuffer_);
    dieSound_.setVolume(50);
}

void Snake::initNodes()
{
    path_.push_back(SnakePathNode(
        Game::GlobalVideoMode.width / 2.0f,
        Game::GlobalVideoMode.height / 2.0f));
    for (int i = 1; i <= 10 * InitialSize; i++)
    {
        path_.push_back(SnakePathNode(
            Game::GlobalVideoMode.width / 2.0f -
                direction_.x * i * nodeRadius_ / 5.0,
            Game::GlobalVideoMode.height / 2.0f -
                direction_.y * i * nodeRadius_ / 5.0));
    }
}

void Snake::handleInput(sf::RenderWindow &window)
{
    if (
        sf::Keyboard::isKeyPressed(sf::Keyboard::Up) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::W))
        direction_ = Direction(0, -1);
    else if (
        sf::Keyboard::isKeyPressed(sf::Keyboard::Down) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        direction_ = Direction(0, 1);
    else if (
        sf::Keyboard::isKeyPressed(sf::Keyboard::Left) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        direction_ = Direction(-1, 0);
    else if (
        sf::Keyboard::isKeyPressed(sf::Keyboard::Right) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        direction_ = Direction(1, 0);

    static double directionSize;

    if (!Game::mouseButtonLocked)
    {
        if (
            sf::Mouse::isButtonPressed(sf::Mouse::Left) ||
            sf::Mouse::isButtonPressed(sf::Mouse::Right))
        {
            static sf::Vector2i MousePosition;
            MousePosition = sf::Mouse::getPosition(window);
            if (
                dis(MousePosition,
                    sf::Vector2f(
                        Game::GlobalVideoMode.width / 15.0f * 14.0f,
                        Game::GlobalVideoMode.width / 15.0f)) >
                Game::GlobalVideoMode.width / 16.0f)
            {
                direction_ =
                    static_cast<sf::Vector2f>(MousePosition) -
                    toWindow(path_.front());
                directionSize = length(direction_);
                direction_.x /= directionSize;
                direction_.y /= directionSize;
            }
        }
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
        speedup_ = true;
    else
        speedup_ = false;
}

void Snake::update(sf::Time delta)
{
    move();
    static int count = 0;
    if (++count >= 40)
    {
        checkOutOfWindow();
        count -= 40;
    }
    checkSelfCollisions();

    // 子弹冷却计时
    bulletTimer_ += delta.asSeconds();
    if (bulletTimer_ >= BulletCooldown) {
        fireBullet();
        bulletTimer_ = 0.f;
    }

    // 更新所有子弹
    for (auto it = bullets_.begin(); it != bullets_.end(); ) {
        it->update(delta.asSeconds());
        // 超出窗口则移除
        if (it->isOutOfBounds(Game::GlobalVideoMode.width, Game::GlobalVideoMode.height))
            it = bullets_.erase(it);
        else
            ++it;
    }
}

void Snake::checkFruitCollisions(std::deque<Fruit> &fruits)
{
    auto toRemove = fruits.end();
    SnakePathNode headnode = path_.front();

    for (auto i = fruits.begin(); i != fruits.end(); ++i)
    {
        if (dis(
                i->shape_.getPosition(), toWindow(headnode)) <
            nodeRadius_ + i->shape_.getRadius())
            toRemove = i;
    }

    if (toRemove != fruits.end())
    {
        pickupSound_.play();
        grow(toRemove->score_);
        

        switch(toRemove->getftype()){//根据吃的果实改变蛇的颜色
        case 0://火焰效果
            nodeShape.setFillColor(sf::Color(0xff0000ff));
            nodeMiddle.setFillColor(sf::Color(0xff0000ff));
            break;
        case 1://冰霜
            nodeShape.setFillColor(sf::Color(0x0000ffff));
            nodeMiddle.setFillColor(sf::Color(0x0000ffff));
            break;
        case 2://闪电
            nodeShape.setFillColor(sf::Color(0xf1c40fff));
            nodeMiddle.setFillColor(sf::Color(0xf1c40fff));
            break;
        case 3://鬼魂
            nodeShape.setFillColor(sf::Color(0x9b59b680));
            nodeMiddle.setFillColor(sf::Color(0x9b59b680));
            break;
        case 4://暗黑
            nodeShape.setFillColor(sf::Color(0x000000ff));
            nodeMiddle.setFillColor(sf::Color(0x000000ff));
            break;
        case 5://普通
            nodeShape.setFillColor(sf::Color::White);
            nodeMiddle.setFillColor(sf::Color::White);
            break;
        default:
            break;
        }

    fruits.erase(toRemove);
    }

    
}

void Snake::grow(int score)
{
    tailOverlap_ += score * 10;
    score_ += score;
    hp_ += score; // 分数每增加一分，血量也增加
}

unsigned Snake::getScore() const
{
    return score_;
}

unsigned int Snake::getHp() const
{
    return hp_;
}

bool Snake::hitSelf() const
{
    return hitSelf_;
}

void Snake::move()
{
    SnakePathNode &headNode = path_.front();
    int times = speedup_ ? 2 : 1;
    for (int i = 1; i <= times; i++)
    {
        path_.push_front(SnakePathNode(
            headNode.x + direction_.x * i * nodeRadius_ / 5.0,
            headNode.y + direction_.y * i * nodeRadius_ / 5.0));
        if (tailOverlap_)
            tailOverlap_--;
        else
            path_.pop_back();
    }
}

void Snake::checkSelfCollisions()
{
    SnakePathNode head = toWindow(path_.front());
    int count = 0;

    for (auto i = path_.begin(); i != path_.end(); i++, count++)
        if (count >= 30 && dis(head, toWindow(*i)) < 2.0f * nodeRadius_)
        {
            dieSound_.play();
            sf::sleep(sf::seconds(dieBuffer_.getDuration().asSeconds()));
            hitSelf_ = true;
            break;
        }
}

void Snake::checkOutOfWindow()
{
    auto inWindow = [](SnakePathNode &node) -> bool
    {
        return node.x >= 0 &&
               node.x <= Game::GlobalVideoMode.width &&
               node.y >= 0 &&
               node.y <= Game::GlobalVideoMode.height;
    };
    bool OutOfWindow = true;
    for (auto i : path_)
    {
        if (inWindow(i))
            OutOfWindow = false;
    }
    if (OutOfWindow)
    {
        SnakePathNode &tail = path_.back();
        if (tail.x < 0)
            for (auto &i : path_)
                i.x = i.x + Game::GlobalVideoMode.width;
        else if (tail.x > Game::GlobalVideoMode.width)
            for (auto &i : path_)
                i.x = i.x - Game::GlobalVideoMode.width;
        else if (tail.y < 0)
            for (auto &i : path_)
                i.y = i.y + Game::GlobalVideoMode.height;
        else if (tail.y > Game::GlobalVideoMode.height)
            for (auto &i : path_)
                i.y = i.y - Game::GlobalVideoMode.height;
    }
}

SnakePathNode Snake::toWindow(SnakePathNode node) const
{
    while (node.x < 0)
        node.x = node.x + Game::GlobalVideoMode.width;
    while (node.x > Game::GlobalVideoMode.width)
        node.x = node.x - Game::GlobalVideoMode.width;
    while (node.y < 0)
        node.y = node.y + Game::GlobalVideoMode.height;
    while (node.y > Game::GlobalVideoMode.height)
        node.y = node.y - Game::GlobalVideoMode.height;
    return node;
}

void Snake::render(sf::RenderWindow &window)
{
    static int count;
    static SnakePathNode lastSnakeNode, lastMiddleNode, nowSnakeNode;
    static float angle;
    static sf::Vector2f recDirection;
    static SnakePathNode wNowHeadNode;

    lastSnakeNode = *path_.begin();
    wNowHeadNode = toWindow(lastSnakeNode);
    headSprite.setPosition(wNowHeadNode);
    recDirection = direction_;
    angle =
        std::acos(recDirection.y / length(recDirection)) /
        3.14159265358979323846 * 180.0;
    if (direction_.x > 0)
        angle = -angle;
    headSprite.setRotation(angle);

    renderNode(wNowHeadNode, headSprite, window, 3);

    count = 1;
    for (auto i = path_.begin() + 1; i != path_.end(); i++, count++)
    {
        if (count % 5 == 0)
        {
            if (count % 2)
                lastMiddleNode = *i;
            else
            {
                nowSnakeNode = *i;

                recDirection = nowSnakeNode - lastSnakeNode;
                angle =
                    std::acos(recDirection.y / length(recDirection)) /
                    3.14159265358979323846 * 180.0;
                if (recDirection.x > 0)
                    angle = -angle;
                nodeMiddle.setRotation(angle);

                static SnakePathNode wNowSnakeNode;
                static SnakePathNode wMiddleNode;
                wNowSnakeNode = toWindow(nowSnakeNode);
                wMiddleNode = toWindow(lastMiddleNode);
                renderNode(wNowSnakeNode, nodeShape, window, 0);
                renderNode(wMiddleNode, nodeMiddle, window, 0);

                lastSnakeNode = nowSnakeNode;
            }
        }
    }

    // 渲染所有子弹
    for (auto& b : bullets_)
        b.render(window);
}

// 发射子弹：从蛇头位置，朝当前方向
void Snake::fireBullet() {
    SnakePathNode head = path_.front();
    sf::Vector2f pos = toWindow(head);
    sf::Vector2f dir = direction_;
    float speed = 200.f; // 可调
    int damage = 25; // 修改子弹伤害为25，确保一发击杀
    bullets_.emplace_back(pos, dir, speed, damage, BulletType::Player);
}

template <typename T>
void Snake::renderNode(sf::Vector2f &nowPosition, T &shape, sf::RenderWindow &window, int offset)
{
    shape.setPosition(nowPosition);
    window.draw(shape);

    if (nowPosition.x <= nodeRadius_ + offset)
    {
        shape.setPosition(nowPosition + sf::Vector2f(Game::GlobalVideoMode.width, 0));
        window.draw(shape);
    }
    else if (nowPosition.x >= Game::GlobalVideoMode.width - nodeRadius_ - offset)
    {
        shape.setPosition(nowPosition - sf::Vector2f(Game::GlobalVideoMode.width, 0));
        window.draw(shape);
    }

    if (nowPosition.y <= nodeRadius_ + offset)
    {
        shape.setPosition(nowPosition + sf::Vector2f(0, Game::GlobalVideoMode.height));
        window.draw(shape);
    }
    else if (nowPosition.y >= Game::GlobalVideoMode.height - nodeRadius_ - offset)
    {
        shape.setPosition(nowPosition - sf::Vector2f(0, Game::GlobalVideoMode.height));
        window.draw(shape);
    }

    if (nowPosition.x <= nodeRadius_ + offset && nowPosition.y <= nodeRadius_ + offset)
    {
        shape.setPosition(nowPosition + sf::Vector2f(Game::GlobalVideoMode.width, Game::GlobalVideoMode.height));
        window.draw(shape);
    }
    else if (nowPosition.x >= Game::GlobalVideoMode.width - nodeRadius_ - offset && nowPosition.y >= Game::GlobalVideoMode.height - nodeRadius_ - offset)
    {
        shape.setPosition(nowPosition - sf::Vector2f(Game::GlobalVideoMode.width, Game::GlobalVideoMode.height));
        window.draw(shape);
    }
}