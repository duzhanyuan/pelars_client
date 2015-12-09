#include "opt.h"

// Keep alive
void aliver(const boost::system::error_code& /*e*/)
{
	
}

void asiothreadfx()
{
	boost::asio::deadline_timer t(io, boost::posix_time::seconds(100000));
	t.async_wait(aliver);
	io.run();
}

const std::string currentDateTime() {
	time_t     now = time(0);
	struct tm  tstruct;
	char       buf[80];
	tstruct = *localtime(&now);
	strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);
	return buf;
}

cv::Mat drawQr(int width, int repetitions, int session)
{
	// Create qr code 
	QRcode * qrcode;
    qrcode = QRcode_encodeString(std::to_string(session).c_str(), 1, QR_ECLEVEL_H, QR_MODE_8, false);

    // Initialize data
    int scale = qrcode->width * 8;
    int position = scale / 2;

    // Write session in the opencv window
    cv::Mat qr(scale * 2, scale * 2, CV_8UC1);
    qr.setTo(cv::Scalar(255, 255, 255));
    cv::putText(qr, std::to_string(session), cv::Point(10, 50), 6, 1, cv::Scalar(0, 0, 0), 1.5, 8);
	int QR_width = qrcode->width;

	// Create actual qr code from data
	if(QR_width * repetitions > width)
	{
		std::cout << "overflow" << std::endl;
		return cv::Mat();
	}
	for(int y = 0; y < QR_width; ++y)
  		for(int x = 0; x < QR_width; ++x)
			for(int yi = 0; yi < repetitions; ++yi)
				for(int xi = 0; xi < repetitions; ++xi)
	  				qr.at<char>(y * repetitions + yi + position, x * repetitions + xi + position) = qrcode->data[y * qrcode->width + x] & 1 ? 0 : 255;
	return qr;  		
}

// Check if escape was inserted as input
void checkEscape(bool visualization, bool special){
	if(!visualization && !special){
		while(std::cin.get() != 27){
			if (to_stop){
				break;
			}
		}
		to_stop = true;
		std::cout << "Stopping" << std::endl;
	}
}

int uploadData(std::string file_name, std::string end_point){

	// Open file
	std::ifstream in(file_name);
	if(!in.is_open()){
		std::cout << "could not open file " << std::endl;
		return -1;
	}

	// Read the session id from the file name
	std::string delimiter = "_";
	size_t last = 0; size_t next = 0; 
	next = file_name.find(delimiter, last);
	last = next + 1;
	next = file_name.find(delimiter, last);
	int s = stoi(file_name.substr(last, next-last));

	// Create new session if the session id is not present on the server
	SessionManager * sm;
	if (s == -1){
		sm = new SessionManager(end_point);
		sm->login();
		s = sm->getNewSession();
	}

	// Connect the websocker on the upload endpoint
	DataWriter collector(end_point + "upload", s, false);
	sleep(1); //else the websocket is not initialized

	// Initialize data for reading
	int packet_size = 0;
	char buffer[1024];
	int num = 0;
	std::cout << "uploading data to " << s << std::endl;

	// Read data and send it in chunks of 300 packets every 30s
	while(!in.eof()){
		in >> packet_size;
		in.read(buffer, packet_size);
		std::string tmp(buffer, packet_size);
		collector.writeData(tmp);
		//std::cout << "packet" << std::endl;
		//std::cout << tmp << std::endl;
		std::cout << "." << std::flush;
		num++;
		if(!(num % 300)){
			std::cout << "sent 300 packets, sleeping 30s" << std::endl;
			sleep(30);
		}
	}
	std::cout << std::endl;
	std::cout << num << " packet sent" << std::endl;

	// Close websocket and session
	collector.stop();
	sm->closeSession(s);
	return 0;
}

