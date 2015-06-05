#include "GlobalMapManager.h"

GlobalMapManager::GlobalMapManager()
{
    for(int i=0; i<99; i++)
    {
        map[i].assign(99,'?');
    }
    map[49][49]='S';
}

GlobalMapManager::~GlobalMapManager()
{

}

void GlobalMapManager::UpdateMap(RobotManager &manager, QString &localMap)
{
    std::vector<int> mapPos;
    for(int i=0; i<28; i++)
    {
        if(localMap[i+4]!='?')
        {
            mapPos = getMapPos(manager.getPosX(), manager.getPosY(), i, manager.getOrientation());
            if(map[mapPos[0]][mapPos[1]]!=localMap[i+4] && map[mapPos[0]][mapPos[1]]=='?')
            {
                map[mapPos[0]][mapPos[1]] = localMap[i+4];
            }
        }
    }
}

std::vector<int> GlobalMapManager::getMapPos(int robotPosX, int robotPosY, int i, ORIENTATION robotOrientation)
{
    std::vector <int> mapPos;

    switch(robotOrientation)
    {
    case Up:
    {
        mapPos.push_back(robotPosX+i%7-3);
        mapPos.push_back(robotPosY+i/7);
        break;
    }
    case Down:
    {
         mapPos.push_back(robotPosX-(i%7-3));
         mapPos.push_back(robotPosY-i/7);
        break;
    }
    case Right:
    {
        mapPos.push_back(robotPosX+i/7);
        mapPos.push_back(robotPosY-(i%7-3));
        break;
    }
    case Left:
    {
        mapPos.push_back(robotPosX-i/7);
        mapPos.push_back(robotPosY+(i%7-3));
        break;
    }
    }

    return mapPos;
}
