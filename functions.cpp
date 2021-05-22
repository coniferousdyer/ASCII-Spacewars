#include "functions.hpp"

#include <ctime>

// ASCII explosion. Made it global because I couldn't find a better place to put it :(
const char *explosion[3] = {"\\||//",
                            "--  --",
                            "//||\\"};

//-----EXTERNAL FUNCTIONS------//

// Function to delay time -> needed for Shoot()
void delay(int noOfSeconds)
{
    int milliSeconds = 1000 * noOfSeconds;
    clock_t start = clock();
    while (clock() < start + milliSeconds)
        ;
}

//-----PLAYER CLASS FUNCTIONS-----//

Player::Player(int xMax, int yMax)
{
    maxWidth = xMax - 10;
    maxHeight = yMax - 6;
    loc = xMax / 2; // loc is now in the middle of the screen
}

void Player::printSpaceship()
{
    for (int i = 0; i < 3; i++)
        mvprintw(maxHeight + i, loc - 3, spaceship[i]);
}

void Player::moveRight()
{
    // Returning if position of spaceship is on the right edge of the screen
    if (loc >= maxWidth)
        return;

    // Erasing spaceship from previous position
    for (int i = 0; i < 3; i++)
        mvprintw(maxHeight + i, loc - 3, "      ");

    // Modifying location
    loc += 2;

    // Printing spaceship in new location
    printSpaceship();
}

void Player::moveLeft()
{
    // Returning if position of spaceship is on the left edge of the screen
    if (loc <= 10)
        return;

    // Erasing spaceship from previous position
    for (int i = 0; i < 3; i++)
        mvprintw(maxHeight + i, loc - 3, "      ");

    // Modifying location
    loc -= 2;

    // Printing spaceship in new location
    printSpaceship();
}

void Player::Shoot(EnemyFleet &E)
{
    for (int i = maxHeight - 2; i >= -1; i--)
    {
        // Detecting the character at the next location of the bullet
        int ch1 = mvinch(i, loc);

        // Detecting the character adjacent to ch1 (to account for movement while bullet is travelling)
        int ch2;

        // If level is odd, check left (as fleet is moving right). 
        // If level is even, check right (as fleet is moving left).
        if (E.level[0] % 2 == 1)
            ch2 = mvinch(i, loc - 1);
        else if (E.level[0] % 2 == 0)
            ch2 = mvinch(i, loc + 1);

        // Checking for a hit
        if (ch1 == 'V' || ch1 == '|' || ch1 == '[' || ch1 == ']' || ch1 == 'W' || ch2 == 'V' || ch2 == '|' || ch2 == '[' || ch2 == ']' || ch2 == 'W')
        {
            // To keep track of which level of the fleet the hit ship is in
            int lvl;

            // To check if any level was found
            bool check = false;

            // Checking which level the hit ship was in
            for (int l = 0; l < 4; l++)
                if (i - 2 == E.level[l])
                {
                    lvl = l;
                    check = true;
                    break;
                }

            // If a level was not found, then skip the following steps
            if (!check)
                continue;

            // Checking each spaceship to see which one was hit
            for (int j = lvl * 10; j < (lvl + 1) * 10; j++)
            {
                // Checking if location of bullet is hitting the spacehship
                if (loc >= E.fleet[j].loc && loc <= E.fleet[j].loc + 5 && !E.hit[j])
                {
                    for (int k = 0; k < 3; k++)
                    {
                        // Replacing the spaceship design with spaces
                        E.fleet[j].spaceship[k] = "      ";

                        // Printing the explosion
                        mvprintw(E.level[j / 10] + k, E.fleet[j].loc, explosion[k]);
                    }

                    // Adding a slight delay so that the explosion is visible
                    refresh();
                    delay(100);

                    // Marking ship as hit
                    E.hit[j] = true;

                    // Print the fleet without the hit ship
                    E.printFleet();

                    goto hit;
                }
            }

        hit:
            // To remove the bullet if it has hit a ship
            mvprintw(i + 1, loc, " ");
            return;
        }

        // Erasing previous location of bullet
        mvprintw(i + 1, loc, " ");

        // Printing new location of bullet
        mvprintw(i, loc, "*");

        // Keeps the fleet moving even while the bullet is travelling
        moveFleet(E);

        // Refreshing screen so that bullet is visible
        refresh();

        // Causing a slight delay so that we can see movement of bullet
        delay(30);
    }
}

