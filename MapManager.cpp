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
        globalMap[i].assign(99, 0); //empty tile
    }
    globalMap[49][49]=20;

    for(int i=0; i<28; i++)
    {
        nearestMap[i] = 0; //empty tile
    }
}

void MapManager::UpdateMap(RobotManager &manager, QString &localMap)
{
     UpdateGlobalMap(manager, localMap);
     UpdateNearestMap(manager);

     FindDeadEnds(manager, 2);

     UpdateNearestMap(manager);

     FindWideCorridor(manager);

     UpdateDirectionWeights();
}

void MapManager::UpdateGlobalMap(RobotManager &manager, QString &localMap)
{
    std::vector<int> mapPos;
    for(int i=0; i<28; i++)
    {
        if(localMap[i+4]!='?')
        {
            mapPos = getGlobalMapPos(manager.getPosX(), manager.getPosY(), i, manager.getOrientation()); //current position on global map
            //add element to global map if is not there
            if(globalMap[mapPos[0]][mapPos[1]]==0) //global map element is empty
            {
                if(localMap[i+4]!='.')
                {
                    if(localMap[i+4]=='O' || localMap[i+4] == '#')
                        globalMap[mapPos[0]][mapPos[1]] = -1; //wall
                    else if(localMap[i+4]=='E')
                        globalMap[mapPos[0]][mapPos[1]] = 500; //finish line
                }
                else
                    globalMap[mapPos[0]][mapPos[1]] = 50; //passage
            }
        }
    }
    if(localMap[2]>'0' && localMap[2]<'4') //last move was NOT a rotation and robot wasn't hit the wall
    {
        //find tiles robot jump over
        mapPos = getSteppedPos(manager.getPosX(), manager.getPosY(), manager.getOrientation());

        //increment element value when stepped
        globalMap[mapPos[0]][mapPos[1]] *= 0.4;
        int stepSize = 1;
        if(localMap[2]>'1') //fast forward
        {
            //increment element value when jumped over
            globalMap[mapPos[2]][mapPos[3]] *= 0.4;
            stepSize = 2;
            if(localMap[2]>'2') //rush
            {
                stepSize=3;
                //increment element value when jumped over
                globalMap[mapPos[4]][mapPos[5]] *= 0.4;
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
    if(ForwardPoints[0]<0 || nearestMap[10]<0)
    {
        ForwardPoints[0]=-1;
        ForwardPoints[1]=-1;
        ForwardPoints[2]=-1;
    }

    //wall on the right
    if(RightPoints[0]<0 || nearestMap[4]<0)
    {
        RightPoints[0]=-1;
        RightPoints[1]=-1;
        RightPoints[2]=-1;
    }

    //wall on the left
    if(LeftPoints[0]<0 || nearestMap[2]<0)
    {
        LeftPoints[0]=-1;
        LeftPoints[1]=-1;
        LeftPoints[2]=-1;
    }

    //when F[1] is crossing roads stepped 2 times and F[0] is stepped only 1 time I still want to make bigger step
    if(ForwardPoints[0]>ForwardPoints[1] && nearestMap[11]<0 && nearestMap[9]<0 && nearestMap[10]<500)
    {
        ForwardPoints[1] = ForwardPoints[0] + 1;
    }

    //found finish line in F[1]
    if(ForwardPoints[0]>0 && ForwardPoints[1]>298)
    {
        ForwardPoints[0] = ForwardPoints[1] - 50;
    }


}

///helps with some wide corridors, but only when robot is completely sore there is wide corridor.
/// Function is not perfect and I afraid it can crash algorithm sometimes
/// and sometimes it will be just not enough to prevent robot from stucking in wide corridors
/// but in many cases is very very helpful
void MapManager::FindWideCorridor(RobotManager &manager)
{
    std::vector<int> mapPos;
    for(int i=0; i<28; i++)
    {

            if(nearestMap[i]==50)
            {
                int sum[2][2] = {{0, 0}, {0, 0}};
                int sum2 = 0;
                mapPos = getGlobalMapPos(manager.getPosX(), manager.getPosY(), i, manager.getOrientation());

                //cut too wide corners
                sum[0][0] = globalMap[mapPos[0]+1][mapPos[1]] + globalMap[mapPos[0]][mapPos[1]-1]
                            + globalMap[mapPos[0]+1][mapPos[1]-1];
                sum[0][1] = globalMap[mapPos[0]-1][mapPos[1]] + globalMap[mapPos[0]][mapPos[1]+1]
                        + globalMap[mapPos[0]-1][mapPos[1]+1];

                sum[1][0] = globalMap[mapPos[0]+1][mapPos[1]] + globalMap[mapPos[0]][mapPos[1]+1]
                            + globalMap[mapPos[0]+1][mapPos[1]+1];
                sum[1][1] = globalMap[mapPos[0]-1][mapPos[1]] + globalMap[mapPos[0]][mapPos[1]-1]
                        + globalMap[mapPos[0]-1][mapPos[1]-1];

                //find when tile is surrounded by free space and only 1 wall
                sum2 = sum[0][1] + sum[0][0] + globalMap[mapPos[0]-1][mapPos[1]-1] + globalMap[mapPos[0]+1][mapPos[1]+1];

                if(((sum[1][0]>89 && sum[1][0]%2==0)  && ((sum[1][1]<-1 && globalMap[mapPos[0]-1][mapPos[1]-1]==0) || sum[1][1]<-2)) ||
                        ((sum[1][1]>89 && sum[1][1]%2==0) && ((sum[1][0]<-1 && globalMap[mapPos[0]+1][mapPos[1]+1]==0) || sum[1][0]<-2)) ||
                        ((sum[0][0]>89 && sum[0][0]%2==0) && ((sum[0][1]<-1 && globalMap[mapPos[0]-1][mapPos[1]+1]==0) || sum[0][1]<-2)) ||
                        ((sum[0][1]>89 && sum[0][1]%2==0) && ((sum[0][0]<-1 && globalMap[mapPos[0]+1][mapPos[1]-1]==0) || sum[0][0]<-2)) ||
                        sum2==259 )
                {
                   globalMap[mapPos[0]][mapPos[1]] = -1;
                   nearestMap[i]=-1;
                }
            }
    }
}

bool MapManager::FindDeadEnds(RobotManager &manager, int iterations)
{
    //current position of the robot
    int posX;
    int posY;

    bool success = false;

    posX = manager.getPosX();
    posY = manager.getPosY();

    if(posX>4 && posX<95 && posY>4 && posY<95)
    {
        for(int k=0; k<iterations; k++)
        {
            for(int i=posX-4; i<posX+4; i++)
            {
                for(int j=posY-4; j<posY+4; j++)
                {

                    if(posX!=i || posY!=j)
                    {
                        if(globalMap[i][j]==50 || globalMap[i][j]==20 || globalMap[i][j]==8)
                        {
                            int sum = 0;
                            int iloczyn = 0;

                            sum = globalMap[i+1][j] + globalMap[i-1][j]
                                + globalMap[i][j-1] + globalMap[i][j+1];
                            iloczyn = globalMap[i+1][j] * globalMap[i-1][j]
                                * globalMap[i][j-1] * globalMap[i][j+1];

                        if((sum==47 || sum == 17 || sum == 5 || sum == 0) && iloczyn>(-51) && iloczyn<0)
                        {
                            globalMap[i][j] = -1;
                            success = true;
                        }
                        }
                    }
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

std::vector<int> MapManager::getSteppedPos(int robotPosX, int robotPosY, ORIENTATION robotOrientation)
{
    std::vector <int> mapPos;
    mapPos.push_back(robotPosX);
    mapPos.push_back(robotPosY);

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
