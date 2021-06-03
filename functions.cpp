#include "functions.hpp"

#include <ctime>
#include <string>

// Global variable deciding the state of the game: whether in progress or whether the game is over
static short int state;

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

//-----MESSAGE FUNCTIONS-----//

void loseMessage()
{
    delay(2000);

    // Clear screen
    clear();

    // Obtaining the dimensions of the screen
    int height = getmaxy(stdscr);
    int width = getmaxx(stdscr);

    mvprintw(height / 2 - 4, width / 2 - 28, "               YOU HAVE FAILED TO SAVE EARTH.");
    refresh();
    delay(2000);
    mvprintw(height / 2 - 2, width / 2 - 28, "The people of Earth were counting on you. You let them down.");
    refresh();
    delay(2000);
    mvprintw(height / 2, width / 2 - 28, "           The Krueldronites have destroyed Earth. ");
    refresh();
    delay(2000);
    mvprintw(height / 2 + 2, width / 2 - 28, "                     MISSION: FAILURE");
    refresh();
    delay(3000);

    // Sets state to GAME_LOST so that we break out of game loop
    state = GAME_LOST;
}

void winMessage()
{
    delay(2000);

    // Clear screen
    clear();

    // Obtaining the dimensions of the screen
    int height = getmaxy(stdscr);
    int width = getmaxx(stdscr);

    mvprintw(height / 2 - 4, width / 2 - 30, "                   YOU HAVE SAVED EARTH.");
    refresh();
    delay(2000);
    mvprintw(height / 2 - 2, width / 2 - 30, "The people of Earth were counting on you. You did not let them down.");
    refresh();
    delay(2000);
    mvprintw(height / 2, width / 2 - 30, "             The Krueldronites have fled Earth.");
    refresh();
    delay(2000);
    mvprintw(height / 2 + 2, width / 2 - 30, "                    MISSION: SUCCESS");
    refresh();
    delay(3000);

    // Sets state to GAME_WON so that we break out of game loop
    state = GAME_WON;
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

void Player::Shoot(Player &P, EnemyFleet &E)
{
    // move -> for player movement
    // currentLoc -> stores the current location of player ship so that bullet travels along that path
    int move, currentLoc = loc;

    for (int i = maxHeight - 2; i >= -1 && !state; i--)
    {
        // Detecting the character at the next location of the bullet
        int ch1 = mvinch(i, currentLoc);

        // Detecting the character adjacent to ch1 (to account for movement while bullet is travelling)
        int ch2;

        // If level is odd, check left (as fleet is moving right).
        // If level is even, check right (as fleet is moving left).
        if (E.level[0] % 2 == 1)
            ch2 = mvinch(i, currentLoc - 1);
        else if (E.level[0] % 2 == 0)
            ch2 = mvinch(i, currentLoc + 1);

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

            // Incrementing the number of hit ships
            ++E.shipsDown;

            // Printing the updated score
            mvprintw(getmaxy(stdscr) - 1, getmaxx(stdscr) - 16, "Ships Left: %d ", 40 - E.shipsDown);

            // Checking each spaceship to see which one was hit
            for (int j = lvl * 10; j < (lvl + 1) * 10; j++)
            {
                // Checking if location of bullet is hitting the spacehship
                if (currentLoc >= E.fleet[j].loc && currentLoc <= E.fleet[j].loc + 5 && !E.hit[j])
                {
                    for (int k = 0; k < 3; k++)
                    {
                        // Printing the explosion
                        mvprintw(E.level[j / 10] + k, E.fleet[j].loc, explosion[k]);
                    }

                    // Adding a slight delay so that the explosion is visible
                    refresh();
                    delay(30);

                    // Marking ship as hit
                    E.hit[j] = true;

                    // Print the fleet without the hit ship
                    E.printFleet();

                    goto hit;
                }
            }

        hit:
            // To remove the bullet if it has hit a ship
            mvprintw(i + 1, currentLoc, " ");

            // To prevent input buffering if ship was hit
            flushinp();

            // Checking if all ships were hit
            if (E.shipsDown == 40)
                winMessage();

            return;
        }

        // Erasing previous location of bullet
        mvprintw(i + 1, currentLoc, " ");

        // Printing new location of bullet
        mvprintw(i, currentLoc, "*");

        // Keeps the fleet moving even while the bullet is travelling
        moveFleet(E);

        // Refreshing screen so that bullet is visible
        refresh();

        // Causing a slight delay so that we can see movement of bullet
        delay(30);

        // Enabling player movement while bullet is travelling
        move = getch();

        // Checking if user pressed any key
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
            case 'p': // p or P
            case 'P':
                pauseGame();
                break;
            case 'e': // e or E
            case 'E':
                // The game will end once the program flow returns to startMenu()
                state = GAME_ENDED;
                goto return_to_menu; // Goes to line 269
            default:;
            }
        }
    }

