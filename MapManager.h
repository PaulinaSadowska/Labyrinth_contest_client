#ifndef GLOBALMAP_H
#define GLOBALMAP_H
#include "RobotManager.h"
#include "QString"
#include <vector>

enum ORIENTATION;

class MapManager
{
public:
    MapManager();
    ~MapManager();

    void UpdateMap(RobotManager &manager, QString &localMap);
    void UpdateGlobalMap(RobotManager &manager, QString &localMap);
    void UpdateNearestMap(RobotManager &manager);

    std::vector<int> getGlobalMapPos(int robotPosX, int robotPosY, int i, ORIENTATION robotOrientation); //z lokalnej na globalne
    std::vector<int> getSteppedPos(QChar stepSize, int robotPosX, int robotPosY, ORIENTATION robotOrientation);

    QChar getGlobalMapElement(int x, int y);
    QChar getNearestMapElement(int i);
    void FindDeadEnds(RobotManager &manager);

    void mapInit();


private:
   std::vector<int> globalMap[99];
   int nearestMap[28];
};

#endif // GLOBALMAP_H
