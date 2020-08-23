#pragma once

#include <QtWidgets/QMainWindow>
#include "calibrationwindow.h"
#include "ui_kinectmocapi.h"

class KinectMocapI : public QMainWindow {
  Q_OBJECT

 public:
  KinectMocapI(QWidget *parent = Q_NULLPTR);

 private:
  Ui::KinectMocapIClass ui;
  std::shared_ptr<CalibrationWindow> cWindow;

 private slots:
  void on_calibration_open();
  void on_calibration_confirm();
  void on_calibration_close();
  void on_calibration_message(QString msg);

 protected:
  void closeEvent(QCloseEvent *event);
};
