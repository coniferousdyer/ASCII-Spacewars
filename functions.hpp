#ifndef _FUNCTIONS_HPP_
#define _FUNCTIONS_HPP_

#include <ncurses.h>

#define GAME_LOST -1
#define IN_PROGRESS 0
#define GAME_WON 1

//-----ENEMY SPACESHIP-----//

// For one enemy spaceship
struct Enemy
{
    // Stores the current leftmost location of the enemy spaceship
    int loc;

    // The enemy spaceship
    const char *spaceship[3] = {"W____W",
                                "[@@@@]",
                                " V||V "};
};

// For the entire fleet of enemy spaceships
class EnemyFleet
{
public:
    // We have 40 spaceships in the enemy fleet
    Enemy fleet[40];

    // level -> Stores possible levels of the spaceships in the fleet (the row on the screen)
    // shipsDown -> Keeps track of number of ships hit
    // xMax -> maximum width the fleet can go  to to the right
    // yMax -> maximum height the fleet can go to to the bottom
    int level[4], shipsDown, yMax, xMax;

    // To check if a ship was hit. hit[i] is true if ship i was hit, else it is false
    bool hit[40] = {false};

    // Initialises the fleet of spaceships
    EnemyFleet(int height, int width);

    // To print the i-th spaceship
    void printSpaceship(int i);

    // TO print the entire fleet
    void printFleet();

    // To shift the fleet left, right or down depending on the level
    void shiftFleetRight();
    void shiftFleetLeft();
    void shiftFleetDown(bool end);
};

//-----PLAYER SPACESHIP-----//

class Player
{
    // loc -> current location of spaceship
    // maxWidth -> rightmost possible position of spaceship
    // maxHeight -> the maximum possible height of the spaceship
    int loc, maxWidth, maxHeight;

    // The spaceship
    const char *spaceship[3] = {"__||__",
                                "[####]",
                                "^^^^^^"};

public:
    // Constructor initialising the position and restrictions on the spaceship
    Player(int yMax, int xMax);

    // Prints the spaceship
    void printSpaceship();

    // Moves the ship right
    void moveRight();

    // Moves the ship left
    void moveLeft();

    // Shoots bullets from the spaceship's gun
    void Shoot(EnemyFleet &E);
};

// Function to print a menu and allow the user to choose options
void startMenu(int height, int width);

// The main function of the program -> starts the game
void startGame(int height, int width);

// Function to move the enemy fleet
void moveFleet(EnemyFleet &E);

// Pauses the game
void pauseGame();

#endif
