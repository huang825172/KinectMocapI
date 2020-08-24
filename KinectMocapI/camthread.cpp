#include "camthread.h"
#include <Kinect.h>
#include <opencv2/imgproc/types_c.h>
#include <qwidget.h>
#include <opencv.hpp>

CamThread::CamThread(QObject *parent) : QThread(parent) {
  qRegisterMetaType<cv::Mat>("cv::Mat");
  qRegisterMetaType<std::vector<cv::Point3f>>("std::vector<cv::Point3f>");
}

CamThread::~CamThread() {}

void CamThread::run() {
  done = false;
  query = false;

  IKinectSensor *sensor = nullptr;
  IColorFrameSource *color_source = nullptr;
  IColorFrameReader *color_reader = nullptr;
  IFrameDescription *color_description = nullptr;
  int color_height = 0, color_width = 0;

  IDepthFrameSource *depth_source = nullptr;
  IDepthFrameReader *depth_reader = nullptr;
  IFrameDescription *depth_description = nullptr;
  int depth_height = 0, depth_width = 0;

  ICoordinateMapper *coordinate_mapper = nullptr;
  bool inited = false;

  if (GetDefaultKinectSensor(&sensor) != S_OK) goto err;
  if (sensor->Open() != S_OK) goto err;

  if (sensor->get_ColorFrameSource(&color_source) != S_OK) goto err;
  if (color_source->OpenReader(&color_reader) != S_OK) goto err;
  if (color_source->get_FrameDescription(&color_description) != S_OK) goto err;
  if (color_description->get_Height(&color_height) != S_OK) goto err;
  if (color_description->get_Width(&color_width) != S_OK) goto err;

  if (sensor->get_DepthFrameSource(&depth_source) != S_OK) goto err;
  if (depth_source->OpenReader(&depth_reader) != S_OK) goto err;
  if (depth_source->get_FrameDescription(&depth_description) != S_OK) goto err;
  if (depth_description->get_Height(&depth_height) != S_OK) goto err;
  if (depth_description->get_Width(&depth_width) != S_OK) goto err;

  if (sensor->get_CoordinateMapper(&coordinate_mapper) != S_OK) goto err;
  inited = true;

  while (!done) {
    IColorFrame *color_frame;
    IDepthFrame *depth_frame;
    cv::Mat cv_color_frame(color_height, color_width, CV_8UC4);
    cv::Mat cv_depth_frame(depth_height, depth_width, CV_16UC1);
    while (color_reader->AcquireLatestFrame(&color_frame) != S_OK)
      ;
    while (depth_reader->AcquireLatestFrame(&depth_frame) != S_OK)
      ;
    color_frame->CopyConvertedFrameDataToArray(color_height * color_width * 4,
                                               cv_color_frame.data,
                                               ColorImageFormat_Bgra);
    depth_frame->CopyFrameDataToArray(depth_height * depth_width,
                                      (UINT16 *)cv_depth_frame.data);
    cv::cvtColor(cv_color_frame, cv_color_frame, CV_BGR2RGB);
    emit sColorFrame(cv_color_frame.clone());
    if (color_frame != nullptr) color_frame->Release();
    if (depth_frame != nullptr) depth_frame->Release();
    if (query) {
      CameraSpacePoint *cam_points =
          new CameraSpacePoint[color_height * color_width];
      coordinate_mapper->MapColorFrameToCameraSpace(
          depth_height * depth_width, (UINT16 *)cv_depth_frame.data,
          color_height * color_width, cam_points);
      std::vector<cv::Point3f> result;
      for (size_t i = 0; i < points_query.size(); i++) {
        size_t x = color_width - static_cast<size_t>(points_query[i].x);
        size_t y = static_cast<size_t>(points_query[i].y);
        cv::Point3f p = cv::Point3f(cam_points[color_width * y + x].X,
                                    cam_points[color_width * y + x].Y,
                                    cam_points[color_width * y + x].Z);
        result.push_back(p);
      }
      delete[] cam_points;
      points_query.clear();
      query = false;
      emit sQueryResult(result);
    }
    cv::waitKey(30);
  }

err:
  if (!inited) emit sErr("Kinect init failed.\n");
  if (coordinate_mapper != nullptr) coordinate_mapper->Release();
  if (color_description != nullptr) color_description->Release();
  if (color_reader != nullptr) color_reader->Release();
  if (color_source != nullptr) color_source->Release();
  if (depth_description != nullptr) depth_description->Release();
  if (depth_reader != nullptr) depth_reader->Release();
  if (depth_source != nullptr) depth_source->Release();
  if (sensor != nullptr) sensor->Release();
}

void CamThread::on_QueryCameraPoints(std::vector<cv::Point2f> points) {
  points_query.swap(points);
  query = true;
}