/*
Snake.cpp
    Copyright (C) 2012  Facundo Muñoz

    This file is part of Snake Game

    Snake Game is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Snake Game is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Snake Game.  If not, see <http://www.gnu.org/licenses/>.

    NOTE: This file is in prototype stage, and is under active development.
*/

#include <fstream>
#include <iostream>
#include <stdexcept>

#include "Snake.h"

/****************************************************/

Coord2::Coord2(int x, int y)
    : x(x),
      y(y)
{}

Coord2::Coord2()
{}

Coord2 Coord2::operator +(const Coord2& c) const
{
    return Coord2(c.x + x, c.y + y);
}

bool Coord2::operator ==(const Coord2& c) const
{
    return (x == c.x) && (y == c.y);
}

/****************************************************/

Level::Level(const std::string& file, char id)
    : _patch("levels/level"),
      _ext(".txt"),
      _levelId(id),
      _maxFood('2'),
      _currentFood('2')
{
    loadMap(file);
}

Level::~Level()
{}

void Level::loadCell(char value, unsigned int x, unsigned int y)
{
    if (value == 's')
    {
        _initialPosSnake.push_back(Coord2(x, y));
        value = '0';
    }

    if (_maxFood < value && value < 's')
        _maxFood = value;

    _matrix[x][y] = value;
}

void Level::loadMap(const std::string& file)
{
    char value;
    std::ifstream input(file.c_str());

    if (!input)
    {
        std::cerr << "File Not Found: " << file << std::endl;
        throw std::exception();
    }

    for (unsigned int y = 0; y < HEIGHT; y++)
        for (unsigned int x = 0; x < WIDHT; x++)
        {
            input >> value;
            loadCell(value, x, y);
        }
}

char Level::getId() const
{
    return _levelId;
}

std::string Level::getPatch() const
{
    return _patch;
}

std::string Level::getExt() const
{
    return _ext;
}

const std::list<Coord2>& Level::getInitialPosSnake() const
{
    return _initialPosSnake;
}

Level::Content Level::whatHave(const Coord2& coor) const
{
    return whatHave(coor.x, coor.y);
}

Level::Content Level::whatHave(unsigned int x, unsigned  int y) const
{
    Content content;
    char value = _matrix[x][y];

    if (value == '1')
        content = Wall;
    else if (value == _currentFood)
        content = Food;
    else
        content = Nothing;

    return content;
}

bool Level::isDone() const
{
    return _currentFood > _maxFood;
}

void Level::consumeFood(const Coord2& coor)
{
    _currentFood = ++(_matrix[coor.x][coor.y]);
    _matrix[coor.x][coor.y] = '0';
}

/****************************************************/

const Coord2 Snake::UP(0, -1);
const Coord2 Snake::DOWN(0, 1);
const Coord2 Snake::LEFT(-1, 0);
const Coord2 Snake::RIGHT(1, 0);

Snake::Snake(Level* level)
    : _isLive(true),
      _actualPos(4, 0),
      _nextPos(RIGHT),
      _level(level)
{
    reset();
}

void Snake::reset()
{
    const std::list<Coord2>& initialPos = _level->getInitialPosSnake();

    _cells.clear();
    _nextPos = _actualPos = RIGHT;

    for (std::list<Coord2>::const_iterator it = initialPos.begin(); it != initialPos.end(); it++)
        _cells.push_back(*it);
}

void Snake::internalMove(Coord2& nextCell)
{
    trans(nextCell);
    Level::Content content = _level->whatHave(nextCell);

    if (content == Level::Food)
        internalMoveWhitFood(nextCell);

    else if (content == Level::Wall)
        internalMoveWhitWall(nextCell);

    else// if (content == Level::Nothing)
        internalMoveWhitNothing(nextCell);
}

unsigned int Snake::length()
{
    return _cells.size();
}

void Snake::getCell(unsigned int pos, Coord2& res)
{
    res.x = _cells[pos].x;
    res.y = _cells[pos].y;
}

void Snake::internalMoveWhitFood(const Coord2& nextCell)
{
    _cells.push_back(nextCell);
    _level->consumeFood(nextCell);
}

void Snake::internalMoveWhitNothing(const Coord2& nextCell)
{
    for (unsigned int i = 0; i < _cells.size() - 1; i++)
        _cells[i] = _cells[i + 1];

    const unsigned int lastPos = _cells.size() - 1;
    _cells[lastPos] = nextCell;
}

void Snake::internalMoveWhitWall(const Coord2&)
{
    _isLive = false;
}

bool Snake::isLive()
{
    return _isLive;
}

bool Snake::autoCollision(const Coord2& nextCell)
{
    bool result = false;

    std::vector<Coord2>::const_iterator it = _cells.begin();

    while (it != _cells.end() && !result)
    {
        result = *it == nextCell;
        it++;
    }

    return result;
}

void Snake::trans(Coord2& nextCell)
{
    if (nextCell.x == int(Level::WIDHT))
        nextCell.x = 0;
    else if (nextCell.x == (-1))
        nextCell.x = Level::WIDHT - 1;
    else if (nextCell.y == int(Level::HEIGHT))
        nextCell.y = 0;
    else if (nextCell.y == (-1))
        nextCell.y = Level::HEIGHT - 1;
}

