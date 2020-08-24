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
  std::vector<cv::Point2f> marker_centers;

 signals:
  void sConfirm();
  void sClose();
  void sMessage(QString msg);
  void sQueryCameraPoints(std::vector<cv::Point2f> points);

 private slots:
  void on_confirmButton_clicked();
  void on_identifyButton_clicked();
  void on_colorframe(cv::Mat frame);
  void on_queryresult(std::vector<cv::Point3f> points);
  void on_camerr(QString err);

 protected:
  void closeEvent(QCloseEvent *event);
};
