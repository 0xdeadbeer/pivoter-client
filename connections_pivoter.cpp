#include "connections_pivoter.h"
#include <iostream>
#include <string>
#include <vector>
#include <Windows.h>

ConnectionsPivoter::ConnectionsPivoter(std::string server, SHORT port) {
	this->server = server; 
	this->port = port; 
}

BOOL send_alive_signal() {
	
}

BOOL send_codes(std::vector<std::string> codes) {

}
