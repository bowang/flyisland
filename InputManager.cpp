#include "InputManager.h"
#include "Root.h"

extern sf::RenderWindow window;

InputManager::InputManager(Root* root)
{
    this->root = root;
    delta = GetPrivateProfileFloat("Input", "delta", 1.0f, root->mConfigFileName.c_str());
    mouseCaptureEnabled = false;
}

void InputManager::handleInput()
{
    sf::Event evt;
    while (window.GetEvent(evt)) {
        switch (evt.Type) {
        case sf::Event::Closed: 
            window.Close(); 
            break;
        case sf::Event::Resized: 
            glViewport(0, 0, evt.Size.Width, evt.Size.Height);
            break;
        case sf::Event::KeyPressed:
            switch (evt.Key.Code) {
            case sf::Key::Escape:
                exit(0);
                break;
            default:
                break;
            }
            break;
        case sf::Event::MouseButtonPressed:
            v = root->target - root->eye;
            lenxz = sqrt(v.x*v.x+v.z*v.z);
            lenxy = sqrt(v.x*v.x+v.y*v.y);
            yaw = asin(v.z/lenxz);
            pitch = - asin(v.y/lenxy);
            mouseCaptureEnabled = true;
            break;
        case sf::Event::MouseButtonReleased:
            mouseCaptureEnabled = false;
            break;
        default: 
            break;
        }
    }

    const sf::Input& input = window.GetInput();

    if(input.IsKeyDown(sf::Key::A)){
        aiVector3D direction = root->target - root->eye;
        aiVector3D d = cross(root->up, direction);
        d.Normalize();
        d *= delta;
        root->target += d;
        root->eye += d;
    }
    if(input.IsKeyDown(sf::Key::D)){
        aiVector3D direction = root->target - root->eye;
        aiVector3D d = cross(root->up, direction);
        d.Normalize();
        d *= delta;
        root->target -= d;
        root->eye -= d;
    }
    if(input.IsKeyDown(sf::Key::W)){
        aiVector3D d = root->target - root->eye;
        d.Normalize();
        d *= delta;
        root->target += d;
        root->eye += d;
    }
    if(input.IsKeyDown(sf::Key::S)){
        aiVector3D d = root->target - root->eye;
        d.Normalize();
        d *= delta;
        root->target -= d;
        root->eye -= d;
    }

    if(mouseCaptureEnabled){
        mouseX = input.GetMouseX();
        mouseY = input.GetMouseY();
        float dyaw = 2*Pi*fabs(v.x)/v.x*float(mouseX - mouseX0)/window.GetWidth();
        float dpitch = 2*Pi*float(mouseY - mouseY0)/window.GetHeight();
        yaw += dyaw;
        pitch += dpitch;
        root->target.x = root->eye.x + fabs(v.x)/v.x*lenxz*cos(yaw);
        root->target.y = root->eye.y - lenxy*sin(pitch);
        root->target.z = root->eye.z + lenxz*sin(yaw);
        mouseX0 = mouseX;
        mouseY0 = mouseY;
    }
    else {
        mouseX0  = input.GetMouseX();
        mouseY0  = input.GetMouseY();
    }
}

