#include "calibrationwindow.h"
#include <opencv2/imgproc/types_c.h>
#include <boost/lexical_cast.hpp>
#include <opencv.hpp>
#include <opencv2/aruco.hpp>
#include <opencv2/highgui.hpp>

CalibrationWindow::CalibrationWindow(QWidget *parent) : QWidget(parent) {
  ui.setupUi(this);
  qRegisterMetaType<std::vector<cv::Point2f>>("std::vector<cv::Point2f>");
  cam_thread = std::shared_ptr<CamThread>(new CamThread(this));
  cam_thread->start();
  connect(cam_thread.get(), SIGNAL(sColorFrame(cv::Mat)), this,
          SLOT(on_colorframe(cv::Mat)));
  connect(this, SIGNAL(sQueryCameraPoints(std::vector<cv::Point2f>)),
          cam_thread.get(),
          SLOT(on_QueryCameraPoints(std::vector<cv::Point2f>)));
  connect(cam_thread.get(), SIGNAL(sQueryResult(std::vector<cv::Point3f>)),
          this, SLOT(on_queryresult(std::vector<cv::Point3f>)));
  connect(cam_thread.get(), SIGNAL(sErr(QString)), this,
          SLOT(on_camerr(QString)));
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
    emit sMessage(QString::fromStdString(
        "Calibration: " + boost::lexical_cast<std::string>(marker_ids.size()) +
        " ArUco markers found.\n"));
    size_t marker_count = marker_ids.size();
    marker_centers.clear();
    for (size_t i = 0; i < marker_count; i++) {
      size_t corner_count = marker_corners[i].size();
      cv::Point2f center;
      for (size_t j = 0; j < corner_count; j++) {
        center.x += marker_corners[i][j].x;
        center.y += marker_corners[i][j].y;
      }
      center /= static_cast<int>(corner_count);
      marker_centers.push_back(center);
    }
    if (marker_count == 4) {
      std::vector<cv::Point2f> marker_centers_orderd;
      for (size_t id = 0; id < 4; id++) {
        for (size_t i = 0; i < marker_count; i++) {
          if (marker_ids[i] == id) {
            size_t corner_count = marker_corners[i].size();
            cv::Point2f center;
            for (size_t j = 0; j < corner_count; j++) {
              center.x += marker_corners[i][j].x;
              center.y += marker_corners[i][j].y;
            }
            center /= static_cast<int>(corner_count);
            marker_centers_orderd.push_back(center);
          }
        }
      }
      if (marker_centers_orderd.size() == 4) {
        sQueryCameraPoints(marker_centers_orderd);
        ui.confirmButton->setEnabled(true);
      } else {
        ui.confirmButton->setEnabled(false);
      }
    } else {
      ui.confirmButton->setEnabled(false);
    }
  }
}

void CalibrationWindow::on_colorframe(cv::Mat frame) {
  if (!cam_thread->done) {
    ui.identifyButton->setEnabled(true);
    cv::flip(frame, frame, 1);
    frame.copyTo(color_frame);
    cv::cvtColor(color_frame, color_frame, CV_BGR2RGB);
    for (size_t i = 0; i < marker_centers.size(); i++) {
      cv::circle(frame, marker_centers[i], 5, cv::Scalar(255, 0, 0), -1);
      cv::putText(frame, boost::lexical_cast<std::string>(marker_ids[i]),
                  marker_centers[i] + cv::Point2f(20, -20),
                  cv::FONT_HERSHEY_SIMPLEX, 3, cv::Scalar(255, 0, 0), 10);
    }
    display = QImage((frame.data), frame.cols, frame.rows,
                     static_cast<int>(frame.step), QImage::Format_RGB888)
                  .scaled(ui.imageLabel->width(), ui.imageLabel->height());
    ui.imageLabel->setPixmap(QPixmap::fromImage(display));
  }
}

void CalibrationWindow::on_queryresult(std::vector<cv::Point3f> points) {
  for (size_t i = 0; i < points.size(); i++) {
    std::string p = "(" + boost::lexical_cast<std::string>(points[i].x) + "," +
                    boost::lexical_cast<std::string>(points[i].y) + "," +
                    boost::lexical_cast<std::string>(points[i].z) + ")\n";
    emit sMessage(QString::fromStdString(p));
  }
  emit sMessage("Calibration: Get results.\n");
}

void CalibrationWindow::on_camerr(QString err) {
  emit sMessage("Cam: [ERR]" + err);
}

void CalibrationWindow::closeEvent(QCloseEvent *event) {
  cam_thread->done = true;
  cam_thread->wait();
  emit sClose();
}