#include "functions.hpp"

int main()
{
    // Initialises the window
    initscr();

    // To prevent getch from displaying characters on the screen
    noecho();

    // Obtaining dimensions of screen
    int height = getmaxy(stdscr);
    int width = getmaxx(stdscr);

    startMenu(height, width);

    clear();
    mvprintw(height / 2, width / 2 - 9, "Press any key to exit.");
    refresh();
    getch();

    // Ends the window
    endwin();

    return 0;
}

// TODO
//------
// Clear buffer for movement
// Fix glitches in shooting
// Display error while shooting
// Game not exiting properly