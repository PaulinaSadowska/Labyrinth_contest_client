#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),socket(0),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    socket=new QTcpSocket(this);

    //local map table
    for(int i=0;i<7;i++)
        for(int j=0;j<4;j++)
            ui->robotVisionTable->setItem(j,i,new QTableWidgetItem("?"));
    for(int i=0;i<7;i++)
        ui->robotVisionTable->setColumnWidth(i,30);
    for(int i=0;i<4;i++)
        ui->robotVisionTable->setRowHeight(i,30);

    //nearest map table
    for(int i=0;i<7;i++)
        for(int j=0;j<4;j++)
            ui->nearestMapTable->setItem(j,i,new QTableWidgetItem("?"));
    for(int i=0;i<7;i++)
        ui->nearestMapTable->setColumnWidth(i,30);
    for(int i=0;i<4;i++)
        ui->nearestMapTable->setRowHeight(i,30);

    //global map table
    for(int i=0;i<99;i++)
        for(int j=0;j<99;j++)
            ui->globalMapTable->setItem(j,i,new QTableWidgetItem(" "));
    for(int i=0;i<99;i++)
        ui->globalMapTable->setColumnWidth(i,20);
    for(int i=0;i<99;i++)
        ui->globalMapTable->setRowHeight(i,20);
}

MainWindow::~MainWindow()
{
    socket->close();
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    socket->abort();
    socket->connectToHost(ui->addressEdit->text(),ui->portEdit->text().toInt());
    if(socket->isWritable())
    {
        ui->pushButtonFastForward->setEnabled(true);
        ui->pushButtonForward->setEnabled(true);
        ui->pushButtonRush->setEnabled(true);
        ui->pushButtonWait->setEnabled(true);
        ui->pushButtonRotateLeft->setEnabled(true);
        ui->pushButtonRotateRight->setEnabled(true);
        socketStream.setDevice(socket);
        connect(socket,SIGNAL(readyRead()),this,SLOT(new_message()));
        connect(socket,SIGNAL(disconnected()),this,SLOT(client_disconnected()));

    }
}

void MainWindow::on_pushButtonRush_clicked()
{
    Rush();
}

void MainWindow::on_pushButtonFastForward_clicked()
{
    MoveFastForward();
}

void MainWindow::on_pushButtonForward_clicked()
{
    MoveForward();
}

void MainWindow::on_pushButtonWait_clicked()
{
    Wait();
}

void MainWindow::on_pushButtonRotateLeft_clicked()
{
    RotateLeft();
}

void MainWindow::on_pushButtonRotateRight_clicked()
{
    RotateRight();
}

void MainWindow::new_message()
{

    if(socket->isReadable())
    {
        QString localMap;
        localMap=socketStream.readLine();
        ui->rotationEdit->setText(localMap.mid(0,1));
        ui->translationEdit->setText(localMap.mid(2,1));
        for(int i=0;i<4;i++)
            for(int j=0;j<7;j++)
                ui->robotVisionTable->item(3-i,j)->setText(localMap.mid(4+i*7+j,1));


        //update robot manager
        robotManager.MoveForward(localMap[2].digitValue());
        if(localMap[0]=='R')
            robotManager.RotateRight();
        else if(localMap[0]=='L')
            robotManager.RotateLeft();
        else if(localMap[0]=='X') //new map
        {
            robotManager.Init();
            mapManager.mapInit();
        }

        //update maps (global and nearest)
        mapManager.UpdateMap(robotManager, localMap);

        //show gobal map on ui
        for(int i=0;i<98;i++)
            for(int j=0;j<98;j++)
                ui->globalMapTable->item(i,j)->setText(mapManager.getGlobalMapElement(j, i));

        //show nearest map on ui
        for(int i=0;i<4;i++)
            for(int j=0;j<7;j++)
                ui->nearestMapTable->item(3-i,j)->setText(mapManager.getNearestMapElement(i*7+j));

        Move();
    }

}

bool MainWindow::Move()
{
    doneFlag = false;

    if(LookForFinishLine())
        return true;

    if(Step00() && !doneFlag)
    {
        if(!Step10() && !doneFlag)
            Step20();
    }
    else if(!doneFlag)
    {
        if(Step01())
        {
            if(Step02())
            {
                if(Step03())
                {
                    if(!Step13() && !doneFlag)
                        Step20();
                }
                else if(!doneFlag)
                {
                    if(!Step04() && !doneFlag)
                        Step20();
                }
            }
            else if(!doneFlag)
            {
                Step12();
            }
        }
        else if(!doneFlag)
        {
            Step20();
        }
    }

}

bool MainWindow::Step00(){
    if(mapManager.getNearestMapElement(4)=='#' && mapManager.getNearestMapElement(2)!='?')
        return 1;
    else
        return 0;
}

bool MainWindow::Step10()
{
    if(mapManager.getNearestMapElement(10)=='#')
    {
        RotateLeft();
        doneFlag = 1;
        return 0;
    }
    else
    {
        return 0;
    }

}

bool MainWindow::Step20()
{
    if(mapManager.getNearestMapElement(11)=='#' && mapManager.getNearestMapElement(17)!='#')
    {
        doneFlag = 1;
        MoveFastForward();
        return 0;
    }
    else
    {
        doneFlag = 1;
        MoveForward();
        return 0;
    }
}

