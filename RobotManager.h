#ifndef ROBOTMANAGER_H
#define ROBOTMANAGER_H

enum ORIENTATION{Right, Up, Left, Down}; ///< possible robot orientations

///stores position and ORIENTATION of the robot
struct Position{
    int posX;   ///<robot position in X axis
    int posY;   ///<robot position in Y axis
    ORIENTATION orientation; ///<robot orientation
};

class RobotManager
{
public:
    RobotManager();
    ~RobotManager();

    ///update robot position when moves forward
    /// \param step length of the step
    void MoveForward(int step);

    ///update robot orientation when rotates left
    void RotateLeft();

    ///update robot orientation when rotates right
    void RotateRight();

    ///sets robot position and orientation to initial values
    void Init();

    ///\returns robot position in X axis
    int getPosX();

    ///\returns robot position in Y axis
    int getPosY();

    ///\returns robot orientation
    ORIENTATION getOrientation();

    int  rotationCounter;

private:
    Position position;  ///< position AND ORIENTATION of the robot
};

#endif // ROBOTMANAGER_H