return_to_menu:;
    // To prevent input buffering if ship was not hit
    flushinp();
}

//------ENEMYFLEET CLASS FUNCTIONS------//

EnemyFleet::EnemyFleet(int height, int width)
{
    // Setting number of ships hit to 0
    shipsDown = 0;

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
    // Checking if the fleet has reached the bottom position
    if (level[3] == getmaxy(stdscr) - 10)
        loseMessage();

    // Erasing spaceships from previous positions
    for (int i = 0; i < 40; i++)
    {
        for (int j = 0; j < 3; j++)
            mvprintw(level[i / 10] + j, fleet[i].loc, "      ");
    }

    for (int i = 0; i < 4; i++)
        ++level[i];

    // Printing the updated distance from bottom
    mvprintw(getmaxy(stdscr) - 1, getmaxx(stdscr) - 40, "Distance left: %d ", getmaxy(stdscr) - 10 - level[3]);

    // Explanation on line 229
    // If fleet is at leftmost position, end is false and we shift right.
    // If fleet is at rightmost position, end is true and we shift left.
    if (end)
        shiftFleetLeft();
    else
        shiftFleetRight();
}

//-----GAME FUNCTIONS-----//

// Displays the title of the game
void gameTitle(int height, int width)
{
    mvwprintw(stdscr, height / 2 - 11, width / 2 - 34, "------------------------------------------------------------------");
    mvwprintw(stdscr, height / 2 - 10, width / 2 - 34, "| ##### ####### ####### ##### ###### ## # ## ###### ###### ##### |");
    mvwprintw(stdscr, height / 2 - 9, width / 2 - 34, "| ##    ##   ## ##   ## ##    ##     ## # ## ##  ## ##   # ##    |");
    mvwprintw(stdscr, height / 2 - 8, width / 2 - 34, "| ##### ####### ####### ##    ###### ## # ## ###### ###### ##### |");
    mvwprintw(stdscr, height / 2 - 7, width / 2 - 34, "|    ## ##      ##   ## ##    ##     ### ### ##  ## ## ##     ## |");
    mvwprintw(stdscr, height / 2 - 6, width / 2 - 34, "| ##### ##      ##   ## ##### ###### ### ### ##  ## ##  ## ##### |");
    mvwprintw(stdscr, height / 2 - 5, width / 2 - 34, "|                                                                |");
    mvwprintw(stdscr, height / 2 - 4, width / 2 - 34, "--------------------By Arjun Muraleedharan------------------------");
}

// Displays the story intro to the game
void gameIntro(int height, int width)
{
    mvprintw(height / 2 - 4, width / 2 - 29, "Aliens from the planet of Krueldron are about to invade Earth.");
    refresh();
    delay(2000);
    mvprintw(height / 2 - 2, width / 2 - 29, "   They have managed to destroy all of Earth's spaceships.");
    refresh();
    delay(2000);
    mvprintw(height / 2, width / 2 - 29, "   You are the only pilot left. You are Earth's last hope.");
    refresh();
    delay(2000);
    mvprintw(height / 2 + 2, width / 2 - 29, "   Your mission is to destroy all Krueldronite spaceships.");
    refresh();
    delay(2000);
}

