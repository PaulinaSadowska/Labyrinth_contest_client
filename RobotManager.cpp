#include "RobotManager.h"

RobotManager::RobotManager()
{
    Init();
}

RobotManager::~RobotManager()
{

}

void RobotManager::Init()
{
    position.posX = 49;
    position.posY = 49;
    position.orientation = Right;
}

void RobotManager::MoveForward(int step)
{
    if( step < 1 || step > 3 )
        return;
    switch(position.orientation)
    {
        case Up : {
            position.posY-=step;
            break;
        }
        case Right : {
            position.posX+=step;
            break;
        }
        case Down : {
            position.posY+=step;
            break;
        }

        case Left : {
            position.posX-=step;
            break;
        }
    }
}


void RobotManager::RotateLeft(){

    switch(position.orientation)
    {
         case Up : {
            position.orientation = Left;
            break;
        }
        case Right : {
            position.orientation = Up;
            break;
        }
        case Down : {
            position.orientation = Right;
            break;
        }
        case Left : {
            position.orientation = Down;
            break;
        }
    }
}

void RobotManager::RotateRight(){

    switch(position.orientation)
    {
         case Up : {
            position.orientation = Right;
            break;
        }
        case Right : {
            position.orientation = Down;
            break;
        }
        case Down : {
            position.orientation = Left;
            break;
        }
        case Left : {
            position.orientation = Up;
            break;
        }
    }

}

int RobotManager::getPosX()
{
    return position.posX;
}

int RobotManager::getPosY()
{
    return position.posY;
}

ORIENTATION RobotManager::getOrientation()
{
    return position.orientation;
}
