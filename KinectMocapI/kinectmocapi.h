#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_kinectmocapi.h"

class KinectMocapI : public QMainWindow
{
    Q_OBJECT

public:
    KinectMocapI(QWidget *parent = Q_NULLPTR);

private:
    Ui::KinectMocapIClass ui;
};