void ControlsAndRules(int height, int width)
{
    clear();

    // Displaying controls
    mvprintw(height / 2 - 6, width / 2 - 18, "             CONTROLS");
    mvprintw(height / 2 - 4, width / 2 - 18, " Press LEFT arrow key to move left.");
    mvprintw(height / 2 - 3, width / 2 - 18, "Press RIGHT arrow key to move right.");
    mvprintw(height / 2 - 2, width / 2 - 18, " Press SPACE key to shoot bullets.");

    // Displaying rules
    mvprintw(height / 2, width / 2 - 39, "                                   RULES");
    mvprintw(height / 2 + 2, width / 2 - 39, "Your mission is to shoot down all the enemy ships before they reach the bottom.");
    mvprintw(height / 2 + 3, width / 2 - 39, "                     If you fail to do so, you lose.");
    mvprintw(height / 2 + 4, width / 2 - 39, "                 If you shoot them all down in time, you win.");

    mvprintw(height / 2 + 9, width / 2 - 20, "Press any key to return to start menu.");

    // Flushing input buffer
    flushinp();
    refresh();

    // Waiting for user input
    getch();
}

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
    int ch;

    // Flushing input buffer
    flushinp();

    // Replaces Pause with Press any key to resume
    mvprintw(getmaxy(stdscr) - 1, 2, "Press any key to resume");
    refresh();

    // Delaying infinitely until user enters input
    while ((ch = getch()) == ERR)
        delay(1);

    // Checking if user entered E or e to exit. If so, setting game state to GAME_ENDED
    if (ch == 'e' || ch == 'E')
        state = GAME_ENDED;

    // Replaces Press any key to resume with Pause
    mvprintw(getmaxy(stdscr) - 1, 2, "P: Pause               ");
    refresh();
}

void startGame(int height, int width)
{
    // Setting state of game to IN_PROGRESS
    state = IN_PROGRESS;

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

    mvprintw(height - 1, 2, "P: Pause                    E: Exit");
    mvprintw(height - 1, width - 16, "Ships Left: %d ", 40 - E.shipsDown);
    mvprintw(height - 1, width - 40, "Distance left: %d ", height - 10 - E.level[3]);

    scrollok(stdscr, true);
    nodelay(stdscr, true);

    // To turn blinking cursor off
    curs_set(0);

    // To store the move the player makes
    int move;

    // This is the game loop
    while (state == IN_PROGRESS)
    {
        // Taking user input
        move = getch();

        if (move != ERR)
        {
            switch (move)
            {
            case KEY_LEFT:  // Left arrow key
                flushinp(); // To prevent input buffering
                P.moveLeft();
                break;
            case KEY_RIGHT: // Right arrow key
                flushinp(); // To prevent input buffering
                P.moveRight();
                break;
            case 32: // Space bar
                P.Shoot(P, E);
                break;
            case 'p': // p or P
            case 'P':
                pauseGame();
                break;
            case 'e': // e or E
            case 'E':
                goto return_to_menu; // Jumps to line 509
            default:
                continue;
            }
        }

        if (!state)
        {
            napms(30);
            moveFleet(E);
        }
    }

return_to_menu:
    scrollok(stdscr, false);
    nodelay(stdscr, false);
}

void startMenu(int height, int width)
{
    clear();

    // Enabling the use of arrow keys
    keypad(stdscr, true);

    std::string options[3] = {"Start Game", "Controls and Rules", "Exit"};

    int chosen_option, highlight = 0;

    while (true)
    {
        // Clears the whole screen instead of just the menu window
        clear();

        gameTitle(height, width);

        // Traversing through the options to check which one is highlighted
        for (int i = 0; i < 3; i++)
            if (i == highlight)
            {
                // Turn on highlight
                wattron(stdscr, A_REVERSE);
                mvprintw(i + height / 2, width / 2 - 8, options[i].c_str());
                // Turn off highlight
                wattroff(stdscr, A_REVERSE);
            }
            else
                mvprintw(i + height / 2, width / 2 - 8, options[i].c_str());

        chosen_option = getch();

        switch (chosen_option)
        {
        case KEY_UP: // UP arrow key
            if (highlight == 0)
                break;
            --highlight;
            break;
        case KEY_DOWN: // DOWN arrow key
            if (highlight == 2)
                break;
            ++highlight;
            break;
        default:;
        }

        // If user presses Enter
        if (chosen_option == 10)
        {
            switch (highlight)
            {
            case 0:
            {
                // Clears screen
                clear();

                // Prints the intro to the game
                gameIntro(height, width);

                // Flushes input buffer
                flushinp();

                mvprintw(height / 2 + 4, width / 2 - 29, "             Press any key to start the game.");
                refresh();
                getch();

                // Starts the game
                startGame(height, width);
            }
            break;
            case 1:
                ControlsAndRules(height, width);
                break;
            case 2:
                goto return_to_start_menu; // Exiting and returning to start menu (line 596)
                break;
            }
        }
    }

return_to_start_menu:;
}
