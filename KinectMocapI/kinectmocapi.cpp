#include "kinectmocapi.h"
#include "stdafx.h"

KinectMocapI::KinectMocapI(QWidget *parent) : QMainWindow(parent) {
  ui.setupUi(this);
  connect(ui.calibration, SIGNAL(triggered()), this,
          SLOT(on_calibration_open()));
}

void KinectMocapI::on_calibration_open() {
  if (cWindow == nullptr) {
    cWindow = std::shared_ptr<CalibrationWindow>(new CalibrationWindow());
    connect(cWindow.get(), SIGNAL(sConfirm()), this,
            SLOT(on_calibration_confirm()));
    connect(cWindow.get(), SIGNAL(sClose()), this,
            SLOT(on_calibration_close()));
    connect(cWindow.get(), SIGNAL(sMessage(QString)), this,
            SLOT(on_calibration_message(QString)));
    cWindow->show();
    ui.textBrowser->insertPlainText("KMI: Open calibration window.\n");
  } else {
    ui.textBrowser->insertPlainText(
        "KMI: Calibration window already opened.\n");
  }
}

void KinectMocapI::on_calibration_confirm() {
  ui.textBrowser->insertPlainText("KMI: Calibration done.\n");
}

void KinectMocapI::on_calibration_close() {
  ui.textBrowser->insertPlainText("KMI: Close calibration window.\n");
  cWindow = nullptr;
}

void KinectMocapI::on_calibration_message(QString msg) {
  ui.textBrowser->insertPlainText(msg);
}

void KinectMocapI::closeEvent(QCloseEvent *event) { cWindow = nullptr; }