#include "Game.hpp"

namespace nnd3d {

Game::Game(View & _view, Sound & _sound, World & _world)
:   view(_view),
    sound(_sound),
    world(_world)
{}

void Game::destroyEverything(int how) {
    int penguin;
    int goatorg;
    if (world.Sprite[1].name == "Selecter") {
        goto dogyup;
    }
    if (world.Sprite[0].name == "GameOverCloudBg") {
        goto dogyup;
    }

    for (int j = 1; j < 15; ++ j) {
        sound.PlayWave("nothing.wav");
    }

 dogyup:

    sound.PlayIsoWave("nothing.wav");

    if (how != 2) {
        sound.PlayBgm("");
    }

    view.LoadTexture(-1, "plainblack.bmp", 25, 25);
    //bgtexture = Nothing
    //for (int j = 0; j < 9; ++ j) {
        // Set AnimationTexture(j) = Nothing
    //}
    world.gotFocus = -1;
    world.CameraX = 0;
    world.CameraY = 0;
    goatorg = 0;
    penguin = world.spritesInUse;
    //If how = 2 Then goatorg = 30: penguin = 95

    for (int j = goatorg; j <= penguin; ++ j) {
        world.Sprite[j] = CharacterSprite{};
    }
}
void Game::PlayGame() {
    // TODO: FILL IN
}

}   // end namespace
