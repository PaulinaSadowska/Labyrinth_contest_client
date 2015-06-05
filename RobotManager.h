#ifndef ROBOTMANAGER_H
#define ROBOTMANAGER_H

enum ORIENTATION{Right, Up, Left, Down};

struct Position{
    int posX;
    int posY;
    ORIENTATION orientation;
};

class RobotManager
{
public:
    RobotManager();
    ~RobotManager();

    void MoveForward(int step);
    void RotateLeft();
    void RotateRight();
    void Init();

    int getPosX();
    int getPosY();
    ORIENTATION getOrientation();

private:
    Position position;
};

#endif // ROBOTMANAGER_H
