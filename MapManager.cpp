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
    globalMap[49][49]=20; //start tile

    for(int i=0; i<28; i++)
    {
        nearestMap[i] = 0; //empty tile
    }
}

void MapManager::UpdateMap(RobotManager &manager, QString &localMap)
{
     UpdateGlobalMap(manager, localMap);
     UpdateNearestMap(manager);

     int maxloop = 4;

     while(FindDeadEnds(manager) && maxloop>0)
         maxloop--;

     ///loop untill there was some wide corridor found (maxloop is used just in case)
     maxloop = 20;
     while(FindWideCorridor(manager) && maxloop)
         maxloop--;

     UpdateNearestMap(manager);

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
    if(localMap[2]>'0' && localMap[2]<'4') //last move was NOT a rotation and robot didn't hit the wall
    {
        //find tiles robot jump over
        mapPos = getSteppedPos(manager.getPosX(), manager.getPosY(), manager.getOrientation());

        //increment element value when stepped
        if(globalMap[mapPos[0]][mapPos[1]]>3) //min value of passage is 3 (possible values: 50, 20, 8, 3)
            globalMap[mapPos[0]][mapPos[1]] *= 0.4;
        int stepSize = 1;
        if(localMap[2]>='2') //last move: fast forward
        {
            //increment element value when jumped over
            if(globalMap[mapPos[2]][mapPos[3]]>3) //min value of passage is 3
                globalMap[mapPos[2]][mapPos[3]] *= 0.4;
            stepSize = 2;
            if(localMap[2]=='3') //last move: rush
            {
                stepSize=3;
                //increment element value when jumped over
                if(globalMap[mapPos[4]][mapPos[5]]>3) //min value of passage is 3
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
/// Function is not perfect and sometimes it will be just not enough to prevent robot from stucking in wide corridors
/// but in many cases is very very helpful
bool MapManager::FindWideCorridor(RobotManager &manager)
{
    bool success = false;
    std::vector<int> mapPos;
    mapPos.assign(2, 0);

    //current robot position
    int posX = manager.getPosX();
    int posY = manager.getPosY();

    //scan sqauare 11x11 next to the robot
    for(int x = -5; x<5; x++)
    {
        for(int y = -5; y<5; y++)
        {
            //position within a global map && nor a robot current position
            if(posX+x < 99 && posX+x > 0 && posY+y < 99 && posY+y > 0 && (x!=0 || y!=0))
            {
                mapPos[0] = posX+x;
                mapPos[1] = posY+y;
                //chacked tile is a passage (not finish tile!)
                if(globalMap[mapPos[0]][mapPos[1]]>0 && globalMap[mapPos[0]][mapPos[1]]<51)
                {
                    int sum[2][2] = {{0, 0}, {0, 0}};
                    int sum2 = 0;

                    //find too wide corners
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


                    if((((sum[1][0]>89 || sum[1][0]==60) && sum[1][0]%2==0)  && ((sum[1][1]<-1 && globalMap[mapPos[0]-1][mapPos[1]-1]==0) || sum[1][1]<-2)) ||
                            (((sum[1][1]>89 || sum[1][1]==60) && sum[1][1]%2==0) && ((sum[1][0]<-1 && globalMap[mapPos[0]+1][mapPos[1]+1]==0) || sum[1][0]<-2)) ||
                            (((sum[0][0]>89 || sum[0][0]==60) && sum[0][0]%2==0) && ((sum[0][1]<-1 && globalMap[mapPos[0]-1][mapPos[1]+1]==0) || sum[0][1]<-2)) ||
                            (((sum[0][1]>89 || sum[0][1]==60) && sum[0][1]%2==0) && ((sum[0][0]<-1 && globalMap[mapPos[0]+1][mapPos[1]-1]==0) || sum[0][0]<-2)) ||
                            sum2==259 || sum2==229)
                    {
                        globalMap[mapPos[0]][mapPos[1]] = -1; //place wall on the map to make passages only 1 tile wide
                        success = true;
                    }
                }
            }
        }
    }
    return success;
}

bool MapManager::FindDeadEnds(RobotManager &manager)
{
    bool success = false;

    //current position of the robot
    int posX = manager.getPosX();
    int posY = manager.getPosY();

    if(posX>4 && posX<95 && posY>4 && posY<95)
    {
            //scan sqauare 9x9 next to the robot
            for(int i=posX-4; i<posX+4; i++)
            {
                for(int j=posY-4; j<posY+4; j++)
                {
                    if(posX!=i || posY!=j) //not a current robot position
                    {
                        if(globalMap[i][j]==50 || globalMap[i][j]==20 ||
                                globalMap[i][j]==8 || globalMap[i][j]==3 || globalMap[i][j]==1)
                        {
                            int sum = 0;
                            int iloczyn = 0;

                            sum = globalMap[i+1][j] + globalMap[i-1][j]
                                + globalMap[i][j-1] + globalMap[i][j+1];
                            iloczyn = globalMap[i+1][j] * globalMap[i-1][j]
                                * globalMap[i][j-1] * globalMap[i][j+1];

                        if((sum==47 || sum == 17 || sum == 5 || sum == 0) && iloczyn>(-51) && iloczyn<0) //only dead end pass
                        {
                            globalMap[i][j] = -1;
                            success = true;
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
    if(x<100 && x>-1 && y<100 && y>-1)
        return globalMap[x][y];

    return 0;
}

int MapManager::getNearestMapElement(int i){
    if(i<28 && i>-1)
        return nearestMap[i];

    return 0;
}

QString MapManager::getGlobalMapElementStr(int x, int y)
{
    if(x<100 && x>-1 && y<100 && y>-1)
    {
        int element = globalMap[x][y];
        return getMapElementStr(element);
    }
    return "X";
}

QString MapManager::getNearestMapElementStr(int i)
{
    if(i>-1 && i<28)
    {
        int element = nearestMap[i];
        return getMapElementStr(element);
    }
    return "X";
}

QString MapManager::getMapElementStr(int element)
{
    if(element>0)
    {
        if(element == 50)
            return "5";
        if(element == 20)
            return "4";
        if(element == 8)
            return "3";
        if(element == 3)
            return "2";
        if(element == 1)
            return "1";
    }
    if(element<0)
        return "#";
    if(element==0)
        return " ";
    if(element == 500)
        return "E";

    return "?";

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
