#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <queue>
#include <unordered_set>
using namespace std;

#define CURL_STATICLIB
#include "curl.h"

#pragma comment(lib, "libcurl_a.lib")

//Function used by CURL to add chunks of data to the response string
size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata);

//Make an HTTP request with CURL
string HTTPrequest(string url);

//View every item in the data structure
void debug_out(const unordered_set<string>& data);

//Save string in file
bool debug_file(const string& str, string path);

//Finds the position of the next valid "href" string, that leads to a real URL
int findhref(const string& response, int offset);

//Search for hrefs in the response, verify if it was already found elsewhere and eventually push it in the todo queue
void crawl(const string& response, unordered_set<string>& data, queue<string>& todo);

int main(void)
{
	string url;
	cout << "URL: ";
	cin >> url;

	string response; //Contains HTTP response

	response = HTTPrequest(url);

	unordered_set<string> urls; //Hash table which contains the URLs found in the response
	queue<string> todo; //Queue containing the urls left to crawl

	crawl(response, urls, todo);

	/*while (!todo.empty())
	{
		url = todo.front();
		todo.pop();
		cout << url << endl;
		response = HTTPrequest(url);
		crawl(response, urls, todo);
	}*/

	debug_file(response, "C:\\Users\\Elia\\Desktop\\CrowLeerDebug.html");
	debug_out(urls);

	cout << "\nEnded";
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
			cout << ">>Network Error: " << curl_easy_strerror(res) << "\n";
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
	cout << endl;
	for (auto it=data.begin(); it!=data.end(); it++)
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
	pos = response.find("href", offset);
	
	while (pos < response.length())
	{
		if (pos == string::npos)
			return pos;
		for (i=pos+4; response[i] == ' '; i++)
			;
		if (response[i] == '=')
		{
			for (i++; response[i] == ' '; i++)
				;
			if(response[i]=='\'' || response[i]=='"')
				return pos;
		}
		pos = response.find("href", i);
	}
}

void crawl(const string& response, unordered_set<string>& urls, queue<string>& todo)
{
	int pos; //Holds the position of the string searched for in the response
	int before, after; //Hold the position of opening and closing quote of the href property
	string temp;

	//Find every href in the page and add the URL to the urls vector
	pos = findhref(response, 0);
	while (pos < response.length() && pos != string::npos)
	{
		before = response.find_first_of("\"'", pos + 4);
		after = response.find(response[before], before + 1);
		temp = response.substr(before + 1, after - before - 1);
		urls.insert(temp);
		todo.push(temp);
		pos = findhref(response, after + 1);
	}
}