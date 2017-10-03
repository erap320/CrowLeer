#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

#define CURL_STATICLIB
#include "curl.h"

#pragma comment(lib, "libcurl_a.lib")

//Function used by CURL to add chunks of data to the response string
size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata);

//View every item in the vector
void debug_vector(vector<string> data);

int main(void)
{
	string url;
	cout << "URL: ";
	cin >> url;

	//CURL parameters
	CURL *curl;
	CURLcode res;

	string response; //Contains HTTP response

	//CURL initialization
	curl = curl_easy_init();
	if (curl) {
		//CURL options
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&response);

		//HTTP request execution
		res = curl_easy_perform(curl);
		if (res != CURLE_OK)
			cout << "curl_easy_perform() failed: " << curl_easy_strerror(res) << "\n";

		//CURL cleanup
		curl_easy_cleanup(curl);
	}

	int pos; //Holds the position of the string searched for in the response
	int before, after; //Hold the position of opening and closing quote of the href property
	vector<string> urls; //Contains the URLs found in the response

	//Find every href in the page and add the URL to the urls vector
	pos = response.find("href");
	while (pos < response.length() && pos != string::npos)
	{
		before = response.find_first_of("\"'", pos + 4);
		after = response.find(response[before], before+1);
		urls.push_back(response.substr(before + 1, after - before - 1));
		pos = response.find("href", after+1);
	}
	
	debug_vector(urls);

	cin.ignore();
	cin.get();
	return 0;
}

size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata)
{
	size_t realsize = size * nmemb;
	string *response = (string*)userdata;
	response->append(ptr, nmemb);

	return realsize;
}

void debug_vector(vector<string> data)
{
	for (int i=0; i<data.size(); i++)
	{
		cout << data[i] << endl;
	}
}