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
        globalMap[i].assign(99, 0);
    }
    globalMap[49][49]=20;

    for(int i=0; i<28; i++)
    {
        nearestMap[i] = 0;
    }
}

void MapManager::UpdateMap(RobotManager &manager, QString &localMap)
{
     UpdateGlobalMap(manager, localMap);
     UpdateNearestMap(manager);
     if(FindDeadEnds(manager))
         UpdateNearestMap(manager);
     UpdateDirectionWeights();
     FindWideCorridor(manager);
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
            if(globalMap[mapPos[0]][mapPos[1]]==0)
            {
                if(localMap[i+4]!='.')
                {
                    if(localMap[i+4]=='O' || localMap[i+4] == '#')
                        globalMap[mapPos[0]][mapPos[1]] = -1;
                    else if(localMap[i+4]=='E')
                        globalMap[mapPos[0]][mapPos[1]] = 500;
                }
                else
                    globalMap[mapPos[0]][mapPos[1]] = 50;
            }
        }
    }
    if(localMap[2]>'0' && localMap[2]<'4') //last move was NOT a rotation and robot wasn't hit the wall
    {
        //increment element value when stepped
        globalMap[manager.getPosX()][manager.getPosY()] *= 0.4;
        if(localMap[2]>'1') //fast forward
        {
            //find tiles robot jump over
            mapPos = getSteppedPos(localMap[2], manager.getPosX(), manager.getPosY(), manager.getOrientation());
            //increment element value when jumped over
            globalMap[mapPos[0]][mapPos[1]] *= 0.4;
            if(localMap[2]>'2') //rush
            {
                //increment element value when jumped over
                globalMap[mapPos[2]][mapPos[3]] *= 0.4;
            }
        }
    }
}

void MapManager::UpdateDirectionWeights()
{
    ForwardPoints[0] = nearestMap[10] + (nearestMap[9]+nearestMap[11])/2;
    ForwardPoints[1] = nearestMap[17] + (nearestMap[16]+nearestMap[18])/2;
    ForwardPoints[2] = nearestMap[24] + (nearestMap[23]+nearestMap[25])/2;

    LeftPoints[0] = nearestMap[2] + nearestMap[9];
    LeftPoints[1] = nearestMap[1] + nearestMap[8];
    LeftPoints[2] = nearestMap[0] + nearestMap[7];

    RightPoints[0] = nearestMap[4] + nearestMap[11];
    RightPoints[1] = nearestMap[5] + nearestMap[12];
    RightPoints[2] = nearestMap[6] + nearestMap[13];

    //find where is the wall

    //wall ahead
    if(ForwardPoints[0]<0)
    {
        ForwardPoints[1]=-1;
        ForwardPoints[2]=-1;
    }

    //wall on the right
    if(RightPoints[0]<0)
    {
        RightPoints[1]=-1;
        RightPoints[2]=-1;
    }

    //wall on the left
    if(LeftPoints[0]<0)
    {
        LeftPoints[1]=-1;
        LeftPoints[2]=-1;
    }

}

