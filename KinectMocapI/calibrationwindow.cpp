#include "calibrationwindow.h"
#include <opencv2/imgproc/types_c.h>
#include <opencv.hpp>
#include <opencv2/aruco.hpp>
#include <opencv2/highgui.hpp>

CalibrationWindow::CalibrationWindow(QWidget *parent) : QWidget(parent) {
  ui.setupUi(this);
  cam_thread = std::shared_ptr<CamThread>(new CamThread(this));
  cam_thread->start();
  connect(cam_thread.get(), SIGNAL(sColorFrame(cv::Mat)), this,
          SLOT(on_colorframe(cv::Mat)));
  ui.identifyButton->setEnabled(false);
  ui.confirmButton->setEnabled(false);
}

CalibrationWindow::~CalibrationWindow() {
  cam_thread->done = true;
  cam_thread->wait();
}

void CalibrationWindow::on_confirmButton_clicked() {
  cam_thread->done = true;
  cam_thread->wait();
  emit sConfirm();
  close();
}

void CalibrationWindow::on_identifyButton_clicked() {
  if (!cam_thread->done) {
    cv::Mat mat;
    color_frame.copyTo(mat);
    const cv::Ptr<cv::aruco::Dictionary> dictionary =
        cv::aruco::getPredefinedDictionary(cv::aruco::DICT_4X4_50);
    cv::aruco::detectMarkers(mat, dictionary, marker_corners, marker_ids);
    if (marker_ids.size() > 0) {
      cv::aruco::drawDetectedMarkers(mat, marker_corners, marker_ids);
      size_t marker_count = marker_corners.size();
      emit sMessage("Calibration: " + QString::number(marker_count) +
                    " ArUco markers found.\n");
      /*for (size_t i = 0; i < marker_count; i++) {
        cv::Point2f center;
        size_t corner_count = marker_corners[i].size();
        for (size_t j = 0; j < corner_count; i++) {
          center += marker_corners[i][j];
        }
        center /= static_cast<int>(corner_count);
        cv::circle(mat, center, 10, cv::Scalar(0, 0, 0), -1);
      }*/
    }
  }
}

void CalibrationWindow::on_colorframe(cv::Mat frame) {
  if (!cam_thread->done) {
    cv::flip(frame, frame, 1);
    frame.copyTo(color_frame);
    cv::cvtColor(color_frame, color_frame, CV_BGR2RGB);
    if (marker_ids.size() > 0) {
      cv::aruco::drawDetectedMarkers(frame, marker_corners, marker_ids);
    }
    ui.identifyButton->setEnabled(true);
    display = QImage((frame.data), frame.cols, frame.rows,
                     static_cast<int>(frame.step), QImage::Format_RGB888)
                  .scaled(ui.imageLabel->width(), ui.imageLabel->height());
    ui.imageLabel->setPixmap(QPixmap::fromImage(display));
  }
}

void CalibrationWindow::closeEvent(QCloseEvent *event) { emit sClose(); }