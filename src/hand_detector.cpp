#include "hand_detector.h"

void handDetector(DataWriter & websocket, float marker_size, bool calibration, ImageSender & image_sender)
{
	aruco::MarkerDetector MDetector;
	vector<aruco::Marker> markers;
	if(visualization)
		cv::namedWindow("hands");
	
	Json::Value root;
	Json::StyledWriter writer;
	root["obj"]["type"] = "hand";

	float tx, ty, tz, rw, rx, ry, rz;
	std::string message;

	K2G k2g(OPENGL);

	TimedSender timer(interval);

	cv::Mat camera_parameters = cv::Mat::eye(3, 3, CV_32F);
	camera_parameters.at<float>(0,0) = k2g.getRgbParameters().fx; 
	camera_parameters.at<float>(1,1) = k2g.getRgbParameters().fy; 
	camera_parameters.at<float>(0,2) = k2g.getRgbParameters().cx; 
	camera_parameters.at<float>(1,2) = k2g.getRgbParameters().cy;
	cv::Mat grey;
	bool to_send;

	cv::Mat calib_matrix = cv::Mat::eye(cv::Size(4, 4), CV_32F);

	if(calibration)
	{	
		cv::FileStorage file("../../data/calibration.xml", cv::FileStorage::WRITE);
		bool found = false;

		while(!found){
			grey = k2g.getGrey();
			MDetector.detect(grey, markers, camera_parameters, cv::Mat(), marker_size);
	
			if(markers.size() > 0){
				for(int i = 0; i < markers.size(); ++i){
					if(markers[i].id == 784){
						calib_matrix.at<float>(0, 3) = markers[i].Tvec.at<float>(0);
						calib_matrix.at<float>(1, 3) = markers[i].Tvec.at<float>(1);
						calib_matrix.at<float>(2, 3) = markers[i].Tvec.at<float>(2);
						cv::Rodrigues(markers[i].Rvec, cv::Mat(calib_matrix, cv::Rect(0, 0, 3, 3)));
						file.release();					
						found = true;
					}
				}		
			}
		}
	}
	else
	{
		cv::FileStorage file("../../data/calibration.xml", cv::FileStorage::READ);
		if(file.isOpened())
		{	
			file["matrix"] >> calib_matrix;
			file.release();
		}else{
			std::cout << "could not find hand calibration file; use -c to calibrate the camera" << std::endl;
			to_stop = true;
		}
	}
	
	cv::Mat camera_inverse = calib_matrix.inv();
	cv::Mat marker_pose = cv::Mat::eye(cv::Size(4, 4), CV_32F);

	while(!to_stop)
	{
		grey = k2g.getGrey();
		if(snapshot_table){
			std::time_t now_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
			std::string now = std::string(std::ctime(&now_time));
			std::remove_if(now.begin(), now.end(), ::isspace);
			imwrite( "../snapshots/table_" + now + ".jpg", grey);
			if(online){
				std::ifstream in( "../snapshots/table_" + now + ".jpg", std::ifstream::binary );
				std::vector<char> data((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
				std::string code = base64_encode((unsigned char*)&data[0], (unsigned int)data.size());
				image_sender.send(code, "jpg");
			}
			snapshot_table = false;
		}

		MDetector.detect(grey, markers, camera_parameters, cv::Mat(), marker_size);

		if(markers.size() > 0){
			to_send = timer.needSend();
			for(int i = 0; i < markers.size(); ++i)
			{
				// Get marker position
				markers[i].draw(grey, cv::Scalar(0, 0, 255), 2);
				//aruco::CvDrawingUtils::draw3dCube(grey, markers[i], camera);
				
				marker_pose.at<float>(0, 3) = markers[i].Tvec.at<float>(0);
				marker_pose.at<float>(1, 3) = markers[i].Tvec.at<float>(1);
				marker_pose.at<float>(2, 3) = markers[i].Tvec.at<float>(2);
				cv::Rodrigues(markers[i].Rvec, cv::Mat(marker_pose, cv::Rect(0, 0, 3, 3)));

				cv::Mat pose = camera_inverse * marker_pose;
				tx = pose.at<float>(0, 3);
				ty = pose.at<float>(1, 3);
				tz = pose.at<float>(2, 3);
				Eigen::Matrix3f temp;
				cv2eigen(pose(cv::Rect(0,0,3,3)), temp);
				Eigen::Quaternionf quaternion(temp);
				
				//std::cout << x << " " << y << " " << z << " " << markers[i].id << std::endl;
				if(to_send){
					root["obj"]["id"] = markers[i].id;
					root["obj"]["tx"] = tx;
					root["obj"]["ty"] = ty;
					root["obj"]["tz"] = tz / 1000;
					root["obj"]["rw"] = quaternion.w();
					root["obj"]["rx"] = quaternion.x();
					root["obj"]["ry"] = quaternion.y();
					root["obj"]["rz"] = quaternion.z();
					root["obj"]["time"] = (double)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start).count();
					message = writer.write(root);	

					// Send the message online and store it offline
					if(online){
						//std::cout << "Hand detector posting data to the server\n " << std::flush;
						io.post( [&websocket, message]() {
							websocket.writeData(message);
							});
						}
					websocket.writeLocal(message);
				}
			}
		}

		if(visualization){
			cv::imshow("hands", grey);
			int c = cv::waitKey(1);
			if((char)c == 'q' )
			{
				to_stop = true;
				std::cout << "Stop requested by hand detector" << std::endl;
			}
		}
	}

	//Destroy the window
	cvDestroyWindow("hands");
	k2g.shutDown();
	return;
}

		