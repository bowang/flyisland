#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H

#include "Framework.h"

class Root;

enum SoundType{
    Background,
    Fire,
    Hit,
    Crash,
    Success,
    Fail
};

class SoundManager{

public:
    SoundManager(Root* root);

    void loadAssets();
    void play(SoundType type);
    void setVolumn(float volumn);
    void loop(SoundType type, bool loop);

private:
    Root* root;
    float volumn;
    sf::SoundBuffer FireBuffer;
    sf::SoundBuffer HitBuffer;
    sf::SoundBuffer CrashBuffer;

    sf::Sound FireSound;
    sf::Sound HitSound;
    sf::Sound CrashSound;
    sf::Music BackgroundMusic;

protected:


};

#endif
