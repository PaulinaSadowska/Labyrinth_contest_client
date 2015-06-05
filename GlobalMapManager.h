#ifndef GLOBALMAP_H
#define GLOBALMAP_H
#include "RobotManager.h"
#include "QString"
#include <vector>

enum ORIENTATION;

class GlobalMapManager
{
public:
    GlobalMapManager();
    ~GlobalMapManager();

    void UpdateMap(RobotManager &manager, QString &localMap);
   /* void UpdateUp();
    void UpdateDown();
    void UpdateRight();
    void UpdateLeft();*/

    std::vector<int> getMapPos(int robotPosX, int robotPosY, int i, ORIENTATION robotOrientation);


private:
   std::vector<QChar> map[99];
};

#endif // GLOBALMAP_H