bool Snake::marchaAtras()
{
    bool marchaAtras = false;

    marchaAtras = marchaAtras || (_actualPos == RIGHT && _nextPos == LEFT);
    marchaAtras = marchaAtras || (_actualPos == LEFT  && _nextPos == RIGHT);
    marchaAtras = marchaAtras || (_actualPos == DOWN  && _nextPos == UP);
    marchaAtras = marchaAtras || (_actualPos == UP    && _nextPos == DOWN);

    return marchaAtras;
}

void Snake::move()
{
    bool mAtras = marchaAtras();

    Coord2 nextCell(_cells.back() + _nextPos);

    if (mAtras) //si hay marcha atras mantengo el movimiento :)
    {
        nextCell = _cells.back() + _actualPos;
        internalMove(nextCell);
    }
    else if ((_isLive = (_isLive && !autoCollision(nextCell))))
    {
        _actualPos = _nextPos;
        internalMove(nextCell);
    }
}

void Snake::changeDirection(const Coord2& newDir)
{
    _nextPos = newDir;
}

void Snake::setLevel(Level* level)
{
    _level = level;
}

/****************************************************/

//TODO: esta configuracion inicial la deberia hacer el director en un metodo
// goToSceneGame()
GameScene::GameScene()
{
    Layer* levelLayer    = new LevelLayer();
    Layer* snakeLayer    = new SnakeLayer();
    Layer* textGameLayer = new TextGameLayer();

    levelLayer->addChild(new WallActor());
    levelLayer->addChild(new FoodActor());

    snakeLayer->addChild(new SnakeActor());

    textGameLayer->addChild(new TextEndActor());

    addChild(levelLayer);
    addChild(snakeLayer);
    addChild(textGameLayer);
}

/****************************************************/

void Layer::addChild(Node* node)
{
    nodes.push_back(node);
}

void Layer::processEvent(sf::Event& e)
{
    for (std::list<Node*>::iterator it = nodes.begin(); it != nodes.end(); it++)
        (*it)->processEvent(e);
}

void Layer::updateState()
{
    for (std::list<Node*>::iterator it = nodes.begin(); it != nodes.end(); it++)
        (*it)->updateState();
}

void Layer::draw()
{
    for (std::list<Node*>::iterator it = nodes.begin(); it != nodes.end(); it++)
        (*it)->draw();
}

Layer::~Layer()
{
    for (std::list<Node*>::iterator it = nodes.begin(); it != nodes.end(); it++)
        delete *it;

}

/****************************************************/

void LevelLayer::updateState()
{
    Layer::updateState();

    Director* director = Director::getInstance();
    Level* level = director->getLevel();

    if (level->isDone())
        director->goToNextLevel();
}

/****************************************************/

void Scene::run()
{}

void Scene::stop()
{}

/****************************************************/

void Director::run()
{
    _currentScene->run();
}

void Director::showGameOver()
{
}

void Director::showMenu()
{
}

void Director::showPauseMenu()
{
}

void Director::showGame()
{
}

Scene* Director::getScene()  const
{
    return _currentScene;
}

Level* Director::getLevel()  const
{
    return _level;
}

Snake* Director::getSnake()  const
{
    return _snake;
}

sf::RenderWindow* Director::getWindow() const
{
    return _window;
}

void Director::setScene(Scene* scene)
{
    _currentScene = scene;
}

void Director::setLevel(Level* level)
{
    delete _level;
    _level = level;
}

void Director::setSnake(Snake* snake)
{
    _snake = snake;
}

void Director::setWindow(sf::RenderWindow* window)
{
    _window = window;
}

void Director::goToNextLevel()
{
    char nextLevelId = _level->getId();
    nextLevelId++;

    std::string map = "";
    map += _level->getPatch();
    map += nextLevelId;
    map += _level->getExt();

    delete _level;

    _level = new Level(map, nextLevelId);
    _snake->setLevel(_level);
    _snake->reset();
}

Director::~Director()
{
    delete _snake;
    delete _level;
    delete _window;
    delete _currentScene;
}

/****************************************************/

TextEndActor::TextEndActor()
{
    _director = Director::getInstance();

    if (!_cheeseburger.LoadFromFile("font/cheeseburger.ttf"))
    {
        std::cerr << "File Not Found: cheeseburger.ttf" << std::endl;
        throw std::exception();
    }

    _textEnd.SetFont(_cheeseburger);
    _textEnd.SetSize(30.f);
    _textEnd.SetColor(sf::Color(255, 255, 255));
    _textEnd.SetPosition(65, 165);

    const std::string text = "     Game Over!     \n"
                             "(press escape to exit)";

    _textEnd.SetText(text);
}

void TextEndActor::processEvent(sf::Event&)
{}

void TextEndActor::updateState()
{}

void TextEndActor::draw()
{
    Snake* snake = _director->getSnake();
    sf::RenderWindow* window = _director->getWindow();

    if (!snake->isLive())
        window->Draw(_textEnd);
}

