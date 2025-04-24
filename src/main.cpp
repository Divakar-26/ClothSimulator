#include "game.h"

Game game(1980, 1240);

int main(int argc, char *argv[])
{
    if (!game.init("Verlet Simulation"))
    {
        return -1;
    }

    while (game.running())
    {
        game.handleEvents();
        game.update();
        game.render();

        SDL_Delay(16); 
    }

    game.clean();

    return 0;
}