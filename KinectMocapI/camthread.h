#pragma once

#include <QThread>

class CamThread : public QThread {
  Q_OBJECT

 public:
  CamThread(QObject *parent);
  ~CamThread();

  void run();
  bool done;

signals:
  void sColorFrame(QImage f);
};
