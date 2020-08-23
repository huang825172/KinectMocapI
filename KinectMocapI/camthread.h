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

 signals:
  void sColorFrame(cv::Mat frame);
};
