#pragma once
#include "k2g.h"
#include "opt.h"
#include <iostream>
#include <aruco/aruco.h>
#include <aruco/cvdrawingutils.h>
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/highgui/highgui.hpp"

extern bool to_stop;
extern bool online;
extern bool visualization;
extern double interval;
extern std::chrono::time_point<std::chrono::system_clock> start;
extern const std::string currentDateTimeNow;
extern bool snapshot;

void handDetector(DataWriter & websocket, float marker_size, bool calibration);

