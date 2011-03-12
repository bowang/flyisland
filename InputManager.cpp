#include "InputManager.h"
#include "Root.h"

extern sf::RenderWindow window;

InputManager::InputManager(Root* root)
{
    this->root = root;
    delta  = GetPrivateProfileFloat("Input", "delta", 1.0f, root->mConfigFileName.c_str());
    dPitch = GetPrivateProfileFloat("Input", "dPitch", 0.0f, root->mConfigFileName.c_str());
    dYaw   = GetPrivateProfileFloat("Input", "dYaw", 0.0f, root->mConfigFileName.c_str());
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
            case sf::Key::C:
                root->viewMode = (enum ViewMode)((int(root->viewMode)+1)%3);
                if(root->viewMode==FlyView)
                    printf("CameraMode: FlyView\n");
                else if(root->viewMode==FollowView)
                    printf("CameraMode: FollowView\n");
                else
                    printf("CameraMode: FreeView\n");
                break;
            case sf::Key::Dash:
                root->mSceneManager->flySpeed -= 0.004f;
                printf("Flying speed = %d\n", (int)(root->mSceneManager->flySpeed*1000.f));
                break;
            case sf::Key::Equal:
                root->mSceneManager->flySpeed += 0.004f;
                printf("Flying speed = %d\n", (int)(root->mSceneManager->flySpeed*1000.f));
                break;
            case sf::Key::Space:
                root->mSceneManager->genCannonParticle();
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

    // free viewer
    if(root->viewMode==FreeView){
        if(input.IsKeyDown(sf::Key::Left)){
            aiVector3D direction = root->target - root->eye;
            aiVector3D d = cross(root->up, direction);
            d.Normalize();
            d *= delta;
            root->target += d;
            root->eye += d;
        }
        if(input.IsKeyDown(sf::Key::Right)){
            aiVector3D direction = root->target - root->eye;
            aiVector3D d = cross(root->up, direction);
            d.Normalize();
            d *= delta;
            root->target -= d;
            root->eye -= d;
        }
        if(input.IsKeyDown(sf::Key::Up)){
            aiVector3D d = root->target - root->eye;
            d.Normalize();
            d *= delta;
            root->target += d;
            root->eye += d;
        }
        if(input.IsKeyDown(sf::Key::Down)){
            aiVector3D d = root->target - root->eye;
            d.Normalize();
            d *= delta;
            root->target -= d;
            root->eye -= d;
        }
    }
    // airplane mode
    else if(root->viewMode==FollowView){
        if(input.IsKeyDown(sf::Key::Left)){
            aiVector3D direction = root->target - root->eye;
            aiVector3D d = cross(root->up, direction);
            if(d.Length()){
                d.Normalize();
                d *= delta;
                root->eye += d;
            }
        }
        if(input.IsKeyDown(sf::Key::Right)){
            aiVector3D direction = root->target - root->eye;
            aiVector3D d = cross(root->up, direction);
            if(d.Length()){
                d.Normalize();
                d *= delta;
                root->eye -= d;
            }
        }
        if(input.IsKeyDown(sf::Key::Up)){
            aiVector3D d = root->target - root->eye;
            if(d.Length()){
                d.Normalize();
                d *= delta;
                root->eye += d;
            }
        }
        if(input.IsKeyDown(sf::Key::Down)){
            aiVector3D d = root->target - root->eye;
            if(d.Length()){
                d.Normalize();
                d *= delta;
                root->eye -= d;
            }
        }
    }
    else if(root->viewMode==FlyView){
        // nothing need to be done, camera is fixed behind the plane
    }

    if(input.IsKeyDown(sf::Key::W)){
        aiVector3D v = root->mSceneManager->flyDirection - root->airplane->mPosition;
        if(v.Length() > 0.00001f) v.Normalize();
        aiVector3D h = aiVector3D(v.x, 0.0f, v.z);
        if(h.Length() > 0.00001f) h.Normalize();
        float pitch = 0.0f;
        if(v.y != 0.0f)
            pitch = fabs(v.y)/v.y*acos(dot(v,h))/Pi*180.0f;
        if(pitch > -60.0f)
            pitch -= dPitch;
        pitch = pitch/180.0f*Pi;
        float yaw = 0.0f;
        if(v.z != 0.0f)
            yaw = fabs(v.z)/v.z*acos(dot(h,aiVector3D(1.f,0.f,0.f)))/Pi*180.0f;
        yaw = yaw/180.0f*Pi;
        v.y = sin(pitch);
        float lenxz = cos(pitch);
        v.x = lenxz * cos(yaw);
        v.z = lenxz * sin(yaw);
        root->mSceneManager->flyDirection = root->airplane->mPosition + v;
    }

    if(input.IsKeyDown(sf::Key::S)){
        aiVector3D v = root->mSceneManager->flyDirection - root->airplane->mPosition;
        if(v.Length() > 0.00001f) v.Normalize();
        aiVector3D h = aiVector3D(v.x, 0.0f, v.z);
        if(h.Length() > 0.00001f) h.Normalize();
        float pitch = 0.0f;
        if(v.y != 0.0f)
            pitch = fabs(v.y)/v.y*acos(dot(v,h))/Pi*180.0f;
        if(pitch < 60.0f)
            pitch += dPitch;
        pitch = pitch/180.0f*Pi;
        float yaw = 0.0f;
        if(v.z != 0.0f)
            yaw = fabs(v.z)/v.z*acos(dot(h,aiVector3D(1.f,0.f,0.f)))/Pi*180.0f;
        yaw = yaw/180.0f*Pi;
        v.y = sin(pitch);
        float lenxz = cos(pitch);
        v.x = lenxz * cos(yaw);
        v.z = lenxz * sin(yaw);
        root->mSceneManager->flyDirection = root->airplane->mPosition + v;
    }

    if(input.IsKeyDown(sf::Key::D)){
        aiVector3D v = root->mSceneManager->flyDirection - root->airplane->mPosition;
        if(v.Length() > 0.00001f) v.Normalize();
        aiVector3D h = aiVector3D(v.x, 0.0f, v.z);
        if(h.Length() > 0.00001f) h.Normalize();
        float pitch = 0.0f;
        if(v.y != 0.0f)
            pitch = fabs(v.y)/v.y*acos(dot(v,h))/Pi*180.0f;
        pitch = pitch/180.0f*Pi;
        float yaw = 0.0f;
        if(v.z != 0.0f)
            yaw = fabs(v.z)/v.z*acos(dot(h,aiVector3D(1.f,0.f,0.f)))/Pi*180.0f;
        //if(yaw < 175.0f)
            yaw += dYaw;
        yaw = yaw/180.0f*Pi;
        v.y = sin(pitch);
        float lenxz = cos(pitch);
        v.x = lenxz * cos(yaw);
        v.z = lenxz * sin(yaw);
        root->mSceneManager->flyDirection = root->airplane->mPosition + v;
    }

    if(input.IsKeyDown(sf::Key::A)){
        aiVector3D v = root->mSceneManager->flyDirection - root->airplane->mPosition;
        if(v.Length() > 0.00001f) v.Normalize();
        aiVector3D h = aiVector3D(v.x, 0.0f, v.z);
        if(h.Length() > 0.00001f) h.Normalize();
        float pitch = 0.0f;
        if(v.y != 0.0f)
            pitch = fabs(v.y)/v.y*acos(dot(v,h))/Pi*180.0f;
        pitch = pitch/180.0f*Pi;
        float yaw = 0.0f;
        if(v.z != 0.0f)
            yaw = fabs(v.z)/v.z*acos(dot(h,aiVector3D(1.f,0.f,0.f)))/Pi*180.0f;
        //if(yaw > -175.0f)
            yaw -= dYaw;
        yaw = yaw/180.0f*Pi;
        v.y = sin(pitch);
        float lenxz = cos(pitch);
        v.x = lenxz * cos(yaw);
        v.z = lenxz * sin(yaw);
        root->mSceneManager->flyDirection = root->airplane->mPosition + v;
    }

    if(input.IsKeyDown(sf::Key::LShift)){
        root->mHighSpeed = true;
    }
    else{
        root->mHighSpeed = false;
    }

    if(mouseCaptureEnabled){
        mouseX = input.GetMouseX();
        mouseY = input.GetMouseY();
        float dyaw = 2*Pi*fabs(v.x)/v.x*float(mouseX - mouseX0)/window.GetWidth();
        float dpitch = 2*Pi*float(mouseY - mouseY0)/window.GetHeight();
        yaw += dyaw;
        pitch += dpitch;
        if(!root->mEnableAirplane){
            root->target.x = root->eye.x + fabs(v.x)/v.x*lenxz*cos(yaw);
            root->target.y = root->eye.y - lenxy*sin(pitch);
            root->target.z = root->eye.z + lenxz*sin(yaw);
        }
        else{
            root->eye.x = root->target.x - fabs(v.x)/v.x*lenxz*cos(yaw);
            root->eye.y = root->target.y + lenxy*sin(pitch);
            root->eye.z = root->target.z - lenxz*sin(yaw);
        }
        mouseX0 = mouseX;
        mouseY0 = mouseY;
    }
    else {
        mouseX0  = input.GetMouseX();
        mouseY0  = input.GetMouseY();
    }
}

