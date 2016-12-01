#include "webcam_publisher.h"

void webcamPublisher(int face_camera_id, ChannelWrapper<ImageFrame> & pc_webcam){

	synchronizer.lock();

	const unsigned int width = 1920;
	const unsigned int height = 1080;

	GstreamerGrabber gs_grabber(width, height, face_camera_id);

	synchronizer.unlock();
	
	while(!to_stop){

		IplImage * frame = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 3);
		gs_grabber.capture(frame); // copies using gst_buffer_extract

		std::shared_ptr<ImageFrame> image = std::make_shared<ImageFrame>();
		image->color_ = cv::Mat(frame); // passes ownership of pointer
		image->type_ = std::string("people");
		image->time_stamp_ = std::chrono::high_resolution_clock::now();

		pc_webcam.write(image);
		
	}
	std::cout << "terminating webcam publisher" << std::endl;

}