#pragma once
#include <string>
#include <boost/network/protocol/http/client.hpp>
#include <json/json.h>

class ImageSender{
	
public:

	ImageSender(int session, std::string endpoint, std::string token);
	void send(std::string & data, std::string type);

private:

	std::string endpoint_, token_;
    boost::network::http::client client_;
	boost::network::http::client::response response_;
	Json::Value root_;
	Json::StyledWriter writer_;
};