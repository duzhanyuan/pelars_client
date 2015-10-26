#pragma once
#include "k2g.h"
#include "opt.h"
#include <iostream>
#include <aruco/aruco.h>
#include <aruco/cvdrawingutils.h>
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/core/eigen.hpp"
#include <opencv2/imgproc/imgproc.hpp>
#include "image_sender.h"
#include "base64.h"


extern bool to_stop;
extern bool online;
extern bool visualization;
extern double interval;
extern bool snapshot_table;

void handDetector(DataWriter & websocket, float marker_size, bool calibration, ImageSender & image_sender);

