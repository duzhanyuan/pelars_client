#pragma once
#include "k2g.h"
#include "opt.h"
#include <iostream>
#include <aruco/aruco.h>
#include <aruco/cvdrawingutils.h>
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/core/eigen.hpp"
#include "image_sender.h"
#include "base64.h"


extern bool to_stop;
extern bool online;
extern bool visualization;
extern double interval;
extern std::chrono::time_point<std::chrono::system_clock> start;
extern bool snapshot_table;

void handDetector(DataWriter & websocket, float marker_size, bool calibration, ImageSender & image_sender);

