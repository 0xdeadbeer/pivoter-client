#ifndef CONNECTIONS_PIVOTER_H
#define CONNECTIONS_PIVOTER_H

#include <iostream>
#include <string>
#include <vector>
#include <Windows.h>

class ConnectionsPivoter {
public:
	std::string server; 
	SHORT port; 
	ConnectionsPivoter(std::string server, SHORT port); 

	BOOL send_alive_signal(); 
	BOOL send_codes(std::vector<std::string> codes); 
private:

};

#endif 