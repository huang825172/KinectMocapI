#include "kinectmocapi.h"
#include "stdafx.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    KinectMocapI w;
    w.show();
    return a.exec();
}
