#include "core/engine.hpp"

int main(int argc, const char** argv)
{
    Engine game(1280, 720, "floating");
    game.start();

    return 0;
}
