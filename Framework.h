#ifndef FRAMEWORK_H
#define FRAMEWORK_H

#define _CRT_SECURE_NO_WARNINGS

// GLEW must be included first, if we need it.
#ifdef _WIN32
#define GLEW_STATIC
#define FRAMEWORK_USE_GLEW
#include <GL/glew.h>
#endif
#ifdef __linux__
#define FRAMEWORK_USE_GLEW
#include <GL/glew.h>
#endif

// SFML automatically includes SDL headers
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

// Open Asset Import Library
#include <assimp.hpp>
#include <aiScene.h>
#include <aiPostProcess.h>

#include <memory>
#include <iostream>
#include <sstream>
#include <string>

#include <Windows.h>

#include "Define.h"

using namespace std;

#endif
