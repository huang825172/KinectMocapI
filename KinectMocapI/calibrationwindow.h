#pragma once

#include <QWidget>
#include <opencv.hpp>
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
  cv::Mat color_frame;
  QImage display;
  std::vector<int> marker_ids;
  std::vector<std::vector<cv::Point2f>> marker_corners;

 signals:
  void sConfirm();
  void sClose();
  void sMessage(QString msg);

 private slots:
  void on_confirmButton_clicked();
  void on_identifyButton_clicked();
  void on_colorframe(cv::Mat frame);

 protected:
  void closeEvent(QCloseEvent *event);
};
