#include "Framework.h"
#include "Shader.h"
#include "Root.h"

sf::WindowSettings settings(24, 8, 2);
sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "FlyIsland", sf::Style::Close, settings);

Root* root;

int main(int argc, char** argv) {

    GLint error = glewInit();
    if (GLEW_OK != error) {
        std::cerr << glewGetErrorString(error) << std::endl;
        exit(-1);
    }

    if(argc > 1)
        root = new Root(argv[1]);
    else
        root = new Root(CONFIG_FILENAME);

    root->run();

    return 0;
}
