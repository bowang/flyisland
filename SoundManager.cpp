#include "SoundManager.h"
#include "Root.h"

SoundManager::SoundManager(Root* root)
{
    this->root = root;
    volumn = 15.f;
}



void SoundManager::loadAssets()
{
    char fileName[BUFFER_SIZE];
    char filePath[2*BUFFER_SIZE];

    // background music
    GetPrivateProfileString("Music", "Background", "", fileName, BUFFER_SIZE, root->mConfigFileName.c_str());
    strcpy(filePath, root->mRootPath.c_str());    strcat(filePath, fileName);
    if(BackgroundMusic.OpenFromFile(filePath)){
        printf("[SoundManager] Background music opened: %s\n", filePath);
    }
    else{
        printf("Failed to open background music: %s\n", filePath);
    }

    // fire music
    GetPrivateProfileString("Music", "Fire", "", fileName, BUFFER_SIZE, root->mConfigFileName.c_str());
    strcpy(filePath, root->mRootPath.c_str());    strcat(filePath, fileName);
    if(FireBuffer.LoadFromFile(filePath)){
        printf("[SoundManager] Fire sound loaded: %s\n", filePath);
    }
    else{
        printf("Failed to load fire sound: %s\n", filePath);
    }
    FireSound.SetBuffer(FireBuffer);

    // hit music
    GetPrivateProfileString("Music", "Hit", "", fileName, BUFFER_SIZE, root->mConfigFileName.c_str());
    strcpy(filePath, root->mRootPath.c_str());    strcat(filePath, fileName);
    if(HitBuffer.LoadFromFile(filePath)){
        printf("[SoundManager] Hit sound loaded: %s\n", filePath);
    }
    else{
        printf("Failed to load hit sound: %s\n", filePath);
    }
    HitSound.SetBuffer(HitBuffer);

    // crash music
    GetPrivateProfileString("Music", "Crash", "", fileName, BUFFER_SIZE, root->mConfigFileName.c_str());
    strcpy(filePath, root->mRootPath.c_str());    strcat(filePath, fileName);
    if(CrashBuffer.LoadFromFile(filePath)){
        printf("[SoundManager] Crash sound loaded: %s\n", filePath);
    }
    else{
        printf("Failed to load crash sound: %s\n", filePath);
    }
    CrashSound.SetBuffer(CrashBuffer);

    // sea music
    GetPrivateProfileString("Music", "Sea", "", fileName, BUFFER_SIZE, root->mConfigFileName.c_str());
    strcpy(filePath, root->mRootPath.c_str());    strcat(filePath, fileName);
    if(SeaMusic.OpenFromFile(filePath)){
        printf("[SoundManager] Sea music opened: %s\n", filePath);
    }
    else{
        printf("Failed to open sea music: %s\n", filePath);
    }

}



void SoundManager::play(SoundType type)
{
    if(!root->mEnableMusic) return;

    switch(type){
    case Background:
        BackgroundMusic.SetVolume(volumn);
        BackgroundMusic.Play();
        break;
    case Fire:
        FireSound.SetVolume(volumn);
        FireSound.Play();
        break;
    case Hit:
        HitSound.SetVolume(volumn);
        HitSound.Play();
        break;
    case Crash:
        CrashSound.SetVolume(volumn);
        CrashSound.Play();
        break;
    case Sea:
        SeaMusic.SetVolume(volumn*0.7f);
        SeaMusic.Play();
        break;
    default:
        break;
    }

}

void SoundManager::loop(SoundType type, bool loop)
{
    switch(type){
    case Background:
        BackgroundMusic.SetLoop(loop);
        break;
    case Fire:
        FireSound.SetLoop(loop);
        break;
    case Hit:
        HitSound.SetLoop(loop);
        break;
    case Crash:
        CrashSound.SetLoop(loop);
        break;
    default:
        break;
    }
}

void SoundManager::setVolumn(float v)
{
    volumn = v;
}