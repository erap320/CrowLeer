#include "utils.hpp"
#include "uri.hpp"
#include "color.hpp"

#include <iostream>
#include <fstream>

#define CURL_STATICLIB
#include "curl.h"

using std::cout; using std::cin;  using std::endl;
using std::ofstream;

mutex queueMutex;
mutex consoleMutex;

size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata)
{
	size_t realsize = size * nmemb;
	string *response = (string*)userdata;
	response->append(ptr, nmemb);

	return realsize;
}

string HTTPrequest(string url)
{
	//CURL parameters
	CURL *curl;
	CURLcode res;

	string response; //Contains HTTP response

	//CURL initialization
	curl = curl_easy_init();
	if (curl) {
		//CURL options
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&response);

		//URL setting
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

		//HTTP request execution
		res = curl_easy_perform(curl);
		if (res != CURLE_OK)
		{
			error_out(">> Network Error: " + (string)curl_easy_strerror(res) + " on " + url);
		}

		//CURL cleanup
		curl_easy_cleanup(curl);
	}

	return response;
}

void debug_out(const unordered_set<string>& data)
{
	for (auto it = data.begin(); it != data.end(); it++)
	{
		cout << *it << endl;
	}
}

bool writeToDisk(const string& str, fs::path path)
{
	ofstream out(path, std::ios::binary);
	if (!out.is_open())
		return false;
	out << str;
	out.close();
	return true;
}

int findhref(const string& response, int offset)
{
	size_t pos; //Holds the position of the string searched for in the response

	int i;
	pos = response.find("href", offset); //Find the string "href"

	while (pos < response.length())
	{
		if (pos == string::npos) //If the string is not in the response
			return (int)pos;
		for (i = (int)(pos + 4); response[i] == ' '; i++) //Point to the first non-space char
			;
		//Check if the href is followed by =' or ="
		if (response[i] == '=')
		{
			for (i++; response[i] == ' '; i++)
				;
			if (response[i] == '\'' || response[i] == '"')
				return (int)pos;
		}
		pos = response.find("href", i); //Look for the next href
	}

	return -1;
}


void crawl(const string& response, unordered_set<string>& urls, queue<uri>& todo, bool saveflag, uri* const parent)
{
	size_t pos; //Holds the position of the string searched for in the response
	size_t before, after; //Hold the position of opening and closing quote of the href property
	string extracted;
	uri temp;

	//Find every href in the page and add the URL to the urls vector
	pos = findhref(response, 0);
	while (pos < response.length() && pos != string::npos)
	{
		before = response.find_first_of("\"'", pos + 4);
		after = response.find(response[before], before + 1);
		extracted = response.substr(before + 1, after - before - 1);

		queueMutex.lock();
			temp = parse(extracted, parent);
			//Add only if never found before
			auto search = urls.find(temp.tostring());
			if (search == urls.end())
			{
				urls.insert(temp.tostring());
				todo.push(temp);
			}
		queueMutex.unlock();

		pos = findhref(response, (int)(after + 1));
	}
}

string validate(string url)
{
	url = trim(url);
	size_t pos;
	pos = url.find("://");
	if (pos == string::npos)
		url = "http://" + url;
	return url;
}

void error_out(string s)
{
	consoleMutex.lock();
		Color('r');
		cout << s << endl;
		Color('w');
	consoleMutex.unlock();
}

void special_out(string s, bool color)
{
	if(color)
		Color('g');
	cout << s;
	if(color)
		Color('w');
}