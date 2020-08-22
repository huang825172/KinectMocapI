#include "kinectmocapi.h"
#include "stdafx.h"

KinectMocapI::KinectMocapI(QWidget *parent) : QMainWindow(parent) {
  ui.setupUi(this);
  connect(ui.calibration, SIGNAL(triggered()), this,
          SLOT(on_calibration_open()));
}

void KinectMocapI::on_calibration_open() {
  cWindow = new CalibrationWindow();
  connect(cWindow, SIGNAL(sConfirm()), this, SLOT(on_calibration_confirm()));
  connect(cWindow, SIGNAL(sClose()), this, SLOT(on_calibration_close()));
  cWindow->show();
  ui.textBrowser->insertPlainText("Open calibration window.\n");
}

void KinectMocapI::on_calibration_confirm() {
  ui.textBrowser->insertPlainText("Calibration done.\n");
}

void KinectMocapI::on_calibration_close() {
  ui.textBrowser->insertPlainText("Close calibration window.\n");
}