//------ENEMYFLEET CLASS FUNCTIONS------//

EnemyFleet::EnemyFleet(int height, int width)
{
    yMax = 1;
    xMax = width - 10;
    for (int i = 0; i < 4; i++)
        level[i] = yMax + 5 * i;

    // Initialising the location of each enemy spaceship
    for (int j = 0; j < 4; j++)
    {
        // Set the leftmost possible position to 9
        int L = 9;

        // Assigning value of loc to each spaceship
        for (int i = 10 * j; i < 10 * (j + 1); i++)
        {
            fleet[i].loc = L;
            L += 8;
        }
    }
}

void EnemyFleet::printSpaceship(int i)
{
    for (int j = 0; j < 3; j++)
        mvprintw(level[i / 10] + j, fleet[i].loc, fleet[i].spaceship[j]);
}

void EnemyFleet::printFleet()
{
    // Printing eadch spaceship in the fleet
    for (int i = 0; i < 40; i++)
        if (!hit[i])
            printSpaceship(i);
}

void EnemyFleet::shiftFleetRight()
{
    // Erasing spaceships from previous positions
    for (int i = 0; i < 40; i++)
    {
        for (int j = 0; j < 3; j++)
            mvprintw(level[i / 10] + j, fleet[i].loc, "      ");
    }

    // Modifying location of each spaceship
    for (int i = 0; i < 40; i++)
        ++fleet[i].loc;

    // Printing fleet
    printFleet();
}

void EnemyFleet::shiftFleetLeft()
{
    // Erasing spaceships from previous positions
    for (int i = 0; i < 40; i++)
    {
        for (int j = 0; j < 3; j++)
            mvprintw(level[i / 10] + j, fleet[i].loc, "      ");
    }

    // Modifying location of each spaceship
    for (int i = 0; i < 40; i++)
        --fleet[i].loc;

    // Printing fleet
    printFleet();
}

void EnemyFleet::shiftFleetDown(bool end)
{
    // Erasing spaceships from previous positions
    for (int i = 0; i < 40; i++)
    {
        for (int j = 0; j < 3; j++)
            mvprintw(level[i / 10] + j, fleet[i].loc, "      ");
    }

    for (int i = 0; i < 4; i++)
        ++level[i];

    // Explanation on line 229
    // If fleet is at leftmost position, end is false and we shift right.
    // If fleet is at rightmost position, end is true and we shift left.
    if (end)
        shiftFleetLeft();
    else
        shiftFleetRight();
}

//-----GAME FUNCTIONS-----//

void moveFleet(EnemyFleet &E)
{
    // We send a bool parameter to shiftFleetDown.
    // If fleet is at leftmost position, we send false.
    // If fleet is at rightmost position, we send true.
    if (E.fleet[0].loc == 8)
    {
        E.shiftFleetDown(false);
    }
    else if (E.fleet[39].loc + 6 == E.xMax)
    {
        E.shiftFleetDown(true);
    }

    // If level is odd, shift right. If level is even, shift left.
    if (E.level[0] % 2 == 1)
        E.shiftFleetRight();
    else if (E.level[0] % 2 == 0)
        E.shiftFleetLeft();
}

void pauseGame()
{
}

void endGame()
{
}

void winMessage()
{
}

void loseMessage()
{
}

void startGame(int height, int width)
{
    // Clearing screen
    clear();

    // Enabling the use of arrow keys
    keypad(stdscr, true);

    // Creating the player's spaceship object
    Player P(width, height);

    // Creating the enemy fleet object
    EnemyFleet E(height, width);

    P.printSpaceship();
    E.printFleet();

    scrollok(stdscr, true);
    nodelay(stdscr, true);
    curs_set(0);

    // To store the move the player makes
    int move;

    // This is the game loop
    for (;;)
    {
        // Taking user input
        move = getch();

        if (move != ERR)
        {
            switch (move)
            {
            case KEY_LEFT: // Left arrow key
                P.moveLeft();
                break;
            case KEY_RIGHT: // Right arrow key
                P.moveRight();
                break;
            case 32: // Space bar
                P.Shoot(E);
                break;
            case 'p': // p or P
            case 'P':
                pauseGame();
                break;
            case 'e': // e or E
            case 'E':
                endGame();
                goto return_to_menu;
            default:
                continue;
            }
        }

        napms(30);
        moveFleet(E);
    }

return_to_menu:;
}