bool MapManager::FindWideCorridor(RobotManager &manager)
{
    std::vector<int> mapPos;

    bool success = false;

    for(int i=0; i<28; i++)
    {

            if(nearestMap[i]==50)
            {
                int sum[2][2] = {{0, 0}, {0, 0}};
                mapPos = getGlobalMapPos(manager.getPosX(), manager.getPosY(), i, manager.getOrientation());

                sum[0][0] = globalMap[mapPos[0]+1][mapPos[1]] + globalMap[mapPos[0]][mapPos[1]-1]
                            + globalMap[mapPos[0]+1][mapPos[1]-1];
                sum[0][1] = globalMap[mapPos[0]-1][mapPos[1]] + globalMap[mapPos[0]][mapPos[1]+1]
                        + globalMap[mapPos[0]-1][mapPos[1]+1];

                sum[1][0] = globalMap[mapPos[0]+1][mapPos[1]] + globalMap[mapPos[0]][mapPos[1]+1]
                            + globalMap[mapPos[0]+1][mapPos[1]+1];
                sum[1][1] = globalMap[mapPos[0]-1][mapPos[1]] + globalMap[mapPos[0]][mapPos[1]-1]
                        + globalMap[mapPos[0]-1][mapPos[1]-1];

                if(((sum[1][0]>50 && sum[1][0]%2==0)  && sum[1][1]<-2) ||
                        ((sum[1][1]>50 && sum[1][1]%2==0) && sum[1][0]<-2) ||
                        ((sum[0][0]>50 && sum[0][0]%2==0) && sum[0][1]<-2) ||
                        ((sum[0][1]>50 && sum[0][1]%2==0) && sum[0][0]<-2))
                {
                   globalMap[mapPos[0]][mapPos[1]] = -1;
                   nearestMap[i]=-1;
                }
            }
    }
    return success;
}

bool MapManager::FindDeadEnds(RobotManager &manager)
{
    std::vector<int> mapPos;

    bool success = false;

    for(int i=0; i<28; i++)
    {

            if(nearestMap[i]==50)
            {
                int sum = 0;
                mapPos = getGlobalMapPos(manager.getPosX(), manager.getPosY(), i, manager.getOrientation());

                sum = globalMap[mapPos[0]+1][mapPos[1]] + globalMap[mapPos[0]-1][mapPos[1]]
                            + globalMap[mapPos[0]][mapPos[1]-1] + globalMap[mapPos[0]][mapPos[1]+1];

                if(sum==47 || sum == 17)
                {
                   globalMap[mapPos[0]][mapPos[1]] = -1;
                   success = true;
                   if(nearestMap[i+1]==50)
                   {
                       mapPos = getGlobalMapPos(manager.getPosX(), manager.getPosY(), i+1, manager.getOrientation());
                   }
                   if(nearestMap[i-1]==50)
                   {
                       mapPos = getGlobalMapPos(manager.getPosX(), manager.getPosY(), i-1, manager.getOrientation());
                   }
                   if(nearestMap[i+7]==50)
                   {
                       mapPos = getGlobalMapPos(manager.getPosX(), manager.getPosY(), i+7, manager.getOrientation());
                   }
                   if(i>7)
                   {
                       if(nearestMap[i-7]==50)
                       { 
                           mapPos = getGlobalMapPos(manager.getPosX(), manager.getPosY(), i-7, manager.getOrientation());
                       }
                   }
                   sum = globalMap[mapPos[0]+1][mapPos[1]] + globalMap[mapPos[0]-1][mapPos[1]]
                               + globalMap[mapPos[0]][mapPos[1]-1] + globalMap[mapPos[0]][mapPos[1]+1];
                   if(sum==47 || sum == 17)
                   {
                       globalMap[mapPos[0]][mapPos[1]] = -1;
                   }
                }
            }
    }
    return success;
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


int MapManager::getGlobalMapElement(int x, int y)
{
    return globalMap[x][y];
}

int MapManager::getNearestMapElement(int i){
    return nearestMap[i];
}

QString MapManager::getGlobalMapElementStr(int x, int y)
{
    int element = globalMap[x][y];
    if(element>0)
    {
        if(element == 50)
            return "3";
        if(element == 20)
            return "2";
        if(element == 8)
            return "1";
    }
    if(element<0)
        return "#";
    if(element==0)
        return " ";
    if(element == 500)
        return "E";

    return 0;
}

QString MapManager::getNearestMapElementStr(int i)
{
    int element = nearestMap[i];
    if(element>0)
    {
        if(element == 50)
            return "3";
        if(element == 20)
            return "2";
        if(element == 8)
            return "1";
    }
    if(element<0)
        return "#";
    if(element==0)
        return " ";
    if(element == 500)
        return "E";

    return 0;

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
