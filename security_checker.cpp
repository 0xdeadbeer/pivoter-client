#define CURL_STATICLIB 

#include <iostream>
#include <string>
#include <curl/curl.h>
#include "security_checker.h"
#include "json/json.hpp"

#pragma comment(lib, "Normaliz.lib")
#pragma comment(lib, "Ws2_32.lib") 
#pragma comment(lib, "Wldap32.lib") 
#pragma comment(lib, "Crypt32.lib") 
#pragma comment(lib, "advapi32.lib") 
#pragma comment(lib, "User32.lib")

using json = nlohmann::json; 

// GLOBAL CONFIG 
CURL* curl; 
CURLcode res; 

size_t write_function(void* delivered_data, size_t size, size_t nmemb, std::string* user_data) {
	user_data->append((char*)delivered_data, size * nmemb); 
	return size * nmemb; 
}

bool check_country(std::string country) {
	CURL* curl;
	CURLcode res; 

	curl_global_init(CURL_GLOBAL_ALL); 

	curl = curl_easy_init(); 
	if (!curl)
		return false; 

	std::string string_response; 

	curl_easy_setopt(curl, CURLOPT_URL, "http://ip-api.com/json/"); 
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &string_response); 
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_function); 
	res = curl_easy_perform(curl);

	if (res != CURLE_OK)
		return false; 

	json json_response = json::parse(string_response); 
	std::string response_country = json_response["country"]; 

	if (response_country != country)
		return false; 

	curl_easy_cleanup(curl); 
	curl_global_cleanup(); 
	return true; 
}
