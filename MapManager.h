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

    ///call functions which updates all maps
    void UpdateMap(RobotManager &manager, QString &localMap);

    /// updates global map with new tiles from local map
    void UpdateGlobalMap(RobotManager &manager, QString &localMap);

    /// updates nearest map (4x7) which robot uses to make decisions
    void UpdateNearestMap(RobotManager &manager);

    ///updates direction weights which are used to make decisions about direction
    void UpdateDirectionWeights();

    ///returns global map position from local position
    /// \param robotPosX robot position in X axis
    /// \param robotPosY robot position in Y axis
    /// \param i tile number from local map
    /// \param robotOrientation orientation of the robot
    /// \returns position of a i'th tile from local map on global map
    std::vector<int> getGlobalMapPos(int robotPosX, int robotPosY, int i, ORIENTATION robotOrientation); //z lokalnej na globalne

    ///returns stepped positions
    /// \param robotPosX robot position in X axis
    /// \param robotPosY robot position in Y axis
    /// \param robotOrientation orientation of the robot
    /// \returns position of a stepped position on global map
    std::vector<int> getSteppedPos(int robotPosX, int robotPosY, ORIENTATION robotOrientation);

    /// \param x coordinate in X axis
    /// \param y coordinate in Y axis
    /// \returns global map element
    int getGlobalMapElement(int x, int y);

    /// \param i number of a tile from local map
    /// \returns local map element
    int getNearestMapElement(int i);

    /// \param element value of tile from map
    /// \returns map element
    QString getMapElementStr(int element);

    /// returns string representation of global map element
    ///  \param x coordinate in X axis
    /// \param y coordinate in Y axis
    /// \returns global map element
    QString getGlobalMapElementStr(int x, int y);

    /// returns string representation of local map element
    /// \param i number of a tile from local map
    /// \returns local map element
    QString getNearestMapElementStr(int i);

    /// finds and eliminates dead ends on local and global map (checks only nearest tiles)
    bool FindDeadEnds(RobotManager &manager);

    /// finds and eliminates SOME (not all :() wide corridors on local and global map (checks only nearest tiles)
    bool FindWideCorridor(RobotManager &manager);

    bool WideCorridorCheck(int posX, int posY);

    ///inits all maps
    void mapInit();

   int RightPoints[3];   ///< points from right tiles
   int ForwardPoints[3]; ///< points from forward tiles
   int LeftPoints[3];    ///< points from left tiles

   //bool deadEndFlag;

private:
   std::vector<int> globalMap[105]; ///< map of all known tiles
   int nearestMap[28];             ///< map of nearest 4x7 tiles, used to make decisions, consist of tiles from global map, store more information than local map!

};

#endif // GLOBALMAP_H
