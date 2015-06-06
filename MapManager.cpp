#include "MapManager.h"

MapManager::MapManager()
{
    mapInit();
}

MapManager::~MapManager()
{

}

void MapManager::mapInit()
{
    for(int i=0; i<99; i++)
    {
        globalMap[i].assign(99,' ');
    }
    globalMap[49][49]='S';

    for(int i=0; i<28; i++)
    {
        nearestMap[i] = ' ';
    }
}

void MapManager::UpdateMap(RobotManager &manager, QString &localMap)
{
     UpdateGlobalMap(manager, localMap);
     UpdateNearestMap(manager);
}

void MapManager::UpdateGlobalMap(RobotManager &manager, QString &localMap)
{
    std::vector<int> mapPos;
    for(int i=0; i<28; i++)
    {
        if(localMap[i+4]!='?')
        {
            mapPos = getGlobalMapPos(manager.getPosX(), manager.getPosY(), i, manager.getOrientation());
            //add element to global map if is not there
            if(globalMap[mapPos[0]][mapPos[1]]!=localMap[i+4] && globalMap[mapPos[0]][mapPos[1]]==' ')
            {
                if(localMap[i+4]!='.')
                {
                    if(localMap[i+4]=='O')
                        globalMap[mapPos[0]][mapPos[1]] = '#';
                    else
                        globalMap[mapPos[0]][mapPos[1]] = localMap[i+4];
                }
                else
                    globalMap[mapPos[0]][mapPos[1]] = '0';
            }
        }
    }
    if(localMap[2]>'0' && localMap[2]<'4') //last move was NOT a rotation and robot wasn't hit the wall
    {
        //increment element value when stepped
        globalMap[manager.getPosX()][manager.getPosY()] = globalMap[manager.getPosX()][manager.getPosY()].digitValue() + 49;
        if(localMap[2]>'1') //fast forward
        {
            //find tiles robot jump over
            mapPos = getSteppedPos(localMap[2], manager.getPosX(), manager.getPosY(), manager.getOrientation());
            //increment element value when jumped over
            globalMap[mapPos[0]][mapPos[1]] = globalMap[mapPos[0]][mapPos[1]].digitValue() + 49;
            if(localMap[2]>'2') //rush
            {
                //increment element value when jumped over
                globalMap[mapPos[2]][mapPos[3]] = globalMap[mapPos[2]][mapPos[3]].digitValue() + 49;
            }
        }
    }
}

void MapManager::UpdateNearestMap(RobotManager &manager)
{
    std::vector<int> mapPos;
    for(int i=0; i<28; i++)
    {
        mapPos = getGlobalMapPos(manager.getPosX(), manager.getPosY(), i, manager.getOrientation());
        nearestMap[i] = globalMap[mapPos[0]][mapPos[1]];
    }
}

std::vector<int> MapManager::getGlobalMapPos(int robotPosX, int robotPosY, int i, ORIENTATION robotOrientation)
{
    std::vector <int> mapPos;

    switch(robotOrientation)
    {
    case Up:
    {
        mapPos.push_back(robotPosX+i%7-3);
        mapPos.push_back(robotPosY-i/7);
        break;
    }
    case Down:
    {
        mapPos.push_back(robotPosX-(i%7-3));
        mapPos.push_back(robotPosY+i/7);
        break;
    }
    case Right:
    {
        mapPos.push_back(robotPosX+i/7);
        mapPos.push_back(robotPosY+(i%7-3));
        break;
    }
    case Left:
    {
        mapPos.push_back(robotPosX-i/7);
        mapPos.push_back(robotPosY-(i%7-3));
        break;
    }
    }

    return mapPos;
}

QChar MapManager::getGlobalMapElement(int x, int y)
{
    return globalMap[x][y];
}

QChar MapManager::getNearestMapElement(int i){
    return nearestMap[i];
}

std::vector<int> MapManager::getSteppedPos(QChar stepSize, int robotPosX, int robotPosY, ORIENTATION robotOrientation)
{
    std::vector <int> mapPos;

    switch(robotOrientation)
    {
    case Up:
    {
        mapPos.push_back(robotPosX);
        mapPos.push_back(robotPosY+1);
        mapPos.push_back(robotPosX);
        mapPos.push_back(robotPosY+2);
        break;
    }
    case Down:
    {
        mapPos.push_back(robotPosX);
        mapPos.push_back(robotPosY-1);
        mapPos.push_back(robotPosX);
        mapPos.push_back(robotPosY-2);
        break;
    }
    case Right:
    {
        mapPos.push_back(robotPosX-1);
        mapPos.push_back(robotPosY);
        mapPos.push_back(robotPosX-2);
        mapPos.push_back(robotPosY);
        break;
    }
    case Left:
    {
        mapPos.push_back(robotPosX+1);
        mapPos.push_back(robotPosY);
        mapPos.push_back(robotPosX+2);
        mapPos.push_back(robotPosY);
        break;
    }
    }

    return mapPos;
}
