#pragma once

#include <QWidget>
#include "camthread.h"
#include "ui_calibrationwindow.h"

class CalibrationWindow : public QWidget {
  Q_OBJECT

 public:
  CalibrationWindow(QWidget *parent = Q_NULLPTR);
  ~CalibrationWindow();

 private:
  Ui::CalibrationWindow ui;
  std::shared_ptr<CamThread> cam_thread;

 signals:
  void sConfirm();
  void sClose();

 private slots:
  void on_confirmButton_clicked();
  void on_color_frame(QImage frame);

protected:
  void closeEvent(QCloseEvent *event);
};
