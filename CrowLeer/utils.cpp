#include "utils.h"
#include "uri.h"

#include <iostream>
#include <fstream>

#define CURL_STATICLIB
#include "curl.h"

#pragma comment(lib, "libcurl_a.lib")

using std::cout; using std::cin;  using std::endl;
using std::ofstream;

mutex lock;

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
			cout << ">> Network Error: " << curl_easy_strerror(res) << "\n";
			cin.clear();
			cin.get();
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

bool debug_file(const string& str, string path)
{
	ofstream out(path);
	if (!out.is_open())
		return false;
	out << str;
	out.close();
}

int findhref(const string& response, int offset)
{
	int pos; //Holds the position of the string searched for in the response

	int i;
	pos = response.find("href", offset); //Find the string "href"

	while (pos < response.length())
	{
		if (pos == string::npos) //If the string is not in the response
			return pos;
		for (i = pos + 4; response[i] == ' '; i++) //Point to the first non-space char
			;
		//Check if the href is followed by =' or ="
		if (response[i] == '=')
		{
			for (i++; response[i] == ' '; i++)
				;
			if (response[i] == '\'' || response[i] == '"')
				return pos;
		}
		pos = response.find("href", i); //Look for the next href
	}
}

void crawl(const string& response, unordered_set<string>& urls, queue<uri>& todo, uri* const parent)
{
	int pos; //Holds the position of the string searched for in the response
	int before, after; //Hold the position of opening and closing quote of the href property
	string extracted;
	uri temp;

	//Find every href in the page and add the URL to the urls vector
	pos = findhref(response, 0);
	while (pos < response.length() && pos != string::npos)
	{
		before = response.find_first_of("\"'", pos + 4);
		after = response.find(response[before], before + 1);
		extracted = response.substr(before + 1, after - before - 1);

		lock.lock();
		//Add only if never found before
		auto search = urls.find(extracted);
		if (search == urls.end())
		{
			urls.insert(extracted);
			todo.push(parse(extracted,parent));
		}
		lock.unlock();

		pos = findhref(response, after + 1);
	}
}

string validate(string url)
{
	url = trim(url);
	int pos;
	pos = url.find("://");
	if (pos == string::npos)
		url = "http://" + url;
	return url;
}