#include "functions.hpp"

int main()
{
    initscr();
    noecho();

    // Obtaining dimensions of screen
    int height = getmaxy(stdscr);
    int width = getmaxx(stdscr);

    // Enabling the use of arrow keys
    keypad(stdscr, true);

    // Moves to centre of screen and prints
    mvprintw(height / 2, width / 2 - 14, "Press any key to start the game.");
    refresh();
    getch();
     
    // Starts the game
    startGame(height, width);

    mvprintw(height / 2, width / 2 - 9, "Press any key to exit.");
    refresh();
    getch();
    endwin();

    return 0;
}

// TODO
//------
// Clear buffer for movement
// Fix glitches in shooting
// Display error while shooting
// Game not exiting properly