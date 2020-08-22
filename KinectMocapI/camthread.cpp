#include "camthread.h"
#include <Kinect.h>
#include <opencv2/imgproc/types_c.h>
#include <qwidget.h>
#include <opencv.hpp>

CamThread::CamThread(QObject *parent) : QThread(parent) {}

CamThread::~CamThread() {}

void CamThread::run() {
  done = false;

  IKinectSensor *sensor = nullptr;
  IColorFrameSource *color_source = nullptr;
  IColorFrameReader *color_reader = nullptr;
  IFrameDescription *description = nullptr;
  int color_height = 0, color_width = 0;

  if (GetDefaultKinectSensor(&sensor) != S_OK) goto err;
  if (sensor->Open() != S_OK) goto err;
  if (sensor->get_ColorFrameSource(&color_source) != S_OK) goto err;
  if (color_source->OpenReader(&color_reader) != S_OK) goto err;
  if (color_source->get_FrameDescription(&description) != S_OK) goto err;
  if (description->get_Height(&color_height) != S_OK) goto err;
  if (description->get_Width(&color_width) != S_OK) goto err;

  while (!done) {
    IColorFrame *color_frame;
    QImage qt_color_frame;
    cv::Mat cv_color_frame(color_height, color_width, CV_8UC4);
    while (color_reader->AcquireLatestFrame(&color_frame) != S_OK)
      ;
    color_frame->CopyConvertedFrameDataToArray(color_height * color_width * 4,
                                               cv_color_frame.data,
                                               ColorImageFormat_Bgra);
    cv::cvtColor(cv_color_frame, cv_color_frame, CV_BGR2RGB);
    qt_color_frame =
        QImage((cv_color_frame.data), cv_color_frame.cols, cv_color_frame.rows,
               static_cast<int>(cv_color_frame.step), QImage::Format_RGB888);
    emit sColorFrame(qt_color_frame.copy());
    if (color_frame != nullptr) color_frame->Release();
    cv::waitKey(30);
  }

err:
  if (description != nullptr) description->Release();
  if (color_reader != nullptr) color_reader->Release();
  if (color_source != nullptr) color_source->Release();
  if (sensor != nullptr) sensor->Release();
}
