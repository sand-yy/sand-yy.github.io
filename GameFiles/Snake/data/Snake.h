/*
Snake.h: Snake Game  Header file.
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

//Dependencias: SFML
//Compilacion para debug:
//$ g++ -g -c -Wall -Wextra -ansi -pedantic Snake.cpp
//$ g++ -g -o Snake Snake.o -lsfml-graphics -lsfml-window -lsfml-system
//Ejecucion:
//$ ./Snake

//TODO: Direccion inicial de Snake en cada nivel, en algun lugar del mapa
//poner una d, u, l, r, en ese lugar poner vacio

//TODO: Ir mostrando puntaje.

//TODO: Sonido al comer.

//TODO: Scene de transicion entre niveles

//TODO: menu

//TODO: guardar puntajes maximos

#include <vector>
#include <list>
#include <string>
#include <SFML/Graphics.hpp>

struct Coord2
{
    Coord2();
    Coord2(int x, int y);

    Coord2 operator+(const Coord2& c) const;
    bool operator==(const Coord2& c) const;

    int x;
    int y;
};

/**
 * Modelo
 */
class Level
{
public:

    Level(const std::string& file, char id);
    ~Level();

    enum Content
    {
        Food,
        Wall,
        Nothing
    };

    static const unsigned int HEIGHT = 15;
    static const unsigned int WIDHT = 15;

    Content whatHave(const Coord2& coor) const;
    Content whatHave(unsigned int x, unsigned int y) const;
    void consumeFood(const Coord2& coor);
    bool isDone() const;
    char getId() const;

    std::string getPatch() const;//deberian ser estaticas
    std::string getExt() const;

    const std::list<Coord2>& getInitialPosSnake() const;

private:
    void loadMap(const std::string& file);
    void loadCell(char value, unsigned int x, unsigned int y);

    const std::string _patch;//deberian ser estaticas
    const std::string _ext;

    const char _levelId;

    std::list<Coord2> _initialPosSnake;
    char _maxFood;
    char _currentFood;
    char _matrix[WIDHT][HEIGHT];
};

/**
 * Modelo
 */
class Snake
{
public:
    Snake(Level* level);

    void move();
    void changeDirection(const Coord2& newDir);
    bool isLive();
    unsigned int length();
    void reset();

    void setLevel(Level* level);

    void getCell(unsigned int pos, Coord2& res);

    static const Coord2 UP;
    static const Coord2 DOWN;
    static const Coord2 LEFT;
    static const Coord2 RIGHT;

private:
    void internalMove(Coord2& nextCell);

    void internalMoveWhitNothing(const Coord2& nextCell);
    void internalMoveWhitFood(const Coord2& nextCell);
    void internalMoveWhitWall(const Coord2& nextCell);

    bool autoCollision(const Coord2& nextCell);
    bool marchaAtras();

    void trans(Coord2& nextCell);

    bool _isLive;
    std::vector<Coord2> _cells;
    Coord2 _actualPos;
    Coord2 _nextPos;
    Level* _level;
};

struct Node
{
    virtual ~Node() {}
    virtual void processEvent(sf::Event& e) = 0;
    virtual void updateState()              = 0;
    virtual void draw()                     = 0;
};

class Layer : public Node
{
public:
    virtual ~Layer();

    virtual void addChild(Node* n);

    virtual void processEvent(sf::Event& e);
    virtual void updateState();
    virtual void draw();
protected:
    std::list<Node*> nodes;
};

struct Scene : public Layer
{
    virtual ~Scene() {}

    virtual void run();
    virtual void stop();
};

struct Actor : public Node
{
    virtual ~Actor() {}
};

class Director
{
public:
    static const unsigned int WIDHT_CELL  = 26;
    static const unsigned int HEIGHT_CELL = 26;

    static Director* getInstance()
    {
        static Director instance;

        return &instance;
    }

    void run();
    void showGameOver();
    void showMenu();
    void showPauseMenu();
    void showGame();

    Scene*            getScene()  const;
    Level*            getLevel()  const;
    Snake*            getSnake()  const;
    sf::RenderWindow* getWindow() const;

    void setScene(Scene* scene);
    void setLevel(Level* level);
    void setSnake(Snake* snake);
    void setWindow(sf::RenderWindow* window);

    void goToNextLevel();

    ~Director();//delete all members class
private:
    Director() {};

    Scene* _currentScene;
    sf::RenderWindow* _window;
    Level* _level;
    Snake* _snake;
};

struct Game
{
    Game();
    ~Game();
    void run();
};

struct GameScene : public Scene
{
    GameScene();
    virtual ~GameScene() {}
};

struct LevelLayer : public Layer
{
    LevelLayer() {}
    virtual ~LevelLayer() {}
    virtual void updateState();
};

struct TextGameLayer : public Layer //para los puntos, tiempo restante etc.
{
    TextGameLayer() {}
    virtual ~TextGameLayer() {}
};

struct SnakeLayer : public Layer
{
    SnakeLayer() {}
    virtual ~SnakeLayer() {}
};

class TextEndActor : public Actor
{
public:
    TextEndActor();
    virtual ~TextEndActor() {}
private:
    virtual void processEvent(sf::Event& e);
    virtual void updateState();
    virtual void draw();

    sf::String _textEnd;
    sf::Font _cheeseburger;

    Director* _director;
};


class SnakeActor : public Actor
{
public:
    SnakeActor();
    virtual ~SnakeActor() {}
private:
    virtual void processEvent(sf::Event& e);
    virtual void updateState();
    virtual void draw();

    void processKeyPressed(sf::Event& e);

    void drawCell(unsigned int x, unsigned int y);

    Director* _director;

    sf::Image _imgCellSnake;
    sf::Sprite _spriteSnake;
};

class WallActor : public Actor
{
public:
    WallActor();
    ~WallActor() {}

private:
    virtual void processEvent(sf::Event& e);
    virtual void updateState();
    virtual void draw();

    void drawCell(unsigned int x, unsigned int y);

    Director* _director;

    sf::Sprite _spriteWall;
    sf::Image _imgCellWall;
};

class FoodActor : public Actor
{
public:
    FoodActor();
    ~FoodActor() {}
private:
    virtual void processEvent(sf::Event& e);
    virtual void updateState();
    virtual void draw();

    void drawCell(unsigned int x, unsigned int y);

    Director* _director;

    sf::Sprite _spriteFood;
    sf::Image _imgFood;
};

int main();
