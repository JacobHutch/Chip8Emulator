#include "Game.h"



int main(int argc, char **argv) {
    Game game = Game(argc,argv);
    game.setup();
    game.gameLoop();
    return 0;
}