/****************************************************/

SnakeActor::SnakeActor()
{
    _director = Director::getInstance();

    _imgCellSnake.LoadFromFile("images/snake.png");
    _spriteSnake.SetImage(_imgCellSnake);
}

void SnakeActor::processEvent(sf::Event& e)
{
    if (e.Type == sf::Event::KeyPressed)
        processKeyPressed(e);
}

void SnakeActor::processKeyPressed(sf::Event& e)
{
    Snake* snake  = _director->getSnake();

    switch (e.Key.Code)
    {
        case sf::Key::A :
        case sf::Key::Left :
            snake->changeDirection(Snake::LEFT);
            break;

        case sf::Key::D :
        case sf::Key::Right :
            snake->changeDirection(Snake::RIGHT);
            break;

        case sf::Key::W :
        case sf::Key::Up :
            snake->changeDirection(Snake::UP);
            break;

        case sf::Key::S :
        case sf::Key::Down :
            snake->changeDirection(Snake::DOWN);
            break;

        default:
            break;
    }
}

void SnakeActor::updateState()
{
    static float tiempoAcumulado = 0;

    Snake* snake = _director->getSnake();
    sf::RenderWindow* window = _director->getWindow();

    tiempoAcumulado += window->GetFrameTime();

    if (tiempoAcumulado >= 0.2)
    {
        tiempoAcumulado -= 0.2;
        snake->move();
    }
}

void SnakeActor::draw()
{
    Coord2 pos;
    Snake* snake = _director->getSnake();

    for (unsigned int i = 0; i < snake->length(); i++)
    {
        snake->getCell(i, pos);
        drawCell(pos.x, pos.y);
    }
}

void SnakeActor::drawCell(unsigned int x, unsigned int y)
{
    x *= Director::WIDHT_CELL;
    y *= Director::HEIGHT_CELL;

    _spriteSnake.SetPosition(x, y);
    _director->getWindow()->Draw(_spriteSnake);
}

/****************************************************/

WallActor::WallActor()
{
    _director = Director::getInstance();

    _imgCellWall.LoadFromFile("images/wall.png");

    _spriteWall.SetImage(_imgCellWall);
}

void WallActor::processEvent(sf::Event&)
{}

void WallActor::updateState()
{}

void WallActor::drawCell(unsigned int x, unsigned int y)
{
    Level* level = _director->getLevel();
    sf::RenderWindow* window = _director->getWindow();

    Level::Content content = level->whatHave(x, y);

    x *= Director::WIDHT_CELL;
    y *= Director::HEIGHT_CELL;

    if (content == Level::Wall)
    {
        _spriteWall.SetPosition(x, y);
        window->Draw(_spriteWall);
    }
}

void WallActor::draw()
{
    for (unsigned int y = 0; y < Level::HEIGHT; y++)
        for (unsigned int x = 0; x < Level::WIDHT; x++)
            drawCell(x, y);
}
/****************************************************/
FoodActor::FoodActor()
{
    _director = Director::getInstance();

    _imgFood.LoadFromFile("images/food.png");
    _spriteFood.SetImage(_imgFood);
}

void FoodActor::processEvent(sf::Event&)
{}

void FoodActor::updateState()
{}

void FoodActor::drawCell(unsigned int x, unsigned int y)
{
    Level* level = _director->getLevel();
    sf::RenderWindow* window = _director->getWindow();

    Level::Content content = level->whatHave(x, y);

    x *= Director::WIDHT_CELL;
    y *= Director::HEIGHT_CELL;

    if (content == Level::Food)
    {
        _spriteFood.SetPosition(x, y);
        window->Draw(_spriteFood);
    }
}

void FoodActor::draw()
{
    for (unsigned int y = 0; y < Level::HEIGHT; y++)
        for (unsigned int x = 0; x < Level::WIDHT; x++)
            drawCell(x, y);
}

/****************************************************/

Game::Game()
{
    Director* director = Director::getInstance();

    Level* level = new Level("levels/level1.txt", '1');

    director->setLevel(level);
    director->setSnake(new Snake(level));

    director->setWindow(new sf::RenderWindow(sf::VideoMode(390, 390), "Snake"));
    director->setScene(new GameScene());
}

//TODO: modularizar
void Game::run()
{
    Director* director = Director::getInstance();
    sf::RenderWindow* window = director->getWindow();
    Scene* scene = NULL;

    sf::Event e;

    while (window->IsOpened())
    {
        scene = director->getScene();

        while (window->GetEvent(e))
        {
            scene->processEvent(e);

            if (e.Type == sf::Event::KeyPressed && e.Key.Code == sf::Key::Escape)
                window->Close();
            else if (e.Type == sf::Event::Closed)
                window->Close();
        }

        scene->updateState();

        window->Clear(sf::Color(0, 0, 0, 255));
        scene->draw();
        window->Display();
    }
}

Game::~Game()
{}

/****************************************************/

int main()
{
    Game g;

    g.run();

    return EXIT_SUCCESS;
}
