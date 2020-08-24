#pragma once

#include <QThread>
#include <opencv.hpp>

class CamThread : public QThread {
  Q_OBJECT

 public:
  CamThread(QObject *parent);
  ~CamThread();

  void run();
  bool done;

 private:
  bool query;
  std::vector<cv::Point2f> points_query;

 signals:
  void sColorFrame(cv::Mat color_frame);
  void sQueryResult(std::vector<cv::Point3f> points);
  void sErr(QString err);

 private slots:
  void on_QueryCameraPoints(std::vector<cv::Point2f> points);
};
