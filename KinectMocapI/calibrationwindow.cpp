#include "calibrationwindow.h"

CalibrationWindow::CalibrationWindow(QWidget *parent) : QWidget(parent) {
  ui.setupUi(this);
  cam_thread = std::shared_ptr<CamThread>(new CamThread(this));
  cam_thread->start();
  connect(cam_thread.get(), SIGNAL(sColorFrame(QImage)), this,
          SLOT(on_color_frame(QImage)));
  ui.confirmButton->setEnabled(false);
}

CalibrationWindow::~CalibrationWindow() {}

void CalibrationWindow::on_confirmButton_clicked() {
  cam_thread->done = true;
  cam_thread->wait();
  emit sConfirm();
  close();
}

void CalibrationWindow::on_color_frame(QImage frame) {
  if (!cam_thread->done) {
    frame = frame.scaled(ui.imageLabel->width(), ui.imageLabel->height());
    ui.imageLabel->setPixmap(QPixmap::fromImage(frame));
  }
}

void CalibrationWindow::closeEvent(QCloseEvent *event) { emit sClose(); }