#include <stdio.h>
#include <iostream>
#include <string>
using namespace std;

#define CURL_STATICLIB
#include "curl.h"

#pragma comment(lib, "libcurl_a.lib")

size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata)
{
	size_t realsize = size * nmemb;
	string *response = (string*)userdata;
	response->append(ptr, nmemb);

	return realsize;
}

int main(void)
{
	string url;
	cout << "URL: ";
	cin >> url;

	CURL *curl;
	CURLcode res;

	string response;

	curl = curl_easy_init();
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&response);

		res = curl_easy_perform(curl);
		if (res != CURLE_OK)
			fprintf(stderr, "curl_easy_perform() failed: %s\n",
				curl_easy_strerror(res));
		curl_easy_cleanup(curl);
	}
	cout << response << endl;
	cout << endl << endl;
	cout << "Size: " << response.length();

	cin.ignore();
	cin.get();
	return 0;
}