bool MainWindow::Step01(){
    if(mapManager.getNearestMapElement(2)=='?' || mapManager.getNearestMapElement(4)=='?')
    {
        doneFlag = 1;
        Wait();
        return 0;
    }
    else
    {
        if(mapManager.getNearestMapElement(2)=='#')
        {
            if(mapManager.getNearestMapElement(4)<=mapManager.getNearestMapElement(10) ||
               mapManager.getNearestMapElement(10) == '#')
            {
                doneFlag = 1;
                RotateRight();
                return 0;
            }
            else
            {
                return 0;
            }
        }
        else
        {
            if((mapManager.getNearestMapElement(2)<mapManager.getNearestMapElement(10) ||
                mapManager.getNearestMapElement(4)<mapManager.getNearestMapElement(10) ) ||
                mapManager.getNearestMapElement(10) == '#')
            {
                return 1;
            }
            else
            {
                return 0;
            }
        }
    }

}

bool MainWindow::Step02(){
    if(mapManager.getNearestMapElement(2) == mapManager.getNearestMapElement(4))
         return 1;
    else
        return 0;
}

bool MainWindow::Step12(){

    if(mapManager.getNearestMapElement(4)<mapManager.getNearestMapElement(2))
    {
        doneFlag=1;
        RotateRight();
        return 0;
    }
    else
    {
        doneFlag=1;
        RotateLeft();
        return 0;
    }

}

bool MainWindow::Step03(){

    if(mapManager.getNearestMapElement(1) == '?' ||
       mapManager.getNearestMapElement(5) == '?')
    {
        doneFlag = 1;
        Wait();
        return 0;
    }
    else
    {
        if(mapManager.getNearestMapElement(1) == '#' ||
           mapManager.getNearestMapElement(5) == '#')
            return 1;
        else
            return 0;
    }
}

bool MainWindow::Step13(){

    if(mapManager.getNearestMapElement(0) == '?' ||
       mapManager.getNearestMapElement(6) == '?')
    {
        doneFlag = 1;
        Wait();
        return 0;
    }
    else
    {
        if(mapManager.getNearestMapElement(6) == '#')
        {
            if(mapManager.getNearestMapElement(0) == '#')
            {
                if(mapManager.getNearestMapElement(10) == '#')
                {
                    doneFlag = 1;
                    RotateRight();
                    return 0;
                }
                else
                {
                    return 0;
                }
            }
            else
            {
                doneFlag = 1;
                RotateLeft();
                return 0;
            }
        }
        else
        {
            doneFlag = 1;
            RotateRight();
            return 0;
        }
    }

}

bool MainWindow::Step04(){

    if(mapManager.getNearestMapElement(1) == '#' && mapManager.getNearestMapElement(5) == '#')
        return 1;
    else
    {
        if(mapManager.getNearestMapElement(1) == '#')
        {
            doneFlag = 1;
            RotateRight();
            return 0;
        }
        else if(mapManager.getNearestMapElement(5) == '#')
        {
            doneFlag = 1;
            RotateLeft();
            return 0;
        }
    }

}

bool MainWindow::LookForFinishLine()
{
    //if finish line in front of robot
    if(mapManager.getNearestMapElement(10)=='E' ||
            mapManager.getNearestMapElement(17)=='E' ||
            mapManager.getNearestMapElement(23)=='E' ||
            mapManager.getNearestMapElement(24)=='E' ||
            mapManager.getNearestMapElement(25)=='E')
    {
        Rush();
        return true;
    }
    if(mapManager.getNearestMapElement(16)=='E' ||
            mapManager.getNearestMapElement(18)=='E' ||
            mapManager.getNearestMapElement(19)=='E' ||
            mapManager.getNearestMapElement(15)=='E')
    {
        MoveFastForward();
        return true;
    }
    if(mapManager.getNearestMapElement(9)=='E' ||
            mapManager.getNearestMapElement(11)=='E')
    {
        MoveForward();
        return true;
    }
    //if finish line is in the right of robot
    if(mapManager.getNearestMapElement(4)=='E' ||
            mapManager.getNearestMapElement(5)=='E' ||
            mapManager.getNearestMapElement(6)=='E' ||
            mapManager.getNearestMapElement(12)=='E' ||
            mapManager.getNearestMapElement(13)=='E')
    {
        RotateRight();
        return true;
    }
    //if finish line is in the left of robot
    if(mapManager.getNearestMapElement(0)=='E' ||
            mapManager.getNearestMapElement(1)=='E' ||
            mapManager.getNearestMapElement(2)=='E' ||
            mapManager.getNearestMapElement(7)=='E' ||
            mapManager.getNearestMapElement(8)=='E')
    {
        RotateLeft();
        return true;
    }
    return false;
}

void MainWindow::Rush()
{
    if(socket->isWritable())
        socketStream<<"Rush"<<endl;

}

void MainWindow::MoveFastForward()
{
    if(socket->isWritable())
        socketStream<<"FastForward"<<endl;
}

void MainWindow::MoveForward()
{
    if(socket->isWritable())
        socketStream<<"Forward"<<endl;
}

void MainWindow::Wait()
{
    if(socket->isWritable())
        socketStream<<"Wait"<<endl;
}

void MainWindow::RotateLeft()
{
    if(socket->isWritable())
        socketStream<<"RotateLeft"<<endl;
}

void MainWindow::RotateRight()
{
    if(socket->isWritable())
        socketStream<<"RotateRight"<<endl;
}

void MainWindow::client_disconnected()
{
    disconnect(socket,SIGNAL(readyRead()),this,SLOT(new_message()));
    disconnect(socket,SIGNAL(disconnected()),this,SLOT(client_disconnected()));
    socket->abort();
}

